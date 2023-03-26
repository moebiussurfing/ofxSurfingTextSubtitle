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
	void drawImGui();
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

	ofParameter<int> fontI{ "FontI", 0, 0, 3 };
	ofParameter<int> fontR{ "FontR", 0, 0, 3 };

	//string textReply = "";

	void myCallback(string response) {
		cout << "GPT: " << response << endl;
	}

	void doRandomInput() {

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
		setInputGPT(editorInput.getText());
		editorReply.setText(textLastResponse);
	};
};
