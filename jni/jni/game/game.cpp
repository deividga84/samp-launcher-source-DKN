#include "../main.h"
#include "game.h"
#include "../vendor/armhook/armhook.h"

void ApplySAMPPatchesInGame();
void InitScripting();

bool bUsedPlayerSlots[PLAYER_PED_SLOTS];

extern char* WORLD_PLAYERS;

char* szGameTextMessage = nullptr;

inline int FindFirstFreePlayerPedSlot()
{
	uint8_t x = 2;
	while (x != PLAYER_PED_SLOTS) {
		if (!bUsedPlayerSlots[x]) return x;
		x++;
	}

	return 0;
}

CGame::CGame()
{
	m_pGameCamera = new CCamera();
	m_pGamePlayer = nullptr;
	m_bCheckpointsEnabled = false;
	m_bRaceCheckpointsEnabled = false;
	m_dwRaceCheckpointHandle = 0;

	m_bClockEnabled = false;
	m_bInputEnable = true;

	memset(bUsedPlayerSlots, 0, sizeof(bUsedPlayerSlots));
	memset(m_bPreloadedVehicleModels, 0, sizeof(m_bPreloadedVehicleModels));
}

CGame::~CGame()
{

}

void CGame::StartGame()
{
	FLog("Starting game..");

	// OnNewGameCheck
	((void (*)(void))(g_libGTASA + 0x2A7200 + 1))();

	//*(int*)(g_libGTASA + 0xA987C8) = 8;
	//*(char*)(g_libGTASA + 0x96B514) = 0;
	//*(short*)(g_libGTASA + 0x6E00C0) = 0;
	//*(int*)(g_libGTASA + 0x6E0098) = 0;
	//*(char*)(g_libGTASA + 0x6E00D9) = 0;

	GameAimSyncInit();
	InitScripting();
}

void InstallSAMPHooks();
void InstallWidgetHooks();

