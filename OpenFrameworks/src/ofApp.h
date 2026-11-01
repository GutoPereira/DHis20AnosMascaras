#pragma once

#include "Face.h"
#include "ofMain.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "globals.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key);
	void toggleDebug();
	void toggleCursor();
	void drawDebug();

private:
	std::vector<std::unique_ptr<Face>> faces;

	bool showCursor = true;
	bool debug = false;

	ofSerial serial;

	uint8_t lastButtons = 0;
	bool buttonsInitialized = false;

	int validFaces = 0;

	void loadConfig();
	void updateSerial();
	void processButtons(uint8_t buttons);
	
	bool lArrowPressed = false;
	bool rArrowPressed = false;
	bool uArrowPressed = false;
	bool dArrowPressed = false;
	
	bool shiftPressed = false;
	
	bool sPressed = false;
};
