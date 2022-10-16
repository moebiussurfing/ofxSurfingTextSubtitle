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

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'd') { subs.setToggleDebug(); }
	if (key == 'e') { subs.setToggleEdit(); }
	if (key == ' ') { subs.setTogglePlay(); }
	if (key == OF_KEY_RETURN) { subs.setToggleAuto(); }

	// browse subs
	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleIndex((int)ofRandom(subs.getNumSubtitles())); };
}