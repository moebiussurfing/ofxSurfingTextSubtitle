
/*

	NOTE

	You must check that ImGui mode is enabled.
	Must be not commented the below line:
	#define USE_IM_GUI__SUBTTITTLES

	Currently checking manual mode: 
	by using doLoadText() and doLoadTextBlocks() functions.
	You should pick the correct manual mode if you see it's changed,
	or not, if you want to test SRT files with other modes!

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
			if (ofRandom(1) < 0.5) subs.doSetTextSlideStartFile("files/txt/text1.txt");
			else subs.doSetTextSlideStartFile("files/txt/text2.txt");
		}
		else {
			b = !b;
			if (ofRandom(1) < 0.5) subs.doSetTextSlideStartFile("files/txt/text3.txt");
			else subs.doSetTextSlideStartFile("files/txt/text4.txt");
		}
	};

	// Function to process a full file and split into blocks/slides.
	//--------------------------------------------------------------
	void doLoadTextBlocks() {
		string path = "files/txt/text2.txt";
		subs.setupTextBlocks(path);
	};
};
