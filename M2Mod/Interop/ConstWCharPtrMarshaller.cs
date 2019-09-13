using System;
using System.Runtime.InteropServices;

namespace M2Mod.Interop
{
    class ConstWCharPtrMarshaller : ICustomMarshaler
    {
        public object MarshalNativeToManaged(IntPtr pNativeData)
        {
            return Marshal.PtrToStringUni(pNativeData);
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

        static readonly ConstWCharPtrMarshaller instance = new ConstWCharPtrMarshaller();

        public static ICustomMarshaler GetInstance(string cookie)
        {
            return instance;
        }
    }
}
