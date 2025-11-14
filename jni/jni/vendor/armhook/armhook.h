#pragma once

/*
	ARMHook - класс для работы с хуками на ARM-архитектуре (THUMB)

	github.com/4x11
	axwellcm@gmail.com
*/

class ARMHook
{
public:
	/* Установить доступ к памяти для всей страницы */
	static void unprotect(uintptr_t addr, uintptr_t size = 100);
	/* 1 nop = 2 bytes */
	static void makeNOP(uintptr_t addr, unsigned int word_count);
	/* 2 bytes */
	static void makeRET(uintptr_t addr);
	/* Запись в память с установлением доступа и очищением кеша процессора */
	static void writeMemory(uintptr_t dest, uintptr_t src, size_t size);
	/* Чтение из памяти с установлением доступа */
	static void readMemory(uintptr_t dest, uintptr_t src, size_t size);

	/* Хук метода класса через виртуальную таблицу методов */
	static void installMethodHook(
		uintptr_t addr,			// адрес указателя на функцию
		uintptr_t hook_func,	// адрес хук-функции
		uintptr_t* orig_func = nullptr	// указатель на оригинальную функцию
	);

	/* Хук функции через  Procedure Linkage Table */
	static void installPLTHook(
		uintptr_t addr,			// адрес указателя на функцию
		uintptr_t hook_func,	// адрес хук-функции
		uintptr_t* orig_func	// указатель на оригинальную функцию
	);

	/* Хук функции (+трамплин) */
	static bool installHook(
		uintptr_t func, // адрес функции
		uintptr_t hook_func, // адрес хук-функции
		uintptr_t* orig_func, // указатель для вызова оригинальной функции через трамплин
		size_t tramp_size = 4
	);
	/* Инъекция в код */
	static void codeInject(
		uintptr_t addr,	// адрес для инъекции 
		uintptr_t func, // адрес naked-функции содержащей код для инъекции
		int reg // регистр для инъекции (!!! только r0-r7 !!!)
	);

	static uintptr_t getLibraryAddress(const char* szLibName);

	static void initializeTrampolines(uintptr_t _trampoline, size_t size);
	static void uninitializeTrampolines();

private:
	
	struct TRAMPOLINE
	{
		char*		base_ptr;
		uintptr_t	current_ptr;
		size_t		size;
	};

	static struct TRAMPOLINE local_trampoline;
	static struct TRAMPOLINE remote_trampoline;

	static void makeBranch(uintptr_t func, uintptr_t addr);
	static void makeJump(uintptr_t addr, uintptr_t func);
};

template <typename Ret, typename... Args>
static Ret CallFunction(unsigned int address, Args... args)
{
	return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
}