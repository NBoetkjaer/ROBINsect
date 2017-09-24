using System;

using Android.App;
using Android.OS;
using Android.Views;
using Android.Widget;
using ROBINinspect;

namespace ROBINspect
{
    public class EditAttributeDialogFragment : DialogFragment
    {
        MonitorActivity monitorAct;
        private BaseNode node;
        private AttributeTypes attr;
        bool setNewValue = false;
        public EditAttributeDialogFragment(BaseNode node, AttributeTypes attr)
        {
            this.node = node;
            this.attr = attr;
        }
        public override void OnActivityCreated(Bundle savedInstanceState)
        {
            base.OnActivityCreated(savedInstanceState);
            // Create fragment here

            monitorAct = Activity as MonitorActivity;
            if (monitorAct == null) return;
        }

        public override View OnCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
        {
            // Use this to return your custom view for this Fragment
            View view = inflater.Inflate(Resource.Layout.EditAttributeDialog, container, false);
            Dialog.SetTitle("Edit Attribute");
            TextView txt = view.FindViewById<TextView>(Resource.Id.textViewAttributeName);
            if(txt != null)
            {
                txt.Text = Attributes.Name(attr);
            }

            EditText edit = view.FindViewById<EditText>(Resource.Id.editTextAttribValue);
            if (edit != null && node != null)
            {
                String strVal = String.Empty;
                if (node.GetAttribute(attr, ref strVal))
                {
                    edit.Text = strVal;
                }
                if(attr == AttributeTypes.value && (node is AbstractNumericNode))
                {
                    edit.InputType = Android.Text.InputTypes.NumberFlagDecimal | Android.Text.InputTypes.ClassNumber;
                }
            }

            Button btn = view.FindViewById<Button>(Resource.Id.btnSetValue);
            if(btn != null)
            {
                btn.Click += delegate
                {
                    setNewValue = true;
                };
            }

            btn = view.FindViewById<Button>(Resource.Id.btnClose);
            if (btn != null)
            {
                btn.Click += delegate
                {
                    Dismiss();
                };
            }


            return view;
        }

        public override void OnStart()
        {
            base.OnStart();
            //monitorAct.Changed += MonitorAct_Changed;
            monitorAct.AddData += MonitorAct_AddData;
            monitorAct.activeFragment = this;
        }

        public override void OnStop()
        {
            base.OnStop();
            //monitorAct.Changed -= MonitorAct_Changed;
            monitorAct.AddData -= MonitorAct_AddData;
        }

        private void MonitorAct_AddData(object sender, EventArgs e)
        {
            if (setNewValue)
            {
                EditText edit = View.FindViewById<EditText>(Resource.Id.editTextAttribValue);
                if (edit != null && node != null)
                {
                    node.SetAttribute(attr, edit.Text);
                }
                setNewValue = false;
            }
        }
    }
}