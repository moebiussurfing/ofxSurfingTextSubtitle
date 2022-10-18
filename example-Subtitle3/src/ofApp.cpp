#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowPosition(-1920, 25);
	ofSetLogLevel("ofxSurfingTextSubtitle", OF_LOG_SILENT);

	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	// Video
	//bLoaded = movie.load("movies/fingers.mov");
	//TODO: some videos make drop down the frame rate a lot.
	// should check installed codecs here?.
	bLoaded = movie.load(ofToDataPath("Z:\\_DATA\\VIDEO\\Huxley_.mp4", true));
	//bLoaded = movie.load(ofToDataPath("Z:\\_DATA\\VIDEO\\LET.mp4", true));
	//bLoaded = movie.load(ofToDataPath("Z:\\_DATA\\\VIDEO\\fingers.mov", true));

	// Subs
	path = "subs/Huxley.srt";
	//path = "subs/Alphaville.srt";
	//path = "subs/spanish.srt";

	subs.setup(path);

	//play();//TODO: crashes bc callbacks
}

//--------------------------------------------------------------
void ofApp::update()
{
	// Start play 1 frame delayed vs setup to avoid crashes.
	if (ofGetFrameNum() == 1) play(); 

	movie.update();
	 
	subs.update();
	
	/*
	//TODO: WIP: link
	//float fps = movie.fps//not implemented?
	//float time = (movie.getDuration() / movie.getTotalNumFrames()) * movie.getCurrentFrame();//not implemented
	uint64_t frame = movie.getCurrentFrame();
	subs.update(frame);
	*/
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	string s = "example-Subtitle3 | " + ofToString(ofGetFrameRate(), 0) + " Fps";
	ofSetWindowTitle(s);

	// Video
	{
		if (!bLoaded)
			if (ofGetFrameNum() % 30 < 15)
				ofDrawBitmapStringHighlight("FILE NOT FOUND OR WRONG VIDEO FORMAT!", ofGetWidth() / 2, 20);

		if (bScale) {
			ofRectangle r = ofGetCurrentViewport();
			ofRectangle rv(0, 0, movie.getWidth(), movie.getHeight());
			rv.scaleTo(r, OF_SCALEMODE_FILL);
			movie.draw(rv);
		}
		else movie.draw(0, 0);
	}

	// Subs
	subs.draw();
	subs.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') { subs.setToggleVisibleGui(); }
	if (key == 'e') { subs.setToggleEdit(); }
	if (key == 's') { bScale = !bScale; }

	if (key == OF_KEY_RETURN) { subs.setToggleAuto(); }
	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	//if (key == OF_KEY_BACKSPACE) { subs.setSubtitleIndex((int)ofRandom(subs.getNumSubtitles())); };
	
	// random video position
	if (key == OF_KEY_BACKSPACE) movie.setPosition(ofRandom(1.f));

	// Play both!
	if (key == ' ') { togglePlay(); }
}

//--------------------------------------------------------------
void ofApp::play() {
	subs.play();
	movie.play();
}
//--------------------------------------------------------------
void ofApp::stop() {
	subs.stop();
	movie.stop();
}
//--------------------------------------------------------------
void ofApp::togglePlay() {
	subs.setTogglePlay();
	if (subs.isPlaying()) movie.play();
	else movie.stop();
}