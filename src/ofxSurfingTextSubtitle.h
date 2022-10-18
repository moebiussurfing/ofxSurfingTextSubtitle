#pragma once

//	srtparser.h
// taken from https://github.com/Jonathhhan/ofEmscriptenExamples
// thanks @Jonathhhan!

/*

	TODO:

	test video player encoding problems...
	split out bDebug again from bEdit. split widgets enable too.
	fix right align
	add list and set custom fonts on runtime
	add load another .srt file on runtime
	fix v center mode
		add auto set layout. ex:
		re center box to a center point
		when different box sizes happens,
		when different amount of lines.

*/

//----

// OPTIONAL

//#define USE_IM_GUI__SUBTITLES // -> Pick your GUI! ofxGui/ImGui
// Requires ofxSurfingImGui and an ofxImGui fork
// Can be commented to use ofxGui only!

#define USE_WIDGET__SUBTITLES
// A floating widget to display some info

#define USE_TIME_CODE__SUBTITLES
// Only to help in some time convertions.

//----

#include "ofMain.h"

#ifdef USE_IM_GUI__SUBTITLES
#include "ofxSurfingImGui.h"
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

	bool isPlaying() const { return bPlay.get(); }
	void setTogglePlay() { bPlay = !bPlay; }
	void play() { bPlay = true; }
	void stop() { bPlay = false; }
	void setToggleAuto() { bPlayForced = !bPlayForced; }
	void setToggleEdit() { bEdit = !bEdit; }
	void setEdit(bool b) { bEdit = b; }
	void setToggleVisibleGui() { bGui = !bGui; }
	//void setToggleDebug() { bEdit = !bEdit; }
	//void setToggleEdit() { box.bEdit = !box.bEdit; }
	//void setDebug(bool b) { bEdit = b; }
	//void setEdit(bool b) { box.bEdit = b; }

	void setSubtitleIndex(int i) { currentDialog = i; }
	void setSubtitlePrevious() { currentDialog--; }
	void setSubtitleNext() { currentDialog++; }
	int getNumSubtitles() const { return (currentDialog.getMax() + 1); }
	ofColor getColorBg() const { return fColorBg.get(); }

	ofParameter<bool> bGui_Internal;

private:

	void setupParams();
	void setupSubs();
	void startup();
	void exit();

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

	ofParameterGroup params;//for the gui and callback
	ofParameterGroup params_Transport;
	ofParameterGroup params_Control;
	ofParameterGroup params_Style;
	ofParameterGroup params_Fade;
	ofParameterGroup params_FadeIn;
	ofParameterGroup params_FadeOut;
	ofParameterGroup params_Preset;//re collect params for preset/settings

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_SrtFull;
	ofParameter<bool> bDraw;
	ofParameter<bool> bEdit;

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
	ofParameter<int> countDownOut;//time before the end to start fadeout from. in ms 
	ofParameter<bool> bResetFades;

	ofParameter<bool> bAutoScroll;
	ofParameter<bool> bCentered; // move up block to center not depending of amount of lines.

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
	uint64_t durationPlaySlide = 0;

	glm::vec2 offset = glm::vec2(0, 0);
	ofColor colorDebug = ofColor::black;

	string textCurrent = "";

	ofParameter<int> currentDialog; // dialog index. current loaded subtitle slide.  

	float boxhMax = 0;

	ofxPanel gui;

	uint64_t tPlay = 0;

	void Changed(ofAbstractParameter& e);

	ofxFontStash font;

	ofRectangle drawTextBox(std::string _t, ofRectangle r, bool bRaw = false);
	ofRectangle getTextBox(std::string _t, ofRectangle r);

	float getOneLineHeight(bool oneOnly = true); // get real letter height to correct anchor offset...
	void drawInsertionPoint(float _x, float _y, float _w = 0, float _h = 0);

	vector<string> subsText;
	string pathSrt;//srt filename

	void doResetFont();
	std::string getAlignNameFromIndex(int index) const;

	void doResetFades();
	void doUpdateSlidePlay(SubtitleItem* element);

};
