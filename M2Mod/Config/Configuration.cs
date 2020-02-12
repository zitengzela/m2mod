using System.Collections.Generic;

namespace M2Mod.Config
{
    public class Configuration
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
    }
}
