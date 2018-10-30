// UniLight by HunterZ

#pragma once

#include "UniLight.h" // ResultT

// utilities for working with Razer Chroma API
namespace RZCUtil
{
	// class for working with Razer Chroma API
	// no constructor because lazy initialization is used
	class RZCUtilC
	{
		public:

			virtual ~RZCUtilC();

			ResultT SetRZCColor(unsigned char red, unsigned char green, unsigned char blue);
	};
}
