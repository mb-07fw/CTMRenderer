#pragma once

namespace CTMRenderer::Shapes
{
	enum class CTMColorType
	{
		BLACK,
		WHITE,
		RED,
		GREEN,
		BLUE,
	};

	// Represents un-normalized RGBA values.
	struct CTMColor
	{
		static constexpr unsigned char RED_CHANNEL = 0;
		static constexpr unsigned char GREEN_CHANNEL = 1;
		static constexpr unsigned char BLUE_CHANNEL = 2;
		static constexpr unsigned char ALPHA_CHANNEL = 3;
		static constexpr unsigned char NUM_CHANNELS = 4;

		inline CTMColor(CTMColorType colorType)
		{
			SetAll(0);
			rgba[ALPHA_CHANNEL] = 255;

			switch (colorType)
			{
			case CTMColorType::BLACK:
				return;
			case CTMColorType::WHITE:
				SetAll(255);
				break;
			case CTMColorType::RED:
				rgba[RED_CHANNEL] = 255;
				break;
			case CTMColorType::GREEN:
				rgba[GREEN_CHANNEL] = 255;
				break;
			case CTMColorType::BLUE:
				rgba[BLUE_CHANNEL] = 255;
				break;
			default:
				return;
			}
		}

		inline CTMColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0)
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

	struct CTMNormColor
	{
		inline CTMNormColor(float r = 0, float g = 0, float b = 0, float a = 1)
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