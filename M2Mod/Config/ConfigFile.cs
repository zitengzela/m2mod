using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace M2Mod.Config
{
    public class ConfigFile
    {
        public Guid ActiveProfile = Guid.Empty;
        public List<SettingsProfile> Profiles = new List<SettingsProfile>();
    }
}
