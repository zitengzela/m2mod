namespace std
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=1), NativeCppClass, DebugInfoInPDB, MiscellaneousBits(0x40)]
    internal struct allocator<char>
    {
        public static unsafe void <MarshalCopy>(allocator<char>*, allocator<char>*)
        {
        }
        [StructLayout(LayoutKind.Sequential, Size=1), DebugInfoInPDB, CLSCompliant(false), NativeCppClass, MiscellaneousBits(0x41)]
        public struct rebind<char>
        {
        }
    }
}

