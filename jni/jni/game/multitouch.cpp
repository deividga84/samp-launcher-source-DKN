
#include "../main.h"
#include "multitouch.h"
#include "nv_event.h"
#include "../vendor/armhook/armhook.h"
#include <string.h>

/* MultiTouch */

extern void AND_TouchEvent_hook(int type, int num, int posX, int posY);

void touch_event(int type, int num, int x, int y)
{
	// LOGI("touch_event: type %d | num %d | x %d | y %d", type, num, x, y);

	AND_TouchEvent_hook(type, num, x, y);
}

int32_t (*orig_NVEventGetNextEvent)(NVEvent *, int);
int32_t hook_NVEventGetNextEvent(NVEvent *ev, int waitMSecs)
{
	int32_t ret = orig_NVEventGetNextEvent(ev, waitMSecs);

	if (ret)
	{
		if (ev->m_type == NV_EVENT_MULTITOUCH)
			ev->m_type = (NVEventType)228;
	}

	NVEvent event;
	NV_Event::GetNextEvent(&event);

	if (event.m_type == NV_EVENT_MULTITOUCH)
	{
		int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
		int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

		int x1 = event.m_data.m_multi.m_x1;
		int y1 = event.m_data.m_multi.m_y1;

		int x2 = event.m_data.m_multi.m_x2;
		int y2 = event.m_data.m_multi.m_y2;

		int x3 = event.m_data.m_multi.m_x3;
		int y3 = event.m_data.m_multi.m_y3;

		int x4 = event.m_data.m_multi.m_x4;
		int y4 = event.m_data.m_multi.m_y4;

		if (type == NV_MULTITOUCH_CANCEL)
			type = NV_MULTITOUCH_UP;

		if ((x1 || y1) || num == 0)
		{
			if (num == 0 && type != NV_MULTITOUCH_MOVE)
				touch_event(type, 0, x1, y1);
			else
				touch_event(NV_MULTITOUCH_MOVE, 0, x1, y1);
		}

		if ((x2 || y2) || num == 1)
		{
			if (num == 1 && type != NV_MULTITOUCH_MOVE)
				touch_event(type, 1, x2, y2);
			else
				touch_event(NV_MULTITOUCH_MOVE, 1, x2, y2);
		}

		if ((x3 || y3) || num == 2)
		{
			if (num == 2 && type != NV_MULTITOUCH_MOVE)
				touch_event(type, 2, x3, y3);
			else
				touch_event(NV_MULTITOUCH_MOVE, 2, x3, y3);
		}

		if ((x4 || y4) || num == 3)
		{
			if (num == 3 && type != NV_MULTITOUCH_MOVE)
				touch_event(type, 3, x4, y4);
			else
				touch_event(NV_MULTITOUCH_MOVE, 3, x4, y4);
		}
	}

	return ret;
}

int g_points[1000];
int g_pointers[1000];

void MultiTouch::initialize()
{
	LOGI("Initializing multi touch..");

	// Points
	memset(g_points, 0, 999 * sizeof(int));
	ARMHook::unprotect(g_libGTASA+(0x679E94));
	*(int **)(g_libGTASA+(0x679E94)) = &g_points[0];

	// pointers
	memset(g_pointers, 0, 999 * sizeof(int));
	ARMHook::unprotect(g_libGTASA+(0x6D7178));
	*(int **)(g_libGTASA+(0x6D7178)) = &g_pointers[0];

	ARMHook::writeMemory(g_libGTASA+(0x26B0BC), (uintptr_t) "\x04\x20", 2); // OS_PointerGetNumber
	ARMHook::writeMemory(g_libGTASA+(0x26B0C6), (uintptr_t) "\x04\x28", 2); // OS_PointerGetType

	ARMHook::writeMemory(g_libGTASA+(0x27012C), (uintptr_t) "\x03\x28", 2); // LIB_PointerGetCoordinates
	ARMHook::writeMemory(g_libGTASA+(0x270198), (uintptr_t) "\x03\x28", 2); // LIB_PointerGetWheel
	ARMHook::writeMemory(g_libGTASA+(0x2701E4), (uintptr_t) "\x03\x28", 2); // LIB_PointerDoubleClicked
	ARMHook::writeMemory(g_libGTASA+(0x270172), (uintptr_t) "\x03\x2A", 2); // LIB_PointerGetButton

	// NVEventGetNextEvent
	ARMHook::installHook(g_libGTASA+(0x2696B4), (uintptr_t)hook_NVEventGetNextEvent, (uintptr_t *)&orig_NVEventGetNextEvent);
}

