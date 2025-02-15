#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Renderer/DirectX/Window/DXWindow.hpp"

namespace CTMRenderer::CTMDirectX::Window
{
    #pragma region Constructors
    DXWindow::DXWindow(const DXRendererSettings& settingsRef, Event::EventDispatcher& eventDispatcherRef, UINT width, UINT height)
        : m_SettingsRef(settingsRef), m_EventDispatcherRef(eventDispatcherRef), m_ClientArea(width, height)
    {
        // Wait until Start is called to initialize the window, due to how CTMRenderer doesn't use RAII,
        // and runs on a separate thread, so the window must be created on the it's thread for CTMRenderer
        // to be able to read messages.
	}
    #pragma endregion

    #pragma region Public API
    void DXWindow::Start() noexcept
    {
        Init();

        m_IsRunning.store(true, std::memory_order_release);
    }

    void DXWindow::HandleMessages(BOOL& result, MSG& msg) noexcept
    {
        RUNTIME_ASSERT(m_IsInitialized.load(std::memory_order_acquire), "The window isn't initialized.\n");

        // Handle all queued messages.
        // (If there are messages, the return value is nonzero, otherwise the return value is zero)
        while ((result = PeekMessageW(&msg, nullptr, 0u, 0u, PM_REMOVE)) > 0)
        {
            if (msg.message == WM_QUIT)
            {
                DEBUG_PRINT("Broadcasting end event.\n");
                
                m_EventDispatcherRef.QueueEvent<Event::EndEvent>(1738u); // ayyy
                return;
            }

            // Translate any raw virtual-key messages in character messages. (e.g., 'w', 'a', 's', 'd', etc)
            TranslateMessage(&msg);

            // Forward the message to the current window procedure.
            DispatchMessageW(&msg);
        }
    }

    void DXWindow::SetTitle(const std::wstring& title) noexcept
    {
        SetWindowTextW(m_WindowHandle, title.c_str());
    }
    #pragma endregion

    #pragma region Private Functions
    void DXWindow::Init()
    {
        RUNTIME_ASSERT(m_IsInitialized.load() == false, "The window is already initialized.\n");

        WNDCLASSEXW wndClass = {};
        wndClass.cbSize = sizeof(WNDCLASSEXW);
        wndClass.lpfnWndProc = WndProcSetup;    /*  Set the window procedure to the static setup function that will eventually thunk (redirect / forward)
                                                 *  Window's messages to an instance function that has access to Window and Renderer state.
                                                 *
                                                 *  The function assigned to lpfnWndProc has to be static due to how instance functions work in C++,
                                                 *  and how windows procedures are defined in the WinAPI.
                                                 *
                                                 *  For context, Instance functions implicitly take in an argument of this, which is incompatible with
                                                 *  the WinAPI window procedure standard, thus static functions are required as they omit the this argument.
                                                 *
                                                 *  In conclusion, the indirection is required to be able to access CTMRenderer::Window state in the window procedure without
                                                 *  the introduction of global state.
                                                 */

        wndClass.hInstance = GetModuleHandleW(nullptr);
        RUNTIME_ASSERT(wndClass.hInstance != nullptr, "Failed to get the HINSTANCE.\n");

        wndClass.lpszClassName = SP_WINDOW_CLASS_NAME;

        bool registeredClass = RegisterClassEx(&wndClass);
        RUNTIME_ASSERT(registeredClass != false, "Failed to register window class.\n");

        constexpr int windowSizePadding = 100;
        RECT windowClientAreaRect = {};

        // Calculate window size to account for the actual client size. (client area with 100 pixel padding)
        windowClientAreaRect.left = windowSizePadding;
        windowClientAreaRect.right = m_ClientArea.width + windowSizePadding;
        windowClientAreaRect.top = windowSizePadding;
        windowClientAreaRect.bottom = m_ClientArea.height + windowSizePadding;

        // Create the window.
        m_WindowHandle = CreateWindowEx(
            0,                                        // 0 for no optional window styles.
            SP_WINDOW_CLASS_NAME,
            SP_WINDOW_TITLE,
            WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // Window styles
            CW_USEDEFAULT, CW_USEDEFAULT,             // X,Y position.
            m_ClientArea.width, m_ClientArea.height,  // Window size.
            nullptr,                                  // Parent window.
            nullptr,                                  // Menu.
            GetModuleHandle(NULL),                    // Handle to the instance of the application. (HINSTANCE)
            this                                      // Other optional program data. (LPVOID)

            /* NOTE : We pass in this to be able to encapsulate the window procedure
             *        as a instance member function so we have access to Window and Renderer state.
             */
        );
        RUNTIME_ASSERT(m_WindowHandle != nullptr, "Failed to create the window.\n");

        m_IsInitialized.store(true, std::memory_order_release);

        ShowWindow(m_WindowHandle, SW_SHOW);
        m_IsShown.store(true, std::memory_order_release);
    }

    LRESULT CALLBACK DXWindow::WndProcSetup(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        // If we get a message before the WM_NCCREATE message, handle with default window procedure provided by the WinAPI.
        // (WM_NCCREATE contains the instance of Window that was passed to CrateWindowEx)
        if (msgCode != WM_NCCREATE)
            return DefWindowProc(windowHandle, msgCode, wParam, lParam);

        // Retrieve create parameter passed into CreateWindowEx that stores an instance of Window.
        const CREATESTRUCTW* const pCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        DXWindow* const pWindow = static_cast<DXWindow*>(pCreateStruct->lpCreateParams);

        // Set WinAPI-managed user data to store the instance to Window.
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

        // Set message procedure to WndProcThunk now that the instance of Window is stored.
        SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(DXWindow::WndProcThunk));

        // Forward message to the instance WndProc.
        return pWindow->WndProc(windowHandle, msgCode, wParam, lParam);
    }

    LRESULT CALLBACK DXWindow::WndProcThunk(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        // Retrieve pointer to instance of Window from WinAPI user data.
        DXWindow* const pWindow = reinterpret_cast<DXWindow*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

        // Forward message to the instance handler.
        return pWindow->WndProc(windowHandle, msgCode, wParam, lParam);
    }

    LRESULT CALLBACK DXWindow::WndProc(HWND windowHandle, UINT msgCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        switch (msgCode)
        {
        case WM_KEYDOWN:
            if (wParam != VK_ESCAPE)
                return S_OK;
            // Fall through if Esc was pressed.
        case WM_CLOSE:
            PostQuitMessage(0);
            return S_OK;
        case WM_MOUSEMOVE:
            m_EventDispatcherRef.QueueEvent<Event::MouseMoveEvent>(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return S_OK;
        default:
            return DefWindowProc(windowHandle, msgCode, wParam, lParam);
        }
    }
    #pragma endregion
}