void CGame::Initialize()
{
	FLog("CGame initializing..");

	// Menu_SwithOffToGame
	((void (*)(void))(g_libGTASA + 0x2A92F0 + 1))();

	ApplySAMPPatchesInGame();
	GameResetRadarColors();

	szGameTextMessage = (char*)malloc(512 + 1);
}
// 0.3.7
void CGame::SetMaxStats()
{
	// CCheat::VehicleSkillsCheat
	((void(*)(void))(g_libGTASA + /*0x2BAED0*/0x2FE620 + 1))();
	// CCheat::WeaponSkillsCheat
	((void(*)(void))(g_libGTASA + /*0x2BAE68*/0x2FE5BA + 1))();
	// CStats::SetStatValue
	ARMHook::makeRET(g_libGTASA + /*0x3B9074*/0x41552C);
}
// 0.3.7
void CGame::ToggleThePassingOfTime(bool bOnOff)
{
	if (bOnOff)
	{
		ARMHook::writeMemory(g_libGTASA + /*0x38C154*/0x3E3378, (uintptr_t)"\xD0\xB5", 2);
		this->m_bClockEnabled = true;
	}
	else
	{
		ARMHook::makeRET(g_libGTASA + /*0x38C154*/0x3E3378);
		this->m_bClockEnabled = false;
	}
}
// 0.3.7
void CGame::EnableClock(bool bEnable)
{
	char byteClockData[] = { '%', '0', '2', 'd', ':', '%', '0', '2', 'd', 0 };
	ARMHook::unprotect(g_libGTASA + /*0x599504*/0x2BD5A8);

	if (bEnable)
	{
		ToggleThePassingOfTime(true);
		memcpy((void*)(g_libGTASA + /*0x599504*/0x2BD5A8), byteClockData, 10);
	}
	else
	{
		ToggleThePassingOfTime(false);
		memset((void*)(g_libGTASA + /*0x599504*/0x2BD5A8), 0, 10);
	}
}
// 0.3.7
void CGame::EnableZoneNames(bool bEnable)
{
	ScriptCommand(&enable_zone_names, bEnable);
}
// 0.3.7
void CGame::SetWorldTime(int iHour, int iMinute)
{
	*(uint8_t*)(g_libGTASA + /*0x8B18A4*/0x953143) = (uint8_t)iMinute;
	*(uint8_t*)(g_libGTASA + /*0x8B18A5*/0x953142) = (uint8_t)iHour;
	ScriptCommand(&set_current_time, iHour, iMinute);
}
// 0.3.7
void CGame::GetWorldTime(int *iHour, int *iMinute)
{
	*iMinute = *(uint8_t*)(g_libGTASA + /*0x8B18A4*/0x953143);
	*iHour = *(uint8_t*)(g_libGTASA + /*0x8B18A5*/0x953142);
}
// 0.3.7
void CGame::PreloadObjectsAnims()
{
	// keep the throwable weapon models loaded
	if(!IsModelLoaded(WEAPON_MODEL_TEARGAS)) RequestModel(WEAPON_MODEL_TEARGAS);
	if(!IsModelLoaded(WEAPON_MODEL_GRENADE)) RequestModel(WEAPON_MODEL_GRENADE);
	if(!IsModelLoaded(WEAPON_MODEL_MOLOTOV)) RequestModel(WEAPON_MODEL_MOLOTOV);

	// special action object
	if(!IsModelLoaded(330)) RequestModel(330);
	if(!IsModelLoaded(OBJECT_PARACHUTE)) RequestModel(OBJECT_PARACHUTE);
	if(!IsModelLoaded(OBJECT_CJ_CIGGY)) RequestModel(OBJECT_CJ_CIGGY);
	if(!IsModelLoaded(OBJECT_DYN_BEER_1)) RequestModel(OBJECT_DYN_BEER_1);
	if(!IsModelLoaded(OBJECT_CJ_BEER_B_2)) RequestModel(OBJECT_CJ_BEER_B_2);
	if(!IsModelLoaded(OBJECT_CJ_PINT_GLASS)) RequestModel(OBJECT_CJ_PINT_GLASS);
	if(!IsModelLoaded(18631)) RequestModel(18631);

	// special action anim
	if(IsAnimationLoaded("PARACHUTE") == 0) RequestAnimation("PARACHUTE");
	if(IsAnimationLoaded("PAULNMAC") == 0) RequestAnimation("PAULNMAC");
	if(IsAnimationLoaded("BAR") == 0) RequestAnimation("BAR");
	if(IsAnimationLoaded("SMOKING") == 0) RequestAnimation("SMOKING");
	if(IsAnimationLoaded("DANCING") == 0) RequestAnimation("DANCING");
	if(IsAnimationLoaded("GFUNK") == 0) RequestAnimation("GFUNK");
	if(IsAnimationLoaded("RUNNINGMAN") == 0) RequestAnimation("RUNNINGMAN");
	if(IsAnimationLoaded("STRIP") == 0) RequestAnimation("STRIP");
	if(IsAnimationLoaded("WOP") == 0) RequestAnimation("WOP");
}
// 0.3.7
void CGame::SetWorldWeather(int byteWeatherID)
{
	*(unsigned char*)(g_libGTASA + /*0x9DB98E*/0xA7D138) = byteWeatherID;

	if (!m_bClockEnabled)
	{
		*(uint16_t*)(g_libGTASA + /*0x9DB990*/0xA7D136) = byteWeatherID;
		*(uint16_t*)(g_libGTASA + /*0x9DB992*/0xA7D134) = byteWeatherID;
	}
}
// 0.3.7
void CGame::DisplayHUD(bool bDisp)
{
	if (bDisp)
	{
		*(uint8_t*)(g_libGTASA + /*0x8ED7D9*/0x819D88) = 1;
		*(uint8_t*)(g_libGTASA + /*0x8EF36B*/0x991FD8) = 0;
	}
	else
	{
		*(uint8_t*)(g_libGTASA + /*0x8ED7D9*/0x819D88) = 0;
		*(uint8_t*)(g_libGTASA + /*0x8EF36B*/0x991FD8) = 1;
	}
}
// 0.3.7
uint8_t CGame::GetActiveInterior()
{
	uint32_t dwRet;
	ScriptCommand(&get_active_interior, &dwRet);
	return (uint8_t)dwRet;
}

