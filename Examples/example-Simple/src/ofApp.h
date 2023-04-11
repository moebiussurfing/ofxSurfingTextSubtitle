
/*

	NOTE

	You must check that ImGui mode is enabled.
	Must be not commented the below line:
	#define USE_IM_GUI__SUBTTITTLES

	Currently checking manual mode.
	You should pick the correct mode
	if you want to test SRT files!

*/

#pragma once
#include "ofMain.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void draw();
	void drawGui();

	ofxSurfingTextSubtitle subs;

	ofxSurfingGui ui;
};
