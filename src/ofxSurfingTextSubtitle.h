#pragma once

//	srtparser.h
// taken from https://github.com/Jonathhhan/ofEmscriptenExamples
// thanks @Jonathhhan!

/*

	TODO:

	block modifyng box when drawing raw.
		add edit/lock toggle.
	add auto set layout. ex: 
		re center box to a center point 
		when different box sizes happens,
		when different amount of lines.
	add load another srt file on runtime
	fix right align
	add API next sub by tick
	add API set sub by index
	add API set custom fonts on runtime

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

	void setup(string pathSrt);//pass the .srt file path to load
	void update();

	void draw();
	void draw(ofRectangle view);
	void drawRaw();//letters only. without boxes, interaction nor gui
	//void drawRaw(ofRectangle view);

	void drawGui();
	void setDisableGuiInternal(bool b) { bGui_Internal = !b; }//disables ofxGui. useful when using ImGui or to disable gui.

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

	ofRectangle drawTextBox(std::string _t, ofRectangle r, bool bRaw = false);
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
