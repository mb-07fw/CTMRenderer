#pragma once

#include "Core/CoreDefines.hpp"
#include "Windows.h"
#include "Event/EventManagerInterface.hpp"

#include <thread> // std::thread
#include <atomic> // std::atomic
#include <mutex>  // std::mutex
#include <condition_variable> // std::condition_variable

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
		Window(Event::EventManagerInterface& eventManagerInterface, unsigned int width = 800, unsigned int height = 600);
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&&) = delete;
		Window& operator=(Window&&) = delete;
	public:
		void Start();
		void Join();
	public:
		[[nodiscard]] inline bool IsInitialized() { return m_IsInitialized; }
		[[nodiscard]] inline bool IsShown()		  { return m_IsShown; }
		[[nodiscard]] inline bool IsRunning()	  { return m_IsRunning; }
	private:
		void Init();
		void MessageLoop();
	private:
		static LRESULT CALLBACK WndProcSetup(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK WndProcThunk(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT CALLBACK WndProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept;
	private:
		static constexpr const wchar_t* SMP_WINDOW_CLASS_NAME = L"TestWindow"; // SMP = static member pointer.
		static constexpr const wchar_t* SMP_WINDOW_TITLE = L"Test Window";
	private:
		Event::EventManagerInterface& m_EventManagerInterface;
		WindowArea m_WindowArea;
		HWND m_WindowHandle;
		std::atomic_bool m_IsInitialized;
		std::atomic_bool m_IsShown;
		std::atomic_bool m_IsRunning;
		std::thread m_MessageLoop;
		std::mutex m_Mutex;
		std::condition_variable m_CV;
	};
}