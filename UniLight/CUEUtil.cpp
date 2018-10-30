// UniLight by HunterZ

#include "CUEUtil.h"

#include "cots/corsair/CUESDK.h"
#include <tchar.h>
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

	ResultT CUEUtilC::SetCUEColor(unsigned char red, unsigned char green, unsigned char blue)
	{
		// scoop up desired LED IDs and associated colors into a vector
		std::vector<CorsairLedColor> ledVector;

		// try adding all CorsairLedId types, since they're not in the context
		//  of a specific device anyway
		for (int i(1); i < CLI_Last; ++i)
		{
			ledVector.push_back(CorsairLedColor{ static_cast<CorsairLedId>(i), red, green, blue });
		}

		if (ledVector.empty())
			return ResultT(false, _T("SetCUEColor() found no IDs"));

		// don't really care if it fully succeeds, so call async with NULL
		if (!CorsairSetLedsColorsAsync(ledVector.size(), ledVector.data(), 0, 0))
			return ResultT(false, _T("CorsairSetLedsColorsAsync() failed"));

		return ResultT(true, _T("SetCUEColor() success"));
	}
}
