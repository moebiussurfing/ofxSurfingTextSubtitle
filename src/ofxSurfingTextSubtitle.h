#pragma once

// srtparser.h
// taken from https://github.com/Jonathhhan/ofEmscriptenExamples
// thanks @Jonathhhan!

/*

	TODO:

	responsive engine
		improve auto size engine
		add engine to adapt font size to amount lines/box size.
		add algorithm/mode to fit exactly the box, 
		changing font size depending of amount lines.

	add fonts list and set custom fonts on runtime
	
	bPlay workflow is a bit messy, 
		bc mode index selector forces play.

	store srt file path to settings to be persistent too,

	test video player encoding problems...
	bad framerate on Debug compilation.

	add presets inside the addon.

*/

//----

// OPTIONAL

// 1. GUI
#define USE_IM_GUI__SUBTITLES 
// -> Pick your GUI! ImGui / ofxGui
// Requires ofxSurfingImGui and an ofxImGui fork
// Can be commented to use ofxGui only!

// 2. VIDEO PLAYER
//#define USE_WIDGET__VIDEO_PLAYER 
// -> Could be disable to use your own video player or to use the subtitler as standalone.
// A simple video player is bundled for commodity, 
// then can be removed just by commenting here.
// (Probably you would want to use your own external video player) 
// If not using the video player we can remove:
// the libs/ofxPlaybackGui folder and the ofxSurfingVideoPlayer.h and ofxSurfingVideoPlayer.cpp files.
// Then, you would use the class outside just by copying the
// srtparser.h, ofxSurfingTextSubtitle.h and ofxSurfingTextSubtitle.cpp files.

//----

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
	//--

#ifdef USE_IM_GUI__SUBTITLES
private:
	ofxSurfingGui* ui;

public:
	void setUiPtr(ofxSurfingGui* _ui) { ui = _ui; }
	void drawImGui();

private:
	void drawImGuiWidgets();
	void drawImGuiWindowParagraph();
	void drawImGuiList();
#endif

	//--

public:

	ofxSurfingTextSubtitle();
	~ofxSurfingTextSubtitle();

	void setup(string _pathSrt);
	//pass the .srt file path to load

	void setPosition(float position);
	void updatePosition(float position);
	void update();

private:
	void updateFrame(uint64_t frame);//TODO:

	void updateFades();
	void updateEngine();
	void updateDebug();

public:

	void draw();
	void draw(ofRectangle view);
	void drawRaw();

private:

	void drawDebug();
	//letters only. without boxes, interaction nor gui
	//void drawRaw(ofRectangle view);

public:

	void drawGui();

	void setDisableGuiInternal(bool b) { bGui_Internal = !b; }
	// disables ofxGui. useful when using ImGui or to disable gui.

	void keyPressed(int key);
	ofParameter<bool> bKeys{ "Keys", true };

	//--

	void setToggleEdit() { bEdit = !bEdit; }
	void setEdit(bool b) { bEdit = b; }
	void setToggleVisibleGui() { bGui = !bGui; }
	void setToggleDebug() { bDebug = !bDebug; }
	void setDebug(bool b) { bDebug = b; }

	void setSubtitleIndex(int i) { currentDialog = i; }
	void setSubtitlePrevious() { bPrev = true; }
	void setSubtitleNext() { bNext = true; }
	void setSubtitleRandomIndex() { (int)ofRandom(getNumSubtitles()); }

	int getNumSubtitles() const { return (currentDialog.getMax() + 1); }
	ofColor getColorBg() const { return fColorBg.get(); };

	// Call before setup. Set duration in ms to be used with play external mode
	//void setDuration(uint64_t duration) { tEndSubsFilm = duration; }
	void setDuration(float duration) { tEndSubsFilm = 1000 * duration; }

	void load(string _pathSrt) {
		setupSubs(_pathSrt); 

		//TODO:
		//indexModes = indexModes.get();
		//indexModes = 0;
	};

private:

	ofParameter<bool> bMinimize;
	ofParameter<bool> bGui_Internal;

	uint64_t tEndSubsFilm = 0;
	bool bForceAddBreakLines = true;
	bool isSlidePlaying = false;

	void setupFont();

	void setupParams();
	void setupSubs(string _pathSrt);
	void buildSubsData(); // not really used by the engine. just to preview the SRT window to display all messages.

	void doOpenFile();
	void processOpenFileSelection(ofFileDialogResult openFileResult);

	void startup();
	void exit();

	string path_SubtitlerSettings = "ofxSurfingTextSubtitle.json";

	SubtitleParserFactory* subParserFactory;
	SubtitleParser* parser;
	std::vector<SubtitleItem*> sub;

	ofxSurfingBoxInteractive box;//main container

#ifdef USE_WIDGET__SUBTITLES
	ofxSurfingBoxHelpText boxInfo;
#endif

