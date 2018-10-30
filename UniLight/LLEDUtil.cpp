// UniLight by HunterZ

#include "LLEDUtil.h"

#include "cots/logitech/LogitechLEDLib.h"
#include <tchar.h>

namespace
{
	const double conversionFactor(100.0 / 255.0);

	int b2p(const unsigned char color)
	{
		return static_cast<int>(static_cast<double>(color) * conversionFactor + 0.5);
	}

	bool initialized(false);

	ResultT initLLED()
	{
		if (initialized)
			return ResultT(true, _T("Already initialized"));

		if (!LogiLedInit())
			return ResultT(false, _T("LogiLedInit() failed"));

		// save previous lighting state as courtesy to user
		// don't care if this succeeds though
		LogiLedSaveCurrentLighting();
		initialized = true;
		return ResultT(true, _T("initLLED() success"));
	}
}

namespace LLEDUtil
{
	LLEDUtilC::LLEDUtilC()
	{
		// SDK says to try to do this early for some reason
		initLLED();
	}

	LLEDUtilC::~LLEDUtilC()
	{
		// attempt to restore original state
		// don't care if these succeed
		LogiLedRestoreLighting();
		LogiLedShutdown();
		initialized = false;
	}

	ResultT LLEDUtilC::SetLLEDColor(unsigned char red, unsigned char green, unsigned char blue)
	{
		const ResultT& result(initLLED());
		if (!result.first)
			return result;

		if (!LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB | LOGI_DEVICETYPE_RGB))
			return ResultT(false, _T("LogiLedSetTargetDevice() failed"));

		if (!LogiLedSetLighting(b2p(red), b2p(green), b2p(blue)))
			return ResultT(false, _T("LogiLedSetLighting() failed"));

		return ResultT(true, _T("SetLLEDColor() success"));
	}
}
