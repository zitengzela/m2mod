using M2Mod.Interop.Structures;

namespace M2Mod
{
    public static class Defaults
    {
        public static Settings Settings => new Settings()
        {
            ForceLoadExpansion = Expansion.None,
            MergeBones = true,
            MergeAttachments = true,
            MergeCameras = true,
            FixSeams = false,
            FixEdgeNormals = true,
            IgnoreOriginalMeshIndexes = false,
            FixAnimationsTest = false,
        };
    };
}
