#include "main.hpp"

#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/ComboUIController.hpp"


static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
using namespace GlobalNamespace;
using namespace UnityEngine;

float combo;
TMPro::TextMeshProUGUI *Session_combo;



// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

MAKE_HOOK_MATCH(Cut_note_hook, &ComboController::HandleNoteWasCut,void, ComboController* self, NoteController* noteController, ByRef<NoteCutInfo> info) {
    Cut_note_hook(self, noteController, info);
    combo += 1;
    Session_combo->SetText(std::to_string(combo));
    }


MAKE_HOOK_MATCH(Miss_note_hook, &ComboController::HandleNoteWasMissed,void, ComboController* self, NoteController* noteController) {
    Miss_note_hook(self, noteController);
    combo = 0;
    Session_combo->SetText(std::to_string(combo));
    }

MAKE_HOOK_MATCH(Player_head_wall, &ComboController::HandlePlayerHeadDidEnterObstacles,void, ComboController* self) {
    Player_head_wall(self);
    combo = 0;
    Session_combo->SetText(std::to_string(combo));
    }

MAKE_HOOK_MATCH(UI, &ComboUIController::Start, void, ComboUIController *self){
auto counter_location = self->get_transform()->get_position();

getLogger().info("creating canvas");
UnityEngine::GameObject *Canvas = QuestUI::BeatSaberUI::CreateCanvas();
getLogger().info("setting canvas position");
Canvas-> get_transform()->set_position(UnityEngine::Vector3(counter_location + UnityEngine::Vector3(0.0f, -1.0f,0.0f)));
Session_combo = QuestUI::BeatSaberUI::CreateText(Canvas->get_transform(),"0");
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

        INSTALL_HOOK(getLogger(), Cut_note_hook);

    getLogger().info("Installed all hooks!");
}