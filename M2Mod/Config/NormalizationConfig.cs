using System.Collections.Generic;
using System.Linq;
using M2Mod.Config.Normalization;
using M2Mod.Controls;

namespace M2Mod.Config
{
    public class NormalizationConfig
    {
        public bool UseAdvanced;
        public SimpleConfig Simple = new SimpleConfig();
        public AdvancedConfig Advanced = new AdvancedConfig();

        public IEnumerable<NormalizationRule> GetRules()
        {
            if (UseAdvanced)
                return Advanced.Rules.Where(_ => _.Enabled);

            var rules = new List<NormalizationRule>()
            {
                new NormalizationRule()
                {
                    SourceType = NormalizeRuleType.Body,
                    TargetType = NormalizeRuleType.Body,
                    PreferSourceDirection = false
                }
            };

            if (Simple.AlignFacials)
            {
                rules.Add(new NormalizationRule()
                {
                    SourceType = NormalizeRuleType.Body,
                    TargetType = NormalizeRuleType.Facial,
                    PreferSourceDirection = true
                });
            }

            if (Simple.AlignHair)
            {
                rules.Add(new NormalizationRule()
                {
                    SourceType = NormalizeRuleType.Body,
                    TargetType = NormalizeRuleType.Hair,
                    PreferSourceDirection = true
                });
            }

            rules.AddRange(SimpleConfig.ParseData(Simple.Data));

            return rules;
        }
    }
}
