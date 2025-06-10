
/*

	NOTE:
	You must disable ImGui mode to force enable ofxGui mode by default!
	Just do it by commenting the below line: 
	// #define USE_IM_GUI__SUBTITLES

*/


#pragma once

#include "ofMain.h"

#include "ofxSurfingTextSubtitle.h"

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();
	void keyPressed(int key);

	ofxSurfingTextSubtitle subs;
	string path;
};
