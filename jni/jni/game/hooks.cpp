#include <GLES2/gl2.h>
#include "../main.h"
#include "../vendor/armhook/armhook.h"
#include "game.h"
#include "../net/netgame.h"
#include "../gui/gui.h"
//#include "../vendor/curl/easy.h"

extern UI* pUI;
extern CGame* pGame;
extern CNetGame *pNetGame;
extern MaterialTextGenerator* pMaterialTextGenerator;

uint8_t byteInternalPlayer = 0;
PED_TYPE* dwCurPlayerActor = 0;
uint8_t byteCurPlayer = 0;

extern "C" uintptr_t get_lib()
{
	return g_libGTASA;
}
// 0.3.7
PLAYERID FindPlayerIDFromGtaPtr(ENTITY_TYPE* pEntity)
{
	if (pEntity == nullptr) return INVALID_PLAYER_ID;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	PLAYERID PlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)pEntity);
	if (PlayerID != INVALID_PLAYER_ID) return PlayerID;

	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)pEntity);
	if (VehicleID != INVALID_VEHICLE_ID)
	{
		for (PLAYERID i = 0; i < MAX_PLAYERS; i++)
		{
			CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
			if (pRemotePlayer && pRemotePlayer->CurrentVehicleID() == VehicleID) {
				return i;
			}
		}
	}

	return INVALID_PLAYER_ID;
}
// 0.3.7
PLAYERID FindActorIDFromGtaPtr(PED_TYPE* pPed)
{
	if (pPed) {
		return pNetGame->GetActorPool()->FindIDFromGtaPtr(pPed);
	}

	return INVALID_PLAYER_ID;
}

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
	return written;
}

/* =============================================================================== */

int(*OS_FileOpen)(uint32_t r0, uintptr_t handle, char* name, int r3);
int OS_FileOpen_hook(int r0, uintptr_t handle, char* name, int r3)
{
	char path[0xff] = { 0 };
	const char* pszStorage = (const char*)(g_libGTASA + /*0x63C4B8*/0x6D687C);

	//============ [Create Tracks] ==============//
	/*int c;
	FILE* fp;
	char ts2[255];
	sprintf(ts2, "%s/tracks2.dat", pszStorage);
	//Check File//
	fp = fopen(ts2, "r");
	if (!fp)
	{
		fp = fopen(ts2, "w");
		char str[20] = "0";
		if (fp)
		{
			for (int i = 0; i < strlen(str); i++)
				putc(str[i], fp);
		}
		fclose(fp);
	}
	fclose(fp);
	//------//
	int c2;
	FILE* fp2;
	char ts4[255];
	sprintf(ts4, "%s/tracks4.dat", pszStorage);
	//Check File//
	fp2 = fopen(ts4, "r");
	if (!fp2)
	{
		fp2 = fopen(ts4, "w");
		char str2[20] = "0";
		if (fp2)
		{
			for (int i = 0; i < strlen(str2); i++)
				putc(str2[i], fp2);
		}
		fclose(fp2);
	}
	fclose(fp2);*/
	//=============================//

	FLog("OS_FileOpen: %s", name);

	if (!strncmp(name, "data\\script\\mainV1.scm", 22)) {
		FLog("Loading mainV1.scm..");
		sprintf(path, "SAMP\\main.scm");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\SCRIPT\\SCRIPTV1.IMG", 24))
	{
		FLog("Loading scriptV1.img..");
		sprintf(path, "SAMP\\script.img");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\GTA.DAT", 12))
	{
		FLog("Loading gta.dat..");
		sprintf(path, "SAMP\\gta.dat");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\PEDS.IDE", 13))
	{
		FLog("Loading peds.ide..");
		sprintf(path, "SAMP\\peds.ide");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\TIMECYC.DAT", 16))//15
	{
		FLog("Loading timecyc.dat..");
		sprintf(path, "SAMP\\timecyc.dat");
		name = path;
		goto ret;
	}

    if (!strncmp(name, "DATA\\HANDLING.CFG", 18)) {
        FLog("Loading handling.cfg..");
        sprintf(path, "SAMP\\handling.cfg");
        name = path;
        goto ret;
    }

	//
	/*if (!strncmp(name, "data\\paths\\tracks2.dat", 22))
	{
		FLog("Loading tracks2.dat...");
		sprintf(path, "tracks2.dat");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "data\\paths\\tracks4.dat", 22))
	{
		FLog("Loading tracks4.dat...");
		sprintf(path, "tracks4.dat");
		name = path;
		goto ret;
	}*/

	/*if (!strncmp(name, "DATA\\paths\\tracks2.dat", 20))
	{
		FLog("Loading tracks2.dat...");
		sprintf(path, "SAMP\\tracks2.dat");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\paths\\tracks4.dat", 20))
	{
		FLog("Loading tracks4.dat...");
		sprintf(path, "SAMP\\tracks4.dat");
		name = path;
		goto ret;
	}*/

	/*if (!strncmp(name, "data\\paths\\tracks2.dat", 21))
	{
		FLog("Loading tracks2.dat...");
		sprintf(path, "SAMP\\tracks2.dat");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "data\\paths\\tracks4.dat", 21))
	{
		FLog("Loading tracks4.dat...");
		sprintf(path, "SAMP\\tracks4.dat");
		name = path;
		goto ret;
	}*/

	/*if (!strncmp(name, "data\\paths\\tracks2.dat", 21))
	{
		FLog("Loading tracks2.dat...");
		const char* SpszStorage = CGame::GetDataDirectory();
		char track2path[255] = { 0 };
		sprintf(track2path, "%stracks2.dat", SpszStorage);
		name = track2path;
		goto ret;
	}

	if (!strncmp(name, "data\\paths\\tracks4.dat", 21))
	{
		FLog("Loading tracks4.dat...");
		const char* SpszStorage = CGame::GetDataDirectory();
		char track4path[255] = { 0 };
		sprintf(track4path, "%stracks4.dat", SpszStorage);
		name = track4path;
		goto ret;
	}*/

	ret:
	return OS_FileOpen(r0, handle, name, r3);
}

/* =============================================================================== */

void(*CStream_InitImageList)();
void CStream_InitImageList_hook()
{
	FLog("Initializing ImageList..");

	*(char*)(g_libGTASA + 0x792DA8) = 0;
	*(int*)	(g_libGTASA + 0x792DA4) = 0;
	*(int*)	(g_libGTASA + 0x792DD4) = 0;
	*(int*)	(g_libGTASA + 0x792E04) = 0;
	*(char*)(g_libGTASA + 0x792DD8) = 0;
	*(int*)	(g_libGTASA + 0x792E34) = 0;
	*(char*)(g_libGTASA + 0x792E08) = 0;
	*(int*)	(g_libGTASA + 0x792E64) = 0;
	*(char*)(g_libGTASA + 0x792E38) = 0;
	*(int*)	(g_libGTASA + 0x792E94) = 0;
	*(char*)(g_libGTASA + 0x792E68) = 0;
	*(int*)	(g_libGTASA + 0x792EC4) = 0;
	*(char*)(g_libGTASA + 0x792E98) = 0;
	*(int*)	(g_libGTASA + 0x792EF4) = 0;
	*(char*)(g_libGTASA + 0x792EC8) = 0;

	// CStreaming::AddImageToList
	((uintptr_t(*)(const char*, int))(g_libGTASA + 0x2CF760 + 1))("TEXDB\\SAMPCOL.IMG", 1);
	((uintptr_t(*)(const char*, int))(g_libGTASA + 0x2CF760 + 1))("TEXDB\\GTA3.IMG", 1);
	((uintptr_t(*)(const char*, int))(g_libGTASA + 0x2CF760 + 1))("TEXDB\\GTA_INT.IMG", 1);
	((uintptr_t(*)(const char*, int))(g_libGTASA + 0x2CF760 + 1))("TEXDB\\SAMP.IMG", 1);
	return;
}

void InitGui();
uint32_t (*CGame__InitialiseRenderWare)();
uint32_t CGame__InitialiseRenderWare_hook()
{
	FLog("Loading SAMP texture database..");
	uint32_t result = CGame__InitialiseRenderWare();
	// TextureDatabaseRuntime::Load()
	((void(*)(const char*, int, int))(g_libGTASA + /*0x1BF244*/0x1EA8E4 + 1))("samp", 0, 5);
	/*((void(*)(const char*, int, int))(g_libGTASA + 0x1EA8E4 + 1))("gui", 0, 5);//gui
	((void(*)(const char*, int, int))(g_libGTASA + 0x1EA8E4 + 1))("gtasa", 0, 5);//gtasa
	((void(*)(const char*, int, int))(g_libGTASA + 0x1EA8E4 + 1))("game", 0, 5);//game
	((void(*)(const char*, int, int))(g_libGTASA + 0x1EA8E4 + 1))("fm", 0, 5);//game*/

	InitGui();
	return result;
}

/* =============================================================================== */

void MainLoop();
void(*Render2dStuff)();
void Render2dStuff_hook()
{
	Render2dStuff();
	MainLoop();
	return;
}

/* =============================================================================== */

#include "../java/jniutil.h"
extern CJavaWrapper *pJavaWrapper;
void(*DisplayScreen)();
void DisplayScreen_hook()
{
	/*RwCamera *camera = *(RwCamera**)(g_libGTASA + 0x9FC93C);

	if (RwCameraBeginUpdate(camera))
	{
		// DefinedState2d
		((void(*)())(g_libGTASA + /0x5D0C64 + 1))();
		// CSprite2d::InitPerFrame()
		((void(*)())(g_libGTASA + 0x5C89A8 + 1))();
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
		// emu_GammaSet()
		((void(*)(bool))(g_libGTASA + 0x1C07D0 + 1))(false);

		// CLoadingScreen::m_PercentLoaded
		float fPercentLoaded = *(float*)(g_libGTASA + 0x9920C0);

		pUI->splashscreen()->setProgressBarValue(fPercentLoaded / 100.0f);
		pUI->render();

		RwCameraEndUpdate(camera);
		RwCameraShowRaster(camera, 0, 0);
	}*/
	pJavaWrapper->ShowLoadingScreen();
}
/* =============================================================================== */

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};

extern struct _ATOMIC_MODEL *ATOMIC_MODELS;
uintptr_t(*CModelInfo_AddAtomicModel)(int iModel);
uintptr_t CModelInfo_AddAtomicModel_hook(int iModel)
{
	uint32_t iCount = *(uint32_t*)(g_libGTASA + /*0x7802C4*/0x820738);
	//FLog("[Model %d] atomics count: %d",iModel, iCount);
	_ATOMIC_MODEL *model = &ATOMIC_MODELS[iCount];
	*(uint32_t*)(g_libGTASA + /*0x7802C4*/0x820738) = iCount + 1;

	((void(*)(_ATOMIC_MODEL*))(*(uintptr_t*)(model->func_tbl + 0x1C)))(model);
	_ATOMIC_MODEL **ms_modelInfoPtrs = (_ATOMIC_MODEL**)(g_libGTASA + /*0x87BF48*/0x91DCB8);
	ms_modelInfoPtrs[iModel] = model;
	return (uintptr_t)model;

	return CModelInfo_AddAtomicModel(iModel);
}

void (*CModelInfo_Initialise)(uintptr_t thiz);
void CModelInfo_Initialise_hook(uintptr_t thiz)
{
	FLog("CModelInfo::Initialise hooked");

	uintptr_t ms_modelInfoPtrs = g_libGTASA + 0x87BF48;
	memset((void*)ms_modelInfoPtrs, 0, sizeof(uintptr_t) * 20000);

	*(uint32_t*)(g_libGTASA + 0x7802C4) = 0;
	*(uint32_t*)(g_libGTASA + 0x843E9C) = 0;
	*(uint32_t*)(g_libGTASA + 0x845430) = 0;
	*(uint32_t*)(g_libGTASA + 0x875B54) = 0;
	*(uint32_t*)(g_libGTASA + 0x77E9C0) = 0;
	*(uint32_t*)(g_libGTASA + 0x83F948) = 0;
	*(uint32_t*)(g_libGTASA + 0x8409B4) = 0;
	*(uint32_t*)(g_libGTASA + 0x8409F4) = 0;
	*(uint32_t*)(g_libGTASA + 0x843194) = 0;
	*(uint32_t*)(g_libGTASA + 0x8431D8) = 0;

	struct ModelInfoHelper {
		static uintptr_t AddAtomicModel(int iModel) {
			return CModelInfo_AddAtomicModel_hook(iModel);
		};

		static void SetColModel(uintptr_t thiz, uintptr_t colModel, bool unk) {
			return ((void(*)(uintptr_t, uintptr_t, bool))(g_libGTASA + 0x335650 + 1))(thiz, colModel, unk);
		}

		static void SetTexDictionary(uintptr_t thiz, const char* name, const char* type) {
			return ((void(*)(uintptr_t, const char*, const char*))(g_libGTASA + 0x3355C0 + 1))(thiz, name, type);
		}
	};

	uintptr_t modelInfo = 0;

	modelInfo = ModelInfoHelper::AddAtomicModel(374);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F44C8, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	modelInfo = ModelInfoHelper::AddAtomicModel(375);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F4498, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	modelInfo = ModelInfoHelper::AddAtomicModel(376);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F4468, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	modelInfo = ModelInfoHelper::AddAtomicModel(377);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F4438, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	modelInfo = ModelInfoHelper::AddAtomicModel(378);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F4408, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	modelInfo = ModelInfoHelper::AddAtomicModel(379);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F43D8, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	modelInfo = ModelInfoHelper::AddAtomicModel(380);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F43A8, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	modelInfo = ModelInfoHelper::AddAtomicModel(381);
	ModelInfoHelper::SetColModel(modelInfo, g_libGTASA + 0x6F4378, false);
	ModelInfoHelper::SetTexDictionary(modelInfo, "generic", "txd");
	*(float*)(modelInfo + 0x30) = 80.0f;

	return;
}

#pragma pack(push, 1)
typedef struct _PED_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[88];
} PED_MODEL; // SIZE = 92
#pragma pack(pop)

PED_MODEL PedsModels[315];
int PedsModelsCount = 0;

PED_MODEL* (*CModelInfo_AddPedModel)(int id);
PED_MODEL* CModelInfo_AddPedModel_hook(int id)
{
	//FLog("Loading skin model %d", id);

	PED_MODEL* model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL));
	model->vtable = (uintptr_t)(g_libGTASA + /*0x5C6E90*/0x667668);

	// CClumpModelInfo::CClumpModelInit()
	((uintptr_t(*)(PED_MODEL*))(*(void**)(model->vtable + 0x1C)))(model);

	*(PED_MODEL**)(g_libGTASA + /*0x87BF48*/0x91DCB8 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	PedsModelsCount++;
	return model;
}

void(*CPools_Initialise)(void);
void CPools_Initialise_hook(void)
{
	FLog("GTA pools initializing..");

	struct PoolAllocator {

		struct Pool {
			void *objects;
			uint8_t *flags;
			uint32_t count;
			uint32_t top;
			uint32_t bInitialized;
		};
		static_assert(sizeof(Pool) == 0x14);

		static Pool* Allocate(size_t count, size_t size) {

			Pool *p = new Pool;

			p->objects = new char[size*count];
			p->flags = new uint8_t[count];
			p->count = count;
			p->top = 0xFFFFFFFF;
			p->bInitialized = 1;

			for (size_t i = 0; i < count; i++) {
				p->flags[i] |= 0x80;
				p->flags[i] &= 0x80;
			}

			return p;
		}
	};

	// 600000 / 75000 = 8
	static auto ms_pPtrNodeSingleLinkPool	= PoolAllocator::Allocate(100000,	8);		// 75000
	// 72000 / 6000 = 12
	static auto ms_pPtrNodeDoubleLinkPool	= PoolAllocator::Allocate(20000,	12);	// 6000
	// 10000 / 500 = 20
	static auto ms_pEntryInfoNodePool		= PoolAllocator::Allocate(20000,	20);	// 500
	// 279440 / 140 = 1996
	static auto ms_pPedPool					= PoolAllocator::Allocate(240,		1996);	// 140
	// 286440 / 110 = 2604
	static auto ms_pVehiclePool				= PoolAllocator::Allocate(2000,		2604);	// 110
	// 840000 / 14000 = 60
	static auto ms_pBuildingPool			= PoolAllocator::Allocate(20000,	60);	// 14000
	// 147000 / 350 = 420
	static auto ms_pObjectPool				= PoolAllocator::Allocate(3000,		420);	// 350
	// 210000 / 3500 = 60
	static auto ms_pDummyPool				= PoolAllocator::Allocate(40000,	60);	// 3500
	// 487200 / 10150 = 48
	static auto ms_pColModelPool			= PoolAllocator::Allocate(50000,	48);	// 10150
	// 64000 / 500 = 128
	static auto ms_pTaskPool				= PoolAllocator::Allocate(5000,		128);	// 500
	// 13600 / 200 = 68
	static auto ms_pEventPool				= PoolAllocator::Allocate(1000,		68);	// 200
	// 6400 / 64 = 100
	static auto ms_pPointRoutePool			= PoolAllocator::Allocate(200,		100);	// 64
	// 13440 / 32 = 420
	static auto ms_pPatrolRoutePool			= PoolAllocator::Allocate(200,		420);	// 32
	// 2304 / 64 = 36
	static auto ms_pNodeRoutePool			= PoolAllocator::Allocate(200,		36);	// 64
	// 512 / 16 = 32
	static auto ms_pTaskAllocatorPool		= PoolAllocator::Allocate(3000,		32);	// 16
	// 92960 / 140 = 664
	static auto ms_pPedIntelligencePool		= PoolAllocator::Allocate(240,		664);	// 140
	// 15104 / 64 = 236
	static auto ms_pPedAttractorPool		= PoolAllocator::Allocate(200,		236);	// 64

	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93E0*/0x95AC38) = ms_pPtrNodeSingleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93DC*/0x95AC3C) = ms_pPtrNodeDoubleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93D8*/0x95AC40) = ms_pEntryInfoNodePool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93D4*/0x95AC44) = ms_pPedPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93D0*/0x95AC48) = ms_pVehiclePool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93CC*/0x95AC4C) = ms_pBuildingPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93C8*/0x95AC50) = ms_pObjectPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93C4*/0x95AC54) = ms_pDummyPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93C0*/0x95AC58) = ms_pColModelPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93BC*/0x95AC5C) = ms_pTaskPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93B8*/0x95AC60) = ms_pEventPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93B4*/0x95AC64) = ms_pPointRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93B0*/0x95AC68) = ms_pPatrolRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93AC*/0x95AC6C) = ms_pNodeRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93A8*/0x95AC70) = ms_pTaskAllocatorPool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93A4*/0x95AC74) = ms_pPedIntelligencePool;
	*(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93A0*/0x95AC78) = ms_pPedAttractorPool;
}

