#pragma once

// srtparser.h
// taken from https://github.com/Jonathhhan/ofEmscriptenExamples
// thanks @Jonathhhan!

/*

	TODO:

	fix first subtitle line hidden
	to implement master time to link both subtitler and video player.
		currently both timer count from play moment! 
		can't do scrubb or jump innto timeline.
	test video player encoding problems...
	fix right align:
		a new approach doing calculating
		the amount of lines and adding \n
		to the paragraph
	add list and set custom fonts on runtime

*/

//----

// OPTIONAL

//#define USE_IM_GUI__SUBTITLES // -> Pick your GUI! ImGui / ofxGui
// Requires ofxSurfingImGui and an ofxImGui fork
// Can be commented to use ofxGui only!

//#define USE_WIDGET__VIDEO_PLAYER // -> Could be disable to use your own video player or to use the subtitler as standalone.
// A simple video player is bundled for commodity, 
// then can be removed just by commenting here.
// (Probably you would want to use your own external video player) 

//--

#define USE_WIDGET__SUBTITLES
// A floating widget to display some info

#define USE_TIME_CODE__SUBTITLES
// Only to help in some time convertions. Can be removed.

//----

#include "ofMain.h"

#ifdef USE_IM_GUI__SUBTITLES
#include "ofxSurfingImGui.h"
#endif

#ifdef USE_WIDGET__VIDEO_PLAYER
#include "ofxSurfingVideoPlayer.h"
#endif

#include "srtparser.h"
#include "ofxFontStash.h"
#include "ofxGui.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfing_ofxGui.h"

#ifdef USE_TIME_CODE__SUBTITLES
#include "ofxTimecode.h"//easily to remove. used to convert time formatting only. ex: ms to hh::mm::ss
#endif

// disable widget when using ImGui bc could be redundant.
#ifdef USE_IM_GUI__SUBTITLES
#undef USE_WIDGET__SUBTITLES
#endif
#ifdef USE_WIDGET__SUBTITLES
#include "ofxSurfingBoxHelpText.h"
#endif

#define MAX_FONT_SIZE 400

//----

class ofxSurfingTextSubtitle
{

#ifdef USE_IM_GUI__SUBTITLES
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

	ofxSurfingTextSubtitle();
	~ofxSurfingTextSubtitle();

	void setup(string _pathSrt);
	//pass the .srt file path to load

	void update(uint64_t frame);
	void update();

	void draw();
	void draw(ofRectangle view);
	void drawRaw();
	//letters only. without boxes, interaction nor gui
	//void drawRaw(ofRectangle view);

	void drawGui();

	void setDisableGuiInternal(bool b) { bGui_Internal = !b; }
	//disables ofxGui. useful when using ImGui or to disable gui.

	void keyPressed(int key);

	//--

	void setToggleEdit() { bEdit = !bEdit; }
	void setEdit(bool b) { bEdit = b; }
	void setToggleVisibleGui() { bGui = !bGui; }
	void setToggleDebug() { bDebug = !bDebug; }
	void setDebug(bool b) { bDebug = b; }

	void setSubtitleIndex(int i) { currentDialog = i; }
	void setSubtitlePrevious() { currentDialog--; }
	void setSubtitleNext() { currentDialog++; }
	void setSubtitleRandomIndex() { (int)ofRandom(getNumSubtitles()); }
	int getNumSubtitles() const { return (currentDialog.getMax() + 1); }
	ofColor getColorBg() const { return fColorBg.get(); };

	ofParameter<bool> bGui_Internal;

private:

	void setupParams();
	void setupSubs(string _pathSrt);

	void doOpenFile();
	void processOpenFileSelection(ofFileDialogResult openFileResult);

	void startup();
	void exit();
	
	string path_SubtitlerSettings= "Subtitler.json";

	SubtitleParserFactory* subParserFactory;
	SubtitleParser* parser;
	std::vector<SubtitleItem*> sub;

	ofxSurfingBoxInteractive box;

#ifdef USE_WIDGET__SUBTITLES
	ofxSurfingBoxHelpText boxInfo;
#endif

#ifdef USING_OFX_TIME_CODE
	ofxTimecode timecode;
#endif

	float fps = 60;

public:

	void setFps(float _fps) { fps = _fps; }

	ofParameterGroup params; // for the gui and callback
	ofParameterGroup params_Transport;
	ofParameterGroup params_Control;
	ofParameterGroup params_Style;
	ofParameterGroup params_Fade;
	ofParameterGroup params_FadeIn;
	ofParameterGroup params_FadeOut;
	ofParameterGroup params_Preset; // re collect params for preset/settings

