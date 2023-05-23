#pragma once

/*

	WIP Example

	TODO

	ui docking
	gpt setup/restart, reconnect.
		make new class.
		model and error/state
	add dual window 1: gui / 2: out
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

	ofxSurfingGui ui;

	ofxSurfingTextSubtitle subs;
	string path;
	void doPopulateText(string s = "");
	void doPopulateTextBlocks();
	void doClearList();

	ChatThread chatGpt;

	void setupGpt();
	void doGptSendMessage(string s, bool bWithHistory);
	void doGptSendMessage(string message);
	void doRegenerate();
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

	ofParameter<bool> bConversation{ "Conversation", false };//not used
	ofParameter<bool> bModeHistory{ "History",false };
	ofParameter<bool> bGui_GptLastReply{ "GPT Last Reply",false };
	ofParameter<bool> bGui_GptHistory{ "GPT History",false };
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
		ofLogVerbose("ofxChatGPT") << "SendData: " << requestBody.dump();

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

	bool doResetEndpointIP()
	{
		CURL* curl;
		CURLcode res;
		struct curl_slist* headers = NULL;

		const std::string api_key = apiKey;

		// Set up the headers
		std::string authorization_header = "Authorization: Bearer " + api_key;
		headers = curl_slist_append(headers, authorization_header.c_str());

		// Set up the URL and payload
		std::string url = "https://api.pawan.krd/resetip";
		std::string payload = "";

		curl = curl_easy_init();
		if (curl) {
			// Set up the request
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_POST, 1L);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

			// Disable SSL certificate verification
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

			// Send the request
			res = curl_easy_perform(curl);
			if (res != CURLE_OK) {
				std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

				string s = "doResetEndpointIP() curl_easy_perform() failed: " + ofToString(curl_easy_strerror(res));
				ui.AddToLog(s, OF_LOG_ERROR);

				// Clean up
				curl_slist_free_all(headers);
				curl_easy_cleanup(curl);

				return false;
			}

			// Clean up
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);

			return true;
		}

		return false;
	}


};
