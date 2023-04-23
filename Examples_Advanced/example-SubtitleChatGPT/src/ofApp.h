#pragma once

/*

	WIP Example

	TODO

	threading workflow
	fix subtitle paragraph clamp
		add ctrl/alt text/paragraph size
		reset box centered
	add two colors user/assistant by tag in Gpt last reply
	add beep sound
	search text formatter cpp lib
	check console imgui widgets

*/

//#define USE_WHISPER


#include "ofMain.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"
#include "surfingSceneTesters.h"
#include "ChatThread.h"
#include "surfingTextEditor.h"
#include "ofxWindowApp.h"

#ifdef USE_WHISPER
#include "surfingWhisper.h"
#endif

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void startup();

	void drawScene();
	void drawImGui();

	ofParameter<bool> bGui;
	ofxSurfingGui ui;

	ofxWindowApp w;

	ofxSurfingTextSubtitle subs;
	string path;
	void doPopulateText(string s = "");

	ChatThread chatGpt;

	void setupGpt();
	void setInputGpt(string s, bool bWithHistory = false);
	void sendMessage(string message);
	void regenerate();

	ofParameterGroup params{ "ofApp" };
	ofParameter<string> keyApi{ "API KEY","" };
	ofParameter<bool> bConversation{ "Conversation", false };
	ofParameter<bool> bGui_History{ "History",false };
	ofParameter<int> fontI{ "FontI", 0, 0, 3 };
	ofParameter<int> fontR{ "FontR", 0, 0, 3 };

	SurfingTextEditor editorResponse;

	SurfingTextEditor editorInput;
	void drawWidgets(); // Advanced: inserted widgets

	bool bError = false;

	std::string textLastResponse;

	ofJson jQuestion;
	ofJson jResponse;

	string strBandname;

	//--

	void doRandomInput();

	void doSwapPrompt();

	string strPrompt;
	int iPrompt = 0;
	string namePrompt;
	vector<pair<string, string> > prompts;
	void setPrompt(int index);

	static string GPT_Prompt_0() {
		return R"(
I want you to act as a music band advertiser. 
You will create a campaign to promote that band.
That campaign consists of 5 short sentences.
These sentences must define the band's career highlights, 
the best albums or the more important musicians members.
The sentences will be short: less than 5 words.
)";
	}

	static string GPT_Prompt_1() {
		return R"(
I want you to act as a music band critic. 
I will pass you a band music name. You will return a list of 10 words.
You will only reply with that words list, and nothing else. 
Words will be sorted starting from less to more relevance.
)";
	}

	static string GPT_Prompt_2() {
		return R"(
I want you to act as a music critic.
As a LastFm maintainer.
I will give you a band name. You will list the 5 more similar bands.
You will only reply that band names list, and nothing else. 
But you must sort that bands, from older to newer. 
)";
	}

#ifdef USE_WHISPER
	surfingWhisper whisper;
	void doUpdatedWhisper();
	void drawImGuiWidgetsWhisper();
#endif
};
