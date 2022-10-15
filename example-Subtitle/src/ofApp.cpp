#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	path = "subs/Huxley.srt";
	//path = "subs/Alphaville.srt";
	//path = "subs/spanish.srt";

	sub.setup(path);
}

//--------------------------------------------------------------
void ofApp::update() {
	sub.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	sub.draw();
	sub.drawGui();
}
