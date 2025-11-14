#include <jni.h>
#include <pthread.h>
//#include <syscall.h>

#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "playertags.h"
#include "audiostream.h"
#include "java/jniutil.h"
#include <dlfcn.h>
#include "StackTrace.h"

// voice
#include "voice_new/Plugin.h"

#include "vendor/armhook/armhook.h"
#include "vendor/str_obfuscator/str_obfuscator.hpp"

#include "settings.h"

#include "crashlytics.h"

/*
Peerapol Unarak
*/

JavaVM* javaVM;

UI* pUI = nullptr;
CGame *pGame = nullptr;

CNetGame *pNetGame = nullptr;
CPlayerTags* pPlayerTags = nullptr;
CSnapShotHelper* pSnapShotHelper = nullptr;
CAudioStream* pAudioStream = nullptr;
CJavaWrapper* pJavaWrapper = nullptr;
CSettings* pSettings = nullptr;
//CVoice* pVoice = nullptr;

MaterialTextGenerator* pMaterialTextGenerator = nullptr;

bool bDebug = false;
bool bGameInited = false;
bool bNetworkInited = false;

uintptr_t g_libGTASA = 0x00;
uintptr_t g_libSAMP = 0x00;

void ApplyGlobalPatches();
void ApplyMultiTouchPatches();
void InstallGlobalHooks();
void InitializeRenderWare();
void FLog(const char* fmt, ...);
//void MyLog(const char* fmt, ...);

int work = 0;

bool Mchat = false;

void ReadSettingFile()
{
	/*char path[255] = { 0 };
	//sprintf(path, "%ssamp.set", pGame->GetDataDirectory());
	sprintf(path, "%sNickName.ini", pGame->GetDataDirectory());

	FILE* fp = fopen(path, "r");
	if (fp == NULL) return;

	char buf[1024];

	// nickname
	if (fgets(buf, 1024, fp) != NULL) {
		buf[strcspn(buf, "\n\r")] = 0;
		strcpy(g_nick, buf);
	}

	fclose(fp);*/

	pSettings = new CSettings();

	firebase::crashlytics::SetUserId(pSettings->Get().szNickName);
}

int hashing(const char* str) {
	int hashing = 5381;
	int c;
	while (c = *str++) {
		hashing = ((hashing << 5) + hashing) + c; /* hash * 33 + c */
		if (hashing < 0) hashing = 100;
	}
	if (hashing < 0) hashing = 100;
	return hashing;
}


int _curlWriteFunc(char* data, size_t size, size_t nmemb, std::string* buffer)
{
	int result = 0;
	if (buffer != nullptr)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

void SkipRockStarLegal()
{
	uintptr_t adr = ARMHook::getLibraryAddress("libSCAnd.so");
	if (adr == 0) return;

	ARMHook::unprotect(adr + /*0x20C670*/0x31C149);
	*(bool*)(adr + /*0x20C670*/0x31C149) = true;
}

void* Init(void*)
{
	SkipRockStarLegal(); //Skip LegalScreenShown

	while (true)
	{
		if (*(int*)(g_libGTASA + 0xA987C8) == 7) {
			pGame->StartGame();
			break;
		}
		else {
			usleep(500);
		}
	}

	pthread_exit(0);
}

void DoDebugLoop()
{
	// ...
}

void DoDebugStuff()
{
	// ...

	MATRIX4X4 mat;
	pGame->FindPlayerPed()->GetMatrix(&mat);
	
	for (int i = 0; i < 100; i++)
	{
		CPlayerPed* ped = pGame->NewPlayer(i, mat.pos.X + i, mat.pos.Y, mat.pos.Z, 0.0f, false, false);
		//ped->SetCollisionChecking(false);
		//ped->SetGravityProcessing(false);
	}
}

void printAddressBacktrace(const unsigned address, void* pc, void* lr)
{
	char filename[0xFF];
	sprintf(filename, "/proc/%d/maps", getpid());
	FILE* m_fp = fopen(filename, "rt");
	if (m_fp == nullptr)
	{
		FLog("ERROR: can't open file %s", filename);
		return;
	}
		Dl_info info_pc, info_lr;
		memset(&info_pc, 0, sizeof(Dl_info));
		memset(&info_lr, 0, sizeof(Dl_info));
		dladdr(pc, &info_pc);
		dladdr(lr, &info_lr);

		rewind(m_fp);
		char buffer[2048] = { 0 };
		while (fgets(buffer, sizeof(buffer), m_fp))
		{
			const auto start_address = strtoul(buffer, nullptr, 16);
			const auto end_address = strtoul(strchr(buffer, '-') + 1, nullptr, 16);

			if (start_address <= address && end_address > address)
			{
				if (*(strchr(buffer, ' ') + 3) == 'x')
					FLog("Call: %X (GTA: %X PC: %s LR: %s) (SAMP: %X) (libc: %X)", address, address - g_libGTASA, info_pc.dli_sname, info_lr.dli_sname, address - ARMHook::getLibraryAddress("libSAMP.so"), address - ARMHook::getLibraryAddress("libc.so"));
				break;
			}
		}
}

struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;

extern int g_iLastProcessedSkinCollision, g_iLastProcessedEntityCollision, g_iLastRenderedObject;
extern uintptr_t g_dwLastRetAddrCrash;
void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act_old.sa_sigaction)
	{
		act_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGSEGV)
	{
		FLog("SIGSEGV | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();
	}

	return;
}

