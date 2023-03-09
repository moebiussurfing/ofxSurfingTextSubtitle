#pragma once

#include "ofMain.h"
#include "ofxWindowApp.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"

#define USE_WHISPER
#ifdef USE_WHISPER
#include "surfingWhisper.h"
#endif

class ofApp : public ofBaseApp
{
public:

	void setup();
	void update();
	void draw();
	void keyPressed(int key);

	ofxSurfingTextSubtitle subs;
	string path;

	ofParameter<bool> bGui;
	ofxSurfingGui ui;

#ifdef USE_WHISPER
	surfingWhisper whisper;
	void doUpdatedWhisper();
#endif
	
	void doPopulateText(string s = "");

	ofxWindowApp w;
};
