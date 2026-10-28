#include "Face.h"

#include <algorithm>

bool Face::setup(const std::string & path) {
	ofLogNotice() << "Carregando " << path;

	if (!movie.load(path)) {
		ofLogError() << "Erro ao carregar: " << path;
		return false;
	}

	movie.setLoopState(OF_LOOP_NONE);
	movie.setVolume(0.0f);

	return true;
}

void Face::setFadeTime(int milliseconds) {
	fadeTime = std::max(0, milliseconds);
}

void Face::update() {
	if (state == State::IDLE) return;

	movie.update();

	float duration = movie.getDuration();
	float time = movie.getPosition() * duration;
	float remaining = duration - time;

	// Fade-out automatico no final do video
	if (state != State::FADING_OUT && duration > 0) {
		if (remaining <= fadeTime / 1000.0f) {
			deactivate();
		}
	}

	// Atualizacao dos fades
	if (state == State::FADING_IN || state == State::FADING_OUT) {

		float progress = 1.0f;

		if (fadeDuration > 0) {
			progress = ofClamp(
				(ofGetElapsedTimeMillis() - fadeStartTime) / (float)fadeDuration,
				0.0f, 1.0f);
		}

		float target = state == State::FADING_IN ? 1.0f : 0.0f;

		alpha = ofLerp(fadeStartAlpha, target, progress);

		if (progress >= 1.0f) {
			if (state == State::FADING_IN) {
				state = State::PLAYING;
			} else {
				stop();
				return;
			}
		}
	}

	if (state == State::PLAYING) {
		alpha = 1.0f;
	}

	// Final do arquivo
	if (duration > 0 && time >= duration) {
		stop();
		return;
	}

	alpha = ofClamp(alpha, 0.0f, 1.0f);

	movie.setVolume(alpha);
}

void Face::draw(float x, float y, float w, float h) {
	if (state == State::IDLE || alpha <= 0.0f) return;

	ofPushStyle();

	ofSetColor(255, 255, 255, alpha * 255.0f);
	movie.draw(x, y, w, h);

	ofPopStyle();
}

void Face::activate() {
	if (state != State::IDLE) return;

	state = State::FADING_IN;

	alpha = 0.0f;
	fadeStartAlpha = 0.0f;

	fadeStartTime = ofGetElapsedTimeMillis();
	fadeDuration = fadeTime;

	movie.setVolume(0.0f);
	movie.setPosition(0.0f);
	movie.play();
}

void Face::deactivate() {
	if (state != State::PLAYING && state != State::FADING_IN) return;

	state = State::FADING_OUT;

	fadeStartAlpha = alpha;
	fadeStartTime = ofGetElapsedTimeMillis();
	fadeDuration = fadeTime;

	float duration = movie.getDuration();

	if (duration > 0) {
		float remaining = duration * (1.0f - movie.getPosition());

		int remainingMs = std::max(
			0, (int)(remaining * 1000.0f));

		fadeDuration = std::min(fadeTime, remainingMs);
	}
}

void Face::stop() {
	movie.setVolume(0.0f);
	movie.stop();

	state = State::IDLE;

	alpha = 0.0f;
	fadeStartAlpha = 0.0f;
}

void Face::toggle() {
	if (state == State::IDLE) {
		activate();
	} else if (state == State::PLAYING) {
		deactivate();
	}
}
