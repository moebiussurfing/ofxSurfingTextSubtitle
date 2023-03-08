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
		if (ui.BeginWindow("ofApp"))
		{
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED);
			if (ui.AddButton("Populate Text!", OFX_IM_BUTTON_BIG_XXL)) {
				//if(subs.get)
				doPopulateText();
			}
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
void ofApp::doPopulateText(string s) {

	// auto generate
	if (s == "") {
		float r = ofRandom(1.f);
		if (r < 0.25f) {
			s = "Hello people, how are you today?";
			s += " And current frame is " + ofToString(ofGetFrameNum());
		}
		else if (r < 0.5f) {
			s = "Hello dude. Ready to wake up?";
		}
		else if (r < 0.75f) {
			s = "I go sleep now. Ready to wake up!";
			s += " Time is " + ofToString(ofGetTimestampString());
		}
		else {
			s = "Dude, hello! Im ready to go out";
			s += " Current time is " + ofToString(ofGetTimestampString());
		}
	}

	ofLogNotice() << s;
	subs.doSetTextSlide(s);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == OF_KEY_F8) {
		doPopulateText();
	}

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
	subs.doSetTextSlide(s);
}
#endif