const char* CGame::GetDataDirectory()
{
	return (const char*)(g_libGTASA + /*0x63C4B8*/0x6D687C);
}
// 0.3.7
void CGame::UpdateCheckpoints()
{
	if (m_bCheckpointsEnabled)
	{
		CPlayerPed* pPlayerPed = this->FindPlayerPed();
		if (pPlayerPed) 
		{
			ScriptCommand(&is_actor_near_point_3d, pPlayerPed->m_dwGTAId,
				m_vecCheckpointPos.X, m_vecCheckpointPos.Y, m_vecCheckpointPos.Z,
				m_vecCheckpointExtent.X, m_vecCheckpointExtent.Y, m_vecCheckpointExtent.Z, 1);

			if (!m_dwCheckpointMarker)
			{
				m_dwCheckpointMarker = CreateRadarMarkerIcon(0, m_vecCheckpointPos.X,
					m_vecCheckpointPos.Y, m_vecCheckpointPos.Z, 1005, 0);
			}
		}
	}
	else if (m_dwCheckpointMarker)
	{
		DisableMarker(m_dwCheckpointMarker);
		m_dwCheckpointMarker = 0;
	}

	if (m_bRaceCheckpointsEnabled)
	{
		CPlayerPed* pPlayerPed = this->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!m_dwRaceCheckpointMarker)
			{
				m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.X,
					m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z, 1005, 0);
			}
		}
	}
	else if (m_dwRaceCheckpointMarker)
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		DisableRaceCheckpoint();
		m_dwRaceCheckpointMarker = 0;
	}
}
// 0.3.7
uint8_t CGame::GetPedSlotsUsed()
{
	uint8_t count = 0;
	for (int i = 2; i < PLAYER_PED_SLOTS; i++)
	{
		if (bUsedPlayerSlots[i])
			count++;
	}

	return count;
}

void CGame::PlaySound(int iSound, float fX, float fY, float fZ)
{
	ScriptCommand(&play_sound, fX, fY, fZ, iSound);
}
// 0.3.7
void CGame::RefreshStreamingAt(float x, float y)
{
	ScriptCommand(&refresh_streaming_at, x, y);
}
// 0.3.7
void CGame::DisableTrainTraffic()
{
	ScriptCommand(&enable_train_traffic, 0);
}
// 0.3.7
void CGame::UpdateGlobalTimer(uint32_t dwTimer)
{
	if (!m_bClockEnabled)
	{
		*(uint32_t*)(g_libGTASA + 0x96B4D8) = dwTimer & 0x3FFFFFFF;
	}
}
// 0.3.7
void CGame::SetGravity(float fGravity)
{
	ARMHook::unprotect(g_libGTASA + /*0x3A0B64*/0x3FE7C0);
	*(float*)(g_libGTASA + /*0x3A0B64*/0x3FE7C0) = -fGravity;
}

bool CGame::IsGamePaused()
{
	return *(uint8_t*)(g_libGTASA + /*0x8C9BA3*/0x96B514);
}

bool CGame::IsGameLoaded()
{
	return true;
}

