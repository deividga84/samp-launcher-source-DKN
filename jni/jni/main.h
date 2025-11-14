#pragma once

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <unistd.h>
#include "log.h"
#include <jni.h>
#include <cstring>
#include "vendor/bass/bass.h"
#include "vendor/bass/bass_fx.h"

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) { if (p) { delete (p); (p) = NULL; } }
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) ( sizeof((a)) / sizeof(*(a)) )
#endif

#define SAMP_VERSION	"0.3.7-R4"

#define SAMP_ARCHIVE_PATH "/Android/data/com.sampmobile.game/samp.data"
#define FONT_NAME "arial_bold.ttf"

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969

extern uintptr_t g_libGTASA;

extern JavaVM* javaVM;

uint32_t GetTickCount();
void LogVoice(const char* fmt, ...);

void FLog(const char* fmt, ...);
void MyLog(const char* fmt, ...);
void MyLog2(const char* fmt, ...);
void ChatLog(const char* fmt, ...);

extern int (*BASS_Init) (uint32_t, uint32_t, uint32_t);
extern int (*BASS_Free) (void);
extern char *(*BASS_GetConfigPtr) (uint32_t);
extern int (*BASS_SetConfigPtr) (uint32_t, const char *);
extern int (*BASS_GetConfig) (uint32_t);
extern int (*BASS_SetConfig) (uint32_t, uint32_t);
extern int (*BASS_ChannelStop) (uint32_t);
extern int (*BASS_StreamCreateURL) (char*, uint32_t, uint32_t, uint32_t);
extern int (*BASS_StreamCreate) (uint32_t, uint32_t, uint32_t, STREAMPROC *, void *);
extern int (*BASS_ChannelPlay) (uint32_t, bool);
extern int (*BASS_ChannelPause) (uint32_t);
extern int (*BASS_StreamFree) (uint32_t);
extern int (*BASS_ErrorGetCode) (void);
extern int (*BASS_Set3DFactors) (float, float, float);
extern int (*BASS_Set3DPosition) (const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
extern int (*BASS_Apply3D) (void);
extern int (*BASS_ChannelSetFX) (uint32_t, HFX);
extern int (*BASS_ChannelRemoveFX) (uint32_t, HFX);
extern int (*BASS_FXSetParameters) (HFX, const void *);
extern int (*BASS_IsStarted) (void);
extern int (*BASS_RecordGetDeviceInfo) (uint32_t, BASS_DEVICEINFO *);
extern int (*BASS_RecordInit) (int);
extern int (*BASS_RecordGetDevice) (void);
extern int (*BASS_RecordFree) (void);
extern int (*BASS_RecordStart) (uint32_t, uint32_t, uint32_t, RECORDPROC *, void *);
extern int (*BASS_ChannelSetAttribute) (uint32_t, uint32_t, float);
extern int (*BASS_ChannelGetData) (uint32_t, void *, uint32_t);
extern int (*BASS_RecordSetInput) (int, uint32_t, float);
extern int (*BASS_StreamPutData) (uint32_t, const void *, uint32_t);
extern int (*BASS_ChannelSetPosition) (uint32_t, uint64_t, uint32_t);
extern int (*BASS_ChannelIsActive) (uint32_t);
extern int (*BASS_ChannelSlideAttribute) (uint32_t, uint32_t, float, uint32_t);
extern int (*BASS_ChannelSet3DAttributes) (uint32_t, int, float, float, int, int, float);
extern int (*BASS_ChannelSet3DPosition) (uint32_t, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
extern int (*BASS_SetVolume) (float);
extern const char *(*BASS_ChannelGetTags) (uint32_t handle, uint32_t tags);
extern int (*BASS_ChannelSetSync) (uint32_t handle, uint32_t type, uint64_t param, SYNCPROC *proc, void *user);

void LoadBassLibrary();

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}