/* =============================================================================== */

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char* name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char* name)
{
	if(pNetGame && !strncmp(name, "CODEWAY", 7))
	{
		float fFindZ = pGame->FindGroundZForCoord(X, Y) + 1.5f;

		if(pNetGame->GetGameState() != GAMESTATE_CONNECTED) return 0;

		RakNet::BitStream bsSend;
		bsSend.Write(X);
		bsSend.Write(Y);
		bsSend.Write(fFindZ);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

/* =============================================================================== */

void(*CRadar_DrawRadarGangOverlay)(uint32_t unk);
void CRadar_DrawRadarGangOverlay_hook(uint32_t unk)
{
	if (pNetGame)
	{
		CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
		if (pGangZonePool) {
			pGangZonePool->Draw(unk);
		}
	}
}

/* =============================================================================== */

#pragma pack(push, 1)
typedef struct {
	VECTOR 		vecPosObject;
	PADDING(_pad1, 16);
	uint16_t 	wModelIndex;
} stLoadObjectInstance;
#pragma pack(pop)

extern int iBuildingToRemoveCount;
extern REMOVEBUILDING_DATA BuildingToRemove[1000];

int (*CFileLoader__LoadObjectInstance)(stLoadObjectInstance *thiz);
int CFileLoader__LoadObjectInstance_hook(stLoadObjectInstance *thiz) {
	if (thiz) {
		if (iBuildingToRemoveCount >= 1) {
			for (int i = 0; i < iBuildingToRemoveCount; i++)
			{
				float fDistance = GetDistance(BuildingToRemove[i].vecPos, thiz->vecPosObject);
				if (fDistance <= BuildingToRemove[i].fRange) {
					if (BuildingToRemove[i].dwModel == -1 || thiz->wModelIndex == (uint16_t) BuildingToRemove[i].dwModel) {
						thiz->wModelIndex = 19300;
						break;
					}
				}
			}
		}
	}

	return CFileLoader__LoadObjectInstance(thiz);
}

/* =============================================================================== */

uint32_t dwParam1, dwParam2;
extern "C" void pickup_pickedup()
{
	if (pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp(((dwParam1 - (g_libGTASA + /*0x70E264*/0x7AFD70)) / 0x20));
	}
}

extern "C" void pickup_orig()
{
	ARMHook::writeMemory(g_libGTASA + 0x31E08C, (uintptr_t)"\x9A\xF8\x1C\x00\x13\x28\x00\xF2\x8F\x80\x01\x21", 12);
}

__attribute__((naked)) void PickupPickUp_hook()
{
	// calculate and save ret address
/*	__asm__ volatile("push {lr}\n\t"
		"push {r0}\n\t"
		"blx get_lib\n\t"
		"add r0, #0x2D0000\n\t"
		"add r0, #0x009A00\n\t"
		"add r0, #1\n\t"
		"mov r1, r0\n\t"
		"pop {r0}\n\t"
		"pop {lr}\n\t"
		"push {r1}\n\t");
*/
	__asm__ volatile(
			"push {lr}\t\n"
			"push {r0}\t\n"
			"blx get_lib\t\n"
			"add r0, #0x310000\n\t"
			"add r0, #0x00E000\n\t"
			"add r0, #0x00008D\n\t"
			"mov r1, r0\n\t"
			"pop {r0}\n\t"
			"pop {lr}\n\t"
			"push {r1}\n\t");

	// 
	__asm__ volatile("push {r0-r11, lr}\n\t"
					 "mov %0, r10" : "=r" (dwParam1));
	__asm__ volatile("blx pickup_pickedup\n\t");
	__asm__ volatile("blx pickup_orig\n\t");
	__asm__ volatile("pop {r0-r11, lr}\n\t");
	__asm__ volatile("pop {pc}");

	// restore
	/*__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
		"sub.w r2, r1, #0xD\n\t"
		"sub.w r2, r1, #8\n\t"
		"cmp r1, #6\n\t"
		"pop {pc}\n\t");
		*/
}

extern "C" void pickup_setuphook()
{
	ARMHook::codeInject(g_libGTASA + /*0x2D99F4*/0x31E08C, (uintptr_t)PickupPickUp_hook, 1);
}

void (*CPickup_Update)(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3);
void CPickup_Update_hook(uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
	pickup_setuphook();
	CPickup_Update(r0, r1, r2, r3);
}

void (*CPlaceable_InitMatrixArray)(void);
void CPlaceable_InitMatrixArray_hook(void)
{
	// CMatrixLinkList::Init
	((void (*)(uintptr_t, size_t))(g_libGTASA + 0x407F84 + 1))(g_libGTASA + 0x95A988, 10000);
}

/* =============================================================================== */
void (*CObject_Render)(uintptr_t thiz);
void CObject_Render_hook(uintptr_t thiz)
{
	ENTITY_TYPE *object = (ENTITY_TYPE*)thiz;
	if(pNetGame && object != 0)
	{
		CObject *pObject = pNetGame->GetObjectPool()->FindObjectFromGtaPtr(object);
		if(pObject)
		{
			RwObject* rwObject = (RwObject*)pObject->GetRWObject();
			if(rwObject)
			{
				// SetObjectMaterial
				if(pObject->m_bHasMaterial)
				{
					((void (*)(void))(g_libGTASA + 0x5D1F48 + 1))();
					//RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialCallBack, pObject);
					RpAtomic* atomic = (RpAtomic*)object->pRpAtomic;
					RpGeometryForAllMaterials(atomic->geometry, ObjectMaterialCallBack, (void*)pObject);
				}
				// SetObjectMaterialText
				if(pObject->m_bHasMaterialText)
					RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialTextCallBack, pObject);
			}


		}
	}

    ((void (*)(void))(g_libGTASA + 0x5D1F48 + 1))();
	CObject_Render(thiz);
    ((void (*)(void))(g_libGTASA + 0x5D1F5C + 1))();
}

/*((void (*)(void))(g_libGTASA + 0x5D1F48 + 1))();
				CObject_Render(thiz);
				// ActivateDirectional
				((void (*)(void))(g_libGTASA + 0x5D1F5C + 1))();*/
/* =============================================================================== */

void (*CGame_Process)();
void CGame_Process_hook()
{
	if(pGame->bIsGameExiting)return;

	CGame_Process();

	if (pNetGame)
	{
		CObjectPool* pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool) {
			pObjectPool->Process();
			pObjectPool->ProcessMaterialText();
		}

		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool) {
			pTextDrawPool->SnapshotProcess();
		}
	}
}

