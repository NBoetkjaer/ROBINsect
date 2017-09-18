using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

using System.Timers;
using ROBINinspect;

namespace ROBINspect
{
    [Activity(Label = "MonitorActivity")]
    public class MonitorActivity : Activity
    {
        Monitor robinMonitor = new Monitor();
        DiscoveryInfo robinInfo;
        Timer tmr = new Timer(50);
        public BaseNode currentNode;
        public Fragment activeFragment {get; set;}
        string path;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            Window.RequestFeature(WindowFeatures.NoTitle); // Hide the title Bar.
            // Set view from the layout resource
            SetContentView(Resource.Layout.Monitor);

            Bundle bundle = Intent.GetBundleExtra("bundle");
            if (savedInstanceState!=null)
            {
                path = savedInstanceState.GetString("path");
            }
            ParcelableDiscoverInfo parcel = bundle.GetParcelable("info") as ParcelableDiscoverInfo;
            if (parcel == null) return;
            robinInfo = parcel.info;

            Button btnConnect = FindViewById<Button>(Resource.Id.btnConnect);
            btnConnect.Click += BtnConnect_Click;
            BtnConnect_Click(btnConnect, EventArgs.Empty);

            FragmentTransaction fragmentTransaction = FragmentManager.BeginTransaction();
            MonitorTreeFragment myFrag = new MonitorTreeFragment();
            activeFragment = myFrag;
            // work here to change Activity fragments (add, remove, etc.).  Example here of adding.
            fragmentTransaction.Replace(Resource.Id.fragment_container, activeFragment);
            fragmentTransaction.SetTransition(FragmentTransit.FragmentFade);
            fragmentTransaction.Commit();
        }

        protected override void OnSaveInstanceState(Bundle outState)
        {
            base.OnSaveInstanceState(outState);
            if (currentNode != null)
            {
                outState.PutString("path", currentNode.GetFullPath());
            }
        }

        public override void OnBackPressed()
        {
            if (currentNode != null && currentNode.ParentNode != null && activeFragment is MonitorTreeFragment)
            {
                currentNode = currentNode.ParentNode;
                OnChanged();
            }
            else
            {
                base.OnBackPressed();
            }
        }

        protected override void OnPause()
        {
            base.OnPause();
            robinMonitor.Close();
        }

        private void BtnConnect_Click(object sender, EventArgs e)
        {
            tmr.Interval = 20;
            tmr.Elapsed += Tmr_Elapsed;
            tmr.Enabled = true;
            if (robinMonitor.IsConnected)
            {
                robinMonitor.Close();
                robinMonitor.TransactionRecieved -= robinMonitor_TransactionRecieved;
                currentNode = null;
            }
            else
            {
                robinMonitor.info = robinInfo;
                robinMonitor.TransactionRecieved += robinMonitor_TransactionRecieved;
                robinMonitor.Connect();
            }
        }

        private void Tmr_Elapsed(object sender, ElapsedEventArgs e)
        {
            RunOnUiThread(() => { Update(); });
        }

        private void Update()
        {
            Button btnConnect = FindViewById<Button>(Resource.Id.btnConnect);
            if (robinMonitor.IsConnected)
            {
                btnConnect.Text = "Close";
                robinMonitor.Update();
                if (robinMonitor.rootNode == null) return;

                robinMonitor.rootNode.ClearAllChanges();
                OnAddData();
                robinMonitor.SendTreeUpdates();// Send any updates.
            }
            else
            {
                btnConnect.Text = "Connect";
            }
        }

        private void robinMonitor_TransactionRecieved(object sender, EventArgs e)
        {
            if(currentNode == null) //Initial connection
            {
                if (!String.IsNullOrEmpty(path))
                {
                    currentNode= robinMonitor.rootNode.FindNode(path);
                }

                if (currentNode == null) // Fall back: Set current node to root node.
                {
                    currentNode = robinMonitor.rootNode;
                }
            }
            OnChanged();
        }

        public event EventHandler Changed;
        protected void OnChanged()
        {
            if (Changed != null)
            {
                Changed(this, EventArgs.Empty);
            }
        }

        public event EventHandler AddData;
        protected void OnAddData()
        {
            if (AddData != null)
            {
                AddData(this, EventArgs.Empty);
                if(currentNode != null && currentNode.AnyChanges())
                {
                    OnChanged();
                }
            }
        }

    }
}