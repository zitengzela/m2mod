
using System;

namespace M2Mod.Interop.Structures
{
    [Flags]
    public enum LogLevel : byte
    {
        Info = 1,
        Warning = 2,
        Error = 4,
        Custom = 8,
        AllDefault = Info| Warning | Error,
    };
}
