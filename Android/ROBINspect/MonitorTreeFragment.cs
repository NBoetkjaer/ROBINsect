using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Util;
using Android.Views;
using Android.Widget;
using ROBINinspect;

namespace ROBINspect
{
    public class MonitorTreeFragment : Fragment
    {
        MonitorActivity monitorAct;

        public override void OnActivityCreated(Bundle savedInstanceState)
        {
            base.OnActivityCreated(savedInstanceState);
            // Create fragment here

            monitorAct = Activity as MonitorActivity;
            if (monitorAct == null) return;

            ListView list = View.FindViewById<ListView>(Resource.Id.listView1);
            list.ItemClick += List_ItemClick;
            list.ItemLongClick += List_ItemLongClick;
        }

        public override void OnStart()
        {
            base.OnStart();
            monitorAct.Changed += MonitorAct_Changed;
            monitorAct.activeFragment = this;
        }
        public override void OnStop()
        {
            base.OnStop();
            monitorAct.Changed -= MonitorAct_Changed;
        }

        private void MonitorAct_Changed(object sender, EventArgs e)
        {
            UpdateListView(monitorAct.currentNode);
        }

        public override View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
        {
            // Use this to return your custom view for this Fragment
            View fragmentView = inflater.Inflate(Resource.Layout.MonitorTree, container, false);
//            ListView list = fragmentView.RootView.FindViewById<ListView>(Resource.Id.listView1);
//            list.ItemClick += List_ItemClick;
            return fragmentView;

            //return base.OnCreateView(inflater, container, savedInstanceState);
        }

        private void List_ItemClick(object sender, AdapterView.ItemClickEventArgs e)
        {
            var listView = sender as ListView;
            if (listView == null) return;
            NodeListAdapter listAdapter = listView.Adapter as NodeListAdapter;
            if (listAdapter != null)
            {
                var node = listAdapter.GetItem(e.Position);
                BaseNode currentNode = monitorAct.currentNode;
                if (node == null && currentNode != null && currentNode.ParentNode != null)
                {
                    currentNode = currentNode.ParentNode;
                }
                if (node != null)
                {
                    currentNode = node;
                }
                monitorAct.currentNode = currentNode;
                if(currentNode.Children.Count == 0)
                {
                    ShowDetailFragment();
                }
                else
                {
                    UpdateListView(currentNode);
                }
            }
        }

        void UpdateListView(BaseNode node)
        {
            if (node == null) return;

            TextView txtPath = monitorAct.FindViewById<TextView>(Resource.Id.textViewFullPath);
            txtPath.Text = node.GetFullPath();

            ListView listView = monitorAct.FindViewById<ListView>(Resource.Id.listView1);
            if (listView.Adapter == null)
            {
                var items = new List<BaseNode>();
                listView.Adapter = new NodeListAdapter(monitorAct, items);
            }
            NodeListAdapter listAdapter = listView.Adapter as NodeListAdapter;
            if (listAdapter != null && node != null)
            {
                listAdapter.Clear();
                if (node.ParentNode != null)
                {
                    listAdapter.Add(null);
                }
                foreach (BaseNode child in node.Children)
                {
                    listAdapter.Add(child);
                }
            }
        }
        private void List_ItemLongClick(object sender, AdapterView.ItemLongClickEventArgs e)
        {
            var listView = sender as ListView;
            if (listView == null) return;
            // Select the clicked node.
            List_ItemClick(sender, new AdapterView.ItemClickEventArgs(e.Parent, e.View, e.Position, e.Id));
            ShowDetailFragment();
        }

        private void ShowDetailFragment()
        {
            if (monitorAct.currentNode == null) return;
            // Start the detail fragment.
            MonitorDetailFragment myFrag = new MonitorDetailFragment();
            FragmentTransaction fragmentTransaction = FragmentManager.BeginTransaction();
            fragmentTransaction.Replace(Resource.Id.fragment_container, myFrag);
            fragmentTransaction.AddToBackStack(null);
            fragmentTransaction.Commit();

        }
    }
}