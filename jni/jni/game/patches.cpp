#include "../main.h"
#include "../game/game.h"
#include "../vendor/armhook/armhook.h"
#include "vehicleColoursTable.h"
#include "../settings.h"
extern CSettings* pSettings;

char* WORLD_PLAYERS = nullptr;

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
} *ATOMIC_MODELS = nullptr;

VehicleAudioPropertiesStruct VehicleAudioProperties[20000];

#include "game.h"
extern CGame* pGame;
void readVehiclesAudioSettings()
{

	char vehicleModel[50];
	int16_t pIndex = 0;

	FILE* pFile;

	char line[300];

	// Zero VehicleAudioProperties
	memset(VehicleAudioProperties, 0x00, sizeof(VehicleAudioProperties));

	VehicleAudioPropertiesStruct CurrentVehicleAudioProperties;

	memset(&CurrentVehicleAudioProperties, 0x0, sizeof(VehicleAudioPropertiesStruct));

	char buffer[0xFF];
	sprintf(buffer, "%sSAMP/vehicleAudioSettings.cfg", pGame->GetDataDirectory());
	pFile = fopen(buffer, "r");
	if (!pFile)
	{
		//Log("Cannot read vehicleAudioSettings.cfg");
		return;
	}

	// File exists
	while (fgets(line, sizeof(line), pFile))
	{
		if (strncmp(line, ";the end", 8) == 0)
			break;

		if (line[0] == ';')
			continue;

		sscanf(line, "%s %d %d %d %d %f %f %d %f %d %d %d %d %f",
			   vehicleModel,
			   &CurrentVehicleAudioProperties.VehicleType,
			   &CurrentVehicleAudioProperties.EngineOnSound,
			   &CurrentVehicleAudioProperties.EngineOffSound,
			   &CurrentVehicleAudioProperties.field_4,
			   &CurrentVehicleAudioProperties.field_5,
			   &CurrentVehicleAudioProperties.field_6,
			   &CurrentVehicleAudioProperties.HornTon,
			   &CurrentVehicleAudioProperties.HornHigh,
			   &CurrentVehicleAudioProperties.DoorSound,
			   &CurrentVehicleAudioProperties.RadioNum,
			   &CurrentVehicleAudioProperties.RadioType,
			   &CurrentVehicleAudioProperties.field_14,
			   &CurrentVehicleAudioProperties.field_16);

		((void (*)(const char* thiz, int16_t* a2))(g_libGTASA + 0x385D48 + 1))(vehicleModel, &pIndex);
		memcpy(&VehicleAudioProperties[pIndex-400], &CurrentVehicleAudioProperties, sizeof(VehicleAudioPropertiesStruct));


	}

	fclose(pFile);
}

void ApplySAMPPatchesInGame()
{
	LOGI("Applying samp patches.. (ingame)");

	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA + /*0x8EA7B0*/0x98D252), 1, 100);
	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_libGTASA + /*0x8EA7A8*/0x98D2B8) = 100;

	// Make pay 'n' spray always free
	*(bool*)(g_libGTASA+0x7A4DB2) = true;

    // displayFPS
    *(unsigned char*)(g_libGTASA + 0x98F1AD) = pSettings->Get().iFPSCounter;

	ARMHook::unprotect(g_libGTASA+0x5E4978);
	*(uint8_t*)(g_libGTASA+0x5E4978) = pSettings->Get().iFPSCount;
	ARMHook::unprotect(g_libGTASA+0x5E4990);
	*(uint8_t*)(g_libGTASA+0x5E4990) = pSettings->Get().iFPSCount;

    // множитель для MaxHealth
    ARMHook::unprotect(g_libGTASA + 0x41C33C);
    *(float*)(g_libGTASA + 0x41C33C) = 176.0f;
    // множитель для Armour
    ARMHook::unprotect(g_libGTASA + 0x2BD94C);
    *(float*)(g_libGTASA + 0x2BD94C) = 176.0;
}

