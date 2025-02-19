#pragma once

namespace CTMRenderer::CTMDirectX::Graphics
{
	enum class DXColorType
	{
		BLACK,
		WHITE,
		RED,
		GREEN,
		BLUE,
	};

	// Represents un-normalized RGBA values.
	struct DXColor
	{
		static constexpr unsigned char RED_CHANNEL = 0;
		static constexpr unsigned char GREEN_CHANNEL = 1;
		static constexpr unsigned char BLUE_CHANNEL = 2;
		static constexpr unsigned char ALPHA_CHANNEL = 3;
		static constexpr unsigned char NUM_CHANNELS = 4;

		inline DXColor(DXColorType colorType)
		{
			SetAll(0);
			rgba[ALPHA_CHANNEL] = 255;

			switch (colorType)
			{
			case DXColorType::BLACK:
				return;
			case DXColorType::WHITE:
				SetAll(255);
				break;
			case DXColorType::RED:
				rgba[RED_CHANNEL] = 255;
				break;
			case DXColorType::GREEN:
				rgba[GREEN_CHANNEL] = 255;
				break;
			case DXColorType::BLUE:
				rgba[BLUE_CHANNEL] = 255;
				break;
			default:
				return;
			}
		}

		inline DXColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0)
		{
			rgba[0] = r;
			rgba[1] = g;
			rgba[2] = b;
			rgba[3] = a;
		}

		inline void SetAll(unsigned char color)
		{
			for (size_t i = 0; i < NUM_CHANNELS; ++i)
				rgba[i] = color;
		}

		inline [[nodiscard]] unsigned char r() const noexcept { return rgba[0]; }
		inline [[nodiscard]] unsigned char g() const noexcept { return rgba[1]; }
		inline [[nodiscard]] unsigned char b() const noexcept { return rgba[2]; }
		inline [[nodiscard]] unsigned char a() const noexcept { return rgba[3]; }

		unsigned char rgba[4];
	};

	struct DXNormColor
	{
		inline DXNormColor(float r = 0, float g = 0, float b = 0, float a = 1)
		{
			rgba[0] = r;
			rgba[1] = g;
			rgba[2] = b;
			rgba[3] = a;
		}

		inline [[nodiscard]] float r() const noexcept { return rgba[0]; }
		inline [[nodiscard]] float g() const noexcept { return rgba[1]; }
		inline [[nodiscard]] float b() const noexcept { return rgba[2]; }
		inline [[nodiscard]] float a() const noexcept { return rgba[3]; }

		float rgba[4];
	};
}