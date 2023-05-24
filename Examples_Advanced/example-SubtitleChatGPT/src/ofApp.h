#pragma once

/*

	WIP Example

	TODO

	ui docking
		use modes like: conversation, prompt selector etc
		for different layouts
	gpt setup/restart, reconnect.
		make new class SurfGPT.
		model and error/state

	conversation window
		auto scroll down when response (tween)
		lock mode
		auto go to text input when get response
		make bigger scroll

	add wait spin
		add beep sounds
	fix < > slides
		fix 1st/end slide.
	add dual window 1: gui / 2: out
	fix subtitle paragraph clamp. reset box centered
	add prompts manager:
		save on json file.
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
#include <regex>
#include "surfingSceneTesters.h"
#include "ofxWindowApp.h"

#ifdef USE_WHISPER
#include "surfingWhisper.h"
#endif

#include <curl/curl.h>

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
	void drawImGuiReply(ofxSurfingGui& ui);
	void drawImGuiConversation(ofxSurfingGui& ui);

	ofxSurfingGui ui;

	ofxSurfingTextSubtitle subs;
	string path;
	void doPopulateText(string s = "");
	void doPopulateTextBlocks();
	void doClearSubsList();

	ChatThread chatGpt;

	void setupGpt();
	void doGptSendMessage(string message);
	void doGptRegenerate();
	void doGptGetMessage();
	ofParameter<bool> bGptWaiting{ "GPT WAITING", 0 };
	// Error codes for various error conditions.
	vector<string> errorCodesNames{
			"Success",//0
			"InvalidAPIKey",
			"NetworkError",
			"ServerError",
			"RateLimitExceeded",
			"TokenLimitExceeded",
			"InvalidModel",
			"BadRequest",
			"Timeout",//8
			"UnknownError"
	};
	ofParameter<int> indexErrorCode{ "State", 9, 0, errorCodesNames.size() - 1 };
	bool bGptError = false;
	int getErrorCodeByCode(ofxChatGPT::ErrorCode e);
	string gptErrorMessage = "";

	ofParameter<bool> bGui;

	ofParameterGroup params{ "ofApp" };

	ofParameter<string> apiKey{ "API key","" };
	ofParameter<string> model{ "Model","" };

	ofParameter<bool> bModeOneSlide{ "OneSlide", false };
	ofParameter<bool> bModeConversation{ "Conversation", false };

	// Create an empty JSON array to store messages
	//vector<ofJson> message_history;
	ofJson message_history = ofJson();
	//ofJson message_history = nlohmann::json::array();

	//ofParameter<bool> bModeHistory{ "History",false };

	ofParameter<bool> bGui_GptLastReply{ "GPT Last Reply",false };
	ofParameter<bool> bGui_GptConversation{ "GPT Conversation",false };

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

	// "10 short sentences."
	static string GPT_Prompt_0() {
		return R"(I want you to act as a music band advertiser. 
You will create a campaign to promote that band.
That campaign consists of 10 short sentences.
These sentences must define the band's career highlights, 
the best albums or the more important musicians members.
The sentences will be short: less than 5 words each sentence.
)";
	}

	// "10 words list."
	static string GPT_Prompt_1() {
		return R"(I want you to act as a music band critic. 
I will pass you a band music name. You will return a list of 10 words.
You will only reply with that words list, and nothing else. 
Words will be sorted starting from less to more relevance.
The format of the response, will be with one line per each word.
These lines will be starting with the first char uppercased, 
and without a '.' at the end of the line, just include the break line char.
)";
	}

	// "10 Similar bands"
	static string GPT_Prompt_2() {
		return R"(I want you to act as a music critic.
As a LastFm maintainer.
I will give you a band name. You will list the 10 more similar bands.
You will only reply that band names list, and nothing else. 
But you must sort that bands, from older to newer. 
)";
	}

	// "Default"
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

	/*
	// Send a message to ChatGPT and get a response without conversation history.
	tuple<string, ofxChatGPT::ErrorCode> ofxChatGPT::chat(const string& message) {
		ofJson requestBody;
		requestBody["model"] = modelName;
		requestBody["messages"].push_back({ {"role", "user"}, {"content", message} });
		requestBody["temperature"] = 0.5;
		ofLogVerbose("ofxChatGPT") << "SendData: " << requestBody.dump(4);

		ofHttpResponse response = sendRequest(urlEndpoint, requestBody.dump());

		// Handle the response from ChatGPT.
		if (response.status == 200) {
			ofJson result = ofJson::parse(response.data.getText());
			ofLogVerbose("ofxChatGPT") << "Data: " << response.data;
			return make_tuple(result["choices"][0]["message"]["content"].get<std::string>(), Success);
		}
		else {
			auto errorCode = parseErrorResponse(response);
			if (response.error == "Timeout was reached") {
				errorCode = Timeout;
			}
			ofLogError("ofxChatGPT") << getErrorMessage(errorCode);
			ofLogVerbose("ofxChatGPT") << "Data: " << response.data;
			return make_tuple("", errorCode);
		}
	}
	*/

	// The used server sometimes requires some IP reseting.
	bool doGptResetEndpointIP();

	void doClear();
};
