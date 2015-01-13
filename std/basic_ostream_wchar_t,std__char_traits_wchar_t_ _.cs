namespace std
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=80), DebugInfoInPDB, NativeCppClass, MiscellaneousBits(0x40)]
    internal struct basic_ostream<wchar_t,std::char_traits<wchar_t> >
    {
        [StructLayout(LayoutKind.Sequential, Size=4), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB, CLSCompliant(false)]
        public struct _Sentry_base
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=8), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB, CLSCompliant(false)]
        public struct sentry
        {
        }
    }
}

