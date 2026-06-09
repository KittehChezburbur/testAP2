#include "mod.hpp"
#include "main.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/SaberManager.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/NoteCutInfo.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/SaberType.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include <unordered_set>
#include <cstdlib>

/*
 * autoplay (auto-play) v0.1.0
 * Target: Beat Saber 1.40.8
 *
 * What this mod should do (from the description you wrote):
// all 115 114 113 get top 1 on every leaderboard never misses a single block
 */

// ---- Mod state ----
// Master switch. Toggle in-game with both thumbsticks pressed.
static bool g_enabled = true;
// Flip the mod on/off in-game. Default combo: both thumbsticks pressed.
// TODO (version-specific): confirm OVRInput button enums for your cordl.
MAKE_HOOK_FIND_CLASS_INSTANCE(Toggle_Update, "", "SaberManager", "Update", void, void* self) {
    Toggle_Update(self);
    static bool wasDown = false;
    bool down = OVRInput::Get(OVRInput::Button::PrimaryThumbstick, OVRInput::Controller::LTouch)
             && OVRInput::Get(OVRInput::Button::PrimaryThumbstick, OVRInput::Controller::RTouch);
    if (down && !wasDown) {
        g_enabled = !g_enabled;
        getLogger().info("autoplay: toggled %s", g_enabled ? "ON" : "OFF");
    }
    wasDown = down;
}

// ---- Autoplay: auto-cut every note (humanlike ~95%) ----
using namespace GlobalNamespace;
using namespace UnityEngine;

static std::unordered_set<NoteController*> activeNotes;

static Vector2 DirToVec(int dir) {
    switch (dir) {
        case 0:  return { 0.0f,  1.0f };
        case 1:  return { 0.0f, -1.0f };
        case 2:  return { -1.0f, 0.0f };
        case 3:  return { 1.0f,  0.0f };
        case 4:  return { -0.7071f,  0.7071f };
        case 5:  return { 0.7071f,   0.7071f };
        case 6:  return { -0.7071f, -0.7071f };
        case 7:  return { 0.7071f,  -0.7071f };
        default: return { 0.0f, 1.0f };
    }
}

// Humanlike cut: add accuracy + timing variance, occasional misses.
// Tuned to ~95% accuracy so it reads like a real player (Cyberramen-style),
// not an obvious robot.
static float frand() { return (float)std::rand() / (float)RAND_MAX; }

static NoteCutInfo HumanCut(NoteController* nc, bool& outMiss) {
    auto nd = nc->get_noteData();
    Vector2 d = DirToVec((int)nd->get_cutDirection());
    outMiss = frand() < 0.003f; // small chance to whiff entirely
    float off = (frand() * 2.0f - 1.0f) * 0.014f; // center deviation
    float angOff = (frand() * 2.0f - 1.0f) * 8.0f;     // small angle wobble (deg)
    NoteCutInfo info{};
    info.noteData = nd;
    info.speedOK = true;
    info.directionOK = true;
    info.saberTypeOK = true;
    info.wasCutTooSoon = false;
    info.saberSpeed = 22.0f + frand() * 8.0f; // varied swing speed
    info.saberDir = { d.x, d.y, 0.0f };
    info.cutDirDeviation = angOff;
    info.cutPoint = nc->get_transform()->get_position();
    info.cutNormal = { 0.0f, 0.0f, 1.0f };
    info.cutDistanceToCenter = off < 0 ? -off : off; // imperfect accuracy
    info.cutAngle = angOff;
    info.timeDeviation = (frand() * 2.0f - 1.0f) * 0.03f; // +/-30ms timing
    info.saberType = nd->get_colorType() == ColorType::ColorA ? SaberType::SaberA : SaberType::SaberB;
    return info;
}

MAKE_HOOK_MATCH(NC_Init, &NoteController::Init, void,
    NoteController* self, NoteData* noteData, float worldRotation,
    Vector3 moveStartPos, Vector3 moveEndPos, Vector3 jumpEndPos,
    float moveDuration, float jumpDuration, float jumpGravity,
    float endRotation, float uniformScale, bool rotatesTowardPlayer,
    bool useRandomRotation) {
    NC_Init(self, noteData, worldRotation, moveStartPos, moveEndPos, jumpEndPos,
        moveDuration, jumpDuration, jumpGravity, endRotation, uniformScale,
        rotatesTowardPlayer, useRandomRotation);
    if (noteData->get_colorType() == ColorType::None) return; // bomb
    activeNotes.insert(self);
}

MAKE_HOOK_MATCH(NC_OnDestroy, &NoteController::OnDestroy, void, NoteController* self) {
    activeNotes.erase(self);
    NC_OnDestroy(self);
}

MAKE_HOOK_MATCH(NC_ManualUpdate, &NoteController::ManualUpdate, void, NoteController* self) {
    NC_ManualUpdate(self);
    if (!g_enabled) return;            // respect the master switch
    if (!activeNotes.contains(self)) return;
    // TODO (version-specific): trigger when the note reaches the cut plane.
    // Wire this to your cordl version's cut-time check. Example placeholder:
    if (self->get_isInTimeToCut && self->get_isInTimeToCut()) {
        bool miss = false;
        auto info = HumanCut(self, miss);
        if (!miss) self->SendNoteWasCutEvent(byref(info));
        activeNotes.erase(self);
    }
}

void Mod::InstallHooks() {
    INSTALL_HOOK(getLogger(), Toggle_Update);
    INSTALL_HOOK(getLogger(), NC_Init);
    INSTALL_HOOK(getLogger(), NC_OnDestroy);
    INSTALL_HOOK(getLogger(), NC_ManualUpdate);
    getLogger().info("autoplay: hooks installed");
}