void CGame::DrawGangZone(float fPos[], uint32_t dwColor, uint32_t dwUnk)
{
	// CRadar::DrawAreaOnRadar
	((void(*)(float*, uint32_t*, uint32_t))(g_libGTASA + /*0x3DE7F8*/0x443C10 + 1))(fPos, &dwColor, dwUnk);
}
// 0.3.7
uint32_t CGame::CreatePickup(int iModel, int iType, float x, float y, float z, int *pdwIndex)
{
	if (!IsValidModel(iModel)) {
		iModel = 18631;
	}

	if (!IsModelLoaded(iModel))
	{
		RequestModel(iModel);
		LoadRequestedModels();
		while (!IsModelLoaded(iModel)) sleep(1);
	}

	uint32_t hnd;
	ScriptCommand(&create_pickup, iModel, iType, x, y, z, &hnd);
	int offset = 32 * (hnd & 0xFFFF);
	if (offset) offset /= 32;
	if (pdwIndex) {
		*pdwIndex = offset;
	}

	return hnd;
}
// 0.3.7
bool CGame::IsModelLoaded(int iModel)
{
	if (iModel > 20000 || iModel < 0) {
		return true;
	}
	else {
		return ScriptCommand(&is_model_available, iModel);
	}
}
// 0.3.7
void CGame::RequestModel(uint16_t iModelId, uint8_t iLoadingStream) 
{
	// CStreaming::RequestModel
	(( void (*)(int32_t, int32_t))(g_libGTASA+0x2D292C+1))(iModelId, iLoadingStream);
	//ScriptCommand(&request_model, iModelID);
}
// 0.3.7
void CGame::LoadRequestedModels()
{
	ScriptCommand(&load_requested_models);
}
// 0.3.7
void CGame::RemoveModel(int iModel, bool bFromStreaming)
{
	if (iModel >= 0 && iModel < 20000)
	{
		if (bFromStreaming)
		{
			if(ScriptCommand(&is_model_available, iModel))
				// CStreaming::RemoveModel
				((void(*)(int))(g_libGTASA + 0x2D00B8 + 1))(iModel);
		}
		else
		{
			if (ScriptCommand(&is_model_available, iModel))
				ScriptCommand(&release_model, iModel);
		}
	}
}
// 0.3.7 (������������ 2 ��������� ��������� ��������� � 0.3DL)
CObject* CGame::NewObject(int iModel, VECTOR vecPos, VECTOR vecRot, float fDrawDistance)
{
	CObject *pObjectNew = new CObject(iModel, vecPos, vecRot, fDrawDistance, 0);
	return pObjectNew;
}
// 0.3.7 (�� ����������� ������ bIsNPC)
CPlayerPed* CGame::NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, bool unk, bool bIsNPC)
{
	uint8_t bytePedSlot = FindFirstFreePlayerPedSlot();
	if (!bytePedSlot) return nullptr;

	CPlayerPed* pPed = new CPlayerPed(bytePedSlot, iSkin, fX, fY, fZ, fRotation);
	if (pPed && pPed->m_pPed) {
		bUsedPlayerSlots[bytePedSlot] = true;
	}

	return pPed;
}
// 0.3.7
bool CGame::RemovePlayer(CPlayerPed* pPed)
{
	if (!pPed) return false;

	delete pPed;
	bUsedPlayerSlots[pPed->m_bytePlayerNumber] = false;
	return true;
}
// 0.3.7
void CGame::DisableMarker(uint32_t dwMarker)
{
	ScriptCommand(&disable_marker, dwMarker);
}
// 0.3.7
uint32_t CGame::CreateRadarMarkerIcon(uint8_t byteType, float fPosX, float fPosY, float fPosZ, uint32_t dwColor, uint8_t byteStyle)
{
	uint32_t dwMapIcon = 0;

	switch (byteStyle)
	{
	case 0:	// MAPICON_LOCAL
		ScriptCommand(&create_radar_marker_without_sphere, fPosX, fPosY, fPosZ, byteType, &dwMapIcon);
		break;

	case 1:	// MAPICON_GLOBAL
		ScriptCommand(&create_marker_icon, fPosX, fPosY, fPosZ, byteType, &dwMapIcon);
		break;

	case 2:	// MAPICON_LOCAL_CHECKPOINT
		ScriptCommand(&create_radar_marker_icon, fPosX, fPosY, fPosZ, byteType, &dwMapIcon);
		break;

	case 3:	// MAPICON_GLOBAL_CHECKPOINT
		ScriptCommand(&create_icon_marker_sphere, fPosX, fPosY, fPosZ, byteType, &dwMapIcon);
		break;
	}

	if (byteType == 0)
	{
		if (dwColor < 1004)
		{
			ScriptCommand(&set_marker_color, dwMapIcon, dwColor);
			ScriptCommand(&show_on_radar, dwMapIcon, 2);
		}
		else
		{
			ScriptCommand(&set_marker_color, dwMapIcon, dwColor);
			ScriptCommand(&show_on_radar, dwMapIcon, 3);
		}
	}

	return dwMapIcon;
}
// 0.3.7
bool CGame::IsAnimationLoaded(const char* szAnimLib)
{
	return ScriptCommand(&is_animation_loaded, szAnimLib);
}
// 0.3.7
void CGame::RequestAnimation(const char* szAnimLib)
{
	ScriptCommand(&request_animation, szAnimLib);
}
// 0.3.7
float CGame::FindGroundZForCoord(float fX, float fY)
{
	/*float fPosZ;
	ScriptCommand(&get_ground_z, fX, fY, fZ, &fPosZ);
	return fPosZ;*/
	return (( float (*)(float, float))(g_libGTASA+0x42A774+1))(fX, fY);
}
// 0.3.7
void CGame::DisableAutoAim()
{
	//ARMHook::makeRET(g_libGTASA + 0x4C6CF4); // CPlayerPed::FindWeaponLockOnTarget
	//ARMHook::makeRET(g_libGTASA + 0x4C7CDC); // CPlayerPed::FindNextWeaponLockOnTarget

	// CPed::SetWeaponLockOnTarget
	ARMHook::makeRET(g_libGTASA + 0x4A82D4/*0x438DB4*/);
}

