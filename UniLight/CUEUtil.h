// UniLight by HunterZ

#pragma once

#include "UniLight.h"

// utilities for working with Corsair Utility Engine (CUE) API
namespace CUEUtil
{
	class CUEUtilC
	{
		public:

			explicit CUEUtilC();

			virtual ~CUEUtilC();

			ResultT SetCUEColor(unsigned char red, unsigned char green, unsigned char blue);
	};
}
