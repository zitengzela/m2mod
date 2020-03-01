using System;
using System.Collections.Generic;

namespace M2Mod.Config
{
    public class Configuration : ICloneable
    {
        public string InputM2Exp;
        public string OutputM2I;
        public string InputM2Imp;
        public string InputM2I;
        public string ReplaceM2;
        public bool ReplaceM2Checked;
        public string OldCompareM2;
        public string NewCompareM2;
        public float CompareWeightThreshold;
        public float CompareSourceScale;
        public bool PredictScale;
        public NormalizationConfig NormalizationConfig = new NormalizationConfig();

        public Configuration()
        {
        }

        public object Clone()
        {
            return new Configuration()
            {
                InputM2Exp = InputM2Exp,
                OutputM2I = OutputM2I,
                InputM2Imp = InputM2Imp,
                InputM2I = InputM2I,
                ReplaceM2 = ReplaceM2,
                ReplaceM2Checked = ReplaceM2Checked,
                OldCompareM2 = OldCompareM2,
                NewCompareM2 = NewCompareM2,
                CompareWeightThreshold = CompareWeightThreshold,
                CompareSourceScale = CompareSourceScale,
                PredictScale = PredictScale,
                NormalizationConfig = NormalizationConfig.Clone() as NormalizationConfig
            };
        }
    }
}
