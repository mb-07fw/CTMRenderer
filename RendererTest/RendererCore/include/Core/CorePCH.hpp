#pragma once

// Include Windows stuff.
#include "CoreDefines.hpp"
#include <Windows.h>
#include <stringapiset.h> // WideCharToMultiByte in CoreUtility.hpp's TranslateHResult(hResult).
#include <WinNls.h> // CP_UTF8 for WideCharToMultiByte in CoreUtility.hpp's TranslateHResult(hResult).

// Include Direct3D stuff.
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <dxgidebug.h>

// Other C++ utilities used throughout RendererCore.
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <thread>
#include <optional>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <type_traits>