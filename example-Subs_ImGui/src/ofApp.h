
/*

	NOTE

	You must check that ImGui mode is enabled.
	Must be not commented the below line:
	#define USE_IM_GUI__SUBTTITTLES

	Currently checking manual mode.
	You should pick the correct mode
	if you want to test SRT files!

*/

//--

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
	void drawGui();
	void keyPressed(int key);

	ofImage image;

	ofxSurfingTextSubtitle subs;
	string path;

	ofxSurfingGui ui;

	//--

	// Debug Tester

	// Function to random populate slides, loading different text files.
	bool bAuto = 0;//enable
	int d = 2;//frequency in seconds
	//--------------------------------------------------------------
	void doLoadText() {
		static bool b = false;
		if (b) {
			b = !b;
			if (ofRandom(1) < 0.5) subs.doSetTextSlideFile("files/text1.txt");
			else subs.doSetTextSlideFile("files/text2.txt");
		}
		else {
			b = !b;
			if (ofRandom(1) < 0.5) subs.doSetTextSlideFile("files/text3.txt");
			else subs.doSetTextSlideFile("files/text4.txt");
		}
	};

	// Function to process a full file and split into slides.
	//--------------------------------------------------------------
	void doLoadTextBlocks() {
		string path = "files/text2.txt";
		subs.setupText(path);
	};
};
