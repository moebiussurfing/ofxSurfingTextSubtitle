#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	//ofxSurfingHelpers::moveWindowToMyLeftMonitor();
	ofxSurfingHelpers::moveWindowToMyRightMonitor();

	// Set the initial SRT file path.
	// But you can load other SRT or TXT files on runtime.
	string path = "files/srt/Huxley.srt";
	//string path = "files/txt/text2.txt";
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

	if (bGui) drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	ui.Begin();
	{
		if (ui.BeginWindow("ofApp")) {
			ui.DrawWidgetsGlobalScaleMini();

			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			ui.Add(fxPro.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);

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
	if (key == 'G' || key == 'g') bGui = !bGui;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::exit() {
}
