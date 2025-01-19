#pragma once

#include <string>

namespace Renderer::Utility
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
}