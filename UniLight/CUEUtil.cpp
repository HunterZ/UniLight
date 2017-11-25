// UniLight by HunterZ

#include "CUEUtil.h"

#include "CUESDK.h"

#include <vector>

namespace CUEUtil
{
	CUEUtilC::CUEUtilC()
	{
		CorsairProtocolDetails cpd(CorsairPerformProtocolHandshake());
		// bool CorsairSetLayerPriority(int priority)
	}

	CUEUtilC::~CUEUtilC()
	{
	}

	bool CUEUtilC::SetCUEColor(unsigned char red, unsigned char green, unsigned char blue)
	{
		// scoop up desired LED IDs and associated colors into a vector
		std::vector<CorsairLedColor> ledVector;

		// try adding all CorsairLedId types, since they're not in the context
		//  of a specific device anyway
		for (int i(1); i < CLI_Last; ++i)
		{
			ledVector.push_back(CorsairLedColor{ static_cast<CorsairLedId>(i), red, green, blue });
		}

		/*
		// get number of Corsair devices
		// need to enumerate devices on every call, in case of hot swapping
		const int deviceCount(CorsairGetDeviceCount());
		// loop over list of devices
		for (int i(0); i < deviceCount; ++i)
		{
			CorsairDeviceInfo* deviceInfoPtr(CorsairGetDeviceInfo(i));
			switch (deviceInfoPtr->type)
			{
				case CDT_Unknown:
					// obviously this is unsupported
				break;

				case CDT_Mouse:
				break;

				case CDT_Keyboard:
				case CDT_MouseMat:
				{
					// keyboards and mousemats support CorsairGetLedPositionsByDeviceIndex()
					CorsairLedPositions* ledPositionsPtr(CorsairGetLedPositionsByDeviceIndex(i));
					for (int j(0); j < ledPositionsPtr->numberOfLed; ++j)
					{
						ledVector.push_back(CorsairLedColor{ ledPositionsPtr->pLedPosition[j].ledId, red, green, blue });
					}
				}
				break;

				case CDT_Headset:
				break;

				case CDT_HeadsetStand:
				break;
			}
		}
		*/

		if (ledVector.empty()) return false;

		// don't really care if it fully succeeds, so call async with NULL
		return CorsairSetLedsColorsAsync(ledVector.size(), ledVector.data(), 0, 0);
	}
}
