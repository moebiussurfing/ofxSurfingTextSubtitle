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


#include "ofMain.h"

#include "ofxWindowApp.h"

#include "ofxSurfingTextSubtitle.h"
#include "ofxSurfingImGui.h"
#include "surfingSceneTesters.h"

#include "ofxChatGPT.h"
#include "surfingTextEditor.h"

//#define USE_WHISPER
#ifdef USE_WHISPER
#include "surfingWhisper.h"
#endif

class ofApp : public ofBaseApp
{
public:
	void setup();
	void startup();
	void exit();
	void update();
	void draw();
	void drawImGui();
	void keyPressed(int key);

	ofParameter<bool> bGui;
	ofxSurfingGui ui;

	ofxWindowApp w;

	ofxSurfingTextSubtitle subs;
	string path;
	void doPopulateText(string s = "");

#ifdef USE_WHISPER
	surfingWhisper whisper;
	void doUpdatedWhisper();
	void drawImGuiWidgetsWhisper();
#endif

	ofxChatGPT chatGPT;

	void setupGPT();
	void setInputGPT(string s, bool bWithHistory = false);

	std::string textLastResponse;

	ofParameterGroup params{ "ofApp" };
	ofParameter<string> keyAPI{ "API KEY","" };
	ofParameter<bool> bConversation{ "Conversation", false };
	ofParameter<int> fontI{ "FontI", 0, 0, 3 };
	ofParameter<int> fontR{ "FontR", 0, 0, 3 };

	SurfingTextEditor editorInput;
	void drawWidgets(); // Advanced: inserted widgets

	SurfingTextEditor editorResponse;

	//--

	void myCallback(string response) {
		cout << "GPT: " << response << endl;
	};

	void doRandomInput() {
		ui.AddToLog("doRandomInput");

		string s = "";
		float r = ofRandom(1);
		int sz = 3;
		float tr = 1.f / (float)sz;
		if (r < tr) {
			s += "Carrero blanco en espanol";
		}
		else if (r < 2 * tr) {
			s += "Primo de rivera en espanol";
		}
		else if (r < 3 * tr) {
			s += "Felipe gonzalez en espanol";
		}

		editorInput.setText(s);
		ui.AddToLog("editorInput.setText");
		ui.AddToLog(s, OF_LOG_NOTICE);
		
		setInputGPT(editorInput.getText(), bConversation);

		// Here textLastResponse is already catched 
		editorResponse.setText(textLastResponse);
		ui.AddToLog("editorResponse.setTex");
	};
};
