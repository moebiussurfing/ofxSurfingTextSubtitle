#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	//ofSetLogLevel("ofxSurfingTextSubtitle", OF_LOG_WARNING);

	ofSetWindowTitle("example-Subs_ofxGui");
	//ofSetWindowPosition(-1920, 25);
	ofSetWindowShape(1280, 720);

	// subtitles file
	path = "subs/Huxley.srt";
	//path = "subs/Alphaville.srt";
	//path = "subs/spanish.srt";

	subs.setup(path);
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(subs.getColorBg());
	subs.draw();
	subs.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	//if (key == 'g') { subs.setToggleVisibleGui(); }
	//if (key == 'e') { subs.setToggleEdit(); }
	//if (key == 'd') { subs.setToggleDebug(); }

	//// transport
	//if (key == ' ') { subs.setTogglePlay(); }
	//if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }

	//// browse subs
	//if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	//if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	//if (key == OF_KEY_BACKSPACE) { subs.setSubtitleIndex((int)ofRandom(subs.getNumSubtitles())); };
}