void handler1(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act1_old.sa_sigaction)
	{
		act1_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGABRT)
	{
		FLog("SIGABRT | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();
	}

	return;
}

void handler2(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act2_old.sa_sigaction)
	{
		act2_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGFPE)
	{
		FLog("SIGFPE | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();
	}

	return;
}

void handler3(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act3_old.sa_sigaction)
	{
		act3_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGBUS)
	{
		FLog("SIGBUS | Fault address: 0x%X", info->si_addr);

		PRINT_CRASH_STATES(context);

		CStackTrace::printBacktrace();
	}

	return;
}

void DoInitStuff()
{
	if (bGameInited == false)
	{
		pPlayerTags = new CPlayerTags();
		pSnapShotHelper = new CSnapShotHelper();
		pMaterialTextGenerator = new MaterialTextGenerator();
		pAudioStream = new CAudioStream();
		pAudioStream->Initialize();

		//pUI->splashscreen()->setVisible(false);
		pJavaWrapper->HideLoadingScreen();
		pUI->chat()->setVisible(true);
		Mchat = true;

		pGame->Initialize();
		pGame->SetMaxStats();
		pGame->ToggleThePassingOfTime(false);

		// voice
		LogVoice("[dbg:samp:load] : module loading...");

		for (const auto& loadCallback : Samp::loadCallbacks) {
			if (loadCallback != nullptr) {
				loadCallback();
			}
		}

		Samp::loadStatus = true;

		LogVoice("[dbg:samp:load] : module loaded");

		if (bDebug)
		{
			pGame->GetCamera()->Restore();
			pGame->GetCamera()->SetBehindPlayer();
			pGame->DisplayHUD(true);
			pGame->EnableClock(false);

			DoDebugStuff();
		}

		bGameInited = true;
	}

	if (!bNetworkInited && !bDebug)
	{
		//ReadSettingFile();

		pNetGame = new CNetGame(cryptor::create("51.81.57.217").decrypt(), 10790, pSettings->Get().szNickName, pSettings->Get().szPassword);
		bNetworkInited = true;
	}
}

extern "C" {
	JNIEXPORT void JNICALL Java_com_sampmobile_game_main_SAMP_initializeSAMP(JNIEnv *pEnv, jobject thiz)
	{
		ReadSettingFile();
		pJavaWrapper = new CJavaWrapper(pEnv, thiz);
	}
	JNIEXPORT void JNICALL Java_com_sampmobile_game_main_SAMP_onInputEnd(JNIEnv *pEnv, jobject thiz, jbyteArray str)
	{
		if(pUI)
		{
			pUI->keyboard()->sendForGB(pEnv, thiz, str);
		}
	}
	JNIEXPORT void JNICALL Java_com_sampmobile_game_main_SAMP_onEventBackPressed(JNIEnv *pEnv, jobject thiz)
	{
		if(pSettings->Get().iAndroidKeyboard) pJavaWrapper->HideKeyboard();
	}
	JNIEXPORT void JNICALL Java_com_sampmobile_game_main_ui_dialog_DialogManager_sendDialogResponse(JNIEnv* pEnv, jobject thiz, jint i3, jint i, jint i2, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);

		if(pNetGame) {
			pNetGame->SendDialogResponse(i, i3, i2, (char*)szStr.c_str());
			//pGame->FindPlayerPed()->TogglePlayerControllableWithoutLock(true);
		}

		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}
}

