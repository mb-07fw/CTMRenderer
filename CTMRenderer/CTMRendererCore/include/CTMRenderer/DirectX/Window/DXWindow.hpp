#pragma once

#include "Windows.h"

#include "CTMRenderer/DirectX/DXRendererSettings.hpp"
#include "CTMRenderer/DirectX/Window/DXWindowGeometry.hpp"
#include "CTMRenderer/DirectX/Control/Mouse.hpp"
#include "Event/EventSystem.hpp"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace CTMRenderer::CTMDirectX::Window
{
	class DXWindow
	{
	public:
		DXWindow(const DXRendererSettings& settingsRef, Event::EventDispatcher& eventDispatcherRef, UINT width = 800, UINT height = 600);
		DXWindow(const DXWindow&) = delete;
		DXWindow(DXWindow&&) = delete;
		DXWindow& operator=(const DXWindow&&) = delete;
		DXWindow& operator=(DXWindow&&) = delete;
	public:
		void Start() noexcept;
		void HandleMessages(BOOL& result, MSG& msg) noexcept;
		void SetTitle(const std::wstring& title) noexcept;
	public:
		inline [[nodiscard]] bool IsInitialized() { return m_IsInitialized.load(std::memory_order_acquire); }
		inline [[nodiscard]] bool IsShown()		  { return m_IsShown.load(std::memory_order_acquire); }
		inline [[nodiscard]] bool IsRunning()	  { return m_IsRunning.load(std::memory_order_acquire); }
		inline [[nodiscard]] Control::Mouse& Mouse() noexcept { return m_Mouse; }
		inline [[nodiscard]] HWND Handle() const noexcept { return m_WindowHandle; }
		inline [[nodiscard]] const Geometry::WindowArea& ClientArea() const noexcept { return m_ClientArea; }
	private:
		void Init();
	private:
		static LRESULT CALLBACK WndProcSetup(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK WndProcThunk(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT CALLBACK WndProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
	private:
		static constexpr const wchar_t* SP_WINDOW_CLASS_NAME = L"TestWindow"; // SMP = static member pointer.
		static constexpr const wchar_t* SP_WINDOW_TITLE = L"Test Window";
	private:
		const DXRendererSettings& m_SettingsRef;
		Event::EventDispatcher& m_EventDispatcherRef;
		Geometry::WindowArea m_ClientArea;
		Control::Mouse m_Mouse;
		std::atomic_bool m_IsInitialized = false;
		std::atomic_bool m_IsShown = false;
		std::atomic_bool m_IsRunning = false;
		HWND m_WindowHandle = nullptr;
	};
}