	ofParameter<bool> bGui;
	ofParameter<void> bOpen;
	ofParameter<bool> bGui_SrtFull;
	ofParameter<bool> bDraw;
	ofParameter<bool> bEdit;
	ofParameter<bool> bDebug;
	ofParameter<bool> bTop;
#ifdef USE_WIDGET__SUBTITLES
	ofParameter<bool> bInfo;
#endif
	ofParameter<bool> bNext;
	ofParameter<bool> bPrev;
	ofParameter<bool> bPlay;
	ofParameter<bool> bPlayForced;
	ofParameter<int> durationPlayForced;
	//ofParameter<float> speedPlayForce;
	//ofParameter<bool> bExternal;

	ofParameter<bool> bAnimatedIn;
	ofParameter<float> speedFadeIn;
	ofParameter<bool> bAnimatedOut;
	ofParameter<float> speedFadeOut;
	ofParameter<int> countDownOut; // time before the end to start fadeout from. in ms 
	ofParameter<bool> bResetFades;

	ofParameter<bool> bAutoScroll;
	ofParameter<bool> bCentered; // move up block to center not depending of amount of lines.
	ofParameter<int> amountLinesTargetCentered;

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
	ofParameter<bool> bResetFont;

private:

	int amountLinesDrawn = 0; // amount lines of the last current drawn 

	float alpha = 1.f;
	float dtAnim = 1.f;
	bool isAnimIn = false;
	bool isAnimOut = false;

	ofParameter<float> progressPlayFilm;
	ofParameter<float> progressPlaySlide;

	ofParameter<float> progressIn;
	ofParameter<float> progressOut;

	uint64_t tPlayStartSlide = 0;
	uint64_t tPlayForce = 0;
	uint64_t tPlayForceFilm = 0;
	uint64_t durationPlaySlide = 0;

	ofColor colorDebugDark = ofColor::black;
	ofColor colorDebugLight = ofColor::white;
	ofParameter<bool> bTheme{ "Theme", false };

	string textCurrent = "";

	ofParameter<int> currentDialog; // dialog index. current loaded subtitle slide.  

	ofxPanel gui;

	uint64_t tPlay = 0;

	void Changed(ofAbstractParameter& e);

	ofxFontStash font;

	ofRectangle drawTextBox(std::string _t, ofRectangle r, bool bRaw = false);
	ofRectangle getTextBox(std::string _t, ofRectangle r);

	// oneOnly true is faster but false is probably more precise.
	float getOneLineHeight(bool oneOnly = true); // get real letter height to correct anchor offset...
	float getSpacingBetweenLines();

	void drawInsertionPoint(float _x, float _y, float _w = 0, float _h = 0);

	vector<string> subsText;
	string path_Srt;//.srt filename

	void doResetFont();
	std::string getAlignNameFromIndex(int index) const;

	void doResetFades();
	void doUpdateSlidePlay(SubtitleItem* element);

	//--

	// Transport

public:

	void setTogglePlayForced() { bPlayForced = !bPlayForced; }

	//--------------------------------------------------------------
	bool isPlaying() const {
		return bPlay.get();
	}

	//--------------------------------------------------------------
	void setTogglePlay() {
		bPlay = !bPlay;
#ifdef USE_WIDGET__VIDEO_PLAYER
		if (bPlay) player.play();
		else player.stop();
#endif
	}

	//--------------------------------------------------------------
	void play() {
		bPlay = true;

//#ifdef USE_WIDGET__VIDEO_PLAYER
//		player.play();
//#endif
	}

	//--------------------------------------------------------------
	void stop() {
		if(bPlay) bPlay = false;
		if(bPlayForced) bPlayForced = false;

		textCurrent = "";
		currentDialog = 0;
		//alpha = 0;

//#ifdef USE_WIDGET__VIDEO_PLAYER
//		player.stop();
//#endif
	}

	/*
	void pause() {
		//player.pause();
	}
	*/

	//--

private:

#ifdef USE_WIDGET__VIDEO_PLAYER
	ofxSurfingVideoPlayer player;
	ofParameter<bool> bLoadBothVideoAndSubs{ "Link2Files" ,true };
	ofEventListeners listeners;
#endif

	// extra info
	bool bDebug2 = false;
};

/*
//TODO: add tweakeable speed
const int SPEED_SCALE = 5;
float speed = ofMap(speedPlayForce, -1, 1, 0.25f, (float)SPEED_SCALE);
uint64_t t = ofGetElapsedTimeMillis() * speed - tPlay;
s += timecode.timecodeForMillis(t);
//s += ofToString(t / 1000);
s += "\n";
*/