/* =============================================================================== */

bool NotifyEnterVehicle(VEHICLE_TYPE *_pVehicle)
{
	FLog("NotifyEnterVehicle");

	if(!pNetGame) {
		return false;
	}

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool) {
		return false;
	}

	CVehicle *pVehicle = nullptr;
	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);

	if(VehicleID <= 0 || VehicleID >= MAX_VEHICLES) {
		return false;
	}

	if(!pVehiclePool->GetSlotState(VehicleID)) {
		return false;
	}

	pVehicle = pVehiclePool->GetAt(VehicleID);
	if(!pVehicle) {
		return false;
	}

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	if(pLocalPlayer) {
		FLog("Vehicle ID: %d", VehicleID);
		pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);
	}

	return true;
}

extern "C" bool TaskEnterVehicle(uintptr_t pVehicle, uintptr_t a2)
{
	FLog("TaskEnterVehicle");

	if(!NotifyEnterVehicle((VEHICLE_TYPE*)pVehicle)) {
		return false;
	}

	// CTask::operator new
	uintptr_t pTask = ((uintptr_t (*)(void))(g_libGTASA + 0x4D6A01))();

	// CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver
	((void (__fastcall *)(uintptr_t, uintptr_t))(g_libGTASA + 0x4F6F71))(pTask, pVehicle);

	// CTaskManager::SetTask
	((int (__fastcall *)(uintptr_t, uintptr_t, int, int))(g_libGTASA + 0x53390B))(a2, pTask, 3, 0);

	return true;
}

void __attribute__((naked)) TaskEnterVehicle_hook(uintptr_t thiz, uintptr_t pVehicle)
{
	// 2.0
	__asm__ volatile("push {r1-r11, lr}\n\t"
					 "mov r0, r8\n\t"
					 "adds r1, r6, #4\n\t"
					 "blx TaskEnterVehicle\n\t"
					 "pop {r1-r11, lr}\n\t"
					 "blx get_lib\n\t"
					 "add r0, #0x400000\n\t"
					 "add r0, #0xAC00\n\t"
					 "add r0, #0x41\n\t"
					 "mov pc, r0\n\t");
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, VEHICLE_TYPE* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_hook(uintptr_t** thiz, VEHICLE_TYPE* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x409A42+1 || dwRetAddr == 0x40A818+1)
	{
		if (pNetGame)
		{
			if (GamePool_FindPlayerPed()->pVehicle == (uint32_t)pVehicle)
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)GamePool_FindPlayerPed()->pVehicle);
				if (VehicleID != INVALID_VEHICLE_ID)
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
					CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
					if (pVehicle && pLocalPlayer)
					{
						if (pVehicle->IsATrainPart())
						{
							MATRIX4X4 mat;
							pVehicle->GetMatrix(&mat);
							pLocalPlayer->GetPlayerPed()->RemoveFromVehicleAndPutAt(mat.pos.X + 2.5f, mat.pos.Y + 2.5f, mat.pos.Z);
						}
						else
						{
							pLocalPlayer->SendExitVehicleNotification(VehicleID);
						}
					}
				}
			}
		}
	}

	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

/* =============================================================================== */

uint32_t(*CHudColours__GetIntColour)(uint32_t index);
uint32_t CHudColours__GetIntColour_hook(uint32_t index)
{
	return TranslateColorCodeToRGBA(index);
}

/* =============================================================================== */

uint32_t(*Idle)(uint32_t r0, uint32_t r1);
uint32_t Idle_hook(uint32_t r0, uint32_t r1)
{
	uint32_t result = Idle(r0, r1);

	if (pUI) pUI->render();

	RwCameraEndUpdate(*(RwCamera**)(g_libGTASA + 0x9FC93C));

	return result;
}

void (*AND_TouchEvent)(int type, int num, int posX, int posY);
void AND_TouchEvent_hook(int type, int num, int posX, int posY)
{
	// imgui
	//bool bRet = pUI->OnTouchEvent(type, num, posX, posY);

	if (pGame->IsGamePaused())
		return AND_TouchEvent(type, num, posX, posY);

	if (pUI != nullptr)
	{
		switch (type)
		{
			case 2: // push
				pUI->touchEvent(ImVec2(posX, posY), TouchType::push);
				break;

			case 3: // move
				pUI->touchEvent(ImVec2(posX, posY), TouchType::move);
				break;

			case 1: // pop
				pUI->touchEvent(ImVec2(posX, posY), TouchType::pop);
				break;
		}

		if (pUI->keyboard()->visible() || pUI->dialog()->visible()) {
			AND_TouchEvent(1, 0, 0, 0);
			return;
		}
		else
		{
			if (pNetGame && pNetGame->GetTextDrawPool())
			{
				if (!pNetGame->GetTextDrawPool()->onTouchEvent(type, num, posX, posY)) {
					return AND_TouchEvent(1, 0, 0, 0);
				}
			}
		}
	}

	if (pGame->IsGameInputEnabled())
		AND_TouchEvent(type, num, posX, posY);
	else
		AND_TouchEvent(1, 0, 0, 0);
}

/* =============================================================================== */

void (*CWorld_ProcessPedsAfterPreRender)();
void CWorld_ProcessPedsAfterPreRender_Hook()
{
	CWorld_ProcessPedsAfterPreRender();

	if (pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool)
			pPlayerPool->ProcessAttachedObjects();
	}
}

/* =============================================================================== */

void (*CWorld_ProcessAttachedEntities)();
void CWorld_ProcessAttachedEntities_Hook()
{
	if (pNetGame)
	{
		CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if(pLocalPlayer)
		{
			//pLocalPlayer->UpdateSurfingPosition();
		}

		for (PLAYERID i = 0; i < MAX_PLAYERS; i++)
		{
			CRemotePlayer* pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(i);
			if(pRemotePlayer)
			{
				if(pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->GetStateFlags())
				{
					pRemotePlayer->ProcessSurfing();
				}
			}
		}
	}

	CWorld_ProcessAttachedEntities();
}

/* =============================================================================== */

void (*CFileMgr_Initialise)();
void CFileMgr_Initialise_hook()
{
	Log::traceLastFunc("Initializing zip archive");

	// ZIP_FileCreate
	uintptr_t zipFile = ((uintptr_t (*)(const char*))(g_libGTASA + 0x26FE54 + 1))(SAMP_ARCHIVE_PATH);
	Log::addParameter("ZIP handle", zipFile);
	if (zipFile)
	{
		// ZIP_AddStorage
		bool result = ((bool (*)(uintptr_t))(g_libGTASA + 0x26FF70 + 1))(zipFile);
		Log::addParameter("ZIP addstorage result", result);
	}

	CFileMgr_Initialise();
}

/* =============================================================================== */

#include "../java/jniutil.h"
extern CJavaWrapper* pJavaWrapper;

void (*CTimer_StartUserPause)();
void CTimer_StartUserPause_hook()
{
	*(uint8_t*)(g_libGTASA + 0x96B514) = 1;
	if (pUI) pUI->setVisible(false);

	if(pJavaWrapper)
	{
		//pJavaWrapper->SetPauseState(true);
		pJavaWrapper->hideWithoutReset();
	}
}

void (*CTimer_EndUserPause)();
void CTimer_EndUserPause_hook()
{
	*(uint8_t*)(g_libGTASA + 0x96B514) = 0;
	if (pUI) pUI->setVisible(true);
	if(pJavaWrapper)
	{
		pJavaWrapper->showWithoutReset();
	}
}

/* =============================================================================== */