// 0.3.7
void CGame::EnabledAutoAim()
{
	ARMHook::makeRET(g_libGTASA + 0x4C6CF4); // CPlayerPed::FindWeaponLockOnTarget
	ARMHook::makeRET(g_libGTASA + 0x4C7CDC); // CPlayerPed::FindNextWeaponLockOnTarget
}
// 0.3.7
CVehicle* CGame::NewVehicle(int iVehicleType, float fX, float fY, float fZ, float fRotation, bool bAddSiren)
{
	bool bPreloaded = false;
	if (m_bPreloadedVehicleModels[iVehicleType - 400] == true) {
		bPreloaded = true;
	}

	CVehicle* pNewVehicle = new CVehicle(iVehicleType, fX, fY, fZ, fRotation, bPreloaded, bAddSiren);

	return pNewVehicle;
}
// 0.3.7
void CGame::SetCheckpointInformation(VECTOR* vecPos, VECTOR* vecSize)
{
	m_vecCheckpointPos.X = vecPos->X;
	m_vecCheckpointPos.Y = vecPos->Y;
	m_vecCheckpointPos.Z = vecPos->Z;

	m_vecCheckpointExtent.X = vecSize->X;
	m_vecCheckpointExtent.Y = vecSize->Y;
	m_vecCheckpointExtent.Z = vecSize->Z;
	
	if (m_dwCheckpointMarker)
	{
		DisableMarker(m_dwCheckpointMarker);
		m_dwCheckpointMarker = 0;

		m_dwCheckpointMarker = CreateRadarMarkerIcon(0,
			m_vecCheckpointPos.X,
			m_vecCheckpointPos.Y,
			m_vecCheckpointPos.Z,
			1005, 0);
	}
}
// 0.3.7
void CGame::SetRaceCheckpointInformation(uint8_t byteType, VECTOR* vecPos, VECTOR* vecNextPos, float fRadius)
{
	m_vecRaceCheckpointPos.X = vecPos->X;
	m_vecRaceCheckpointPos.Y = vecPos->Y;
	m_vecRaceCheckpointPos.Z = vecPos->Z;

	m_vecRaceCheckpointNextPos.X = vecNextPos->X;
	m_vecRaceCheckpointNextPos.Y = vecNextPos->Y;
	m_vecRaceCheckpointNextPos.Z = vecNextPos->Z;

	m_byteRaceType = byteType;
	m_fRaceCheckpointRadius = fRadius;

	if (m_dwRaceCheckpointMarker)
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		
		m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0,
			m_vecRaceCheckpointPos.X,
			m_vecRaceCheckpointPos.Y,
			m_vecRaceCheckpointPos.Z,
			1005,
			0);
	}

	MakeRaceCheckpoint();
}
// 0.3.7
void CGame::MakeRaceCheckpoint()
{
	DisableRaceCheckpoint();

	ScriptCommand(&create_racing_checkpoint, (int)m_byteRaceType,
		m_vecRaceCheckpointPos.X, m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z,
		m_vecRaceCheckpointNextPos.X, m_vecRaceCheckpointNextPos.Y, m_vecRaceCheckpointNextPos.Z,
		m_fRaceCheckpointRadius, &m_dwRaceCheckpointHandle);

	m_bRaceCheckpointsEnabled = true;
}
// 0.3.7
void CGame::DisableRaceCheckpoint()
{
	if (m_dwRaceCheckpointHandle)
	{
		ScriptCommand(&destroy_racing_checkpoint, m_dwRaceCheckpointHandle);
		m_dwRaceCheckpointHandle = 0;
	}

	m_bRaceCheckpointsEnabled = false;
}

