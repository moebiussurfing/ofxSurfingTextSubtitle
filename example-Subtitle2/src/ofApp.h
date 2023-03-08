
/*

	NOTE:
	You must enable ImGui mode un commenting the below line:
	#define USE_IM_GUI__SUBTTITTLES

*/

// // Optional
// #define USE_PRESETS // Uses ofxSurfingHelpers

//-

#pragma once

#include "ofMain.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"

// #ifdef USE_PRESETS
// #include "ofxSurfingPresetsLite.h"
// #endif

class ofApp : public ofBaseApp 
{
public:

	void setup();
	void update();
	void draw();
	void keyPressed(int key);

	ofxSurfingTextSubtitle subs;
	string path;

	ofxSurfingGui ui;

// #ifdef USE_PRESETS
// 	ofxSurfingPresetsLite presets;
// #endif
};
