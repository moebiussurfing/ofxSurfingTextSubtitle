#pragma once

/*

	WIP Example

	TODO

	log gpt setup and error/state

	add dual window 1: gui / 2: out
	+ docking
	fix < > slides
	fix 1st/end slide.
	add wait spin
		add beep sounds
	fix subtitle paragraph clamp. reset box centered
	add conversation/history mode
	add two colors user/assistant by tag in Gpt last reply
	search text formatter cpp lib
		remove 1._ etc using regex
	add prompts manager; save on json file.
		create prompt struct
		add new. edit.

*/


//--

 
// Optional Modules
//#define USE_WHISPER
//#define USE_EDITOR_INPUT
#define USE_EDITOR_RESPONSE

//--

#include "ofMain.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"
#include "surfingTextEditor.h"
#include "BigTextInput.h"
#include "ChatThread.h"

#include "surfingSceneTesters.h"
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

	//void drawScene();
	void drawImGui();

	ofxSurfingGui ui;

	ofxSurfingTextSubtitle subs;
	string path;
	void doPopulateText(string s = "");
	void doPopulateTextBlocks();
	void doClearList();

	ChatThread chatGpt;

	void setupGpt();
	void doSendMessageToGpt(string s, bool bWithHistory);
	void doSendMessageToGpt(string message);
	void doRegenerate();

	ofParameter<bool> bGui;

	ofParameterGroup params{ "ofApp" };
	ofParameter<string> apiKey{ "API key","" };
	ofParameter<bool> bConversation{ "Conversation", false };//not used
	ofParameter<bool> bGui_History{ "History",false };
	ofParameter<int> fontI{ "FontI", 0, 0, 3 };
	ofParameter<int> fontR{ "FontR", 0, 0, 3 };

	BigTextInput bigTextInput;
	void doAttendCallbackTextInput();
	ofEventListener eTextInput;
	ofParameter<string> textInput{ "TextInput", "" };

#ifdef USE_EDITOR_INPUT
	SurfingTextEditor editorInput;
#endif

#ifdef USE_EDITOR_RESPONSE
	SurfingTextEditor editorResponse;
#endif

	void drawWidgets(); // Advanced: inserted widgets

	bool bError = false;

	string textLastResponse;

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

	//--

	static string GPT_Prompt_0() {
		return R"(I want you to act as a music band advertiser. 
You will create a campaign to promote that band.
That campaign consists of 10 short sentences.
These sentences must define the band's career highlights, 
the best albums or the more important musicians members.
The sentences will be short: less than 5 words each sentence.
)";
	}

	static string GPT_Prompt_1() {
		return R"(I want you to act as a music band critic. 
I will pass you a band music name. You will return a list of 10 words.
You will only reply with that words list, and nothing else. 
Words will be sorted starting from less to more relevance.
)";
	}

	static string GPT_Prompt_2() {
		return R"(I want you to act as a music critic.
As a LastFm maintainer.
I will give you a band name. You will list the 10 more similar bands.
You will only reply that band names list, and nothing else. 
But you must sort that bands, from older to newer. 
)";
	}

	static string GPT_Prompt_3() {
		return R"(Act as your default ChatGPT behavior following the conversation.
)";
	}

	float v = 0;

	//--

#ifdef USE_WHISPER
	surfingWhisper whisper;
	void doUpdatedWhisper();
	void drawImGuiWidgetsWhisper();
#endif

	ofxWindowApp w;
};
