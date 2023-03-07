#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofSetWindowPosition(-1920, 25);
	//ofSetFrameRate(60);

	subs.setUiPtr(&ui);
	subs.setDisableGuiInternal(true);

	path = "subs/Huxley.srt";
	//path = "subs/Alphaville.srt";
	//path = "subs/spanish.srt";

	subs.setup(path);

// #ifdef USE_PRESETS
// 	presets.setUiPtr(&ui);
// 	presets.AddGroup(subs.params_Preset);
// #endif

#ifdef USE_WHISPER
	whisper.setup();
#endif
}

//--------------------------------------------------------------
void ofApp::update() {
	string s = "example-Whisper " + ofToString(ofGetFrameRate(), 0) + " Fps";
	ofSetWindowTitle(s);

	subs.update();

#ifdef USE_WHISPER
	whisper.update();
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(subs.getColorBg());

	subs.draw();

	subs.drawGui();

	ui.Begin();
	{
		if (ui.BeginWindow("ofApp")) {
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED);

// #ifdef USE_PRESETS
// 			ui.Add(presets.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED);
// #endif
			ui.EndWindow();
		}

		subs.drawImGui();

// #ifdef USE_PRESETS
// 		presets.drawImGui(true);
// #endif
	}
	ui.End();

#ifdef USE_WHISPER
	whisper.draw();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') {
		subs.setToggleVisibleGui();
#ifdef USE_PRESETS
		presets.setVisibleGui(subs.getVisibleGui());
#endif
	}

	if (key == 'l') { subs.setToggleLive(); }
	if (key == 'e') { subs.setToggleEdit(); }

	if (key == ' ') { subs.setTogglePlay(); }
	if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }

	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
}
