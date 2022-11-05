#include "main.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/ComboUIController.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "TMPro/TextMeshProUGUI.hpp"


static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
using namespace GlobalNamespace;
using namespace UnityEngine;

int combo_count, prev_combo;
TMPro::TextMeshProUGUI *Session_combo;



// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}


MAKE_HOOK_MATCH(ComboUIController_HandleComboDidChange, &ComboUIController::HandleComboDidChange, void, ComboUIController *self, int combo) {
    ComboUIController_HandleComboDidChange(self, combo_count);


    //auto combo_text = QuestUI::ArrayUtil::Last(GameObject::FindObjectsOfType<TMPro::TextMeshProUGUI*>(), [](TMPro::TextMeshProUGUI* text){return text->get_name() == "ComboText";});
    //combo_text->SetText("Session Combo");
}

MAKE_HOOK_MATCH(ComboUIController_HandleComboBreakingEventHappened, &GlobalNamespace::ComboUIController::HandleComboBreakingEventHappened, void, GlobalNamespace::ComboUIController* self) {
    ComboUIController_HandleComboBreakingEventHappened(self);
    combo_count = 0;
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::ComboUIController*>().First()->HandleComboDidChange(combo_count);
}

MAKE_HOOK_MATCH(ComboController_HandleNoteWasCut, &ComboController::HandleNoteWasCut, void, ComboController* self, NoteController *noteController, ByRef<NoteCutInfo> info) {
    ComboController_HandleNoteWasCut(self, noteController,info);
    if (info ->get_allIsOK()){
        combo_count += 1;
    }
    else{
        combo_count = 0;
    }
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::ComboUIController*>().First()->HandleComboDidChange(combo_count);
    }

MAKE_HOOK_MATCH(ComboUIController_Start, &ComboUIController::Start, void, GlobalNamespace::ComboUIController* self) {
    ComboUIController_Start(self);
    // this should reload the combo but it does not seem to 
    self->comboText->SetText(std::to_string(combo_count));
    auto combo_text = QuestUI::ArrayUtil::Last(GameObject::FindObjectsOfType<TMPro::TextMeshProUGUI*>(), [](TMPro::TextMeshProUGUI* text){return text->get_name() == "ComboText";});
    combo_text->SetText("Session Combo");
}





// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");

    INSTALL_HOOK(getLogger(), ComboUIController_HandleComboDidChange);
    INSTALL_HOOK(getLogger(), ComboUIController_HandleComboBreakingEventHappened);
    INSTALL_HOOK(getLogger(), ComboController_HandleNoteWasCut);
    INSTALL_HOOK(getLogger(), ComboUIController_Start)

    getLogger().info("Installed all hooks!");

}