uint32_t(*CTaskSimpleUseGun_SetPedPosition)(uintptr_t thiz, PED_TYPE* pPed);
uint32_t CTaskSimpleUseGun_SetPedPosition_hook(uintptr_t thiz, PED_TYPE* pPed)
{
	dwCurPlayerActor = pPed;
	byteInternalPlayer = *pbyteCurrentPlayer;
	byteCurPlayer = FindPlayerNumFromPedPtr(pPed);

	if (dwCurPlayerActor && byteCurPlayer && byteInternalPlayer == 0)
	{
		uint8_t byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		uint16_t wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) {
			*wCameraMode2 = 0;
		}

		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);

		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(byteCurPlayer);

		*pbyteCurrentPlayer = byteCurPlayer;

		CTaskSimpleUseGun_SetPedPosition(thiz, pPed);

		GameSetLocalPlayerSkills();

		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		GameSetLocalPlayerCameraExtZoomAndAspect();

		*pbyteCurrentPlayer = 0;

		GameSetLocalPlayerAim();
	}
	else
	{
		CTaskSimpleUseGun_SetPedPosition(thiz, pPed);
	}

	return 0;
}

void (*CPed__ProcessControl)(PED_TYPE* thiz);
void CPed__ProcessControl_hook(PED_TYPE* thiz)
{
	dwCurPlayerActor = thiz;
	byteInternalPlayer = *pbyteCurrentPlayer;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

	if (dwCurPlayerActor && (byteCurPlayer != 0) && byteInternalPlayer == 0)
	{
		// REMOTE PLAYER

		uint8_t byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		uint16_t wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) {
			*wCameraMode2 = 0;
		}

		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(byteCurPlayer);
		*pbyteCurrentPlayer = byteCurPlayer;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		//NOP(g_libGTASA + 0x439B7A, 2);
		ARMHook::makeNOP(g_libGTASA + 0x4A2A22, 2);

		// call original
		CPed__ProcessControl(thiz);
		// restore
		//WriteMemory(g_libGTASA + 0x439B7A, (uintptr_t)"\xFA\xF7\x1D\xF8", 4);
		ARMHook::writeMemory(g_libGTASA + 0x4A2A22, (uintptr_t)"\xF0\xF4\x42\xEB", 4);

        GameSetLocalPlayerSkills();
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;
		GameSetLocalPlayerCameraExtZoomAndAspect();
		*pbyteCurrentPlayer = 0;
		GameSetLocalPlayerAim();
	}
	else
	{
		// LOCAL PLAYER

		// Apply the original code to set ped rot from Cam
		//WriteMemory(g_libGTASA + 0x4BED92, (uintptr_t)"\x10\x60", 2);
		ARMHook::writeMemory(g_libGTASA + 0x539BA6, (uintptr_t)"\xC4\xF8\x60\x55", 4);

		(*CPed__ProcessControl)(thiz);

		// Reapply the no ped rots from Cam patch
		//WriteMemory(g_libGTASA + 0x4BED92, (uintptr_t)"\x00\x46", 2);
		ARMHook::makeNOP(g_libGTASA + 0x539BA6, 2);
	}

    return;
}

uint32_t (*CPed__GetWeaponSkill)(PED_TYPE *thiz);
uint32_t CPed__GetWeaponSkill_hook(PED_TYPE *thiz)
{
	bool bWeaponSkillStored = false;

	dwCurPlayerActor = thiz;
	byteInternalPlayer = *pbyteCurrentPlayer;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

	if(dwCurPlayerActor && byteCurPlayer != 0 && byteInternalPlayer == 0)
	{
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(byteCurPlayer);
		bWeaponSkillStored = true;
	}

	// CPed::GetWeaponSkill
	uint32_t result = (( uint32_t (*)(PED_TYPE *, uint32_t))(g_libGTASA+0x4A55E2+1))(thiz, thiz->WeaponSlots[thiz->byteCurWeaponSlot].dwType);

	if(bWeaponSkillStored)
	{
		GameSetLocalPlayerSkills();
		bWeaponSkillStored = false;
	}

	return result;
}

float fSavedBikeLean;
float dwSavedBikeUnk;
MATRIX4X4 *matSavedMatrix;
VECTOR vecSavedMoveSpeed;

void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
	VEHICLE_TYPE* pVehicle = (VEHICLE_TYPE*)thiz;
	uintptr_t this_vtable = pVehicle->entity.vtable;
	this_vtable -= g_libGTASA;

	uintptr_t call_addr = 0;

	switch (this_vtable)
	{
		// CAutomobile
		case /*0x5CC9F0*/0x66D688:
			call_addr =/* 0x4E314C*/0x553DD4;
			break;

			// CBoat
		case /*0x5CCD48*/0x66DA30:
			call_addr = /*0x4F7408*/0x56BE50;
			break;

			// CBike
		case /*0x5CCB18*/0x66D800:
			call_addr = /*0x4EE790*/0x561A20;
			break;

			// CPlane
		case /*0x5CD0B0*/0x66DD94:
			call_addr = /*0x5031E8*/0x575C88;
			break;

			// CHeli
		case /*0x5CCE60*/0x66DB44:
			call_addr = /*0x4FE62C*/0x571238;
			break;

			// CBmx
		case /*0x5CCC30*/0x66D918:
			call_addr = /*0x4F3CE8*/0x568B14;
			break;

			// CMonsterTruck
		case /*0x5CCF88*/0x66DC6C:
			call_addr = /*0x500A34*/0x5747F4;
			break;

			// CQuadBike
		case /*0x5CD1D8*/0x66DEBC:
			call_addr = /*0x505840*/0x57A280;
			break;

			// CTrain
		case /*0x5CD428*/0x66E10C:
			call_addr = /*0x50AB24*/0x57D030;
			break;

			// CTrailer
		case 0x66DFE4:
			call_addr = 0x57B304;
			break;
	}

	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle->pDriver && pVehicle->pDriver->dwPedType == 0 &&
		pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) // CWorld::PlayerInFocus
	{
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		uint8_t byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		uint8_t usSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if(*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->dwPedType == 0 &&
			GamePool_FindPlayerPed() == pVehicle->pDriver)
		{
			if (pVehicle->byteFlags & 0x10)
			{
				pVehicle->entity.nControlFlags &= 0xDF;
			}
			else
			{
				if(call_addr == 0x571238)
					pVehicle->entity.nControlFlags |= 0x20;
			}
		}

		// bike lean
		if(call_addr == 0x561A20 || call_addr == 0x568B14)
		{
			fSavedBikeLean = pVehicle->fBikeLean;
			dwSavedBikeUnk = pVehicle->dwBikeUnk;
		}

		//CWorld::PlayerInFocus
		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->dwPedType = 4;
		uint8_t byteSavedControlFlags = pVehicle->entity.nControlFlags;
		pVehicle->entity.nControlFlags = 0x1A; // fix helicopter sound bug

		//CAEVehicleAudioEntity::Service
		((void (*)(uintptr_t))(g_libGTASA + /*0x364B64*/0x3ACDB4 + 1))(thiz + /*0x138*/0x13C);
		pVehicle->entity.nControlFlags = byteSavedControlFlags;
		pVehicle->pDriver->dwPedType = 0;

		*pbyteCurrentPlayer = byteCurPlayer;

		// matrix pos Z and vecmovespeed Z
		if(call_addr == 0x56BE50 || call_addr == 0x568B14 || call_addr == 0x571238)
		{
			matSavedMatrix = pVehicle->entity.mat;
			vecSavedMoveSpeed = pVehicle->entity.vecMoveSpeed;
		}

		// VEHTYPE::ProcessControl()
		((void (*)(VEHICLE_TYPE*))(g_libGTASA + call_addr + 1))(pVehicle);

		// restore matrix pos Z and vecmovespeed Z
		if(call_addr == 0x56BE50 || call_addr == 0x568B14 || call_addr == 0x571238)
		{
			pVehicle->entity.vecMoveSpeed.Z = vecSavedMoveSpeed.Z;
			pVehicle->entity.mat->pos.Z = matSavedMatrix->pos.Z;
		}

		// restore bike lean.
		if(call_addr == 0x561A20 || call_addr == 0x568B14)
		{
			pVehicle->fBikeLean = fSavedBikeLean;
			pVehicle->dwBikeUnk = dwSavedBikeUnk;

			if((float)pVehicle->dwBikeUnk < pVehicle->fBikeLean)
				pVehicle->fBikeLean = pVehicle->fBikeLean - (pVehicle->fBikeLean - (float)pVehicle->dwBikeUnk) * 0.5;
		}

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = usSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else
	{
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->dwPedType == 0 &&
			GamePool_FindPlayerPed() == pVehicle->pDriver)
		{
			if (pVehicle->byteFlags & 0x10)
			{
				pVehicle->entity.nControlFlags &= 0xDF;
			}
			else
			{
				if(call_addr == 0x571238)
					pVehicle->entity.nControlFlags |= 0x20;
			}
		}

		((void (*)(uintptr_t))(g_libGTASA + /*0x364B64*/0x3ACDB4 + 1))(thiz + /*0x138*/0x13C);

		if (pVehicle->pDriver)
		{
			if (pVehicle->dwFlags.bTyresDontBurst)
			{
				pVehicle->dwFlags.bTyresDontBurst = 0;
			}
			if(!pVehicle->dwFlags.bCanBeDamaged) pVehicle->dwFlags.bCanBeDamaged = true;
		}
		else
		{
			if (!pVehicle->dwFlags.bTyresDontBurst)
			{
				pVehicle->dwFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->dwFlags.bCanBeDamaged) pVehicle->dwFlags.bCanBeDamaged = false;
		}

		// VEHTYPE::ProcessControl()
		((void (*)(VEHICLE_TYPE*))(g_libGTASA + call_addr + 1))(pVehicle);
	}
}

/* =============================================================================== */

extern CPlayerPed* g_pCurrentFiredPed;
extern BULLET_DATA* g_pCurrentBulletData;

extern int g_iLagCompensationMode;

void SendBulletSync(VECTOR* vecOrigin, VECTOR* a2, VECTOR* vecPos, ENTITY_TYPE** ppEntity)
{
    MATRIX4X4 mat1, mat2;

    static BULLET_DATA bulletData;
    memset(&bulletData, 0, sizeof(BULLET_DATA));

    bulletData.vecOrigin.X = vecOrigin->X;
    bulletData.vecOrigin.Y = vecOrigin->Y;
    bulletData.vecOrigin.Z = vecOrigin->Z;

    bulletData.vecPos.X = vecPos->X;
    bulletData.vecPos.Y = vecPos->Y;
    bulletData.vecPos.Z = vecPos->Z;

    if (ppEntity)
    {
        ENTITY_TYPE* pEntity = *ppEntity;
        if (pEntity && pEntity->mat)
        {
            if (g_iLagCompensationMode != 0)
            {
                bulletData.vecOffset.X = vecPos->X - pEntity->mat->pos.X;
                bulletData.vecOffset.Y = vecPos->Y - pEntity->mat->pos.Y;
                bulletData.vecOffset.Z = vecPos->Z - pEntity->mat->pos.Z;
            }
            else
            {
                memset(&mat1, 0, sizeof(MATRIX4X4));
                memset(&mat2, 0, sizeof(MATRIX4X4));
                // RwMatrixOrthoNormalize
                ((void (*)(MATRIX4X4*, MATRIX4X4*))(g_libGTASA + 0x1E34A0 + 1))(&mat2, pEntity->mat);
                // RwMatrixInvert
                ((void (*)(MATRIX4X4*, MATRIX4X4*))(g_libGTASA + 0x1E3A28 + 1))(&mat1, &mat2);
                ProjectMatrix(&bulletData.vecOffset, &mat1, vecPos);
            }

            bulletData.pEntity = pEntity;
        }
    }

    pGame->FindPlayerPed()->ProcessBulletData(&bulletData);
}

