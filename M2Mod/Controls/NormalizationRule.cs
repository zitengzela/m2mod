using System;
using System.Collections.Generic;
using System.Linq;

namespace M2Mod.Controls
{
    public static class SpecialFlags
    {
        public static byte AnyDigit = 0x10;
    }

    public class RuleList : ICloneable
    {
        public List<string> Rules { get; private set; } = new List<string>();

        public RuleList()
        {
        }

        public RuleList(List<string> rules)
        {
            Rules = rules;
        }

        public RuleList(string rules)
        {
            Load(rules);
        }

        public void Load(string rules)
        {
            Rules = rules.Trim().ToLower().Split(',', ' ').Where(_ => !string.IsNullOrWhiteSpace(_)).ToList();
        }

        public List<uint> Serialize()
        {
            return Rules.Select(Serialize).ToList();
        }

        private static uint Serialize(string rule)
        {
            rule = rule.Trim().ToLower();
            if (rule.Length > 4)
                throw new Exception($"Wrong entry format: '{rule}'");

            var res = 0u;

            for (var i = 0; i < rule.Length; ++i)
            {
                var c = rule[rule.Length - i - 1];

                if (char.IsDigit(c) && uint.TryParse(c.ToString(), out var charVal))
                    res |= charVal << (i * 8);
                else if (rule[i] == 'x')
                    res |= (uint)SpecialFlags.AnyDigit << (i * 8);
                else
                    throw new Exception($"Can't parse '{rule}'");
            }

            return res;
        }

        public object Clone()
        {
            return new RuleList()
            {
                Rules = new List<string>(Rules)
            };
        }
    }

    public class NormalizationRule : ICloneable
    {
        public NormalizeRuleType SourceType = NormalizeRuleType.Body;
        public RuleList SourceRules = new RuleList();
        public NormalizeRuleType TargetType = NormalizeRuleType.Body;
        public RuleList TargetRules = new RuleList();
        public bool PreferSourceDirection;
        public bool Enabled = true;

        public static byte GetDigit(uint rule, int index)
        {
            if (index < 0 || index > 3)
                throw new Exception($"Bad index {index}");

            return (byte) ((rule >> index) & 0xFF);
        }

        public static bool IsAnyDigit(uint rule, int index)
        {
            try
            {
                var digit = GetDigit(rule, index);

                return (digit & SpecialFlags.AnyDigit) != 0;
            }
            catch
            {
                return false;
            }
        }

        public object Clone()
        {
            return new NormalizationRule()
            {
                SourceType = SourceType,
                SourceRules = SourceRules.Clone() as RuleList,
                TargetType = TargetType,
                TargetRules = TargetRules.Clone() as RuleList,
                PreferSourceDirection = PreferSourceDirection,
                Enabled = Enabled
            };
        }
    }
}