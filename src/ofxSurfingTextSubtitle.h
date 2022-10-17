#pragma once

//	srtparser.h
// taken from https://github.com/Jonathhhan/ofEmscriptenExamples
// thanks @Jonathhhan!

/*

	TODO:

	fix center mode
	fix right align
	add API set custom fonts on runtime
	add load another srt file on runtime
	add auto set layout. ex:
		re center box to a center point
		when different box sizes happens,
		when different amount of lines.

*/

//----

// OPTIONAL

//#define USE_IM_GUI__SUBTTITTLES
// Requires ofxSurfingImGui and an ofxImGui fork
// Can be commented to use ofxGui only!

#define USE_WIDGET__SUBTTITTLES
// A floating widget to display some info

#define USE_TIME_CODE__SUBTTITTLES
// Only to help in some time convertions.

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

#ifdef USE_TIME_CODE__SUBTTITTLES
#include "ofxTimecode.h"//easily to remove. used to convert time formatting only. ex: ms to hh::mm::ss
#endif

// disable widget when using ImGui bc could be redundant.
#ifdef USE_IM_GUI__SUBTTITTLES
#undef USE_WIDGET__SUBTTITTLES
#endif
#ifdef USE_WIDGET__SUBTTITTLES
#include "ofxSurfingBoxHelpText.h"
#endif

#define MAX_FONT_SIZE 400

//----

class ofxSurfingTextSubtitle
{

#ifdef USE_IM_GUI__SUBTTITTLES
private:
	ofxSurfingGui* ui;
public:
	void setUiPtr(ofxSurfingGui* _ui) { ui = _ui; }
	void drawImGui();
	void drawImGuiWidgets();
	void drawImGuiSrtFull();
#endif

	//--

public:

	ofxSurfingTextSubtitle::ofxSurfingTextSubtitle() {
		bGui.set("SUBTITLES", true);
		bGui_SrtFull.set("SRT", false);
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
	void setToggleAuto() { bPlayForce = !bPlayForce; }
	void setToggleEdit() { bEdit = !bEdit; }
	void setEdit(bool b) { bEdit = b; }
	//void setToggleDebug() { bEdit = !bEdit; }
	//void setToggleEdit() { box.bEdit = !box.bEdit; }
	//void setDebug(bool b) { bEdit = b; }
	//void setEdit(bool b) { box.bEdit = b; }

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

#ifdef USING_OFX_TIME_CODE
	ofxTimecode timecode;
#endif

	float fps = 30;

public:

	void setFps(float _fps) { fps = _fps; }

	ofParameterGroup params;
	ofParameterGroup params_Transport;
	ofParameterGroup params_Control;
	ofParameter<bool> bGui;
	ofParameter<bool> bGui_SrtFull;
	ofParameter<bool> bDraw;
	ofParameter<bool> bEdit;

	//ofParameter<bool> bExternal;
	ofParameter<bool> bPlay;
	ofParameter<bool> bNext;
	ofParameter<bool> bPrev;
	ofParameter<bool> bPlayForce;
	ofParameter<float> speedForce;

	ofParameterGroup params_Preset;

	ofParameterGroup params_Fade;
	ofParameter<bool> bAnimated;
	ofParameter<float> speedFade;
	//ofParameter<bool> bAnimatedOut;
	//ofParameter<float> speedFadeOut;
	ofParameter<bool> AutoScroll;

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
	ofParameter<bool> bCentered; // move up block to center not depending of amount of lines.
	ofParameter<bool> bResetFont;

private:

	float dtAnim;
	float alpha;
	bool isAnim;
	
	ofParameter<float> progressForce;
	uint64_t tPlaySlideStart = 0;
	uint64_t tPlaySlideDuration = 0;
	ofParameter<float>progressPrc;

	glm::vec2 offset = glm::vec2(0, 0);
	ofColor colorDebug = ofColor::black;

	string textCurrent = "";

	ofParameter<int> currentLine;//current loaded subtitle slide 

	float boxhMax;

	ofxPanel gui;

	uint64_t tPlay;

	//void refreshFontStyles();
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
