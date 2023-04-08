
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
	bool bAuto = 1;
	int td = 2;
	// function to random populate slides, loading different text files.
	void doText() {
		string s;
		static bool b = false;
		if (b) {
			b = !b;
			if (ofRandom(1) < 0.5)
				s = subs.loadFileText("files/text1.txt");
			else
				s = subs.loadFileText("files/text2.txt");
		}
		else {
			b = !b;
			if (ofRandom(1) < 0.5)
				s = subs.loadFileText("files/text3.txt");
			else
				s = subs.loadFileText("files/text4.txt");
		}
		subs.doSetTextSlide(s);
	};
};