extern bool g_customFire;
/* 0.3.7 */
uint32_t(*CWeapon_FireInstantHit)(WEAPON_SLOT_TYPE* thiz, PED_TYPE* pFiringEntity, VECTOR* vecOrigin, VECTOR* muzzlePosn, ENTITY_TYPE* targetEntity,
								  VECTOR* target, VECTOR* originForDriveBy, bool arg6, bool muzzle);
uint32_t CWeapon_FireInstantHit_hook(WEAPON_SLOT_TYPE* thiz, PED_TYPE* pFiringEntity, VECTOR* vecOrigin, VECTOR* muzzlePosn, ENTITY_TYPE* targetEntity,
									 VECTOR* target, VECTOR* originForDriveBy, bool arg6, bool muzzle)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x5DBB6E + 1 ||	// CWeapon::Fire
		dwRetAddr == 0x5DBBC6 + 1)		// CWeapon::Fire
    {
       	if(pFiringEntity != GamePool_FindPlayerPed())
			return muzzle;

		if(pNetGame)
		{
			CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
			if(pPlayerPool)
				pPlayerPool->ApplyCollisionChecking();
		}

		if(pGame)
		{
			CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed)
				pPlayerPed->FireInstant();
		}

		if(pNetGame)
		{
			CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
			if(pPlayerPool)
				pPlayerPool->ResetCollisionChecking();
		}

		return muzzle;
    }

    return CWeapon_FireInstantHit(thiz, pFiringEntity, vecOrigin, muzzlePosn, targetEntity,
                                  target, originForDriveBy, arg6, muzzle);
}

uint32_t(*CWorld_ProcessLineOfSight)(VECTOR*, VECTOR*, VECTOR*, ENTITY_TYPE**, bool, bool, bool, bool, bool, bool, bool, bool);
uint32_t CWorld_ProcessLineOfSight_hook(VECTOR* vecOrigin, VECTOR* vecEnd, VECTOR* vecPos, ENTITY_TYPE** ppEntity,
										bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));

	dwRetAddr -= g_libGTASA;

	LOGW("dwRetAddr 0x%X", dwRetAddr);

	if (dwRetAddr == 0x5DC468 + 1 || // true
		dwRetAddr == 0x5DC5D0 + 1 ||
		dwRetAddr == 0x5DC7A0 + 1 || //
		dwRetAddr == 0x5DCD06 + 1 || //
		dwRetAddr == 0x5DD060 + 1 || // true
		dwRetAddr == 0x5D7294 + 1)	// CBulletInfo::Update
	{
		LOGI("CWorld_ProcessLineOfSight iLagCompensationMode: %d", g_iLagCompensationMode);
		ENTITY_TYPE* pEntity = nullptr;
		static VECTOR vecPosPlusOffset = { 0.0f, 0.0f, 0.0f };

		if (g_iLagCompensationMode != 2)
		{
			if (g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if (g_pCurrentBulletData)
				{
					pEntity = g_pCurrentBulletData->pEntity;
					if (pEntity && pEntity->vtable != (g_libGTASA + 0x667D24)) // CPlaceable
					{
						if (pEntity->mat)
						{
							if (g_iLagCompensationMode)
							{
								vecPosPlusOffset.X = pEntity->mat->pos.X + g_pCurrentBulletData->vecOffset.X;
								vecPosPlusOffset.Y = pEntity->mat->pos.Y + g_pCurrentBulletData->vecOffset.Y;
								vecPosPlusOffset.Z = pEntity->mat->pos.Z + g_pCurrentBulletData->vecOffset.Z;
							}
							else
							{
								ProjectMatrix(&vecPosPlusOffset, pEntity->mat, &g_pCurrentBulletData->vecOffset);
							}

							vecEnd->X = vecPosPlusOffset.X - vecOrigin->X + vecPosPlusOffset.X;
							vecEnd->Y = vecPosPlusOffset.Y - vecOrigin->Y + vecPosPlusOffset.Y;
							vecEnd->Z = vecPosPlusOffset.Z - vecOrigin->Z + vecPosPlusOffset.Z;
						}
					}
				}
			}
		}

		uint32_t result = CWorld_ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

		if (g_iLagCompensationMode == 2)
		{
			if (g_pCurrentFiredPed == pGame->FindPlayerPed()) {
				SendBulletSync(vecOrigin, vecEnd, vecPos, ppEntity);
			}
			return result;
		}

		if (g_pCurrentFiredPed)
		{
			if (g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if (g_pCurrentBulletData)
				{
					if (g_pCurrentBulletData->pEntity == nullptr)
					{
						PED_TYPE* pLocalPed = GamePool_FindPlayerPed();
						if (*ppEntity == (ENTITY_TYPE*)GamePool_FindPlayerPed() ||
							IN_VEHICLE(pLocalPed) && *ppEntity == (ENTITY_TYPE*)pLocalPed->pVehicle)
						{
							result = 0;
							*ppEntity = nullptr;
							vecPos->X = 0.0f;
							vecPos->Y = 0.0f;
							vecPos->Z = 0.0f;
							return result;
						}
					}
				}
			}
			else {
				SendBulletSync(vecOrigin, vecEnd, vecPos, ppEntity);
			}
		}

		return result;
	}

	return CWorld_ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}
// 0.3.7
uint32_t(*CWeapon_FireSniper)(WEAPON_SLOT_TYPE* thiz, PED_TYPE* pFiringEntity, ENTITY_TYPE* victim, VECTOR* target);
uint32_t CWeapon_FireSniper_hook(WEAPON_SLOT_TYPE* thiz, PED_TYPE* pFiringEntity, ENTITY_TYPE* victim, VECTOR* target)
{
	if (pFiringEntity == GamePool_FindPlayerPed())
	{
		if (pGame)
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed) {
				pPlayerPed->FireInstant();
			}
		}
	}

	return true;
}
// 0.3.7
bool(*CBulletInfo_AddBullet)(CEntity* creator, int weaponType, VECTOR pos, VECTOR velocity);
bool CBulletInfo_AddBullet_hook(CEntity* creator, int weaponType, VECTOR pos, VECTOR velocity)
{
	velocity.X *= 50.0f;
	velocity.Y *= 50.0f;
	velocity.Z *= 50.0f;

	CBulletInfo_AddBullet(creator, weaponType, pos, velocity);

	// CBulletInfo::Update
	((void (*)())(g_libGTASA + 0x5D7044 + 1))();
	return true;
}

#pragma pack(push, 1)
struct CPedDamageResponseCalculator
{
	PED_TYPE* m_pDamager;
	float m_fDamageFactor;
	int m_pedPieceType;
	int m_weaponType;
};
#pragma pack(pop)
// 0.3.7
bool ComputeDamageResponse(CPedDamageResponseCalculator* calculator, PED_TYPE* pPed)
{
	PED_TYPE* pGamePed = GamePool_FindPlayerPed();
	bool isLocalPed = false;

	if (!pNetGame) return false;

	PED_TYPE* pDamager = calculator->m_pDamager;
	if (pDamager != pGamePed && (pPed && pPed->entity.vtable == (g_libGTASA + 0x668AA4))) /* CCivilianPed */
		return true;

	if (pPed == pGamePed) {
		isLocalPed = true;
	}
	else if (pDamager != pGamePed) {
		return false;
	}

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	PLAYERID PlayerID;

	if (isLocalPed)
	{
		PlayerID = FindPlayerIDFromGtaPtr(&pDamager->entity);
		pLocalPlayer->SendTakeDamageEvent(PlayerID,
										  calculator->m_fDamageFactor,
										  calculator->m_weaponType,
										  calculator->m_pedPieceType);
	}
	else
	{
		PlayerID = FindPlayerIDFromGtaPtr(&pPed->entity);
		if (PlayerID != INVALID_PLAYER_ID)
		{
			pLocalPlayer->SendGiveDamageEvent(PlayerID,
											  calculator->m_fDamageFactor,
											  calculator->m_weaponType,
											  calculator->m_pedPieceType);
			if (pPlayerPool->GetAt(PlayerID)->IsNPC())
				return true;
		}
		else
		{
			PLAYERID ActorID = FindActorIDFromGtaPtr(pPed);
			if (ActorID != INVALID_PLAYER_ID) {
				pLocalPlayer->SendGiveDamageEvent(ActorID,
												  calculator->m_fDamageFactor,
												  calculator->m_weaponType,
												  calculator->m_pedPieceType);
				return true;
			}
		}
	}


	// :check_friendly_fire
	if (!pNetGame->m_pNetSet->bFriendlyFire)
		return false;
	uint8_t byteTeam = pPlayerPool->GetLocalPlayer()->m_byteTeam;
	if (byteTeam == NO_TEAM ||
		PlayerID == INVALID_PLAYER_ID ||
		pPlayerPool->GetAt(PlayerID)->m_byteTeam != byteTeam) {
		return false;
	}

	return true;
}

// 0.3.7
void (*CPedDamageResponseCalculator__ComputeDamageResponse)(CPedDamageResponseCalculator* thiz, PED_TYPE* pPed, uint32_t a3, uint32_t a4);
void CPedDamageResponseCalculator__ComputeDamageResponse_hook(CPedDamageResponseCalculator* thiz, PED_TYPE* pPed, uint32_t a3, uint32_t a4)
{
	if (thiz != nullptr && pPed != nullptr)
	{
		if (ComputeDamageResponse(thiz, pPed))
			return;
	}

	return CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pPed, a3, a4);
}

void (*CRenderer_RenderEverythingBarRoads)();
void CRenderer_RenderEverythingBarRoads_hook() {

	CRenderer_RenderEverythingBarRoads();

	if (pNetGame) {
		CObjectPool* pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool) {
			for (OBJECTID i = 0; i < MAX_OBJECTS; i++) {
				CObject* pObject = pObjectPool->GetAt(i);
				if (pObject && pObject->m_bForceRender) {
					pObject->Render();
				}
			}
		}
	}
}

#include "CFPSFix.h"
CFPSFix g_fps;

void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1)
{
	g_fps.PushThread(gettid());

	ANDRunThread(a1);
}