void CGame::SetWantedLevel(uint8_t level)
{
	//ARMHook::writeMemory(g_libGTASA+0x2BDF6E, (uintptr_t)&level, 1);
}

void CGame::EnableStuntBonus(bool bEnable)
{
	ARMHook::unprotect(0x7BE2A8);
	*(int*)(g_libGTASA+0x7BE2A8) = (int)bEnable;
}
// 0.3.7
void CGame::DisplayGameText(const char* szStr, int iTime, int iSize)
{
	if (iSize > 200) return;

	ScriptCommand(&text_clear_all);
	CFont::AsciiToGxtChar(szStr, (uint16_t*)szGameTextMessage);

	// CMessages::AddBigMesssage
	((void (*)(char*, int, int))(g_libGTASA + 0x54C5BC + 1))(szGameTextMessage, iTime, iSize);
}
// 0.3.7
void CGame::AddToLocalMoney(int iAmmount)
{
	ScriptCommand(&add_to_player_money, 0, iAmmount);
}
// 0.3.7
void CGame::ResetLocalMoney()
{
	int iMoney = GetLocalMoney();
	if (!iMoney) return;

	if (iMoney < 0)
		AddToLocalMoney(abs(iMoney));
	else
		AddToLocalMoney(-(iMoney));
}
// 0.3.7
int CGame::GetLocalMoney()
{
	return *(int*)(WORLD_PLAYERS + 0xB8);
}
// 0.3.7
void CGame::DisableEnterExits()
{
	uintptr_t poolEntryExit = *(uintptr_t*)(g_libGTASA + 0x7A1E18);
	int size = *(int*)(poolEntryExit + 8);
	if (!size) return;

	uintptr_t pEntry = *(uintptr_t*)poolEntryExit;

	for (int i = 0; i < size; i++)
	{
		*(uint16_t*)(pEntry + 0x30) = 0;
		pEntry += 0x3C;
	}
}

void CGame::ToggleCJWalk(bool bUseCJWalk)
{
	//if (bUseCJWalk)
	//	ARMHook::writeMemory(g_libGTASA + 0x4C661C/*0x45477E*/, (uintptr_t)"\xC4\xF8\xDC\x64", 4);
	//else
	//	ARMHook::makeNOP(g_libGTASA + 0x4C661C/*0x45477E*/, 2);
	ARMHook::unprotect(g_libGTASA+0x4C5EFA);
    if(bUseCJWalk)
        ARMHook::writeMemory(g_libGTASA+0x4C5EFA, (uintptr_t)"\xCA\xF8\xE0\x04", 4);
    else
		*(uint8_t *)(g_libGTASA+0x4C5EFA) = 0;
}