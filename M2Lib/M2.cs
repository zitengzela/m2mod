namespace M2Lib
{
    using Microsoft.VisualC;
    using System;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential, Size=0xc54), DebugInfoInPDB, MiscellaneousBits(0x40), NativeCppClass, UnsafeValueType]
    internal struct M2
    {
        [StructLayout(LayoutKind.Sequential, Size=0x40), DebugInfoInPDB, UnsafeValueType, MiscellaneousBits(0x40), CLSCompliant(false), NativeCppClass]
        public struct CElement_Animation
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=20), MiscellaneousBits(0x40), DebugInfoInPDB, UnsafeValueType, NativeCppClass, CLSCompliant(false)]
        public struct CElement_AnimationBlock
        {
            [StructLayout(LayoutKind.Sequential, Size=8), NativeCppClass, CLSCompliant(false), MiscellaneousBits(0x40), DebugInfoInPDB]
            public struct CChannel
            {
            }

            [CLSCompliant(false), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB]
            public enum EInterpolationType : ushort
            {
            }

            [StructLayout(LayoutKind.Sequential, Size=4), MiscellaneousBits(0x41), NativeCppClass, DebugInfoInPDB, UnsafeValueType, CLSCompliant(false)]
            public struct SKey_Float32
            {
            }

            [StructLayout(LayoutKind.Sequential, Size=12), MiscellaneousBits(0x41), UnsafeValueType, NativeCppClass, DebugInfoInPDB, CLSCompliant(false)]
            public struct SKey_Float32x3
            {
            }

            [StructLayout(LayoutKind.Sequential, Size=8), MiscellaneousBits(0x41), DebugInfoInPDB, CLSCompliant(false), UnsafeValueType, NativeCppClass]
            public struct SKey_SInt16x4
            {
            }

            [StructLayout(LayoutKind.Sequential, Size=2), CLSCompliant(false), MiscellaneousBits(0x41), NativeCppClass, UnsafeValueType, DebugInfoInPDB]
            public struct SKey_UInt16
            {
            }

            [StructLayout(LayoutKind.Sequential, Size=4), NativeCppClass, DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x41), UnsafeValueType]
            public struct SKey_UInt32
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=2), NativeCppClass, DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40)]
        public struct CElement_AnimationLookup
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=40), UnsafeValueType, MiscellaneousBits(0x40), CLSCompliant(false), NativeCppClass, DebugInfoInPDB]
        public struct CElement_Attachment
        {
            [MiscellaneousBits(0x40), CLSCompliant(false), NativeCppClass, DebugInfoInPDB]
            public enum EID : uint
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=4), DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass]
        public struct CElement_AttachmentLookup
        {
            [NativeCppClass, MiscellaneousBits(0x40), CLSCompliant(false), DebugInfoInPDB]
            public enum EAttachmentLookup
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=0x58), DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass, UnsafeValueType]
        public struct CElement_Bone
        {
            [CLSCompliant(false), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB]
            public enum EFlags : uint
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=2), DebugInfoInPDB, NativeCppClass, CLSCompliant(false), MiscellaneousBits(0x40)]
        public struct CElement_BoneLookup
        {
            [DebugInfoInPDB, CLSCompliant(false), NativeCppClass, MiscellaneousBits(0x40)]
            public enum EBoneLookup : short
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=12), NativeCppClass, CLSCompliant(false), DebugInfoInPDB, UnsafeValueType, MiscellaneousBits(0x40)]
        public struct CElement_BoundingNormals
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=6), CLSCompliant(false), DebugInfoInPDB, MiscellaneousBits(0x40), NativeCppClass, UnsafeValueType]
        public struct CElement_BoundingTriangle
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=12), UnsafeValueType, DebugInfoInPDB, NativeCppClass, CLSCompliant(false), MiscellaneousBits(0x40)]
        public struct CElement_BoundingVertices
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x74), NativeCppClass, UnsafeValueType, CLSCompliant(false), DebugInfoInPDB, MiscellaneousBits(0x40)]
        public struct CElement_Camera
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=2), NativeCppClass, MiscellaneousBits(0x40), CLSCompliant(false), DebugInfoInPDB]
        public struct CElement_CameraLookup
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=40), MiscellaneousBits(0x40), NativeCppClass, UnsafeValueType, CLSCompliant(false), DebugInfoInPDB]
        public struct CElement_Color
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x24), MiscellaneousBits(0x40), NativeCppClass, UnsafeValueType, DebugInfoInPDB, CLSCompliant(false)]
        public struct CElement_Event
        {
            [StructLayout(LayoutKind.Sequential, Size=8), NativeCppClass, MiscellaneousBits(0x40), CLSCompliant(false), DebugInfoInPDB]
            public struct CTimeLine
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=4), CLSCompliant(false), NativeCppClass, DebugInfoInPDB, MiscellaneousBits(0x40)]
        public struct CElement_GlobalSequence
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x9c), DebugInfoInPDB, CLSCompliant(false), UnsafeValueType, MiscellaneousBits(0x40), NativeCppClass]
        public struct CElement_Light
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=4), MiscellaneousBits(0x40), DebugInfoInPDB, NativeCppClass, CLSCompliant(false)]
        public struct CElement_Name
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x1a0), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB, UnsafeValueType, CLSCompliant(false)]
        public struct CElement_ParticleEmitter
        {
            [StructLayout(LayoutKind.Sequential, Size=0x90), MiscellaneousBits(0x41), DebugInfoInPDB, CLSCompliant(false), UnsafeValueType, NativeCppClass]
            public struct <unnamed-type-Attributes>
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=2), NativeCppClass, MiscellaneousBits(0x40), CLSCompliant(false), DebugInfoInPDB]
        public struct CElement_PartitionedBoneLookup
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0xac), DebugInfoInPDB, NativeCppClass, UnsafeValueType, MiscellaneousBits(0x40), CLSCompliant(false)]
        public struct CElement_RibbonEmitter
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x10), UnsafeValueType, NativeCppClass, CLSCompliant(false), MiscellaneousBits(0x40), DebugInfoInPDB]
        public struct CElement_Texture
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=6), CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB]
        public struct CElement_TextureFlag
        {
            [CLSCompliant(false), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB]
            public enum EBlend
            {
            }

            [NativeCppClass, DebugInfoInPDB, MiscellaneousBits(0x40), CLSCompliant(false)]
            public enum EFlags : ushort
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=2), NativeCppClass, DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40)]
        public struct CElement_TextureLookup
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=2), NativeCppClass, MiscellaneousBits(0x40), CLSCompliant(false), DebugInfoInPDB]
        public struct CElement_TextureReplace
        {
            [NativeCppClass, DebugInfoInPDB, CLSCompliant(false), MiscellaneousBits(0x40)]
            public enum ETexture : short
            {
            }
        }

        [StructLayout(LayoutKind.Sequential, Size=2), NativeCppClass, MiscellaneousBits(0x40), DebugInfoInPDB, CLSCompliant(false)]
        public struct CElement_TextureUnits
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=20), CLSCompliant(false), DebugInfoInPDB, NativeCppClass, MiscellaneousBits(0x40), UnsafeValueType]
        public struct CElement_Transparency
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=2), NativeCppClass, DebugInfoInPDB, MiscellaneousBits(0x40), CLSCompliant(false)]
        public struct CElement_TransparencyLookup
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=60), DebugInfoInPDB, MiscellaneousBits(0x40), UnsafeValueType, NativeCppClass, CLSCompliant(false)]
        public struct CElement_UVAnimation
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=2), MiscellaneousBits(0x40), NativeCppClass, DebugInfoInPDB, CLSCompliant(false)]
        public struct CElement_UVAnimationLookup
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x30), UnsafeValueType, MiscellaneousBits(0x40), CLSCompliant(false), NativeCppClass, DebugInfoInPDB]
        public struct CElement_Vertex
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=0x138), UnsafeValueType, MiscellaneousBits(0x40), DebugInfoInPDB, NativeCppClass, CLSCompliant(false)]
        public struct CM2Header
        {
            [StructLayout(LayoutKind.Sequential, Size=20), NativeCppClass, MiscellaneousBits(0x41), CLSCompliant(false), DebugInfoInPDB, UnsafeValueType]
            public struct <unnamed-type-Description>
            {
            }

            [StructLayout(LayoutKind.Sequential, Size=0x124), NativeCppClass, DebugInfoInPDB, UnsafeValueType, MiscellaneousBits(0x41), CLSCompliant(false)]
            public struct <unnamed-type-Elements>
            {
            }
        }

        [CLSCompliant(false), NativeCppClass, DebugInfoInPDB, MiscellaneousBits(0x40)]
        public enum EElement
        {
        }

        [StructLayout(LayoutKind.Sequential, Size=8), CLSCompliant(false), DebugInfoInPDB, NativeCppClass, MiscellaneousBits(0x41)]
        public struct SExternalAnimation
        {
        }
    }
}

