#include "main.hpp"
#include "mod.hpp"

#include "scotland2/shared/modloader.h"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

static modloader::ModInfo modInfo{ MOD_ID, VERSION, 0 };

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

extern "C" void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = 0;
    getLogger().info("autoplay setup");
}

extern "C" void late_load() {
    il2cpp_functions::Init();
    getLogger().info("autoplay late_load - installing hooks");
    Mod::InstallHooks();
}
