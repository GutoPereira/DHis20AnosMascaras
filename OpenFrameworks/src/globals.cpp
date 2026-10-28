#include "ofMain.h"
#include "globals.h"

int globals::paddingL = 0;
int globals::paddingT = 0;
int globals::videoW = 50;
int globals::videoH = 100;
int globals::videoSpacingH = 100;
int globals::videoSpacingV = 150;
int globals::fadeTime = 2000;
bool globals::fullscreen = false;
std::string globals::serialPort = "";
int globals::baudrate = 9600;
std::vector<std::string> globals::facesPaths;

void globals::loadConfig() {
	globals::facesPaths.clear();

	ofXml xml;

	if (!xml.load("config.xml")) {
		ofLogError() << "Erro ao carregar config.xml";
		return;
	}

	ofXml app = xml;

	if (app.getName() != "App") {
		app = xml.getChild("App");
	}

	if (app.getName() != "App") {
		ofLogError() << "Elemento App nao encontrado";
		return;
	}

	auto getString = [&](const std::string & name,
						 const std::string & fallback) {
		std::string value = app.getAttribute(name).getValue();

		return value.empty() ? fallback : value;
	};

	auto getInt = [&](const std::string & name, int fallback) {
		std::string value = app.getAttribute(name).getValue();

		if (value.empty()) return fallback;

		try {
			return std::stoi(value);
		} catch (...) {
			return fallback;
		}
	};

	// Arduino
	globals::serialPort = getString("arduinoPort", globals::serialPort);
	globals::baudrate = getInt("baudrate", globals::baudrate);

	// Layout
	globals::paddingL = getInt("paddingL", globals::paddingL);
	globals::paddingT = getInt("paddingT", globals::paddingT);

	globals::videoW = getInt("videoW", globals::videoW);
	globals::videoH = getInt("videoH", globals::videoH);

	globals::videoSpacingH = getInt("videoSpacingH", globals::videoSpacingH);

	globals::videoSpacingV = getInt("videoSpacingV", globals::videoSpacingV);

	globals::fadeTime = std::max(0, getInt("fadeTime", globals::fadeTime));
	
	
	globals::fullscreen = getString("fullscreen", globals::serialPort) == "true";

	// Carregamento das faces
	for (auto faceXml : app.getChildren("face")) {
		std::string path = "videos/" + faceXml.getAttribute("file").getValue();

		if (path.empty()) continue;

		facesPaths.push_back(path);
	}

	ofLogNotice()
		<< "Faces carregadas: " << facesPaths.size();
}
