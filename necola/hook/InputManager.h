#pragma once
#include "../sdk/SDK.h"
#include <functional>

enum class m_state
{
	none = 1,
	down,
	up,
	pressed
};


class CGlobal_InputManager {
public:
	void Init();
	void undo();

	m_state GetKeyState(uint32_t vk);

	bool IsKeyDown(uint32_t vk);
	bool WasKeyPressed(uint32_t vk);

	void AddHotkey(uint32_t vk, std::function<void(void)> f);
	void RemHotkey(uint32_t vk);

	HWND GetWindow();
	WNDPROC GetWndProcOriginal();

	bool ProcessMessage(UINT, WPARAM, LPARAM);
	bool ProcessMouseMessage(UINT, WPARAM, LPARAM);
	bool ProcessKeybdMessage(UINT, WPARAM, LPARAM);
private:
	HWND                                     m_hwnd;
	WNDPROC                                  m_old_wnd_proc;
	m_state                                  m_key_map[256];
	std::function<void(void)>                m_hotkeys[256];
};



namespace G { inline CGlobal_InputManager InputManagerI; }