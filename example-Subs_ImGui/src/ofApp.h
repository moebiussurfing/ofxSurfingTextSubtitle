
/*

	NOTE:
	You must enable ImGui mode un commenting the below line:
	#define USE_IM_GUI__SUBTTITTLES

*/

//-

#pragma once

#include "ofMain.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"

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

	string load(string path);
};
