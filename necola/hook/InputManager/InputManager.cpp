#include "InputManager.h"

using namespace G;

unsigned long WINAPI hook(HWND h, UINT m, WPARAM w, LPARAM l)
{
	G::InputManagerI.ProcessMessage(m, w, l);
	return CallWindowProcA(G::InputManagerI.GetWndProcOriginal(), h, m, w, l);
}

void CGlobal_InputManager::Init()
{
	m_hwnd = FindWindowA("Valve001", 0);
	m_old_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongA(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hook)));
}

void CGlobal_InputManager::undo()
{
	if (m_old_wnd_proc)
		SetWindowLongA(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_old_wnd_proc));

	m_old_wnd_proc = 0;
}
m_state CGlobal_InputManager::GetKeyState(std::uint32_t vk)
{
	return m_key_map[vk];
}

bool CGlobal_InputManager::IsKeyDown(std::uint32_t vk)
{
	return m_key_map[vk] == m_state::down;
}

bool CGlobal_InputManager::WasKeyPressed(std::uint32_t vk)
{
	if (m_key_map[vk] == m_state::pressed)
	{
		m_key_map[vk] = m_state::up;
		return true;
	}
	return false;
}

void CGlobal_InputManager::AddHotkey(std::uint32_t vk, std::function<void(void)> f)
{
	m_hotkeys[vk] = f;
}

void CGlobal_InputManager::RemHotkey(std::uint32_t vk)
{
	m_hotkeys[vk] = nullptr;
}

HWND CGlobal_InputManager::GetWindow()
{
	return m_hwnd;
}

WNDPROC CGlobal_InputManager::GetWndProcOriginal()
{
	return m_old_wnd_proc;
}

bool CGlobal_InputManager::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseMessage(uMsg, wParam, lParam);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return ProcessKeybdMessage(uMsg, wParam, lParam);
	default:
		return false;
	}
}

bool CGlobal_InputManager::ProcessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = VK_LBUTTON;
	auto state = m_state::none;

	switch (uMsg)
	{
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		state = uMsg == WM_MBUTTONUP ? m_state::up : m_state::down;
		key = VK_MBUTTON;
		break;
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		state = uMsg == WM_RBUTTONUP ? m_state::up : m_state::down;
		key = VK_RBUTTON;
		break;
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		state = uMsg == WM_LBUTTONUP ? m_state::up : m_state::down;
		key = VK_LBUTTON;
		break;
	case WM_XBUTTONDBLCLK:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		state = uMsg == WM_XBUTTONUP ? m_state::up : m_state::down;
		key = (HIWORD(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		break;
	default:
		return false;
	}

	if (state == m_state::up && m_key_map[key] == m_state::down)
		m_key_map[key] = m_state::pressed;
	else
		m_key_map[key] = state;

	return true;
}

bool CGlobal_InputManager::ProcessKeybdMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = wParam;
	auto state = m_state::none;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		state = m_state::down;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		state = m_state::up;
		break;
	default:
		return false;
	}

	if (state == m_state::up && m_key_map[int(key)] == m_state::down)
	{
		m_key_map[int(key)] = m_state::pressed;

		auto& hotkey_callback = m_hotkeys[key];

		if (hotkey_callback)
			hotkey_callback();
	}
	else
	{
		m_key_map[int(key)] = state;
	}

	return true;
}