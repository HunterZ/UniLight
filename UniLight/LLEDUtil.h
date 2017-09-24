// UniLight by HunterZ

#pragma once

// utilities for working with Logitech Gaming LED API
namespace LLEDUtil
{
	class LLEDUtilC
	{
		public:

			explicit LLEDUtilC();

			virtual ~LLEDUtilC();

			bool SetLLEDColor(unsigned char red, unsigned char green, unsigned char blue);
	};
}
