// UniLight by HunterZ

#pragma once

#include "UniLight.h"

// utilities for working with Logitech Gaming LED API
namespace LLEDUtil
{
	class LLEDUtilC
	{
		public:

			explicit LLEDUtilC();

			virtual ~LLEDUtilC();

			ResultT SetLLEDColor(unsigned char red, unsigned char green, unsigned char blue);
	};
}
