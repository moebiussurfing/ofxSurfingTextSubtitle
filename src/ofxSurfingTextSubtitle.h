#pragma once

//	srtparser.h
// taken from https://github.com/Jonathhhan/ofEmscriptenExamples
// thanks @Jonathhhan!

/*

	TODO:

	add imgui big text with scroll to look full subs.
	fix center mode
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

//----

// OPTIONAL

#define USE_IM_GUI__SUBTTITTLES
// Requires ofxSurfingImGui and an ofxImGui fork

#define USE_WIDGET__SUBTTITTLES
// A floating widget to display some info
 
//----

#include "ofMain.h"

#ifdef USE_IM_GUI__SUBTTITTLES
#include "ofxSurfingImGui.h"
#endif

#include "srtparser.h"
#include "ofxFontStash.h"
#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfing_ofxGui.h"
#include "ofxTimecode.h"//easily to remove. used to convert time formatting only. ex: ms to hh::mm::ss

// disable widget when using ImGui bc could be redundant.
#ifdef USE_IM_GUI__SUBTTITTLES
#undef USE_WIDGET__SUBTTITTLES
#endif
#ifdef USE_WIDGET__SUBTTITTLES
#include "ofxSurfingBoxHelpText.h"
#endif

#define MAX_FONT_SIZE 400

//----

class ofxSurfingTextSubtitle {

#ifdef USE_IM_GUI__SUBTTITTLES
private:
	ofxSurfingGui* ui;
public:
	void setUiPtr(ofxSurfingGui* _ui) { ui = _ui; }
	void drawImGui();
	void drawImGuiWidgets();
#endif

public:
	
	ofxSurfingTextSubtitle::ofxSurfingTextSubtitle() {
		bGui.set("SUBTITLES", true);
		bGui_ViewFull.set("SRT", false);
		bGui_Internal.set("Gui Internal", true);
	};

	ofxSurfingTextSubtitle::~ofxSurfingTextSubtitle() {
		exit();
	};

	void setup(string _pathSrt);//pass the .srt file path to load
	void update();

	void draw();
	void draw(ofRectangle view);
	void drawRaw();//letters only. without boxes, interaction nor gui
	//void drawRaw(ofRectangle view);

	void drawGui();
	
	void setDisableGuiInternal(bool b) { bGui_Internal = !b; }//disables ofxGui. useful when using ImGui or to disable gui.

	void setTogglePlay() { bPlay = !bPlay; }
	void setToggleAuto() { bAuto = !bAuto; }
	void setToggleDebug() { bDebug = !bDebug; }
	void setToggleEdit() { box.bEdit = !box.bEdit; }
	void setDebug(bool b) { bDebug = b; }
	void setEdit(bool b) { box.bEdit = b; }

	void setSubtitleIndex(int i) { currentLine = i; }
	void setSubtitlePrevious() { currentLine--; }
	void setSubtitleNext() { currentLine++; }
	int getNumSubtitles() const { return (currentLine.getMax() + 1); }
	ofColor getColorBg() const { return fColorBg.get(); }

	ofParameter<bool> bGui_Internal;

private:

	void setupParams();
	void setupSubs();
	void startup();
	void exit();

	//bool bGui_Internal = true;

	SubtitleParserFactory* subParserFactory;
	SubtitleParser* parser;
	std::vector<SubtitleItem*> sub;

	ofxSurfingBoxInteractive box;

#ifdef USE_WIDGET__SUBTTITTLES
	ofxSurfingBoxHelpText boxInfo;
#endif

	ofxTimecode timecode;
	float fps = 30;

public:

	void setFps(float _fps) { fps = _fps; }

	ofParameterGroup params;
	ofParameterGroup params_Transport;
	ofParameterGroup params_Control;
	ofParameter<bool> bGui;
	ofParameter<bool> bGui_ViewFull;
	ofParameter<bool> bDraw;
	ofParameter<bool> bDebug;

	ofParameter<bool> bPlay;
	//ofParameter<bool> bExternal;
	ofParameter<bool> bNext;
	ofParameter<bool> bPrev;
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
	ofParameter<ofColor> fColorBg;
	ofParameter<int> fAlign;
	ofParameter<std::string> fAlign_str;
	ofParameter<bool> bCentered;//move up block to center not depending of amount of lines.
	ofParameter<bool> bResetFont;

private:
	glm::vec2 offset = glm::vec2(0, 0);
	ofColor colorDebug = ofColor::black;

	string textCurrent = "";

	ofParameter<int> currentLine;
	//int currentLine;

	float boxhMax;

	ofxPanel gui;

	uint64_t tPlay;

	void refreshFontStyles();
	void Changed(ofAbstractParameter& e);

	ofxFontStash font;

	ofRectangle drawTextBox(std::string _t, ofRectangle r, bool bRaw = false);
	ofRectangle getTextBox(std::string _t, ofRectangle r);

	float getOneLineHeight(bool oneOnly = true); // get real letter height to correct anchor offset...
	void drawInsertionPoint(float _x, float _y, float _w = 0);
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
	void doResetFont() {
		bResetFont = false;
		fSizePrc = 0.05;
		fSpacing = 0;
		fLineHeight = 0.75;
		//fColor = ofColor(255, 255);
	}
};
