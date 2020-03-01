using System;
using System.Collections.Generic;
using System.Linq;
using M2Mod.Controls;

namespace M2Mod.Config.Normalization
{
    public class AdvancedConfig : ICloneable
    {
        public List<NormalizationRule> Rules = new List<NormalizationRule>();

        public object Clone()
        {
            return new AdvancedConfig()
            {
                Rules = Rules.Clone().ToList()
            };
        }
    }
}
