#pragma once

//	srtparser.h
// taken from https://github.com/Jonathhhan/ofEmscriptenExamples
// thanks @Jonathhhan!

/*

	TODO:

	fix right align
	force box height to fit paragraph
	add next sub by tick
	set custom font

*/


#include "ofMain.h"

#include "srtparser.h"
#include "ofxFontStash.h"
#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfing_ofxGui.h"

#define MAX_FONT_SIZE 200

class ofxSurfingTextSubtitle {

public:

	ofxSurfingTextSubtitle::ofxSurfingTextSubtitle() {
		bGui.set("SUBTITLES", true);
	};

	ofxSurfingTextSubtitle::~ofxSurfingTextSubtitle() {
		exit();
	};
	
	void setup(string pathSrt);
	void update();

	void draw();
	void draw(ofRectangle view);
	//TODO:
	//letters only. without boxes nor gui
	void drawRaw();
	//void drawRaw(ofRectangle view);

	void drawGui();
	void setDisableGuiInternal(bool b){ bGui_Internal = !b; }

private:

	void setupParams();
	void setupSubs();
	void startup();
	void exit();

	bool bGui_Internal = true;

	SubtitleParserFactory* subParserFactory;
	SubtitleParser* parser;
	std::vector<SubtitleItem*> sub;

	ofxSurfingBoxInteractive box;
	ofxSurfingBoxHelpText boxInfo;

public:

	ofParameterGroup params;
	ofParameter<bool> bGui;
	ofParameter<bool> bDraw;
	ofParameter<bool> bDebug;
	ofParameter<bool> bAuto;
	ofParameter<float> speedAuto;

	ofParameterGroup params_Style;
	ofParameter<std::string> fName;
	ofParameter<std::string> fPath; // hardcoded file fonts paths
	ofParameter<float> fSizePrc; // relative to column width font size
	ofParameter<float> fSize; // real font raw size in px
	ofParameter<float> fSpacing;
	ofParameter<float> fLineHeight;
	ofParameter<ofColor> fColor;
	ofParameter<int> fAlign;
	ofParameter<std::string> fAlign_str;
	ofParameter<bool> bResetFont;

	ofParameter<bool> bPlay;

private:

	ofColor colorDebug = ofColor::black;

	string textCurrent;
	int currentSub;
	float boxhMax;

	ofxPanel gui;

	uint64_t tPlay;

	void refreshFontStyles();
	void Changed(ofAbstractParameter& e);

	ofxFontStash font;

	ofRectangle drawTextBox(std::string _t, ofRectangle r);
	float getOneLineHeight(bool oneOnly = true); // get real letter height to correct anchor offset...
	void drawInsertionPoint(float _x, float _y, float _w);
	//--------------------------------------------------------------
	std::string getAlignNameFromIndex(int index) const {
		std::string n = "UNKNOWN";
		switch (index)
		{
		case 0:
			n = "IGNORE";
			break;
		case 1:
			n = "LEFT";
			break;
		case 2:
			n = "RIGHT";
			break;
		case 3:
			n = "CENTER";
			break;
		}
		return n;
	}

	vector<string> subsText;
	string pathSrt;

};