static constexpr float ar43 = 4.0f/3.0f;
float *ms_fAspectRatio;
void (*DrawCrosshair)(uintptr_t* thiz);
void DrawCrosshair_hook(uintptr_t* thiz)
{
	float save1 = *CCamera::m_f3rdPersonCHairMultX;
	*CCamera::m_f3rdPersonCHairMultX = 0.530f - (*ms_fAspectRatio - ar43) * 0.01125f;

	float save2 = *CCamera::m_f3rdPersonCHairMultY;
	*CCamera::m_f3rdPersonCHairMultY = 0.400f + (*ms_fAspectRatio - ar43) * 0.03600f;

	DrawCrosshair(thiz);

	*CCamera::m_f3rdPersonCHairMultX = save1;
	*CCamera::m_f3rdPersonCHairMultY = save2;
}

VECTOR& (*FindPlayerSpeed)(int a1);
VECTOR& FindPlayerSpeed_hook(int a1)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x43E1F6 + 1)
	{
		if(pNetGame)
		{
			CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed &&
			   pPlayerPed->IsInVehicle() &&
			   pPlayerPed->IsAPassenger())
			{
				VECTOR vec = _VECTOR(-1.0f);
				return vec;
			}
		}
	}

	return FindPlayerSpeed(a1);
}

int (*RwFrameAddChild)(int a1, int a2);
int RwFrameAddChild_hook(int a1, int a2)
{
	if(a1 == 0 || a2 == 0) return 0;
	return RwFrameAddChild(a1, a2);
}

// widget fix
uintptr_t (*CWidget)(uintptr_t thiz, const char* name, int a3, int a4, int a5, int a6);
uintptr_t CWidget_hook(uintptr_t thiz, const char* name, int a3, int a4, int a5, int a6)
{
	// Log(OBFUSCATE("[debug:hooks:CWidget]: New Widget: \"%s\" 0x%X"), name, thiz-g_libGTASA);

	SetWidgetFromName(name, thiz);
	return CWidget(thiz, name, a3, a4, a5, a6);
}

void (*CWidget__Update)(uintptr_t pWidget);
void CWidget__Update_hook(uintptr_t pWidget)
{
	if(pNetGame)
	{
		switch(ProcessFixedWidget(pWidget))
		{
			case STATE_NONE: break;
			case STATE_FIXED: return;
		}
	}

	CWidget__Update(pWidget);
}

void (*CWidget__SetEnabled)(uintptr_t pWidget, bool bEnabled);
void CWidget__SetEnabled_hook(uintptr_t pWidget, bool bEnabled)
{
	if(pNetGame)
	{
		switch(ProcessFixedWidget(pWidget))
		{
			case STATE_NONE: break;
			case STATE_FIXED:
				bEnabled = false;
				break;
		}
	}

	CWidget__SetEnabled(pWidget, bEnabled);
}

int iLastTouchedWidgetId = -1;

int (*CTouchInterface__IsTouched)(int iWidgetId, int iUnk, int iEnableWidget);
int CTouchInterface__IsTouched_hook(int iWidgetId, int iUnk, int iEnableWidget)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(pNetGame)
	{
		switch(ProcessFixedWidgetFromId(iWidgetId))
		{
			case STATE_NONE: break;
			case STATE_FIXED:
				iEnableWidget = 0;
				break;
		}
	}

	int iTouched = CTouchInterface__IsTouched(iWidgetId, iUnk, iEnableWidget);
	if(iTouched && iEnableWidget)
	{
		iLastTouchedWidgetId = iWidgetId;
	}

	return iTouched;
}

int iLastReleasedWidgetId = -1;

int (*CTouchInterface__IsReleased)(int iWidgetId, int iUnk, int iEnableWidget);
int CTouchInterface__IsReleased_hook(int iWidgetId, int iUnk, int iEnableWidget)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	int iReleased = CTouchInterface__IsReleased(iWidgetId, iUnk, iEnableWidget);
	if(iReleased && iEnableWidget)
	{
		iLastReleasedWidgetId = iWidgetId;
	}

	return iReleased;
}

int (*CTextureDatabaseRuntime__GetEntry)(uintptr_t thiz, const char* a2, bool* a3);
int CTextureDatabaseRuntime__GetEntry_hook(uintptr_t thiz, const char* a2, bool* a3)
{
	if (!thiz)
	{
		return -1;
	}
	return CTextureDatabaseRuntime__GetEntry(thiz, a2, a3);
}

uintptr_t (*CTxdStore__TxdStoreFindCB)(const char *a1);
uintptr_t CTxdStore__TxdStoreFindCB_hook(const char *a1)
{
	static char* texdbs[] = { "samp", "gta_int", "gta3" };
	for(auto &texdb : texdbs)
	{
		// TextureDatabaseRuntime::GetDatabase
		uintptr_t db_handle = ((uintptr_t (*)(const char *))(g_libGTASA+0x1EAC8C+1))(texdb);

		// TextureDatabaseRuntime::registered
		uint32_t unk_61B8D4 = *(uint32_t*)(g_libGTASA+0x6BD174+4);
		if(unk_61B8D4)
		{
			// TextureDatabaseRuntime::registered
			uintptr_t dword_61B8D8 = *(uintptr_t*)(g_libGTASA+0x6BD174+8);

			int index = 0;
			while(*(uint32_t*)(dword_61B8D8 + 4 * index) != db_handle)
			{
				if(++index >= unk_61B8D4)
					goto GetTheTexture;
			}

			continue;
		}

		GetTheTexture:
		// TextureDatabaseRuntime::Register
		((void (*)(int))(g_libGTASA+0x1E9BC8+1))(db_handle);

		// TextureDatabaseRuntime::GetTexture
		uintptr_t tex = ((uintptr_t (*)(const char *))(g_libGTASA+0x1E9CE4+1))(a1);

		// TextureDatabaseRuntime::Unregister
		((void (*)(int))(g_libGTASA+0x1E9C80+1))(db_handle);

		if(tex) return tex;
	}

	// RwTexDictionaryGetCurrent
	int current = ((int (*)(void))(g_libGTASA+0x1DBA64+1))();
	if(current)
	{
		while(true)
		{
			// RwTexDictionaryFindNamedTexture
			uintptr_t tex = ((int (*)(int, const char *))(g_libGTASA+0x1DB9B0+1))(current, a1);
			if(tex) return tex;

			// CTxdStore::GetTxdParent
			current = ((int (*)(int))(g_libGTASA+0x5D428C+1))(current);
			if(!current) return 0;
		}
	}

	return 0;
}

int (*CCustomRoadsignMgr_RenderRoadsignAtomic)(int a1, int a2);
int CCustomRoadsignMgr_RenderRoadsignAtomic_hook(int a1, int a2)
{
	if ( a1 )
		return CCustomRoadsignMgr_RenderRoadsignAtomic(a1, a2);
}

int (*_RwTextureDestroy)(int a1);
int _RwTextureDestroy_hook(int a1)
{
	int result; // r0

	if ( (unsigned int)(a1 + 1) >= 2 )
		result = _RwTextureDestroy(a1);
	else
		result = 0;
	return result;
}

int (*CPed_UpdatePosition)(PED_TYPE* a1);
int CPed_UpdatePosition_hook(PED_TYPE* a1)
{
	int result; // r0

	if ( GamePool_FindPlayerPed() == a1 )
		result = CPed_UpdatePosition(a1);
	return result;
}

int (*RpClumpForAllAtomics)(int a1);
int RpClumpForAllAtomics_hook(int a1)
{
	int result; // r0

	if ( a1 )
		result = RpClumpForAllAtomics(a1);
	else
		result = 0;
	return result;
}

void (*CCamera__Process)(uintptr_t thiz);
void CCamera__Process_hook(uintptr_t thiz)
{
	if(pGame->GetCamera())
		pGame->GetCamera()->Update();

	CCamera__Process(thiz);
}

extern char* WORLD_PLAYERS;
uint32_t (*CWorld__FindPlayerSlotWithPedPointer)(unsigned int a1);
uint32_t CWorld__FindPlayerSlotWithPedPointer_hook(unsigned int a1)
{
	uint32_t result = 0;

	uint32_t *dwWorldPlayers = (uint32_t*)WORLD_PLAYERS;
	while(*dwWorldPlayers != a1)
	{
        ++result;
		dwWorldPlayers += 101;
		if(result > 210)
			return 0;
	}

	return result;
}

#include "java/jniutil.h"
extern CJavaWrapper* pJavaWrapper;
void (*MainMenuScreen__OnExit)();
void MainMenuScreen__OnExit_hook()
{
	pGame->bIsGameExiting = true;

	pNetGame->GetRakClient()->Disconnect(0);

	pJavaWrapper->exitGame();
}

int (*mpg123_param)(void* mh, int key, long val, int ZERO, double fval);
int mpg123_param_hook(void* mh, int key, long val, int ZERO, double fval) {
	// 0x2000 = MPG123_SKIP_ID3V2
	// 0x200  = MPG123_FUZZY
	// 0x100  = MPG123_SEEKBUFFER
	// 0x40   = MPG123_GAPLESS
	return mpg123_param(mh, key, val | (0x2000 | 0x200 | 0x100 | 0x40), ZERO, fval);
}

void (*rqVertexBufferSelect)(unsigned int **result);
void rqVertexBufferSelect_hook(unsigned int **result)
{
	uint32_t buffer = *(uint32_t *)*result;
	*result += 4;
	if ( buffer )
	{
		glBindBuffer(34962, *(uint32_t *)(buffer + 8));
		*(uint32_t*)(g_libGTASA + 0x6B8AF0) = 0;
	}
	else
	{
		glBindBuffer(34962, 0);
	}
}

uintptr_t* (*rpMaterialListDeinitialize)(RpMaterialList* matList);
uintptr_t* rpMaterialListDeinitialize_hook(RpMaterialList* matList)
{
	if(!matList || !matList->materials)
		return nullptr;

	return rpMaterialListDeinitialize(matList);
}

void (*rqVertexBufferDelete)(unsigned int **result);
void rqVertexBufferDelete_hook(unsigned int **result)
{
	uint32_t* buffer = *(uint32_t **)*result;
	*result += 4;
	glDeleteBuffers(1, reinterpret_cast<const GLuint *>(buffer + 2));
	buffer[2] = 0;
	if ( buffer )
		(*(void (**)(uint32_t *))(*buffer + 4))(buffer);
}

int (*rqSetAlphaTest)(char **a1);
int rqSetAlphaTest_hook(char **a1)
{
	if ( ((int (*)(const char *))(g_libGTASA + 0x19E2CC))("glAlphaFuncQCOM") )
		return rqSetAlphaTest(a1);
	char *result = *a1 + 8;
	*a1 = result;
	return (int)result;
}

void rotate_ped_if_local(unsigned int *a1, unsigned int *a2)
{
	if ( GamePool_FindPlayerPed() == (PED_TYPE*)a2 )
		*(uint32_t *)(a2 + 0x560) = *a1;
}

