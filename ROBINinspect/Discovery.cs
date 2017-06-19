using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;

namespace ROBINinspect
{
    public class DiscoveryInfo {
        public String deviceName;
        public int serialNumber;
        public String ipName;
        public String ipAddr;
        public String connectionStatus;

        // Constructor
        public DiscoveryInfo(string msg)
        {
            Initialize (msg);
        }
        public DiscoveryInfo(string msg, EndPoint ep)
        {
            Initialize (msg);
            IPEndPoint ipep = ep as IPEndPoint;
            if(ipep != null)
                ipAddr = ipep.Address.ToString();
        }

        private void Initialize(string msg)
        {
        }

        public string DisplayString()
        {
            string dispText = deviceName + Environment.NewLine +
                " SN:" + serialNumber.ToString() + Environment.NewLine;
            dispText += Environment.NewLine;

            dispText += ipName + " (IP:" + ipAddr + ")" + Environment.NewLine +
                connectionStatus;
            return dispText;
        }

    }
#if UNITY
    public class Discovery : MonoBehaviour
#else
    public class Discovery
#endif
    {
        private Socket socketDiscovery;
        Int32 portDiscovery = 1974;
        public Dictionary<long,DiscoveryInfo> dicoveryMap = new Dictionary<long, DiscoveryInfo>();

        // Use this for initialization
        void Start () {

        }

        // Update is called once per frame
        public void Update()
        {
            if (socketDiscovery == null || !socketDiscovery.IsBound) return;
            int waitingData = socketDiscovery.Available;
            if (waitingData < 1) return;

            byte[] recv_buffer = new Byte[waitingData];
            EndPoint remoteEP = new IPEndPoint(IPAddress.Any, 0);
            Int32 bytes = socketDiscovery.ReceiveFrom(recv_buffer, ref remoteEP);
            String recvStr = System.Text.Encoding.ASCII.GetString(recv_buffer, 0, bytes);

            DiscoveryInfo info = new DiscoveryInfo(recvStr, remoteEP);
            IPEndPoint ipep = remoteEP as IPEndPoint;
            if (ipep != null)
            {
                dicoveryMap[ipep.Address.GetHashCode()] = info;
                // Fire discover event 
                OnDiscover(EventArgs.Empty);
            }
        }

        public event EventHandler Discover;

        // Invoke the Changed event; called whenever list changes
        protected virtual void OnDiscover(EventArgs e)
        {
            if (Discover != null)
                Discover(this, e);
        }

        public void StartDiscovery()
        {
            if (socketDiscovery == null) {
                // Create a UDP socket.
                socketDiscovery = new Socket (AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                socketDiscovery.EnableBroadcast = true;
                // Listen on all interfaces (IPAddress.Any).
                IPEndPoint localEP = new IPEndPoint (IPAddress.Any, 0);
                socketDiscovery.Bind (localEP);
                socketDiscovery.ReceiveTimeout = 10;
            }
            dicoveryMap.Clear ();
            byte[] send_buffer = Encoding.ASCII.GetBytes("Hello ROBIN.");
            BroadcastOnAllInterfaces(send_buffer, socketDiscovery, portDiscovery);
        }

        private void BroadcastOnAllInterfaces(byte[] msg, Socket socketBroadcast, Int32 port)
        {

            byte[] addr = { 127, 0, 0, 1 };
            IPAddress broadcastAddress = new IPAddress(addr);
            IPEndPoint remoteEP = new IPEndPoint(broadcastAddress, port);
            socketBroadcast.SendTo(msg, remoteEP);
/*
            IPHostEntry host = Dns.GetHostEntry(Dns.GetHostName());
            
            foreach (IPAddress ip in host.AddressList)
            {
                if (ip.AddressFamily != AddressFamily.InterNetwork) continue;
                IPAddress broadcastAddress = GetBroadcastAddress(ip);
                IPEndPoint remoteEP = new IPEndPoint(broadcastAddress, port);
                socketBroadcast.SendTo(msg, remoteEP);
            }
 */
        } 

        private IPAddress GetBroadcastAddress(IPAddress address)
        {
            byte[] local = address.GetAddressBytes();
            foreach (NetworkInterface adapter in NetworkInterface.GetAllNetworkInterfaces())
            {
                foreach (UnicastIPAddressInformation unicastIPAddressInformation in adapter.GetIPProperties().UnicastAddresses)
                {
                    if (unicastIPAddressInformation.Address.AddressFamily != AddressFamily.InterNetwork) continue;
                    if (address.Equals(unicastIPAddressInformation.Address))
                    {
                        // Calculate broadcast address for this interface.
                        byte[] mask = unicastIPAddressInformation.IPv4Mask.GetAddressBytes();
                        for (int i = 0; i < mask.Length; ++i)
                        {
                            mask[i] = (byte)((int)local[i] | ~mask[i]);
                        }
                        return new IPAddress(mask);
                    }
                }
            }
            throw new ArgumentException(string.Format("Can't find subnet mask for IP address '{0}'", address));
        }
    }
}
