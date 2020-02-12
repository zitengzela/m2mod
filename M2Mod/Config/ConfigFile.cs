using System;
using System.Collections.Generic;

namespace M2Mod.Config
{
    public class ConfigFile
    {
        public Guid ActiveProfile = Guid.Empty;
        public List<SettingsProfile> Profiles = new List<SettingsProfile>();
    }
}
