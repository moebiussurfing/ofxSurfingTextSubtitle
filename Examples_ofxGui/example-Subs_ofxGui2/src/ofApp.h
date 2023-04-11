
/*

	NOTES:
	
	You must disable ImGui mode commenting the below line: 
	//#define USE_IM_GUI__SUBTITLES
	
	Uncomment to enable bundled video player in ofxSurfingTextSubtitle.h .
	// 2. VIDEO PLAYER
	#define USE_WIDGET__VIDEO_PLAYER 

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
