#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	subs.setUiPtr(&ui);
	subs.setDisableGuiInternal(true);

	path = "subs/Huxley.srt";
	//path = "subs/Alphaville.srt";
	//path = "subs/spanish.srt";

	subs.setup(path);

#ifdef USE_WHISPER
	whisper.setup();
	whisper.vCallback.addListener(this, &ofApp::doUpdatedWhisper);
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

			ui.EndWindow();
		}

		subs.drawImGui();
	}
	ui.End();

#ifdef USE_WHISPER
	whisper.draw();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') { subs.setToggleVisibleGui(); }
	if (key == 'l') { subs.setToggleLive(); }
	if (key == 'e') { subs.setToggleEdit(); }
	if (key == ' ') { subs.setTogglePlay(); }
	if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }
	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
}

#ifdef USE_WHISPER
//--------------------------------------------------------------
void ofApp::doUpdatedWhisper() {
	string s = whisper.getLastText();
	ofLogNotice() << "doUpdatedWhisper(): " << s;
	subs.setTextSlide(s);
}
#endif