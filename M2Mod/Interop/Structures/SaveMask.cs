using System;

namespace M2Mod.Interop.Structures
{
    [Flags]
    enum SaveMask : byte
    {
        None = 0,
        M2 = 1,
        Skin = 2,
        Skeleton = 4,
        Other = 8,
        All = M2 | Skin | Skeleton | Other,
    }
}
