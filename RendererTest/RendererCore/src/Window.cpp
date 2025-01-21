#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Window.hpp"
#include "Color.hpp"

namespace Renderer::Window
{
    #pragma region Constructors
    Window::Window(Event::EventManagerInterface& eventManagerInterface, unsigned int width, unsigned int height)
        : m_EventManagerInterface(eventManagerInterface), m_WindowArea(width, height), m_Graphics(m_WindowArea), m_WindowHandle(nullptr),
          m_IsInitialized(false), m_IsShown(false), m_IsRunning(false), m_MessageLoop(), m_Mutex(), m_CV()
    {
        // Start the message loop thread.
        m_MessageLoop = std::thread(&Window::MessageLoop, this);
	}
    #pragma endregion

    #pragma region Public API
    void Window::Start() noexcept
    {
        m_IsRunning.store(true, std::memory_order_release);
        m_CV.notify_one();
    }

    void Window::Join() noexcept
    {
        if (!m_MessageLoop.joinable())
        {
            DEBUG_PRINT_ERROR("Unable to join Window's message loop.\n");
            return;
        }

        m_IsRunning.store(false, std::memory_order_release);

        m_MessageLoop.join();
    }

    void Window::DoFrame() noexcept
    {
        RUNTIME_ASSERT(m_IsRunning.load(std::memory_order_acquire), "Window is not running.\n");
        
        m_Graphics.StartFrame();
        m_Graphics.Draw();
        m_Graphics.EndFrame();
    }
    #pragma endregion

    #pragma region Private Functions
    void Window::Init()
    {
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
                                                 *  In conclusion, the indirection is required to be able to access Window state in the window procedure without
                                                 *  the introduction of global state.
                                                 */

                                                 // Get handle of the file that created the .exe process.
        wndClass.hInstance = GetModuleHandle(NULL);
        RUNTIME_ASSERT(wndClass.hInstance != nullptr, "Failed to get the HINSTANCE.\n");

        wndClass.lpszClassName = SMP_WINDOW_CLASS_NAME;

        bool registeredClass = RegisterClassEx(&wndClass);
        RUNTIME_ASSERT(registeredClass != false, "Failed to register window class.\n");

        constexpr int windowSizePadding = 100;
        RECT windowClientAreaRect;

        // Calculate window size to account for the actual client size. (640 x 480 client area with 100 pixel padding)
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
             *         as a instance member function so we have access to Window and Renderer state.
             *
             *        For more info on why or how, see comments on line 29.
             */
        );

        RUNTIME_ASSERT(m_WindowHandle != nullptr, "Failed to create the window.\n");

        m_Graphics.Init(m_WindowHandle);

        m_IsInitialized = true;
    }

    void Window::MessageLoop()
    {
        DEBUG_PRINT("Message loop start.\n");

        // Create the window on the message loop thread to be able to read it's messages.
        // (You have to create the window on the same thread you read it's messages on)
        Init();

        // Wait until Start is called.
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_CV.wait(lock, [this] { return m_IsRunning.load(std::memory_order_acquire); });

        ShowWindow(m_WindowHandle, SW_SHOW);

        m_IsShown.store(true, std::memory_order_relaxed);

        BOOL result;
        MSG msg;

        while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        RUNTIME_ASSERT(result != -1, "GetMessage received an invalid parameter, or another error occurred.\n");

        DEBUG_PRINT("Broadcasting end event.\n");
        m_EventManagerInterface.BroadcastEventSafe(Event::EventType::RENDERER_END);

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