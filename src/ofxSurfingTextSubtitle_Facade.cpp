#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
bool ofxSurfingTextSubtitle::loadSrtFile(const std::string& path) {
	setupSubs(path);
	return bLoadedFileSubs;
}

//--------------------------------------------------------------
bool ofxSurfingTextSubtitle::loadTextFile(const std::string& path) {
	setupTextBlocks(path);
	return bLoadedFileText;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setPlaybackMode(ofxSurfingSubtitle::PlaybackMode mode) {
	const int modeIndex = apiManager.toIndex(mode);
	if (indexModes != modeIndex) {
		indexModes = modeIndex;
	}
}

//--------------------------------------------------------------
ofxSurfingSubtitle::PlaybackMode ofxSurfingTextSubtitle::getPlaybackMode() const {
	return apiManager.fromIndex(indexModes.get());
}

//--------------------------------------------------------------
const char* ofxSurfingTextSubtitle::getPlaybackModeName() const {
	return apiManager.getPlaybackModeName(getPlaybackMode());
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setPlaybackEnabled(bool bEnabled) {
	if (!bEnabled) {
		stop();
		return;
	}

	switch (getPlaybackMode()) {
	case ofxSurfingSubtitle::PlaybackMode::External:
		bPlayExternal = true;
		break;
	case ofxSurfingSubtitle::PlaybackMode::Standalone:
		bPlayStandalone = true;
		break;
	case ofxSurfingSubtitle::PlaybackMode::Forced:
		bPlayForced = true;
		break;
	case ofxSurfingSubtitle::PlaybackMode::Manual:
		bPlayManual = true;
		break;
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setTextSlide(const std::string& text) {
	if (getPlaybackMode() != ofxSurfingSubtitle::PlaybackMode::Manual) {
		setPlaybackMode(ofxSurfingSubtitle::PlaybackMode::Manual);
	}
	if (!bPlayManual) {
		bPlayManual = true;
	}
	doSetTextSlideStart(text);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setStyleSimple(float sizePercent, float lineHeight, float spacing, int alignIndex) {
	const float clampedSize = ofClamp(sizePercent, fSizePrc.getMin(), fSizePrc.getMax());
	const float clampedLineHeight = ofClamp(lineHeight, fLineHeight.getMin(), fLineHeight.getMax());
	const float clampedSpacing = ofClamp(spacing, fSpacing.getMin(), fSpacing.getMax());
	const int clampedAlign = ofClamp(alignIndex, fAlign.getMin(), fAlign.getMax());

	fSizePrc = clampedSize;
	fLineHeight = clampedLineHeight;
	fSpacing = clampedSpacing;
	fAlign = clampedAlign;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setColorsSimple(const ofColor& textColor, const ofColor& backgroundColor) {
	colorTextFloat = ofFloatColor(textColor);
	colorBgFloat = ofFloatColor(backgroundColor);
}

