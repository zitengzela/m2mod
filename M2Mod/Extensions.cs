using System;
using System.Collections.Generic;
using System.Linq;

namespace M2Mod
{
    public static class Extensions
    {
        public static IEnumerable<T> Clone<T>(this IEnumerable<T> source) where T : ICloneable
        {
            return source.Select(_ => (T) _.Clone());
        }
    }
}
