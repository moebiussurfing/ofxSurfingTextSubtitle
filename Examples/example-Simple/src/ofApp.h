
/*

	NOTE

	You must check that ImGui mode is enabled.
	Must be not commented the below line:
	#define USE_IM_GUI__SUBTTITTLES

	Here we want to test SRT files with common use!
	Currently I recommend to check Standalone or Forced transport modes.
	You should pick these correct modes but could experiment with other modes.
	External mode can be tested by forcing the Position slider.
	Manual mode is for passing text slides by code manually, 
	so it can not be tested on this example as is!

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
