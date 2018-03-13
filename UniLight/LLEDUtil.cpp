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
		if (initialized) return true;

		if (LogiLedInit())
		{
			LogiLedSaveCurrentLighting();
			initialized = true;
		}

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

		LogiLedShutdown();
	}

	bool LLEDUtilC::SetLLEDColor(unsigned char red, unsigned char green, unsigned char blue)
	{
		return
		(
			initLLED() &&
			LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB | LOGI_DEVICETYPE_RGB) &&
			LogiLedSetLighting(b2p(red), b2p(green), b2p(blue))
		);
	}
}
