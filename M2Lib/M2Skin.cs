namespace M2Lib
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=0x8e0), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB, UnsafeValueType]
    internal struct M2Skin
    {
        [StructLayout(LayoutKind.Sequential, Size=4), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB, CLSCompliant(false)]
        public struct CElement_BoneIndices
        {
            [StructLayout(LayoutKind.Explicit, Size=4), DebugInfoInPDB, NativeCppClass, MiscellaneousBits(0x42), UnsafeValueType]
            internal struct $UnnamedClass$0x7ece1f2c$25$
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=12), DebugInfoInPDB, MiscellaneousBits(0x40), NativeCppClass, CLSCompliant(false), UnsafeValueType]
        public struct CElement_Flags
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=4), NativeCppClass, DebugInfoInPDB, MiscellaneousBits(0x40), CLSCompliant(false)]
        public struct CElement_Index
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x18), UnsafeValueType, NativeCppClass, CLSCompliant(false), MiscellaneousBits(0x40), DebugInfoInPDB]
        public struct CElement_Material
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x30), UnsafeValueType, CLSCompliant(false), NativeCppClass, DebugInfoInPDB, MiscellaneousBits(0x40)]
        public struct CElement_SubMesh
        {
            [DebugInfoInPDB, MiscellaneousBits(0x40), NativeCppClass, CLSCompliant(false)]
            public enum ECategory
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=2), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB, CLSCompliant(false)]
        public struct CElement_Vertex
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=60), UnsafeValueType, DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass]
        public struct CM2SkinHeader
        {
        }

        [DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass]
        public enum EElement
        {
        }
    }
}

