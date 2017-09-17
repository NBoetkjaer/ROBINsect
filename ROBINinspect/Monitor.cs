using System;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Text;

namespace ROBINinspect
{
    public class Monitor

    {
        public DiscoveryInfo info;
        public BaseNode rootNode;
        NodeXmlConverter xmlConverter = new NodeXmlConverter();

        Socket socketXML;
        const Int32 portXML = 1975;

        const string transferStart = "<" + NodeXmlConverter.Transaction;
        const string transferEnd = "</" + NodeXmlConverter.Transaction + ">";

        int transactionStartPos = -1;
        string transactionReceiveBuffer;

        // Use this for initialization
        void Start () {
        
        }
        
        // Update is called once per frame
        public void Update () {
            ReceiveXML();
        }

        public event EventHandler TransactionRecieved;

        public void OnTransactionRecieved()
        {
            if (TransactionRecieved != null)
            {
                TransactionRecieved(this, EventArgs.Empty);
            }
        }
        
        public bool IsConnected
        { 
            get
            { 
                if (socketXML != null && socketXML.Connected)
                { 
                    return true;
                }
                return false;
            }
        }
        public void Close()
        {
            if (socketXML != null)
            {
                socketXML.Shutdown(SocketShutdown.Both);
                socketXML.Close();
                socketXML = null;
            }
        }
        public void Connect()
        {
            rootNode = null;
            transactionReceiveBuffer = string.Empty;
            if (socketXML == null || !socketXML.Connected)
            {
                // Create a TCP/IP  socket.
                socketXML = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IPAddress ipAddress = IPAddress.Parse(info.ipAddr);
                IPEndPoint remoteEP = new IPEndPoint(ipAddress, portXML);
                // Connect the socket to the remote endpoint. Catch any errors.
                try
                {
                    socketXML.Connect(remoteEP);
                }
                catch (SocketException se)
                {
                    Console.WriteLine("SocketException : {0}", se.ToString());
                }
            }
        }

        static int ReadFromSocket(Socket sock, Byte[] byteArray, ref int offset)
        {            
            sock.ReceiveTimeout = 10;
            try
            {
                bool bMoreData = (sock.Available > 0);
                int bufferLen = byteArray.Length - offset;
                int maxBufferLen = bufferLen;
                while (bMoreData && bufferLen > 0)
                {
                    Int32 bytes = sock.Receive(byteArray, offset, bufferLen, 0);
                    bufferLen -= bytes;
                    offset += bytes;
                    bMoreData = (sock.Available > 0);
                }
                // Return number of received bytes.
                return maxBufferLen - bufferLen;
            }
            catch (SocketException se)
            {
                Console.WriteLine("SocketException : {0}", se.ToString());
            }
            return 0;
        }
        static string ReadFromSocket(Socket sock)
        {
            //Byte[] recvBytes = new Byte[2048];
            Byte[] recvBytes = new Byte[4096];
            String responseData = String.Empty;
            sock.ReceiveTimeout = 10;
            try
            {
                bool bMoreData = (sock.Available > 0);
                while (bMoreData)
                {
                    Int32 bytes = sock.Receive(recvBytes, recvBytes.Length, 0);
                    responseData += System.Text.Encoding.ASCII.GetString(recvBytes, 0, bytes);
                    bMoreData = (sock.Available > 0);
                }
            }
            catch (SocketException se)
            {
                Console.WriteLine("SocketException : {0}", se.ToString());
            }
            return responseData;
        }
        void AddXMLToNodeTree(String xmlStr)
        {
            if (rootNode == null)
            {
                rootNode = new BaseNode("root");
                xmlConverter.ResolveMirrors = true;
            }
            xmlConverter.UpdateTreeFromXml(rootNode, xmlStr);
            OnTransactionRecieved(); 
        }

        void ReceiveXML()
        {
            if (socketXML != null && socketXML.Connected)
            {   
                string newString = ReadFromSocket(socketXML);
                if(newString.Length == 0) return;
                int currPos = transactionReceiveBuffer.Length;
                transactionReceiveBuffer += newString;
                int posStart, posEnd;
                while(true) // ??
                {
                    // No current transfer.
                    if(transactionStartPos < 0)
                    {
                        // Look for start of a new transaction.
                        posStart = Math.Max(0, currPos - transferStart.Length);
                        posStart = transactionReceiveBuffer.IndexOf(transferStart, posStart);
                        if (posStart > -1)
                        {
                            // Found start now look for end of a transaction.
                            posEnd = transactionReceiveBuffer.IndexOf(transferEnd, posStart);
                            if (posEnd > -1)
                            {   // Found entire transaction.
                                // Move current position forward past transfer end.
                                currPos = posEnd + transferEnd.Length;
                                // Handle message.
                                AddXMLToNodeTree(transactionReceiveBuffer.Substring(posStart, currPos - posStart));
                                // Remove old message from recieve buffer.
                                transactionReceiveBuffer = transactionReceiveBuffer.Substring(currPos);
                                currPos = 0;

                            }
                            else
                            {   // Save start of transaction.
                                transactionStartPos = posStart;
                                return;
                            }
                        }
                        else
                        {
                            //Console.WriteLine(Environment.NewLine + "xmldata:" + Environment.NewLine + transactionReceiveBuffer + Environment.NewLine + Environment.NewLine);
                            return;
                        }
                    }
                    else
                    {// Look for end of a transaction.
                        posEnd = Math.Max(0, currPos - transferEnd.Length);
                        posEnd = transactionReceiveBuffer.IndexOf(transferEnd, posEnd);
                        if(posEnd > -1)
                        { 
                            // Move current position forward past transfer end.
                            currPos = posEnd + transferEnd.Length;
                            // Handle message end
                            AddXMLToNodeTree(transactionReceiveBuffer.Substring(transactionStartPos, currPos - transactionStartPos));
                            transactionStartPos = -1;
                            // Remove old message from recieve buffer.
                            transactionReceiveBuffer = transactionReceiveBuffer.Substring(currPos);
                            currPos = 0;
                        }
                        else
                        {
                            return;
                        }
                    }
                }
            }
        }

        public void SendTreeUpdates()
        {
            if (rootNode.AnyChanges() && IsConnected)
            {
                String strXml = String.Empty;
                xmlConverter.ConvertToXml(rootNode, ref strXml, FlagType.none, true);
                strXml = transferStart+ ">" + strXml + transferEnd;
                byte[] send_buffer = Encoding.ASCII.GetBytes(strXml);
                socketXML.Send(send_buffer);
            }
        }
    } // class
} // namespace
