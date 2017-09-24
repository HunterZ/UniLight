// UniLight by HunterZ

#pragma once

// utilities for working with Dell LightFX/AlienFX API
namespace LFXUtil
{
	// class for working with Dell LightFX/AlienFX API
	// a class is used here because we need to call cleanup functions on program exit
	// lazy initialization is used, so it's safe to instantiate this at any point
	class LFXUtilC
	{
		public:

			virtual ~LFXUtilC();

			// set LFX color to given RGB value
			// returns true on success, false on failure
			bool SetLFXColor(unsigned char red, unsigned char green, unsigned char blue);
	};
}