extern "C"
{
	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_multiTouchEvent4Ex(JNIEnv *env, jobject obj, jint action,
		jint pointer, jint x1, jint y1, jint x2, jint y2, jint x3, jint y3, jint x4, jint y4)
	{
		static jclass motionEvent = env->FindClass("android/view/MotionEvent");

		static jfieldID ACTION_DOWN_id = env->GetStaticFieldID(motionEvent, "ACTION_DOWN", "I");
		static jfieldID ACTION_UP_id = env->GetStaticFieldID(motionEvent, "ACTION_UP", "I");

		static jfieldID ACTION_POINTER_DOWN_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_DOWN", "I");
		static jfieldID ACTION_POINTER_UP_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_UP", "I");

		static jfieldID ACTION_CANCEL_id = env->GetStaticFieldID(motionEvent, "ACTION_CANCEL", "I");
		static jfieldID ACTION_POINTER_INDEX_SHIFT_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_ID_SHIFT", "I");
		static jfieldID ACTION_POINTER_INDEX_MASK_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_ID_MASK", "I");
		static jfieldID ACTION_MASK_id = env->GetStaticFieldID(motionEvent, "ACTION_MASK", "I");

		static int ACTION_DOWN = env->GetStaticIntField(motionEvent, ACTION_DOWN_id);
		static int ACTION_UP = env->GetStaticIntField(motionEvent, ACTION_UP_id);

		static int ACTION_POINTER_DOWN = env->GetStaticIntField(motionEvent, ACTION_POINTER_DOWN_id);
		static int ACTION_POINTER_UP = env->GetStaticIntField(motionEvent, ACTION_POINTER_UP_id);

		static int ACTION_CANCEL = env->GetStaticIntField(motionEvent, ACTION_CANCEL_id);
		static int ACTION_POINTER_INDEX_MASK = env->GetStaticIntField(motionEvent, ACTION_POINTER_INDEX_MASK_id);
		static int ACTION_POINTER_INDEX_SHIFT = env->GetStaticIntField(motionEvent, ACTION_POINTER_INDEX_SHIFT_id);
		static int ACTION_MASK = env->GetStaticIntField(motionEvent, ACTION_MASK_id);

		NVEvent ev;

		ev.m_type = NV_EVENT_MULTITOUCH;

		if (action == ACTION_UP)
			ev.m_data.m_multi.m_action = NV_MULTITOUCH_UP;
		else if (action == ACTION_DOWN)
			ev.m_data.m_multi.m_action = NV_MULTITOUCH_DOWN;
		else if (action == ACTION_POINTER_DOWN)
			ev.m_data.m_multi.m_action = NV_MULTITOUCH_DOWN;
		else if (action == ACTION_POINTER_UP)
			ev.m_data.m_multi.m_action = NV_MULTITOUCH_UP;
		else if (action == ACTION_CANCEL)
			ev.m_data.m_multi.m_action = NV_MULTITOUCH_CANCEL;
		else
			ev.m_data.m_multi.m_action = NV_MULTITOUCH_MOVE;

		ev.m_data.m_multi.m_action =
			(NVMultiTouchEventType)(ev.m_data.m_multi.m_action | (pointer << NV_MULTITOUCH_POINTER_SHIFT));

		ev.m_data.m_multi.m_x1 = x1;
		ev.m_data.m_multi.m_y1 = y1;

		ev.m_data.m_multi.m_x2 = x2;
		ev.m_data.m_multi.m_y2 = y2;

		ev.m_data.m_multi.m_x3 = x3;
		ev.m_data.m_multi.m_y3 = y3;

		ev.m_data.m_multi.m_x4 = x4;
		ev.m_data.m_multi.m_y4 = y4;

		NV_Event::InsertNewest(&ev);

		return JNI_TRUE;
	}
}
