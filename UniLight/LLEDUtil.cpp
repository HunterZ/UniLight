// UniLight by HunterZ

#include "LLEDUtil.h"

#include "LogitechLEDLib.h"

namespace
{
	const double conversionFactor(100.0 / 255.0);

	int b2p(const unsigned char color)
	{
		return static_cast<int>(static_cast<double>(color) * conversionFactor + 0.5);
	}

	bool initialized(false);

	bool initLLED()
	{
		if (!initialized) initialized = LogiLedInit();

		// don't really care if this succeeds
		LogiLedSaveCurrentLighting();

		return initialized;
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
		LogiLedRestoreLighting();

		void LogiLedShutdown();
	}

	bool LLEDUtilC::SetLLEDColor(unsigned char red, unsigned char green, unsigned char blue)
	{
		if (!initialized && !initLLED()) return false;

		// target only RGB devices (as opposed to monochrome)
		if (!LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB | LOGI_DEVICETYPE_RGB)) return false;

		return LogiLedSetLighting(b2p(red), b2p(green), b2p(blue));
	}
}
