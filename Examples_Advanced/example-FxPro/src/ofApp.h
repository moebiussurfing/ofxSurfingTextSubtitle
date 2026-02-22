
#pragma once
#include "ofMain.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingFxPro.h"
#include "ofxSurfingImGui.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void drawGui();
	void keyPressed(int key);
	void keyReleased(int key);
	void windowResized(int w, int h);
	void exit();

	ofxSurfingTextSubtitle subs;
	ofxSurfingFxPro fxPro;

	ofxSurfingGui ui;
	ofParameter<bool> bGui { "GUI", true };
};
