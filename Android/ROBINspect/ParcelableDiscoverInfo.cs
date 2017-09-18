using Android.OS;
using Android.Runtime;

using Java.Interop;

using Object = Java.Lang.Object;
using ROBINinspect;

namespace ROBINspect
{
    public class ParcelableDiscoverInfo : Object, IParcelable
    {
        public DiscoveryInfo info = new DiscoveryInfo("Unknown");
        public ParcelableDiscoverInfo()
        { }
        public ParcelableDiscoverInfo(Parcel src)
        {
            info.ipAddr = src.ReadString();
            info.serialNumber = src.ReadInt();
        }

        public int DescribeContents()
        {
            return 0;
        }
        public void WriteToParcel(Parcel dest, [GeneratedEnum] ParcelableWriteFlags flags)
        {
            dest.WriteString(info.ipAddr);
            dest.WriteInt(info.serialNumber);
        }
        [ExportField("CREATOR")]
        public static ParcelableDiscoverInfoCreator InitializeCreator()
        {
            return new ParcelableDiscoverInfoCreator();
        }
    }

    public class ParcelableDiscoverInfoCreator : Object, IParcelableCreator
    {
        public Object CreateFromParcel(Parcel source)
        {
            return new ParcelableDiscoverInfo(source);
        }

        public Object[] NewArray(int size)
        {
            return new Object[size];
        }
    }
}