#include "ofApp.h"

#include <string>
#include <bitset>

void ofApp::setup() {
	ofSetFrameRate(60);
	ofBackground(0);

	loadConfig();
	ofLogNotice() << "Aplicativo iniciado. Teclas 1 a 8 acionam as faces; Esc encerra.";

	if (globals::serialPort.empty()) {
		ofLogNotice() << "Porta serial nao configurada. Controle por teclado disponivel.";
		return;
	}

	// Inicializa a comunicacao com o Arduino
	serial.listDevices();

	if (serial.setup(globals::serialPort, globals::baudrate)) {
		ofLogNotice()
			<< "Arduino conectado: "
			<< globals::serialPort
			<< " @ " << globals::baudrate;
	} else {
		ofLogError()
			<< "Nao foi possivel abrir: "
			<< globals::serialPort;
	}

	toggleCursor();
}

void ofApp::loadConfig() {
	faces.clear();
	for (const auto & path : globals::facesPaths) {
		auto face = std::make_unique<Face>();
		if (!face->setup(path)) {
			// Mantem a correspondencia entre o indice da face e seu botao.
			faces.push_back(nullptr);
			continue;
		}
		face->setFadeTime(globals::fadeTime);
		faces.push_back(std::move(face));
		validFaces++;
	}
}

void ofApp::update() {
	updateSerial();

	for (auto & face : faces) {
		if (face) face->update();
	}
}

void ofApp::updateSerial() {
	if (!serial.isInitialized()) return;

	int latest = -1;

	while (serial.available() > 0) {
		int received = serial.readByte();

		if (received >= 0 && received <= 255) {
			latest = received;
		}
	}

	if (latest >= 0) {
		processButtons(static_cast<uint8_t>(latest));
	}
}

void ofApp::processButtons(uint8_t buttons) {
	// Primeiro byte estabelece o estado inicial,
	// sem gerar acionamentos.
	if (!buttonsInitialized) {
		lastButtons = buttons;
		buttonsInitialized = true;
		return;
	}

	// Detecta somente transicoes de 0 para 1
	uint8_t pressed = buttons & ~lastButtons;

	cout << "data received: " << std::bitset<8>(pressed) << endl;

	// Botao 1 = bit 0
	// Botao 8 = bit 7
	for (int i = 0;
		i < globals::N_BUTTONS && i < (int)faces.size();
		i++) {

		if (faces[i] && (pressed & (1u << i))) {
			faces[i]->toggle();
		}
	}

	// Atualiza o estado anterior mesmo quando
	// alguma Face ignora o comando durante o fade.
	lastButtons = buttons;
}

void ofApp::draw() {
	ofBackground(0);

	for (size_t i = 0; i < faces.size(); i++) {
		if (!faces[i]) continue;
		int index = static_cast<int>(i);
		int x = globals::paddingL + globals::videoSpacingH * (index % globals::N_COLS);

		int y = globals::paddingT + globals::videoSpacingV * (index / globals::N_COLS);

		faces[i]->draw(x, y, globals::videoW, globals::videoH);
	}

	if (debug) drawDebug();
}

void ofApp::keyReleased(int key) {
	// Mantem o teclado como alternativa para testes.
	int index = key - '1';

	if (index >= 0 && index < (int)faces.size() && faces[index]) {
		faces[index]->toggle();
	} else {
		switch (key) {
			case 'f':
			case 'F':
				ofToggleFullscreen();
				break;
			case 'd':
			case 'D':
				toggleDebug();
				break;
			case 'c':
			case 'C':
				toggleCursor();
				break;
		}
	}
}

void ofApp::toggleCursor() {
	if (showCursor)
		ofHideCursor();
	else
		ofShowCursor();
	showCursor = !showCursor;
}


void ofApp::toggleDebug() {
	debug = !debug;
}

void ofApp::drawDebug() {
	std::string debugTxt = "FPS: " + ofToString(ofGetFrameRate()) + "\n";
	debugTxt += "Posicoes de face: " + ofToString(faces.size()) + "\n";
	debugTxt += "Faces validas:" + ofToString(validFaces) + "\n";

	ofDrawBitmapStringHighlight(debugTxt, 20, 20, 0, 255);
}
