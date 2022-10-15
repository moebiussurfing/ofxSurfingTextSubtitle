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
void ofApp::draw() {
	sub.draw();
}
