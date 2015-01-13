namespace std
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=1), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB]
    internal struct allocator<wchar_t>
    {
        public static unsafe void <MarshalCopy>(allocator<wchar_t>*, allocator<wchar_t>*)
        {
        }
        [StructLayout(LayoutKind.Sequential, Size=1), CLSCompliant(false), MiscellaneousBits(0x41), NativeCppClass, DebugInfoInPDB]
        public struct rebind<wchar_t>
        {
        }
    }
}

