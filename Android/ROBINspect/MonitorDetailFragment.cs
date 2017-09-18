using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;

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
    public class MonitorDetailFragment : Fragment
    {
        MonitorActivity monitorAct;
        bool integerNode;
        string strNewValue;
        float seekerOffset;
        float seekerScale;
        const int seekerMax = 1000000;
        public override void OnActivityCreated(Bundle savedInstanceState)
        {
            base.OnActivityCreated(savedInstanceState);
            // Create fragment here

            monitorAct = Activity as MonitorActivity;
            if (monitorAct == null) return;
        }

        private void Seeker_ProgressChanged(object sender, SeekBar.ProgressChangedEventArgs e)
        {            
            strNewValue = (e.Progress * seekerScale + seekerOffset).ToString();
        }

        public override View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
        {
            // Use this to return your custom view for this Fragment
            return inflater.Inflate(Resource.Layout.MonitorDetail, container, false);
            //return base.OnCreateView(inflater, container, savedInstanceState);
        }
        private bool GetRange(BaseNode node, out float minVal, out float maxVal)
        {
            minVal = float.NaN;
            maxVal = float.NaN;
            if (!node.IsAttributeUsed(AttributeTypes.range)) return false;
            if(node is FloatNode)
            {
                float min, max;
                (node as FloatNode).GetRange(out min, out max);
                minVal = min; 
                maxVal = max;
                return true;
            }
            if (node is DoubleNode)
            {
                double min, max;
                (node as DoubleNode).GetRange(out min, out max);
                minVal = (float)min;
                maxVal = (float)max;
                return true;
            }
            if (node is UInt16Node)
            {
                UInt16 min, max;
                (node as UInt16Node).GetRange(out min, out max);
                minVal = (float)min;
                maxVal = (float)max;
                return true;
            }

            return false;
        }
        private void UpdateView(BaseNode node)
        {
            if (node== null) return;

            UpdateListView(node);

            LinearLayout valueGroup = monitorAct.FindViewById<LinearLayout>(Resource.Id.valueGroup);
            valueGroup.Visibility = ViewStates.Invisible;

            SeekBar seeker = monitorAct.FindViewById<SeekBar>(Resource.Id.seekBarValue);
            seeker.Visibility = ViewStates.Invisible;

            // Set the node name.
            TextView txtView = monitorAct.FindViewById<TextView>(Resource.Id.textViewNodeName);
            txtView.Text = monitorAct.currentNode.Name;
            NodeTypes nType = NodeFactory.NodeType(node);
            txtView.Text += " (" + NodeFactory.TypeAsString(nType) + ")";

            // Setup for a value attribute.
            if (node is AbstractValueNode)
            {
                valueGroup.Visibility = ViewStates.Visible;
                if (node is AbstractNumericNode)
                {
                    if (node is FloatNode || node is DoubleNode)
                    {
                        integerNode = false;
                    }
                    float min, max;
                    if(GetRange(node, out min, out max))
                    {
                        float nodeValue = (max - min) / 2f;
                        string strValue = String.Empty;
                        if (node.GetAttribute(AttributeTypes.value, ref strValue))
                        {
                            nodeValue = float.Parse(strValue);
                        }
                        if (integerNode)
                        {
                            seeker.Max = seekerMax;
                            seekerOffset = min;
                            seekerScale = (max - min) / seekerMax;
                        }
                        else
                        {
                            seeker.Max = (int)Math.Round(max - min);
                            seekerOffset = min;
                            seekerScale = 1.0f;
                        }
                        seeker.Visibility = ViewStates.Visible;
                        seeker.Progress = (int)Math.Round((nodeValue - seekerOffset) / seekerScale);
                        seeker.ProgressChanged += Seeker_ProgressChanged;
                    }
                }
            }
        }
        public override void OnStart()
        {
            base.OnStart();
            UpdateView(monitorAct.currentNode);
            monitorAct.Changed += MonitorAct_Changed;
            monitorAct.AddData += MonitorAct_AddData;
            monitorAct.activeFragment = this;
        }

        public override void OnStop()
        {
            base.OnStop();
            monitorAct.currentNode = monitorAct.currentNode.ParentNode;
            monitorAct.Changed -= MonitorAct_Changed;
            monitorAct.AddData -= MonitorAct_AddData;
        }

        private void MonitorAct_Changed(object sender, EventArgs e)
        {
            if (!monitorAct.currentNode.AnyChanges()) return;
            
            String strVal = String.Empty;
            monitorAct.currentNode.GetAttribute(AttributeTypes.value, ref strVal);

            TextView txt = monitorAct.FindViewById<TextView>(Resource.Id.textViewValue);
            txt.Text = "value " + strVal;

            UpdateListView(monitorAct.currentNode);
        }

        private void MonitorAct_AddData(object sender, EventArgs e)
        {
            if (String.IsNullOrEmpty(strNewValue)) return;
            monitorAct.currentNode.SetAttribute(AttributeTypes.value, strNewValue);
        }

        void UpdateListView(BaseNode node)
        {
            if (node == null) return;

            ListView listView = monitorAct.FindViewById<ListView>(Resource.Id.listView1);
            if (listView.Adapter == null)
            {
                listView.ItemClick += ListView_ItemClick;
                var items = new List<AttributeTypes>();
                foreach (var attr in Attributes.ValidAttributes)
                {
                    if (!node.IsAttributeUsed(attr))
                        continue;
                    items.Add(attr);
                 }
                listView.Adapter = new AttributeListItemAdapter(monitorAct, items, node);
            }

            AttributeListItemAdapter listAdapter = listView.Adapter as AttributeListItemAdapter;
            if (listAdapter != null)
            {
                listAdapter.NotifyDataSetChanged();
            }
        }

        private void ListView_ItemClick(object sender, AdapterView.ItemClickEventArgs e)
        {
            var listView = sender as ListView;
            if (listView == null) return;
            // Select the clicked attribute.

            AttributeListItemAdapter listAdapter = listView.Adapter as AttributeListItemAdapter;
            if (listAdapter != null)
            {
                AttributeTypes attr = listAdapter.GetItem(e.Position);
                showDialog(monitorAct.currentNode, attr);
            }
        }

        void showDialog(BaseNode node, AttributeTypes attr)
        {
            // DialogFragment.show() will take care of adding the fragment
            // in a transaction.  We also want to remove any currently showing
            // dialog, so make our own transaction and take care of that here.
            FragmentTransaction ft = FragmentManager.BeginTransaction();
            Fragment prev = FragmentManager.FindFragmentByTag("dialog");
            if (prev != null)
            {
                ft.Remove(prev);
            }
            ft.AddToBackStack(null);

            // Create and show the dialog.
            DialogFragment newFragment = new EditAttributeDialogFragment(node, attr);
            newFragment.Show(ft, "dialog");
        }
    }
}