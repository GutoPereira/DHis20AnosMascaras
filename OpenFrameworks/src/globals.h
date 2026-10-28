#pragma once
#include <ofXml.h>
#include <ofLog.h>
#include <algorithm>
#include <string>
#include <vector>

class globals {
	public:
		static const int N_COLS = 4;
		static const int N_BUTTONS = 8;

		static int paddingL;
		static int paddingT;
		static int videoW;
		static int videoH;
		static int videoSpacingH;
		static int videoSpacingV;
		static int fadeTime;
		static bool fullscreen;

		static std::string serialPort;

		static int baudrate;

		static std::vector<std::string> facesPaths;

		static void loadConfig();
};