#ifdef USING_OFX_TIME_CODE
	ofxTimecode timecode;
#endif

	float fps = 60;

public:

	void setFps(float _fps) { fps = _fps; }

	ofParameter<bool> bGui;
	ofParameter<bool> bDraw;
	ofParameter<int> durationPlayForced;
	ofParameterGroup params_Preset; // re collect params for preset/settings

private:

	ofParameterGroup params; // for the gui and callback
	ofParameterGroup params_Transport;
	ofParameterGroup params_Control;
	ofParameterGroup params_Style;
	ofParameterGroup params_Fade;
	ofParameterGroup params_FadeIn;
	ofParameterGroup params_FadeOut;

	ofParameter<void> bOpen;
	ofParameter<bool> bGui_List;
	ofParameter<bool> bGui_Paragraph;
	ofParameter<bool> bEdit;
	ofParameter<bool> bDebug;
	ofParameter<bool> bLive; // hide all
	ofParameter<bool> bTop;
	ofParameter<bool> bLeft;
#ifdef USE_WIDGET__SUBTITLES
	ofParameter<bool> bDrawWidgetInfo;
#endif
	ofParameter<bool> bNext;
	ofParameter<bool> bPrev;

	ofParameter<void> bStop;
	ofParameter<bool> bPlay;
	ofParameter<bool> bPlayForced;

	ofParameter<bool> bResponsive;
	ofParameter<float> resizeResponsive;

	ofParameter<bool> bPlayExternal;
	ofParameter<float> tPosition;

	ofParameter<bool> bAnimatedIn;
	ofParameter<float> speedFadeIn;
	ofParameter<bool> bAnimatedOut;
	ofParameter<float> speedFadeOut;
	ofParameter<int> countDownOut; // time before the end to start fadeout from. in ms 
	ofParameter<bool> bResetFades;

	ofParameter<bool> bAutoScroll;
	ofParameter<bool> bCenteredV; // move up block to center not depending of amount of lines.
	ofParameter<int> amountLinesTarget;

	ofParameter<std::string> fName; // name to display only
	ofParameter<std::string> fPath; // hardcoded file fonts paths
	ofParameter<float> fSizePrc; // relative to column width font size
	ofParameter<float> fSize; // real font raw size in px
	ofParameter<float> fSpacing;
	ofParameter<float> fLineHeight;
	ofParameter<ofFloatColor> fColorTxt;
	ofParameter<ofFloatColor> fColorBg;
	ofParameter<int> fAlign;
	ofParameter<std::string> fAlign_str;
	ofParameter<bool> bCapitalize;
	ofParameter<bool> bResetFont;

private:

	ofParameter<bool> bFine{ "Fine", false };

	vector<string> names_Align{ "IGNORE","LEFT","RIGHT","CENTER" };
	
	vector<string> names_Modes{ "EXTERNAL", "STANDALONE", "FORCED" };
	ofParameter<int> indexModes;

	int amountLinesDrawn = 0; // amount lines of the last current drawn 

	float alpha = 1.f;
	float dtAnim = 1.f;
	bool isAnimIn = false;
	bool isAnimOut = false;

	ofParameter<float> progressPlayFilm;//measured in subs dialogs not in time
	ofParameter<float> progressPlaySlide;

	ofParameter<float> progressIn;
	ofParameter<float> progressOut;

	uint64_t tPlayStartSlide = 0;
	uint64_t tPlayForce = 0;
	uint64_t tPlayForceFilm = 0;
	uint64_t durationPlaySlide = 0;

	ofColor colorDebugDark = ofColor::black;
	ofColor colorDebugLight = ofColor::white;
	ofParameter<bool> bTheme{ "Theme", true };

	string textCurrent = "";

	ofParameter<int> currentDialog; // dialog index. current loaded subtitle slide.  

	ofxPanel gui;

	uint64_t tPlay = 0;
	
	bool isPrecoutingStart = false;

	void Changed(ofAbstractParameter& e);

	ofxFontStash font;

	ofRectangle drawTextBox(std::string _t, ofRectangle r, bool bRaw = false);
	ofRectangle getTextBox(std::string _t, ofRectangle r);

	// oneOnly true is faster but false is probably more precise.
	float getOneLineHeight(bool oneOnly = true); // get real letter height to correct anchor offset...
	float getSpacingBetweenLines();

	void drawInsertionPoint(float _x, float _y, float _w = 0, float _h = 0);

	vector<string> subsDataText;
	string path_Srt;//.srt filename
	string name_Srt;//.srt name

	void doResetFont();
	std::string getAlignNameFromIndex(int index) const;

	void doResetFades();
	void doUpdateSlidePlay(SubtitleItem* element);

	//--

	// Transport

public:

	void setTogglePlayForced();
	bool isPlaying() const;
	void setTogglePlay();
	void play();
	void stop();
	void pause();

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