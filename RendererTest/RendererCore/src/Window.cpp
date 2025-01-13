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

        // Start the message loop thread.
        m_MessageLoop = std::thread(
            [this]() { 
                this->MessageLoop();
            }
        );
    }

    void Window::JoinMessageLoop()
    {
        if (!m_MessageLoop.joinable())
        {
            std::cerr << "[Window | JoinMessageLoop] Unable to join the message loop.\n";
            return;
        }

        PostQuitMessage(0);  // Signal to exit the message loop
        m_IsRunning = false; // Ensure the running flag is false

        m_MessageLoop.join();
    }
    #pragma endregion

    #pragma region Private Implementations
    void Window::MessageLoop()
    {
        DEBUG_PRINT("Message loop start.\n");

        BOOL result;
        MSG msg;

        while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
        {
            DEBUG_PRINT("GOT MESSAGE.\n");
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (result == -1)
            std::cerr << "GetMessage received an invalid parameter.\n";

        else if (result == 0)
            DEBUG_PRINT("Window exiting.\n");

        DEBUG_PRINT("Message loop end.\n");
    }

    LRESULT CALLBACK Window::WndProcSetup(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        // If we get a message before the WM_NCCREATE message, handle with default window procedure provided by the WinAPI.
        //   (WM_NCCREATE contains the instance of Window that was passed to CrateWindowEx)
        if (msgCode != WM_NCCREATE)
            return DefWindowProc(windowHandle, msgCode, wParam, lParam);

        // Retrieve create parameter passed into CreateWindowEx that stores an instance of Window.
        const CREATESTRUCTW* const pCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWindow = static_cast<Window*>(pCreateStruct->lpCreateParams);

        // Set WinAPI-managed user data to store the instance to Window.
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

        // Set message procedure that WndProcThunk now that the instance of Window is stored.
        SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Window::WndProcThunk));

        // Forward message to the instance WndProc.
        return pWindow->WndProc(windowHandle, msgCode, wParam, lParam);
    }

    LRESULT CALLBACK Window::WndProcThunk(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        // Retrieve pointer to instance of Window from WinAPI user data.
        Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

        // Forward message to the instance handler.
        return pWindow->WndProc(windowHandle, msgCode, wParam, lParam);
    }

    LRESULT CALLBACK Window::WndProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        DEBUG_PRINT("WNDPROC.\n");

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