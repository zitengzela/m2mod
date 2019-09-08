using System;
using M2Mod.Interop.Structures;
using Newtonsoft.Json;

namespace M2Mod.Config
{
    public class SettingsProfile
    {
        public readonly Guid Id;
        public string Name;
        public Settings Settings;
        public FormData FormData;

        public SettingsProfile(string name, Settings settings, FormData formData)
        {
            Id = Guid.NewGuid();
            Name = name;
            Settings = settings;
            FormData = formData;
        }

        public override string ToString()
        {
            return Name;
        }
    }
}
