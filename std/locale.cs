namespace std
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=4), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB]
    internal struct locale
    {
        [StructLayout(LayoutKind.Sequential, Size=0x20), CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB]
        public struct _Locimp
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=8), NativeCppClass, DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40)]
        public struct facet
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=4), CLSCompliant(false), DebugInfoInPDB, MiscellaneousBits(0x40), NativeCppClass]
        public struct id
        {
        }
    }
}

