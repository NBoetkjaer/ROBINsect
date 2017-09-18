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
    class AttributeListItemAdapter : ArrayAdapter<AttributeTypes>
    {
        Activity context;
        BaseNode node;
        public AttributeListItemAdapter(Activity context, IList<AttributeTypes> objects, BaseNode node)
            : base(context, Android.Resource.Id.Text1, objects)
        {
            this.context = context;
            this.node = node;
        }

        public override View GetView(int position, View convertView, ViewGroup parent)
        {
            //var view = context.LayoutInflater.Inflate(Android.Resource.Layout.SimpleListItem2, null);
            var view = context.LayoutInflater.Inflate(Resource.Layout.AttributeListItem, null);
            var attr = GetItem(position);

            view.FindViewById<TextView>(Resource.Id.Text1).Text = Attributes.Name(attr);
            String attribVal = String.Empty;
            node.GetAttribute(attr, ref attribVal);
            view.FindViewById<TextView>(Resource.Id.Text2).Text = attribVal;
            return view;
        }
    }
}