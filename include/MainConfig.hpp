#pragma once

#include "config-utils/shared/config-utils.hpp"


// set the default config values
DECLARE_CONFIG(MainConfig,
    CONFIG_VALUE(Mod_active, bool, "Enabled", true);





    // intialise config values
    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(Mod_active);

    )
)