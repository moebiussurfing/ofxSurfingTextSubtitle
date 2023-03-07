
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
#include "ofxWindowApp.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"

// #ifdef USE_PRESETS
// #include "ofxSurfingPresetsLite.h"
// #endif

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

	ofxSurfingGui ui;

// #ifdef USE_PRESETS
// 	ofxSurfingPresetsLite presets;
// #endif

	ofxWindowApp w;

#ifdef USE_WHISPER
	surfingWhisper whisper;
#endif
};
