namespace std
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=1), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB]
    internal struct allocator<unsigned short>
    {
        [StructLayout(LayoutKind.Sequential, Size=1), DebugInfoInPDB, NativeCppClass, CLSCompliant(false), MiscellaneousBits(0x41)]
        public struct rebind<unsigned short>
        {
        }
    }
}

