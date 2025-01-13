#pragma once

#include "CorePCH.hpp"

namespace Renderer::Window
{
	struct WindowArea
	{
		WindowArea(unsigned int width, unsigned int height)
			: width(width), height(height) {}

		unsigned int width, height;
	};

	class Window
	{
	public:
		Window(unsigned int width = 800, unsigned int height = 600);
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&&) = delete;
		Window& operator=(Window&&) = delete;
	public:
		void StartMessageLoop();
		void JoinMessageLoop();
	public:
		[[nodiscard]] inline bool IsInitialized() { return m_IsInitialized; }
		[[nodiscard]] inline bool IsShown()		  { return m_IsShown; }
		[[nodiscard]] inline bool IsRunning()	  { return m_IsRunning; }
	private:
		void MessageLoop();
	private:
		static LRESULT CALLBACK WndProcSetup(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK WndProcThunk(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT CALLBACK WndProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
	private:
		static constexpr const wchar_t* SMP_WINDOW_CLASS_NAME = L"TestWindow"; // SMP = static member pointer.
		static constexpr const wchar_t* SMP_WINDOW_TITLE = L"Test Window";
	private:
		WindowArea m_WindowArea;
		HWND m_WindowHandle = nullptr;
		bool m_IsInitialized = false;
		bool m_IsShown = false;
		bool m_IsRunning = false;
		std::thread m_MessageLoop;
	};
}