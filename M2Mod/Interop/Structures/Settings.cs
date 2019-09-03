using System.Runtime.InteropServices;

namespace M2Mod.Interop.Structures
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Settings
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string OutputDirectory;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string WorkingDirectory;

        public Expansion ForceLoadExpansion;
        [MarshalAs(UnmanagedType.U1)] public bool MergeBones;
        [MarshalAs(UnmanagedType.U1)] public bool MergeAttachments;
        [MarshalAs(UnmanagedType.U1)] public bool MergeCameras;
        [MarshalAs(UnmanagedType.U1)] public bool FixSeams;
        [MarshalAs(UnmanagedType.U1)] public bool FixEdgeNormals;
        [MarshalAs(UnmanagedType.U1)] public bool IgnoreOriginalMeshIndexes;
        [MarshalAs(UnmanagedType.U1)] public bool FixAnimationsTest;
    }
}
