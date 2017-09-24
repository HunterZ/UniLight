// UniLight by HunterZ

#pragma once

#include <windows.h> // COLORREF

// functions for working with Windows accent color API
namespace ColorUtil
{
	// get current Windows accent color into colorRef
	// returns true on success, false on failure
	bool GetColorizationColor(COLORREF& colorRef);
}