void (*player_control_zelda)(unsigned int *a2, unsigned int *a3);
void player_control_zelda_hook(unsigned int *a2, unsigned int *a3)
{
	rotate_ped_if_local(a2, a3);
}

#pragma pack(push, 1)
typedef struct _RES_ENTRY_OBJ
{
	PADDING(_pad0, 48); // 0-48
	uintptr_t validate; // 48-52
	PADDING(_pad1, 4); // 52-56
} RES_ENTRY_OBJ;
#pragma pack(pop)

// 006778B0
int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if(!resEntry || !flags)
		return 0;

	uint16_t size = *(uint16_t *)(resEntry+26);
	if(size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ*)(resEntry+28);
		if(arr)
		{
			uint32_t validFlag = flags & 0x84;
			if(validFlag)
			{
				for(int i = 0; i < size; i++)
				{
					if(!arr[i].validate) break;

					uintptr_t *v4 = *(uintptr_t**)(arr[i].validate);
					if(v4)
					{
						if(!*v4 || v4 > (uintptr_t*)0xFFFFFF00)
							return 0;
					}
				}
			}
		}
	}

	return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

// 00677CB4
int (*CCustomBuildingDNPipeline__CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CCustomBuildingDNPipeline__CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if(!resEntry || !flags)
		return 0;

	uint16_t size = *(uint16_t *)(resEntry+26);
	if(size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ*)(resEntry+28);
		if(arr)
		{
			uint32_t validFlag = flags & 0x84;
			if(validFlag)
			{
				for(int i = 0; i < size; i++)
				{
					if(!arr[i].validate) break;

					uintptr_t *v4 = *(uintptr_t**)(arr[i].validate);
					if(v4)
					{
						if(!*v4 || v4 > (uintptr_t*)0xFFFFFF00)
							return 0;
					}
				}
			}
		}
	}

	return CCustomBuildingDNPipeline__CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*EmuShader_Select)(uintptr_t *result);
int EmuShader_Select_hook(uintptr_t *result)
{
	int result1;
	if ( *result >= 0x1000 )
		return EmuShader_Select(result);
	return 0;
}

float float_4DD9E8;
float ms_fTimeStep;
float fMagic = 50.0f / 30.0f;
void (*CTaskSimpleUseGun__SetMoveAnim)(uintptr_t *thiz, uintptr_t *a2);
void CTaskSimpleUseGun__SetMoveAnim_hook(uintptr_t *thiz, uintptr_t *a2)
{
	ms_fTimeStep = *(float*)(g_libGTASA + 0x96B500);
	float_4DD9E8 = *(float*)(g_libGTASA + 0x4DD9E8);
	float_4DD9E8 = (fMagic) * (0.1f / ms_fTimeStep);
	CTaskSimpleUseGun__SetMoveAnim(thiz, a2);
}

int (*CAnimManager_UncompressAnimation)(int result);
int CAnimManager_UncompressAnimation_hook(int result)
{
	if ( result )
		return CAnimManager_UncompressAnimation(result);
	return 0;
}

// CStreaming::ms_memoryUsed	00792B74
// CStreaming::ms_memoryAvailable	00685FA0
// CStreaming::RemoveLeastUsedModel(uint)	002D549C
// CStreaming::MakeSpaceFor(int)	002D3974
void (*CStreaming__MakeSpaceFor)(uintptr_t *thiz, size_t memoryToCleanInBytes);
void CStreaming__MakeSpaceFor_hook(uintptr_t *thiz, size_t memoryToCleanInBytes)
{
    size_t ms_memoryUsed = *(size_t*)(g_libGTASA+0x00792B74);
    size_t ms_memoryAvailable = *(size_t*)(g_libGTASA+0x00685FA0);
    auto lastmemused = ms_memoryUsed;
    while (ms_memoryUsed >= ms_memoryAvailable - memoryToCleanInBytes) {
        lastmemused = ms_memoryUsed;
        if (!((int (*)(uintptr_t*, unsigned int))(g_libGTASA + 0x2D549C+1))(thiz, 0) || lastmemused == ms_memoryUsed) {
            //  DeleteRwObjectsBehindCamera(ms_memoryAvailable - memoryToCleanInBytes);
            return;
        }
    }
}

void (*CStreaming_Init2)();
void CStreaming_Init2_hook()
{
	CStreaming_Init2();
	ARMHook::unprotect(g_libGTASA + 0x685FA0);
	*(uint32_t *)(g_libGTASA + 0x685FA0) *= 3;
}

void readVehiclesAudioSettings();

void (*CVehicleModelInfo__SetupCommonData)();

void CVehicleModelInfo__SetupCommonData_hook() {
	CVehicleModelInfo__SetupCommonData();
	readVehiclesAudioSettings();
}

extern VehicleAudioPropertiesStruct VehicleAudioProperties[20000];
static uintptr_t addr_veh_audio = (uintptr_t) &VehicleAudioProperties[0];

void (*CAEVehicleAudioEntity__GetVehicleAudioSettings)(uintptr_t thiz, int16_t a2, int a3);

void CAEVehicleAudioEntity__GetVehicleAudioSettings_hook(uintptr_t dest, int16_t a2, int ID) {
	memcpy((void *) dest, &VehicleAudioProperties[(ID - 400)], sizeof(VehicleAudioPropertiesStruct));
}

void (*CRadar_ClearBlip)(uint32_t a2);
void CRadar_ClearBlip_hook(uint32_t a2)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	//LOGI("[CRadar::ClearBlip]: %d called from 0x%X", (uint16_t)a2, dwRetAddr);

	if ( (uint16_t)a2 > 249 )
	{
		LOGI("[CRadar::ClearBlip]: Invalid blip ID (%d) called from 0x%X", (uint16_t)a2, dwRetAddr);
	}
	else
	{
		CRadar_ClearBlip(a2);
	}
}

/* =============================================================================== */

void InstallHuaweiCrashFixHooks()
{
	ARMHook::installPLTHook(g_libGTASA + 0x677498, (uintptr_t)rqVertexBufferSelect_hook, (uintptr_t*)&rqVertexBufferSelect);
	ARMHook::installPLTHook(g_libGTASA + 0x679B14, (uintptr_t)rqVertexBufferDelete_hook, (uintptr_t*)&rqVertexBufferDelete);
	ARMHook::installPLTHook(g_libGTASA + 0x677B6C, (uintptr_t)rqSetAlphaTest_hook, (uintptr_t*)&rqSetAlphaTest);
}

void InstallCrashFixHooks()
{
	// some crashfixes
	ARMHook::installPLTHook(g_libGTASA + 0x66F5AC, (uintptr_t)CCustomRoadsignMgr_RenderRoadsignAtomic_hook, (uintptr_t*)&CCustomRoadsignMgr_RenderRoadsignAtomic);
	ARMHook::installPLTHook(g_libGTASA + 0x67332C, (uintptr_t)_RwTextureDestroy_hook, (uintptr_t*)&_RwTextureDestroy);
	ARMHook::installPLTHook(g_libGTASA + 0x671458, (uintptr_t)CPed_UpdatePosition_hook, (uintptr_t*)&CPed_UpdatePosition);
	ARMHook::installPLTHook(g_libGTASA + 0x675490, (uintptr_t)RwFrameAddChild_hook, (uintptr_t*)&RwFrameAddChild);
	ARMHook::installPLTHook(g_libGTASA + 0x672D14, (uintptr_t)CTextureDatabaseRuntime__GetEntry_hook, (uintptr_t*)&CTextureDatabaseRuntime__GetEntry);
	//ARMHook::installPLTHook(g_libGTASA + 0x66FBD0, (uintptr_t)RpClumpForAllAtomics_hook, (uintptr_t*)&RpClumpForAllAtomics);
	ARMHook::installPLTHook(g_libGTASA + 0x6730F0, (uintptr_t)rpMaterialListDeinitialize_hook, (uintptr_t*)&rpMaterialListDeinitialize);
	//ARMHook::installPLTHook(g_libGTASA + 0x6778B0, (uintptr_t)rxOpenGLDefaultAllInOneRenderCB_hook, (uintptr_t*)&rxOpenGLDefaultAllInOneRenderCB);
	//ARMHook::installPLTHook(g_libGTASA + 0x677CB4, (uintptr_t)CCustomBuildingDNPipeline__CustomPipeRenderCB_hook, (uintptr_t*)&CCustomBuildingDNPipeline__CustomPipeRenderCB);
	//ARMHook::installPLTHook(g_libGTASA + 0x66F9E8, (uintptr_t)EmuShader_Select_hook, (uintptr_t*)&EmuShader_Select);
	ARMHook::installPLTHook(g_libGTASA + 0x6750D4, (uintptr_t)CAnimManager_UncompressAnimation_hook, (uintptr_t*)&CAnimManager_UncompressAnimation);
	//ARMHook::installPLTHook(g_libGTASA + 0x670E1C, (uintptr_t)CStreaming__MakeSpaceFor_hook, (uintptr_t*)&CStreaming__MakeSpaceFor);
	ARMHook::installPLTHook(g_libGTASA + 0x6700D0, (uintptr_t)CStreaming_Init2_hook, (uintptr_t*)&CStreaming_Init2);
}

void InstallWeaponFireHooks()
{
	ARMHook::installPLTHook(g_libGTASA + 0x6716D0, (uintptr_t)CWeapon_FireInstantHit_hook, (uintptr_t*)&CWeapon_FireInstantHit);
	ARMHook::installPLTHook(g_libGTASA + 0x671F10, (uintptr_t)CWorld_ProcessLineOfSight_hook, (uintptr_t*)&CWorld_ProcessLineOfSight);
	ARMHook::installPLTHook(g_libGTASA + 0x670A10, (uintptr_t)CWeapon_FireSniper_hook, (uintptr_t*)&CWeapon_FireSniper);
	ARMHook::installPLTHook(g_libGTASA + 0x66EAC4, (uintptr_t)CBulletInfo_AddBullet_hook, (uintptr_t*)&CBulletInfo_AddBullet);
}

