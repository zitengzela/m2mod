using System;
using M2Mod.Interop.Structures;

namespace M2Mod.Config
{
    public class SettingsProfile : ICloneable
    {
        public Guid Id;
        public string Name;
        public Settings Settings;
        public Configuration Configuration = new Configuration();

        // needed for proper json deserialization
        public SettingsProfile()
        {
        }

        public SettingsProfile(string name, Settings settings, Configuration configuration)
        {
            Id = Guid.NewGuid();
            Name = name;
            Settings = settings;
            Configuration = configuration;
        }

        public void SetName(string name)
        {
            Name = name;
        }

        public override string ToString()
        {
            return Name;
        }

        public object Clone()
        {
            return new SettingsProfile(Name, Settings, Configuration.Clone() as Configuration);
        }
    }
}
