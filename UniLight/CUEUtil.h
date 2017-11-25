// UniLight by HunterZ

#pragma once

// utilities for working with Corsair Utility Engine (CUE) API
namespace CUEUtil
{
	class CUEUtilC
	{
		public:

			explicit CUEUtilC();

			virtual ~CUEUtilC();

			bool SetCUEColor(unsigned char red, unsigned char green, unsigned char blue);
	};
}
