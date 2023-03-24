#pragma once

/*

	WIP Example

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
	void keyPressed(int key);

	ofxSurfingTextSubtitle subs;
	string path;

	ofParameter<bool> bGui;
	ofxSurfingGui ui;

#ifdef USE_WHISPER
	surfingWhisper whisper;
	void doUpdatedWhisper();
	void drawImGuiWidgetsWhisper();
#endif

	void doPopulateText(string s = "");

	ofxWindowApp w;

	ofxChatGPT chatGPT;
	void setupGPT();
	void setInputGPT(string s);
	std::string textLastResponse;

	ofParameterGroup params{ "ofApp" };
	ofParameter<string> keyAPI{ "API KEY","" };

	SurfingTextEditor editorInput;
	SurfingTextEditor editorReply;

	//string textReply = "";

	void myCallback(string response) {
		cout << "GPT: " << response << endl;
	}
};
