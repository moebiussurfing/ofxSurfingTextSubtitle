#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowPosition(-1920, 25);
	ofSetLogLevel("ofxSurfingTextSubtitle", OF_LOG_SILENT);

	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	// Subs
	path = "subs/Huxley.srt";
	//path = "subs/Alphaville.srt";
	//path = "subs/spanish.srt";

	subs.setup(path);

	//subs.play();//TODO: crashes bc callbacks
}

//--------------------------------------------------------------
void ofApp::update()
{
	// Start play 1 frame delayed vs setup to avoid crashes.
	if (ofGetFrameNum() == 1) subs.play();
	 
	subs.update();
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	string s = "example-Subtitle3 | " + ofToString(ofGetFrameRate(), 0) + " Fps";
	ofSetWindowTitle(s);

	subs.draw();
	subs.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') { subs.setToggleVisibleGui(); }
	if (key == 'e') { subs.setToggleEdit(); }

	// Browse subs
	if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }
	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };

	// Play both!
	if (key == ' ') { subs.setTogglePlay(); }

	subs.keyPressed(key);
}
