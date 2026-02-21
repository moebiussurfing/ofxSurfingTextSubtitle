#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//ofxSurfingHelpers::moveWindowToMyLeftMonitor();
	ofxSurfingHelpers::moveWindowToMyRightMonitor();

	// Set the initial SRT file path.
	// But you can load other SRT or TXT files on runtime.
	string path = "files/srt/Huxley.srt";
	subs.setup(path);

	subs.setUiPtr(&ui);
}

//--------------------------------------------------------------
void ofApp::update() {
	fxPro.begin();
	{
		ofClear(subs.getColorBg());
		subs.draw();
	}
	fxPro.end(false);
}

//--------------------------------------------------------------
void ofApp::draw() {

	fxPro.draw();

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	ui.Begin();
	{
		if (ui.BeginWindow("ofApp")) {
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			ui.Add(fxPro.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);

			ui.DrawWidgetsGlobalScaleMini();

			ui.EndWindow();
		}

		subs.drawImGui();
	}
	ui.End();

	subs.drawGui();
	fxPro.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'G' || key =='g') bGui = !bGui;

	//fxPro.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

	//fxPro.keyReleased(key);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	//fxPro.windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::exit() {
}
