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

    [Activity(Label = "DiscoveryActivity", MainLauncher = true, Icon = "@drawable/icon")]
    public class DiscoveryActivity : Activity
    {
        Discovery discover;
        Timer tmr;
        ListView listView;
        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
        }
        protected override void OnStart()
        {
            base.OnStart();
            discover = new Discovery();
            tmr = new Timer(200);

            // Set view from the layout resource
            SetContentView(Resource.Layout.Discovery);

            discover.Discover += Discover_Discover;
            Button btnSearch= FindViewById<Button>(Resource.Id.btnSearch);
            btnSearch.Click += BtnSearch_Click;
            BtnSearch_Click(btnSearch, EventArgs.Empty);
            listView = FindViewById<ListView>(Resource.Id.listView1);
            listView.ItemClick += ListView_ItemClick;
        }
        
        private void ListView_ItemClick(object sender, AdapterView.ItemClickEventArgs e)
        {
            tmr.Stop();
            tmr.Dispose();
            DiscoveryInfo info = discover.dicoveryMap.ElementAt(e.Position).Value;
            ParcelableDiscoverInfo parcel = new ParcelableDiscoverInfo();
            parcel.info = info;

            Bundle bundle = new Bundle();
            bundle.PutParcelable("info", parcel);
            Intent subActivity = new Intent(this, typeof(MonitorActivity));
            subActivity.PutExtra("bundle", bundle);
            StartActivity(subActivity);
        }

        private void Discover_Discover(object sender, EventArgs e)
        {

            // Update list
            List<String> deviceList = new List<string>();
            foreach (DiscoveryInfo info in discover.dicoveryMap.Values)
            {
                //Toast.MakeText(this, info.DisplayString(), ToastLength.Long).Show();
                deviceList.Add(info.DisplayString());
            }
            ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, Android.Resource.Layout.SimpleListItem1, deviceList);
            listView.Adapter = adapter;
        }
        private void BtnSearch_Click(object sender, EventArgs e)
        {
            tmr.Interval = 100;
            tmr.Elapsed += Tmr_Elapsed;
            tmr.Enabled = true;
            discover.StartDiscovery();
        }

        private void Tmr_Elapsed(object sender, ElapsedEventArgs e)
        {
            RunOnUiThread(() => { Update(); });
        }

        private void Update()
        {
            discover.Update();
        }
    }
}