void ApplyGlobalPatches()
{
	LOGI("Applying global patches..");

	//Fix ShowPlayerMarkers(0); and
	//fix problem when enter police vehicle it will get shotgun
	ARMHook::makeNOP(g_libGTASA + 0x5847E0, 2);
	ARMHook::makeNOP(g_libGTASA + 0x584822, 2);
	ARMHook::makeRET(g_libGTASA + 0x40C296);//fix hospital car

	// relocate CWorld::Players[]
	WORLD_PLAYERS = new char[0x404 * PLAYER_PED_SLOTS];
	memset(WORLD_PLAYERS, 0, 0x404 * PLAYER_PED_SLOTS);
	ARMHook::unprotect(g_libGTASA + /*0x5D021C*/0x6783C8);
	*(char**)(g_libGTASA + /*0x5D021C*/0x6783C8) = WORLD_PLAYERS;
	FLog("CWorld::Players new address: 0x%X", WORLD_PLAYERS);

	// allocate Atoomic models pool
	ATOMIC_MODELS = new _ATOMIC_MODEL[20000];
	for (int i = 0; i < 20000; i++) {
		// CBaseModelInfo::CBaseModelInfo
		((void(*)(_ATOMIC_MODEL*))(g_libGTASA + /*0x33559C*/0x384F88 + 1))(&ATOMIC_MODELS[i]);
		// vtable
		ATOMIC_MODELS[i].func_tbl = g_libGTASA + /*0x5C6C68*/0x667454;
		memset(ATOMIC_MODELS[i].data, 0, sizeof(ATOMIC_MODELS->data));
	}
	FLog("AtomicModelsPool new address: 0x%X", ATOMIC_MODELS);

	// path CVehicleModelInfo allcator
	ARMHook::writeMemory(g_libGTASA + 0x468B7E, (uintptr_t)"\x4F\xF4\x00\x30", 4); // MOV R0, #0ApplyGlobalPatchesx20000
	ARMHook::writeMemory(g_libGTASA + 0x468B88, (uintptr_t)"\xF7\x20", 2); // MOVS r0, #0xF7
	ARMHook::writeMemory(g_libGTASA + 0x468B8A, (uintptr_t)"\xF7\x25", 2); // MOVS r5, #0xF7
	ARMHook::writeMemory(g_libGTASA + 0x468BCC, (uintptr_t)"\xF7\x28", 2); // CMP R0, #0xF7


	// CAudioEngine::StartLoadingTune
	ARMHook::makeNOP(g_libGTASA + /*0x56C150*/0x5E4916, 2);
	// DefualtPCSaveFileName
	char* DefaultPCSaveFileName = (char*)(g_libGTASA + /*0x60EAE8*/0x6B012C);
	memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);

	// menu_newGame Menu_SWithOffToGame
	ARMHook::makeNOP(g_libGTASA + 0x2A7258, 2);

	// CPlayerPed::CPlayerPed
	ARMHook::writeMemory(g_libGTASA + 0x4C3673, (uintptr_t)"\xB3", 1);

	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
	//ARMHook::makeRET(g_libGTASA + 0x3976AC);

	// CPlaceName::Process
	ARMHook::makeRET(g_libGTASA + 0x4211A0);

	// CHud::DrawVehicleName
	ARMHook::makeRET(g_libGTASA + 0x438634);

	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
	ARMHook::makeRET(g_libGTASA + 0x538C8C);

	// CEntryExit::GenerateAmbientPeds
	ARMHook::makeRET(g_libGTASA + 0x306EC0);

	// CFileLoader::LoadPickup
	ARMHook::makeRET(g_libGTASA + 0x46B548);

	//	CHud::SetHelpMessage
	ARMHook::makeRET(g_libGTASA + 0x436F5C);

	// CTheCarGenerators::Process
	ARMHook::makeRET(g_libGTASA + 0x56E350);

	// CPlane::DoPlaneGenerationAndRemoval
	ARMHook::makeRET(g_libGTASA + 0x579214);

	// CPopulation::AddPed
	ARMHook::makeRET(g_libGTASA + 0x4CF26C);

	// CCarEnterExit::SetPedInCarDirect
	ARMHook::makeRET(g_libGTASA + 0x50AA58);

	// CPlayerPed::ProcessAnimGroups
	ARMHook::makeNOP(g_libGTASA + 0x4C5EFA, 2);

	// CCarCtrl::GenerateRandomCars
	ARMHook::makeRET(g_libGTASA + 0x2E82CC);

	//  CPlayerInfo::KillPlayer -> CMessages::AddBigMessage
	ARMHook::makeNOP(g_libGTASA + 0x40BED6, 2);

	// CRealTimeShadowManager::ReturnRealTimeShadow
	ARMHook::makeNOP(g_libGTASA + 0x3FCD34, 2);
	ARMHook::makeNOP(g_libGTASA + 0x3FCD74, 2);

	// CRealTimeShadowManager::Update
	ARMHook::makeRET(g_libGTASA + 0x5B83FC);

	// RpWorldAddLight direct
	//ARMHook::makeNOP(g_libGTASA + 0x46FC54, 2);

	// CPlayerPed::GetPlayerInfoForThisPlayerPed (CPed::RemoveWeaponWhereEnteringVehicle)
	ARMHook::makeNOP(g_libGTASA + 0x4A5328, 6);

	// CVehicleModelInfo::ms_vehicleColourTable
	ARMHook::unprotect(g_libGTASA + 0x677654);
	*(uintptr_t*)(g_libGTASA + 0x677654) = (uintptr_t)VehicleColoursTableRGBA;

	// alpha RasterCreate
	ARMHook::writeMemory(g_libGTASA + 0x1AE95E, (uintptr_t)"\x01\x22", 2);

	// CBike::ProcessAI
	ARMHook::makeNOP(g_libGTASA + 0x564CC0, 1);

	// CHud::SetHelpMessageStatUpdate
	ARMHook::makeRET(g_libGTASA + 0x436FCC);

	// radar draw blips
	ARMHook::makeNOP(g_libGTASA + 0x43FE0A, 2);
	ARMHook::makeNOP(g_libGTASA + 0x44095E, 2);
	ARMHook::makeNOP(g_libGTASA + 0x43FE08, 3);
	ARMHook::makeNOP(g_libGTASA + 0x44095C, 3);

	// CCamera::CamShake
	ARMHook::makeNOP(g_libGTASA + 0x5D87A6, 2);
	ARMHook::makeNOP(g_libGTASA + 0x5D8734, 2);

	// RwCameraEndUpdate (for Idle hook)
	ARMHook::makeNOP(g_libGTASA + 0x3F6C8C, 2);

	// fpslimit
	//ARMHook::writeMemory(g_libGTASA + 0x5E4978, (uintptr_t)"\x64", 1);
	//ARMHook::writeMemory(g_libGTASA + 0x5E4990, (uintptr_t)"\x64", 1);

	// CRadar::Draw3dMarkers (translate color)
	ARMHook::writeMemory(g_libGTASA + 0x4420D0, (uintptr_t)"\x2C\xE0", 2); // B 0x44212C
	ARMHook::writeMemory(g_libGTASA + 0x44212C, (uintptr_t)"\x30\x46", 2); // mov r0, r6
	// CRadar::DrawEntityBlip (translate color)
	ARMHook::writeMemory(g_libGTASA + 0x440470, (uintptr_t)"\x3A\xE0", 2); // B0x4404E8
	ARMHook::writeMemory(g_libGTASA + 0x4404E8, (uintptr_t)"\x30\x46", 2); // mov r0, r6
	// CRadar::DrawCoordBlip (translate color)
	ARMHook::writeMemory(g_libGTASA + 0x43FB0E, (uintptr_t)"\x12\xE0", 2); // B
	ARMHook::writeMemory(g_libGTASA + 0x43FB36, (uintptr_t)"\x48\x46", 2); // mov r0, r9
    ARMHook::writeMemory(g_libGTASA + 0x2AB556, (uintptr_t)"\x00\x21", 2);

	// FindObjectToSteal patch
	ARMHook::makeRET(g_libGTASA + 0x40B028);
	// Interior_c::AddPickups
	ARMHook::makeRET(g_libGTASA + 0x445E98);
	// Interior_c::Exit
	ARMHook::makeRET(g_libGTASA + 0x448984);

	// no vehicle audio processing
	ARMHook::makeNOP(g_libGTASA + 0x553E26, 2); // CAutomobile
	ARMHook::makeNOP(g_libGTASA + 0x561A52, 2); // CBike
	ARMHook::makeNOP(g_libGTASA + 0x56BE64, 2); // CBoat
	ARMHook::makeNOP(g_libGTASA + 0x57D054, 2); // CTrain

	// camera_on_actor path
	ARMHook::writeMemory(g_libGTASA + 0x341F34, (uintptr_t)"\x00\xF0\x21\xBE", 4);

	ARMHook::makeNOP(g_libGTASA + 0x005E54AA, 2); // не сохранять при сворачивании. черный экран

	//ARMHook::writeMemory(g_libGTASA + 0x1C8064, (uintptr_t)"\x01", 1);
	//ARMHook::writeMemory(g_libGTASA + 0x1C8082, (uintptr_t)"\x01", 1);

	/*ARMHook::writeMemory(g_libGTASA + 0x5EAB20 + 52, (uintptr_t)"      ", 6);
	ARMHook::writeMemory(g_libGTASA + 0x5EAB94 + 52, (uintptr_t)"      ", 6);
	ARMHook::writeMemory(g_libGTASA + 0x5EAC96 + 75, (uintptr_t)"      ", 6);
	ARMHook::writeMemory(g_libGTASA + 0x5EABE8 + 53, (uintptr_t)"      ", 6);
	ARMHook::writeMemory(g_libGTASA + 0x5EABE8 + 99, (uintptr_t)"      ", 6);*/

	// gamepad fix?
	ARMHook::makeRET(g_libGTASA + 0x28CC04);

	// lower threads sleeping timers p.s. killman
	//ARMHook::writeMemory(g_libGTASA + 0x1D248E, (uintptr_t)"\x08", 1);
	//ARMHook::writeMemory(g_libGTASA + 0x266D3A, (uintptr_t)"\x08", 1);
	//ARMHook::writeMemory(g_libGTASA + 0x26706E, (uintptr_t)"\x08", 1);
	//ARMHook::writeMemory(g_libGTASA + 0x26FDCC, (uintptr_t)"\x08", 1);

	// Disable pay 'n' spray messages
	ARMHook::makeRET(g_libGTASA+0x311E3C); // CGarages::TriggerMessage

	// NOP calling CUpsideDownCarCheck::UpdateTimers from CTheScripts::Process
	ARMHook::makeNOP(g_libGTASA+0x32AEC6, 2);

	// Increase pickup distance visibility
	ARMHook::unprotect(g_libGTASA+0x31D4BC);
	*(float*)(g_libGTASA+0x31D4BC) = 10000.0f;

	// Aggressively RETing automobile flying
	ARMHook::makeRET(g_libGTASA+0x5524CC); // CAutomobile::ProcessFlyingCarStuff

	// Remove cellphone holding
	ARMHook::unprotect(g_libGTASA+0x4F0CF6);
	*(float*)(g_libGTASA+0x4F0CF6) = 0xFFFFFFFF;
	ARMHook::unprotect(g_libGTASA+0x4F10F6);
	*(float*)(g_libGTASA+0x4F10F6) = 0xFFFFFFFF;

	// Increase bullet world range
	ARMHook::unprotect(g_libGTASA+0x5D7410);
	*(float*)(g_libGTASA+0x5D7410) = -20000.0f;
	ARMHook::unprotect(g_libGTASA+0x5D7414);
	*(float*)(g_libGTASA+0x5D7414) = 20000.0f;

	// No vehicle audio processing
	//ARMHook::makeRET(g_libGTASA+0x3ACDB4);

	// CPlayerPed::DoesPlayerWantNewWeapon
	ARMHook::makeRET(g_libGTASA+0x4C6708);

	// Disable cutscene processing
	ARMHook::makeNOP(g_libGTASA+0x3F4000, 2); // NOP calling CCutsceneMgr::Update from CGame::Process

	// Disable camera jump-cut after respawning
	ARMHook::makeNOP(g_libGTASA+0x307BDE, 2); // NOP calling CCamera::RestoreWithJumpCut from CGameLogic::RestorePlayerStuffDuringResurrection

	// Wanted level hook
	//ARMHook::writeMemory(g_libGTASA+0x2BDF70, (uintptr_t)"\x4F\xF0\x00\x08", 4); // CWidgetPlayerInfo::DrawWanted

	// Make peds dummy
	ARMHook::makeNOP(g_libGTASA+0x37C01C, 9); // NOPing out case 31 from CEventHandler::ComputeEventResponseTask

	// No IPL Vehicle
	ARMHook::makeNOP(g_libGTASA+0x3F411E, 2); // NOP calling CTheCarGenerators::Process from CGame::Process
	ARMHook::makeRET(g_libGTASA+0x2FB258); // CCarCtrl::GenerateOneEmergencyServicesCar

	// CGame::InitialiseOnceBeforeRW
	ARMHook::writeMemory(g_libGTASA+0x3F3648, (uintptr_t)"\x06\x20", 2);

	// fix crash in menu (wardumbs :D)
	ARMHook::makeNOP(g_libGTASA + 0x2AB998, 2); // CFont::PrintString in Menu_MapRender
	ARMHook::makeNOP(g_libGTASA + 0x2AB9A4, 2); // CRadar::DrawLegend in Menu_MapRender
	ARMHook::makeNOP(g_libGTASA + 0x2AB940, 2); // CFont::GetStringWidth check in Menu_MapRender
	ARMHook::makeRET(g_libGTASA + 0x5AB020); // CFont::GetStringWidth this peace of shit :/
	ARMHook::makeRET(g_libGTASA + 0x441A08); // CRadar::AddBlipToLegendList
	ARMHook::makeRET(g_libGTASA + 0x441B74); // CRadar::DrawLegend
	ARMHook::makeNOP(g_libGTASA + 0x43FE0A, 2); // NOP calling CSprite2d::Draw from CRadar::DrawCoordBlip
	ARMHook::makeNOP(g_libGTASA + 0x44095E, 2); // NOP calling CSprite2d::Draw from CRadar::DrawEntityBlip

	// why to set task if i create my task hmmm
	//ARMHook::writeMemory(g_libGTASA + 0x40AC28, (uintptr_t)"\x8F\xF5\x3A\xEF", 4); // CTaskComplexEnterCarAsDriver
	//ARMHook::makeNOP(g_libGTASA + 0x40AC30, 2); // NOP calling CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver from CPlayerInfo::Process
	ARMHook::makeNOP(g_libGTASA + 0x40AC3C, 2); // CTaskManager::SetTask in CPLayerInfo::Process

	// psInitialize

	//graficos novos
	ARMHook::makeNOP(g_libGTASA + 0x1BDA64, 3);
}

void InstallVehicleEngineLightPatches()
{
	// типо фикс задних фар
	ARMHook::writeMemory(g_libGTASA + 0x591272, (uintptr_t)"\x02", 1);
	ARMHook::writeMemory(g_libGTASA + 0x59128E, (uintptr_t)"\x02", 1);
}