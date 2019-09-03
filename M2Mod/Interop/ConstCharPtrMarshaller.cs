using System;
using System.Runtime.InteropServices;

namespace M2Mod.Interop
{
    class ConstCharPtrMarshaller : ICustomMarshaler
    {
        public object MarshalNativeToManaged(IntPtr pNativeData)
        {
            return Marshal.PtrToStringAnsi(pNativeData);
        }

        public IntPtr MarshalManagedToNative(object managedObj)
        {
            return IntPtr.Zero;
        }

        public void CleanUpNativeData(IntPtr pNativeData)
        {
        }

        public void CleanUpManagedData(object managedObj)
        {
        }

        public int GetNativeDataSize()
        {
            return IntPtr.Size;
        }

        static readonly ConstCharPtrMarshaller instance = new ConstCharPtrMarshaller();

        public static ICustomMarshaler GetInstance(string cookie)
        {
            return instance;
        }
    }
}