void MainLoop()
{
	if (pGame->bIsGameExiting) return;

	DoInitStuff();

	if (bDebug) {
		DoDebugLoop();
	}

	if (pNetGame) {
		pNetGame->Process();

		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if(pTextDrawPool) pTextDrawPool->Draw();
	}

	if (pNetGame)
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CPlayerPed* pLocalPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
				if (pLocalPlayerPed && pNetGame->GetGameState() == GAMESTATE_CONNECTED)
				{
					pGame->DisplayHUD(false);
					*(uint8_t*)(g_libGTASA + /*0x8EF36B*/0x991FD8) = 0;
					pJavaWrapper->UpdateHud(pLocalPlayerPed->GetHealth(),
											pLocalPlayerPed->GetArmour(),
											pNetGame->eatProcent,
											pGame->GetLocalMoney(),
											pLocalPlayerPed->GetCurrentWeapon(),	pLocalPlayerPed->GetCurrentWeaponSlot()->dwAmmo);

				}
				else {
					*(uint8_t*)(g_libGTASA + /*0x8EF36B*/0x991FD8) = 1;
					pJavaWrapper->HideHud();
				}

			}
		}
	}

	if (pAudioStream) {
		pAudioStream->Process();
	}

}

void InitGui()
{
	// new voice
	Plugin::OnPluginLoad();
	Plugin::OnSampLoad();

	std::string font_path = string_format("%sfonts/%s", pGame->GetDataDirectory(), FONT_NAME);
	pUI = new UI(ImVec2(RsGlobal->maximumWidth, RsGlobal->maximumHeight), font_path.c_str());
	pUI->initialize();
	pUI->performLayout();
}

#include "game/multitouch.h"
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	javaVM = vm;
	LOGI("SA-MP library loaded! Build time: " __DATE__ " " __TIME__);

	g_libGTASA = ARMHook::getLibraryAddress("libGTASA.so");
	if (g_libGTASA == 0x00) {
		LOGE("libGTASA.so address was not found! ");
		return JNI_VERSION_1_6;
	}

	g_libSAMP = ARMHook::getLibraryAddress("libSAMP.so");
	if (g_libSAMP == 0x00) {
		LOGE("libSAMP.so address was not found! ");
		return JNI_VERSION_1_6;
	}

	firebase::crashlytics::Initialize();

	uintptr_t libgtasa = ARMHook::getLibraryAddress("libGTASA.so");
	uintptr_t libsamp = ARMHook::getLibraryAddress("libSAMP.so");
	uintptr_t libc = ARMHook::getLibraryAddress("libc.so");

	FLog("libGTASA.so: 0x%x", libgtasa);
	FLog("libSAMP.so: 0x%x", libsamp);
	FLog("libc.so: 0x%x", libc);

	char str[100];

	sprintf(str, "0x%x", libgtasa);
	firebase::crashlytics::SetCustomKey("libGTASA.so", str);
	
	sprintf(str, "0x%x", libsamp);
	firebase::crashlytics::SetCustomKey("libSAMP.so", str);

	sprintf(str, "0x%x", libc);
	firebase::crashlytics::SetCustomKey("libc.so", str);

	LOGI("Loading bass library..");
	LoadBassLibrary();

	ARMHook::initializeTrampolines(g_libGTASA +/*0x180044*/0x1A9E0C, 1024);

	InstallGlobalHooks();
	ApplyGlobalPatches();
	InitializeRenderWare();
	MultiTouch::initialize();

	pGame = new CGame();

	//pVoice = new CVoice();
	//pVoice->Initialize(VOICE_FREQUENCY, CODEC_FREQUENCY, VOICE_SENDRRATE);

	pthread_t thread;
	pthread_create(&thread, 0, Init, 0);

	struct sigaction act;
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, &act_old);

	struct sigaction act1;
	act1.sa_sigaction = handler1;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags = SA_SIGINFO;
	sigaction(SIGABRT, &act1, &act1_old);

	struct sigaction act2;
	act2.sa_sigaction = handler2;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = SA_SIGINFO;
	sigaction(SIGFPE, &act2, &act2_old);

	struct sigaction act3;
	act3.sa_sigaction = handler3;
	sigemptyset(&act3.sa_mask);
	act3.sa_flags = SA_SIGINFO;
	sigaction(SIGBUS, &act3, &act3_old);
		
	return JNI_VERSION_1_6;
}

// never called on Android :(
void JNI_OnUnload(JavaVM *vm, void *reserved)
{
	FLog("SA-MP library unloaded!");

	ARMHook::uninitializeTrampolines();
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}	

void FLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = CGame::GetDataDirectory();


	if (flLog == nullptr && pszStorage != nullptr)
	{
		sprintf(buffer, "%s/samp_log.txt", pszStorage);
		LOGI("buffer: %s", buffer);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	LOGI("%s", buffer);
	firebase::crashlytics::Log(buffer);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

void ChatLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = CGame::GetDataDirectory();


	if (flLog == nullptr && pszStorage != nullptr)
	{
		sprintf(buffer, "%s/chat_log.txt", pszStorage);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

void MyLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = CGame::GetDataDirectory();


	if (flLog == nullptr && pszStorage != nullptr)
	{
		sprintf(buffer, "%s/samp_log.txt", pszStorage);
		LOGI("buffer: %s", buffer);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

void MyLog2(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = CGame::GetDataDirectory();


	if (flLog == nullptr && pszStorage != nullptr)
	{
		sprintf(buffer, "%s/samp_log.txt", pszStorage);
		LOGI("buffer: %s", buffer);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	if (pUI) pUI->chat()->addDebugMessage(buffer);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);
	return;
}

void LogVoice(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;
	const char* pszStorage = CGame::GetDataDirectory();

	if (flLog == nullptr && pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/%s", pszStorage, SV::kLogFileName);
		flLog = fopen(buffer, "w");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

int (*BASS_Init) (uint32_t, uint32_t, uint32_t);
int (*BASS_Free) (void);
char *(*BASS_GetConfigPtr) (uint32_t);
int (*BASS_SetConfigPtr) (uint32_t, const char *);
int (*BASS_GetConfig) (uint32_t);
int (*BASS_SetConfig) (uint32_t, uint32_t);
int (*BASS_ChannelStop) (uint32_t);
int (*BASS_StreamCreateURL) (char *, uint32_t, uint32_t, uint32_t);
int (*BASS_StreamCreate) (uint32_t, uint32_t, uint32_t, STREAMPROC *, void *);
int (*BASS_ChannelPlay) (uint32_t, bool);
int (*BASS_ChannelPause) (uint32_t);
int (*BASS_StreamFree) (uint32_t);
int (*BASS_ErrorGetCode) (void);
int (*BASS_Set3DFactors) (float, float, float);
int (*BASS_Set3DPosition) (const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
int (*BASS_Apply3D) (void);
int (*BASS_ChannelSetFX) (uint32_t, HFX);
int (*BASS_ChannelRemoveFX) (uint32_t, HFX);
int (*BASS_FXSetParameters) (HFX, const void *);
int (*BASS_IsStarted) (void);
int (*BASS_RecordGetDeviceInfo) (uint32_t, BASS_DEVICEINFO *);
int (*BASS_RecordInit) (int);
int (*BASS_RecordGetDevice) (void);
int (*BASS_RecordFree) (void);
int (*BASS_RecordStart) (uint32_t, uint32_t, uint32_t, RECORDPROC *, void *);
int (*BASS_ChannelSetAttribute) (uint32_t, uint32_t, float);
int (*BASS_ChannelGetData) (uint32_t, void *, uint32_t);
int (*BASS_RecordSetInput) (int, uint32_t, float);
int (*BASS_StreamPutData) (uint32_t, const void *, uint32_t);
int (*BASS_ChannelSetPosition) (uint32_t, uint64_t, uint32_t);
int (*BASS_ChannelIsActive) (uint32_t);
int (*BASS_ChannelSlideAttribute) (uint32_t, uint32_t, float, uint32_t);
int (*BASS_ChannelSet3DAttributes) (uint32_t, int, float, float, int, int, float);
int (*BASS_ChannelSet3DPosition) (uint32_t, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
int (*BASS_SetVolume) (float);
const char *(*BASS_ChannelGetTags) (uint32_t handle, uint32_t tags);
int (*BASS_ChannelSetSync) (uint32_t handle, uint32_t type, uint64_t param, SYNCPROC *proc, void *user);

void LoadBassLibrary() {
	void *v0 = dlopen("/data/data/com.sampmobile.game/lib/libbass.so", 1);
	if (!v0) FLog("%s", dlerror());

	BASS_Init = (int (*)(uint32_t, uint32_t, uint32_t)) dlsym(v0, "BASS_Init");
	BASS_Free = (int (*)(void)) dlsym(v0, "BASS_Free");
	BASS_GetConfigPtr = (char *(*)(uint32_t)) dlsym(v0, "BASS_GetConfigPtr");
	BASS_SetConfigPtr = (int (*)(uint32_t, const char *)) dlsym(v0, "BASS_SetConfigPtr");
	BASS_GetConfig = (int (*)(uint32_t)) dlsym(v0, "BASS_GetConfig");
	BASS_SetConfig = (int (*)(uint32_t, uint32_t)) dlsym(v0, "BASS_SetConfig");
	BASS_ChannelStop = (int (*)(uint32_t)) dlsym(v0, "BASS_ChannelStop");
	BASS_StreamCreateURL = (int (*)(char *, uint32_t, uint32_t, uint32_t)) dlsym(v0,
																				 "BASS_StreamCreateURL");
	BASS_StreamCreate = (int (*)(uint32_t, uint32_t, uint32_t, STREAMPROC *, void *)) dlsym(v0,
																							"BASS_StreamCreate");
	BASS_ChannelPlay = (int (*)(uint32_t, bool)) dlsym(v0, "BASS_ChannelPlay");
	BASS_ChannelPause = (int (*)(uint32_t)) dlsym(v0, "BASS_ChannelPause");
	BASS_StreamFree = (int (*)(uint32_t)) dlsym(v0, "BASS_StreamFree");
	BASS_ErrorGetCode = (int (*)(void)) dlsym(v0, "BASS_ErrorGetCode");
	BASS_Set3DFactors = (int (*)(float, float, float)) dlsym(v0, "BASS_Set3DFactors");
	BASS_Set3DPosition = (int (*)(const BASS_3DVECTOR *, const BASS_3DVECTOR *,
								  const BASS_3DVECTOR *, const BASS_3DVECTOR *)) dlsym(v0,
																					   "BASS_Set3DPosition");
	BASS_Apply3D = (int (*)(void)) dlsym(v0, "BASS_Apply3D");
	BASS_ChannelSetFX = (int (*)(uint32_t, HFX)) dlsym(v0, "BASS_ChannelSetFX");
	BASS_ChannelRemoveFX = (int (*)(uint32_t, HFX)) dlsym(v0, "BASS_ChannelRemoveFX");
	BASS_FXSetParameters = (int (*)(HFX, const void *)) dlsym(v0, "BASS_FXSetParameters");
	BASS_IsStarted = (int (*)(void)) dlsym(v0, "BASS_IsStarted");
	BASS_RecordGetDeviceInfo = (int (*)(uint32_t, BASS_DEVICEINFO *)) dlsym(v0,
																			"BASS_RecordGetDeviceInfo");
	BASS_RecordInit = (int (*)(int)) dlsym(v0, "BASS_RecordInit");
	BASS_RecordGetDevice = (int (*)(void)) dlsym(v0, "BASS_RecordGetDevice");
	BASS_RecordFree = (int (*)(void)) dlsym(v0, "BASS_RecordFree");
	BASS_RecordStart = (int (*)(uint32_t, uint32_t, uint32_t, RECORDPROC *, void *)) dlsym(v0,
																						   "BASS_RecordStart");
	BASS_ChannelSetAttribute = (int (*)(uint32_t, uint32_t, float)) dlsym(v0,
																		  "BASS_ChannelSetAttribute");
	BASS_ChannelGetData = (int (*)(uint32_t, void *, uint32_t)) dlsym(v0, "BASS_ChannelGetData");
	BASS_RecordSetInput = (int (*)(int, uint32_t, float)) dlsym(v0, "BASS_RecordSetInput");
	BASS_StreamPutData = (int (*)(uint32_t, const void *, uint32_t)) dlsym(v0,
																		   "BASS_StreamPutData");
	BASS_ChannelSetPosition = (int (*)(uint32_t, uint64_t, uint32_t)) dlsym(v0,
																			"BASS_ChannelSetPosition");
	BASS_ChannelIsActive = (int (*)(uint32_t)) dlsym(v0, "BASS_ChannelIsActive");
	BASS_ChannelSlideAttribute = (int (*)(uint32_t, uint32_t, float, uint32_t)) dlsym(v0,
																					  "BASS_ChannelSlideAttribute");
	BASS_ChannelSet3DAttributes = (int (*)(uint32_t, int, float, float, int, int, float)) dlsym(v0,
																								"BASS_ChannelSet3DAttributes");
	BASS_ChannelSet3DPosition = (int (*)(uint32_t, const BASS_3DVECTOR *, const BASS_3DVECTOR *,
										 const BASS_3DVECTOR *)) dlsym(v0,
																	   "BASS_ChannelSet3DPosition");
	BASS_SetVolume = (int (*)(float)) dlsym(v0, "BASS_SetVolume");
	BASS_ChannelGetTags = (const char *(*)(uint32_t, uint32_t)) dlsym(v0, "BASS_ChannelGetTags");
	BASS_ChannelSetSync = (int (*)(uint32_t, uint32_t, uint64_t, SYNCPROC *, void *)) dlsym(v0,
																							"BASS_ChannelSetSync");
}