#pragma once

#include <string>

namespace Renderer::Utility
{
	inline constexpr std::string BoolToString(bool state)
	{
		if (state == true)
			return "true";
		else if (state == false)
			return "false";
		else
			return "UNKNOWN BOOLEAN";
	}
}