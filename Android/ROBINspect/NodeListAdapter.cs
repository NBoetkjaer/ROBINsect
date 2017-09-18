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
using ROBINinspect;

namespace ROBINspect
{
    class NodeListAdapter : ArrayAdapter<BaseNode>
    {
        Activity context;
        public NodeListAdapter(Activity context, IList<BaseNode> objects)
            : base(context, Android.Resource.Id.Text1, objects)
        {
            this.context = context;
        }

        public override View GetView(int position, View convertView, ViewGroup parent)
        {
            var view = context.LayoutInflater.Inflate(Android.Resource.Layout.SimpleListItem2, null);
//            view.LongClickable = true;
            var node = GetItem(position);
            // Test
            if(node == null)
            {
                view.FindViewById<TextView>(Android.Resource.Id.Text1).Text = "..";
                view.FindViewById<TextView>(Android.Resource.Id.Text2).Text = String.Empty;
                return view;
            }
            // End test
            String name = node.Name;
            if(node.Children.Count > 0)
            {
                name = "[+]" + name;
            }
            String value = String.Empty;
            if (node.GetAttribute(AttributeTypes.value, ref value))
            {
                value = "value = " + value;
            }
            view.FindViewById<TextView>(Android.Resource.Id.Text1).Text = name;
            view.FindViewById<TextView>(Android.Resource.Id.Text2).Text = value;
            return view;
        }
    }
}