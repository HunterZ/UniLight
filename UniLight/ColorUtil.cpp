// UniLight by HunterZ

#include "ColorUtil.h"

#include <dwmapi.h> // DwmGetColorizationColor() and friends

namespace
{
	struct ColorS
	{
		UINT8 blue;
		UINT8 green;
		UINT8 red;
		UINT8 alpha;
	};

	union ColorU
	{
		ColorS cs;
		UINT32 ci;
	};
}

namespace ColorUtil
{
	COLORREF Dword2ColorRef(const DWORD dword)
	{
		static ColorU color;
		color.ci = dword;
		return RGB(color.cs.red, color.cs.green, color.cs.blue);
	}

	bool GetColorizationColor(COLORREF& colorRef)
	{
		static DWORD dwColor(0);
		static BOOL opaque(TRUE);
		static bool useResult(false);
		useResult = SUCCEEDED(DwmGetColorizationColor(&dwColor, &opaque));
		if (useResult)
		{
			colorRef = Dword2ColorRef(dwColor);
		}
		return useResult;
	}
}
