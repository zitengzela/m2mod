namespace std
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=0x38), MiscellaneousBits(0x40), DebugInfoInPDB, NativeCppClass]
    internal struct ios_base
    {
        [StructLayout(LayoutKind.Sequential, Size=12), MiscellaneousBits(0x41), DebugInfoInPDB, NativeCppClass]
        internal struct _Fnarray
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x10), MiscellaneousBits(0x41), NativeCppClass, DebugInfoInPDB]
        internal struct _Iosarray
        {
        }

        [CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB]
        public enum @event
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=20), DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass]
        public struct failure
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=1), DebugInfoInPDB, NativeCppClass, MiscellaneousBits(0x40), CLSCompliant(false)]
        public struct Init
        {
        }
    }
}

