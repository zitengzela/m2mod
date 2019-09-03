using Microsoft.Win32;

namespace M2Mod.Registry
{
    public static class RegistryStore
    {
        private static readonly RegistryKey _root = null;

        static RegistryStore()
        {
            _root = Microsoft.Win32.Registry.CurrentUser.CreateSubKey("M2Mod");
        }

        public static object GetValue(RegistryValue key)
        {
            return _root.GetValue(key.ToString());
        }

        public static void SetValue(RegistryValue Key, object value)
        {
            _root.SetValue(Key.ToString(), value);
        }
    }
}