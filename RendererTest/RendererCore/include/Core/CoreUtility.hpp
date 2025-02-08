#pragma once

#include "CoreMacros.hpp"

#include <Windows.h>

#include <string>
#include <stringapiset.h>
#include <WinNls.h>

#include <filesystem>

namespace CTMRenderer::Utility
{
	/* Converts a boolean state to an std::string.
	 * Returns "true", "false", or "NON-BOOLEAN" for non boolean values. */
	inline constexpr std::string BoolToString(bool state)
	{
		if (state == true)
			return "true";
		else if (state == false)
			return "false";
		else
			return "NON-BOOLEAN";
	}

	inline std::string TranslateHResult(const HRESULT hResult) noexcept
    {
        if (hResult == S_OK)
            return "NO_ERROR";

        wchar_t* pMsgBuffer = nullptr;
        DWORD msgLength = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            hResult,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&pMsgBuffer),
            0,
            nullptr
        );

        if (msgLength == 0 || pMsgBuffer == nullptr)
            return "UNKNOWN";

        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pMsgBuffer, -1, nullptr, 0, nullptr, nullptr);
        std::string errorString(bufferSize, 0);
        WideCharToMultiByte(CP_UTF8, 0, pMsgBuffer, -1, &errorString[0], bufferSize, nullptr, nullptr);

        LocalFree(pMsgBuffer);

        return errorString;
    }

    inline std::filesystem::path GetBinDirectory()
    {
        return std::filesystem::absolute("../../bin/");
    }

    inline std::filesystem::path GetOutDirectory()
    {
        return std::filesystem::path(OUTPUT_DIR);
    }

    inline double MinDB(double value, double min)
    {
        return value < min ? min : value;
    }
}