void InstallSAMPHooks()
{
	ARMHook::installPLTHook(g_libGTASA + 0x677EA0, (uintptr_t)MainMenuScreen__OnExit_hook, (uintptr_t*)&MainMenuScreen__OnExit);
	// samp main loop
	ARMHook::installPLTHook(g_libGTASA + 0x67589C, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
	// imgui
	ARMHook::installPLTHook(g_libGTASA + 0x6710C4, (uintptr_t)Idle_hook, (uintptr_t*)&Idle);
	ARMHook::installPLTHook(g_libGTASA + 0x675DE4, (uintptr_t)AND_TouchEvent_hook, (uintptr_t*)&AND_TouchEvent);
	// splashscreen
	ARMHook::installHook(g_libGTASA + 0x43AF28, (uintptr_t)DisplayScreen_hook, (uintptr_t*)&DisplayScreen);
	// gangzones
	ARMHook::installPLTHook(g_libGTASA + 0x67196C, (uintptr_t)CRadar_DrawRadarGangOverlay_hook, (uintptr_t*)&CRadar_DrawRadarGangOverlay);
	// radar
	ARMHook::installPLTHook(g_libGTASA+0x675914, (uintptr_t)CRadar__SetCoordBlip_hook, (uintptr_t*)&CRadar__SetCoordBlip);
	// removebuilding
	ARMHook::installPLTHook(g_libGTASA + 0x675E6C, (uintptr_t)CFileLoader__LoadObjectInstance_hook, (uintptr_t*)&CFileLoader__LoadObjectInstance);
	// pickup
	ARMHook::installPLTHook(g_libGTASA + 0x66EC10, (uintptr_t)CPickup_Update_hook, (uintptr_t*)& CPickup_Update);
	// obj material
	ARMHook::installHook(g_libGTASA + 0x454EF0, (uintptr_t)CObject_Render_hook, (uintptr_t*)& CObject_Render);
	// textdraw models
	ARMHook::installPLTHook(g_libGTASA + 0x66FE58, (uintptr_t)CGame_Process_hook, (uintptr_t*)& CGame_Process);
	// enter vehicle as driver
	ARMHook::codeInject(g_libGTASA + 0x40AC28, (uintptr_t)TaskEnterVehicle_hook, 0);
    //ARMHook::installPLTHook(g_libGTASA+0x6733F0, (uintptr_t)TaskEnterVehicle_hook, (uintptr_t*)&TaskEnterVehicle);
	// radar color
	ARMHook::installPLTHook(g_libGTASA + 0x673950, (uintptr_t)CHudColours__GetIntColour_hook, (uintptr_t*)& CHudColours__GetIntColour);
	// exit vehicle
	ARMHook::installPLTHook(g_libGTASA + 0x671984, (uintptr_t)CTaskComplexLeaveCar_hook, (uintptr_t*)& CTaskComplexLeaveCar);
    ARMHook::installPLTHook(g_libGTASA + 0x675320, (uintptr_t)CTaskComplexLeaveCar_hook, (uintptr_t*)& CTaskComplexLeaveCar);
    // attach obj to ped
	ARMHook::installPLTHook(g_libGTASA + 0x675C68, (uintptr_t)CWorld_ProcessPedsAfterPreRender_Hook, (uintptr_t*)&CWorld_ProcessPedsAfterPreRender);
	// game pause
	ARMHook::installPLTHook(g_libGTASA + 0x672644, (uintptr_t)CTimer_StartUserPause_hook, (uintptr_t*)&CTimer_StartUserPause);
	ARMHook::installPLTHook(g_libGTASA + 0x67056C, (uintptr_t)CTimer_EndUserPause_hook, (uintptr_t*)&CTimer_EndUserPause);
	// aim
	ARMHook::installPLTHook(g_libGTASA + 0x66969C, (uintptr_t)CTaskSimpleUseGun_SetPedPosition_hook, (uintptr_t*)&CTaskSimpleUseGun_SetPedPosition);
	// CPlayerPed::ProcessControl
	ARMHook::installPLTHook(g_libGTASA + 0x6692B4, (uintptr_t)CPed__ProcessControl_hook, (uintptr_t*)&CPed__ProcessControl);
	// all vehicles ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CCA1C*/0x66D6B4, (uintptr_t)AllVehicles__ProcessControl_hook); // CAutomobile::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CCD74*/0x66DA5C, (uintptr_t)AllVehicles__ProcessControl_hook); // CBoat::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CCB44*/0x66D82C, (uintptr_t)AllVehicles__ProcessControl_hook); // CBike::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CD0DC*/0x66DDC0, (uintptr_t)AllVehicles__ProcessControl_hook); // CPlane::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CCE8C*/0x66DB70, (uintptr_t)AllVehicles__ProcessControl_hook); // CHeli::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CCC5C*/0x66D944, (uintptr_t)AllVehicles__ProcessControl_hook); // CBmx::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CCFB4*/0x66DC98, (uintptr_t)AllVehicles__ProcessControl_hook); // CMonsterTruck::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CD204*/0x66DEE8, (uintptr_t)AllVehicles__ProcessControl_hook); // CQuadBike::ProcessControl
	ARMHook::installMethodHook(g_libGTASA + /*0x5CD454*/0x66E138, (uintptr_t)AllVehicles__ProcessControl_hook); // CTrain::ProcessControl
	// ComputeDamageResponse
	ARMHook::installPLTHook(g_libGTASA + 0x66F0EC, (uintptr_t)CPedDamageResponseCalculator__ComputeDamageResponse_hook, (uintptr_t*)&CPedDamageResponseCalculator__ComputeDamageResponse);

	// Crosshair Fix
	ms_fAspectRatio = (float*)(g_libGTASA+0xA26A90);
	ARMHook::installPLTHook(g_libGTASA + 0x672880, (uintptr_t)DrawCrosshair_hook, (uintptr_t*)&DrawCrosshair);

	// fix radar in passenger
	ARMHook::installPLTHook(g_libGTASA+0x671BBC, (uintptr_t)FindPlayerSpeed_hook, (uintptr_t*)&FindPlayerSpeed);

	// fix texture loading
	ARMHook::installPLTHook(g_libGTASA + 0x676034, (uintptr_t)CTxdStore__TxdStoreFindCB_hook, (uintptr_t*)&CTxdStore__TxdStoreFindCB);

	// interpolate camera fix
	ARMHook::installPLTHook(g_libGTASA + 0x6717BC, (uintptr_t)CCamera__Process_hook, (uintptr_t*)&CCamera__Process);

	// for surfing
	//ARMHook::installPLTHook(g_libGTASA + 0x66EAE8, (uintptr_t)CWorld_ProcessAttachedEntities_Hook, (uintptr_t*)&CWorld_ProcessAttachedEntities);

	//ARMHook::installPLTHook(g_libGTASA + 0x67193C, (uintptr_t)player_control_zelda_hook, (uintptr_t*)&player_control_zelda);

	ARMHook::installHook(g_libGTASA + 0x4DD5E8, (uintptr_t)CTaskSimpleUseGun__SetMoveAnim_hook, (uintptr_t*)&CTaskSimpleUseGun__SetMoveAnim);

    // hueta ne rabotaet no pust budet (tipo ne kak v 1.08)
	ARMHook::installPLTHook(g_libGTASA + 0x674280, (uintptr_t) CVehicleModelInfo__SetupCommonData_hook, (uintptr_t*)&CVehicleModelInfo__SetupCommonData);
	ARMHook::installPLTHook(g_libGTASA + 0x06D008, (uintptr_t) CAEVehicleAudioEntity__GetVehicleAudioSettings_hook, (uintptr_t*)&CAEVehicleAudioEntity__GetVehicleAudioSettings);

	ARMHook::installPLTHook(g_libGTASA + 0x66FF0C, (uintptr_t)CRadar_ClearBlip_hook, (uintptr_t*)&CRadar_ClearBlip);

	// skills
	ARMHook::installPLTHook(g_libGTASA + 0x6749D0, (uintptr_t)CPed__GetWeaponSkill_hook, (uintptr_t*)&CPed__GetWeaponSkill);

    //InstallHuaweiCrashFixHooks();
	InstallCrashFixHooks();
	InstallWeaponFireHooks();
	HookCPad();
}

void InstallWidgetHooks()
{
	ARMHook::installPLTHook(g_libGTASA+0x66F660, (uintptr_t)CWidget_hook, (uintptr_t*)&CWidget);
	ARMHook::installPLTHook(g_libGTASA+0x66FBCC, (uintptr_t)CWidget__Update_hook, (uintptr_t*)&CWidget__Update);
	ARMHook::installPLTHook(g_libGTASA+0x672A0C, (uintptr_t)CWidget__SetEnabled_hook, (uintptr_t*)&CWidget__SetEnabled);
	ARMHook::installPLTHook(g_libGTASA+0x674388, (uintptr_t)CTouchInterface__IsTouched_hook, (uintptr_t*)&CTouchInterface__IsTouched);
	ARMHook::installPLTHook(g_libGTASA+0x66F6E0, (uintptr_t)CTouchInterface__IsReleased_hook, (uintptr_t*)&CTouchInterface__IsReleased);
}

void InstallGlobalHooks()
{
	//ARMHook::installHook(g_libGTASA + 0x22F2E8, (uintptr_t) mpg123_param_hook, (uintptr_t*)&mpg123_param);
	//ARMHook::installPLTHook(g_libGTASA + 0x671B20, (uintptr_t)LIB_PointerGetButton_hook, (uintptr_t*)& LIB_PointerGetButton);
	ARMHook::installPLTHook(g_libGTASA + 0x670268, (uintptr_t)CWorld__FindPlayerSlotWithPedPointer_hook, (uintptr_t*)&CWorld__FindPlayerSlotWithPedPointer);
	//
	ARMHook::installHook(g_libGTASA + 0x26BF20, (uintptr_t)ANDRunThread_hook, (uintptr_t*)& ANDRunThread);
	// filesystem
	ARMHook::installPLTHook(g_libGTASA + 0x6753A4, (uintptr_t)OS_FileOpen_hook, (uintptr_t*)&OS_FileOpen);
	// custom ZIP archive
	ARMHook::installPLTHook(g_libGTASA + 0x66FF40, (uintptr_t)CFileMgr_Initialise_hook, (uintptr_t*)&CFileMgr_Initialise);
	// SAMP IMG archive
	ARMHook::installPLTHook(g_libGTASA + 0x674C68, (uintptr_t)CStream_InitImageList_hook, (uintptr_t*)&CStream_InitImageList);
	// SAMP textures
	ARMHook::installPLTHook(g_libGTASA + 0x66F2D0, (uintptr_t)CGame__InitialiseRenderWare_hook, (uintptr_t*)&CGame__InitialiseRenderWare);
	// increase Atomic models pool
	ARMHook::installPLTHook(g_libGTASA + 0x67579C, (uintptr_t)CModelInfo_AddAtomicModel_hook, (uintptr_t*)&CModelInfo_AddAtomicModel);
	// ped models pool
	ARMHook::installPLTHook(g_libGTASA + 0x675D98, (uintptr_t)CModelInfo_AddPedModel_hook, (uintptr_t*)&CModelInfo_AddPedModel);
	// game pools
	ARMHook::installPLTHook(g_libGTASA + 0x672468, (uintptr_t)CPools_Initialise_hook, (uintptr_t*)&CPools_Initialise);
	// placeable matrix alloc
	ARMHook::installPLTHook(g_libGTASA + 0x675554, (uintptr_t)CPlaceable_InitMatrixArray_hook, (uintptr_t*)& CPlaceable_InitMatrixArray);
	// render objects 3000+- pos
	ARMHook::installPLTHook(g_libGTASA + 0x673794, (uintptr_t)CRenderer_RenderEverythingBarRoads_hook, (uintptr_t*)&CRenderer_RenderEverythingBarRoads);

	InstallSAMPHooks();
	InstallWidgetHooks();
}