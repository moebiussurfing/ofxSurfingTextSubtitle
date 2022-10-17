#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	path = "subs/Huxley.srt";
	//path = "subs/Alphaville.srt";
	//path = "subs/spanish.srt";

	subs.setup(path);
}

//--------------------------------------------------------------
void ofApp::update() {
	subs.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	subs.draw();
	subs.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'e') { subs.setToggleEdit(); }
	if (key == ' ') { subs.setTogglePlay(); }
	if (key == OF_KEY_RETURN) { subs.setToggleAuto(); }

	// browse subs
	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleIndex((int)ofRandom(subs.getNumSubtitles())); };
}