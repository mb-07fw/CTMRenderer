#include "CorePCH.hpp"
#include "Window.hpp"
#include "RendererMacros.hpp"

namespace Renderer::Window
{
    #pragma region Constructors
    Window::Window(unsigned int width, unsigned int height)
        : m_WindowArea(width, height)
    {
        WNDCLASSEXW wndClass = {};

        wndClass.cbSize = sizeof(WNDCLASSEXW);
        wndClass.lpfnWndProc = WndProcSetup;    /* Set the window procedure to the static setup function that will eventually thunk (redirect, forward)
                                                *  Window's messages to an instance function that has access to state.
                                                * 
                                                *  The function assigned to lpfnWndProc has to be static due to how instance functions work in C++, 
                                                *  and how windows procedures are defined in the WinAPI. 
                                                * 
                                                *  For context, Instance functions implicitly take in an argument of this, which is incompatible with
                                                *  the WinAPI window procedure standard, thus static functions are required as they omit the this argument.
                                                *  
                                                *  In conclusion, the indirection is required to be able to access Window state in the window procedure without
                                                *  the introduction of global state.
                                                */

        // Get handle of the file that created the .exe process.
        wndClass.hInstance = GetModuleHandle(NULL);
        wndClass.lpszClassName = SMP_WINDOW_CLASS_NAME;

        if (wndClass.hInstance == nullptr)
        {
            std::cerr << "Failed to get the HINSTANCE.\n";
            return;
        }

        if (RegisterClassEx(&wndClass) == 0)
        {
            std::cerr << "Failed to register window class.\n";
            return;
        }

        DEBUG_PRINT("Registered window class.\n");

        RECT windowClientAreaRect;

        constexpr int windowSizePadding = 100;

        // Calculate window size to account for the actual client size.
        windowClientAreaRect.left = windowSizePadding;
        windowClientAreaRect.right = m_WindowArea.width + windowSizePadding;
        windowClientAreaRect.top = windowSizePadding;
        windowClientAreaRect.bottom = m_WindowArea.height + windowSizePadding;

        // Create the window.
        m_WindowHandle = CreateWindowEx(
            0,                                        // 0 for no optional window styles.
            SMP_WINDOW_CLASS_NAME,
            SMP_WINDOW_TITLE,
            WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // Window styles
            CW_USEDEFAULT, CW_USEDEFAULT,             // X,Y position.
            m_WindowArea.width, m_WindowArea.height,  // Window size.
            nullptr,                                  // Parent window.
            nullptr,                                  // Menu.
            GetModuleHandle(NULL),                    // Handle to the window instance. (HINSTANCE)
            this                                      // Other optional program data. (LPVOID)

            /* NOTE : We pass in this to be able to encapsulate the window procedure 
            *         as a instance member function so we have access to Window state.
            *
            *         For more info on why, see comments on line 13.
            */
        );

        if (m_WindowHandle == nullptr)
        {
            std::cerr << "Failed to create the window.\n";
            return;
        }

        m_IsInitialized = true;

        DEBUG_PRINT("Created the window.\n");
	}
    #pragma endregion

    #pragma region Public API
    void Window::StartMessageLoop()
    {
        m_IsRunning = true;

        ShowWindow(m_WindowHandle, SW_SHOW);

        m_IsShown = true;

        // TODO : Start the message loop thread.
        //  Pseudo ---> m_MessageLoop = std::thread(MessageLoop);
    }
    #pragma endregion

    #pragma region Private Implementations
    LRESULT CALLBACK Window::WndProcSetup(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        return DefWindowProc(windowHandle, msgCode, wParam, lParam);

        // TODO : Implement the storing of the Window instance (this) in user data, and set the window procedure to WndProcThunk.
    }

    LRESULT CALLBACK Window::WndProcThunk(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        return DefWindowProc(windowHandle, msgCode, wParam, lParam);

        // TODO : Implement the retrieval of the Window instance from user data and the calling of the instance's WndProc.
    }

    LRESULT CALLBACK Window::WndProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        switch (msgCode)
        {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(windowHandle, msgCode, wParam, lParam);
    }
    #pragma endregion
}