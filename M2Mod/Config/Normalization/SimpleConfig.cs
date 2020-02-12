using System.Collections.Generic;
using System.Linq;
using M2Mod.Controls;

namespace M2Mod.Config.Normalization
{
    public class SimpleConfig
    {
        public bool AlignFacials;
        public bool AlignHair;

        public string Data = "";

        public static IEnumerable<NormalizationRule> ParseData(string data)
        {
            var tmp = data
                .Split('\r', '\n');
            var tmp2 = tmp.Select(_ =>
            {
                if (_.IndexOf('#') != -1)
                    _ = _.Substring(0, _.IndexOf('#'));

                return _.Trim();
            });
            var tmp3 = tmp2.Where(_ => !string.IsNullOrWhiteSpace(_));
            var tmp4 = tmp3.Select(_ => new NormalizationRule()
            {
                SourceType = NormalizeRuleType.Body,
                TargetType = NormalizeRuleType.Custom,
                TargetRules = new RuleList(_),
                PreferSourceDirection = true,
            });

            return tmp4;
        }
    }
}
