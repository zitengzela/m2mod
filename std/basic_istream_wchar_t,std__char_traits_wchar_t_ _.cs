namespace std
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=0x58), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB]
    internal struct basic_istream<wchar_t,std::char_traits<wchar_t> >
    {
        [StructLayout(LayoutKind.Sequential, Size=4), DebugInfoInPDB, CLSCompliant(false), NativeCppClass, MiscellaneousBits(0x40)]
        public struct _Sentry_base
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=8), NativeCppClass, CLSCompliant(false), MiscellaneousBits(0x40), DebugInfoInPDB]
        public struct sentry
        {
        }
    }
}

