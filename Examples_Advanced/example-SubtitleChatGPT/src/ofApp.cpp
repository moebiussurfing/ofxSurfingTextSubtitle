#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::exit()
{
	ofxSurfingHelpers::save(params);
}

//--------------------------------------------------------------
void ofApp::setInputGpt(string s, bool bWithHistory)
{
	// Spacing
	size_t n = 20;
	for (size_t i = 0; i < n; i++)
	{
		if (i == n / 2) ofLogNotice("ofApp") << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ setInputGpt ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
		else ofLogNotice("ofApp") << "|";
	}

	ui.AddToLog("setInputGpt()");

	doSendMessageToGpt(s);
}

//--------------------------------------------------------------
void ofApp::setup()
{
	w.doReset();
	ofxSurfingHelpers::setMonitorsLayout(0);

	//--

	ui.setup();

	subs.setUiPtr(&ui);
	subs.setup(); // Startup with no subs mode

	//--

#ifdef USE_WHISPER
	whisper.setup();
	whisper.vCallback.addListener(this, &ofApp::doUpdatedWhisper);
#endif

	bGui.set("GPT", true);

	//ui.ClearLogDefaultTags();
	//ui.AddLogTag(ofColor::white);

	//--

	// Editors

	editorInput.setup("Input");
	editorInput.setCustomFonts(ui.getFontsPtr(), ui.getFontsNames());

	// custom keyword
	editorInput.addKeyword("\"user\":");
	editorInput.addKeyword("\"assistant\":");

	// Advanced
	callback_t myFunctionDraw = std::bind(&ofApp::drawWidgets, this);
	//std::function<void()> myFunctionDraw = std::bind(&ofApp::drawWidgets, this);
	editorInput.setDrawWidgetsFunction(myFunctionDraw);

	//--

	editorResponse.setup("Response");
	editorResponse.setCustomFonts(ui.getFontsPtr(), ui.getFontsNames());

	//--

	params.add(subs.bGui);
	params.add(fontI);
	params.add(fontR);
	params.add(keyApi);
	params.add(bConversation);
	params.add(bGui_History);

	//--

	startup();
}

//--------------------------------------------------------------
void ofApp::setupGpt()
{
	if (keyApi.get() == "")
	{
		ui.AddToLog("No settled API key to run Setup GPT", OF_LOG_ERROR);
		return;
	}

	{
		ofSetLogLevel(OF_LOG_VERBOSE);
		ui.setLogLevel(OF_LOG_VERBOSE);

		ui.AddToLog("Setup GPT");

		//--

		ofxChatGPT chappy;
		chappy.setup(keyApi.get());
		vector<string> models;

		ofxChatGPT::ErrorCode err;
		tie(models, err) = chappy.getModelList();

		//ofLogNotice("ofApp") << "Available OpenAI GPT models:";
		ui.AddToLog("Available OpenAI GPT models:");
		for (auto model : models) {
			if (ofIsStringInString(model, "gpt")) {
				//ofLogNotice("ofApp") << model;
				ui.AddToLog(model);
			}
		}

		//--

		string model = "gpt-3.5-turbo";
		//string model = "gpt-4";

		chatGpt.setup(model, keyApi);
	}

	//--

	//// 1st query
	//if (0)
	//{
	//	string userMessage = "Hello, are you ChatGPT?";
	//	ui.AddToLog("User: " + userMessage);
	//	string assistantMessage = chatGpt.chatWithHistory(userMessage);
	//	ui.AddToLog("GPT: " + assistantMessage);

	//	userMessage = "Can you answer anything?";
	//	ui.AddToLog("User: " + userMessage);
	//	assistantMessage = chatGpt.chatWithHistory(userMessage);
	//	ui.AddToLog("GPT: " + assistantMessage);

	//	userMessage = "That's amazing, I can answer anything too. can i ask you a question?";
	//	ui.AddToLog("User: " + userMessage);
	//	assistantMessage = chatGpt.chatWithHistory(userMessage);
	//	ui.AddToLog("GPT: " + assistantMessage);
	//}

	//--

	// Create prompts
	prompts.clear();
	prompts.push_back(std::pair<std::string, std::string> { "5 short sentences.", GPT_Prompt_0() });
	prompts.push_back(std::pair<std::string, std::string> {  "10 words list.", GPT_Prompt_1() });
	prompts.push_back(std::pair<std::string, std::string> { "Similar bands", GPT_Prompt_2() });

	setPrompt(0);
}

//--------------------------------------------------------------
void ofApp::setPrompt(int index)
{
	if (index > prompts.size() - 1) {
		ofLogError("ofApp") << "Index " << index << "out of range";
		return;
	}

	iPrompt = index;
	namePrompt = ofToString(prompts[iPrompt].first);
	strPrompt = prompts[iPrompt].second;

	chatGpt.setSystemMessage(strPrompt);

	ui.AddToLog("Prompt: " + namePrompt);
	ui.AddToLog(strPrompt);

	//ofLogNotice("ofApp") << "namePrompt: " << namePrompt;
	//ofLogNotice("ofApp") << "strPrompt: " << strPrompt;
}

//--------------------------------------------------------------
void ofApp::startup()
{
	ofxSurfingHelpers::load(params);

	setupGpt();

	//ui.ClearLogDefaultTags();
}

//--------------------------------------------------------------
void ofApp::update()
{
	string s = "example-SubtitleChatGPT | " + ofToString(ofGetFrameRate(), 0) + "fps";
	ofSetWindowTitle(s);

	//--

	if (chatGpt.hasMessage())
	{
		string gptResponse;
		ofxChatGPT::ErrorCode errorCode;
		tie(gptResponse, errorCode) = chatGpt.getMessage();

		if (errorCode == ofxChatGPT::Success)
		{
			bError = false;
			ofLogNotice("ofApp") << "ofxChatGPT Success.";

			ofJson newGPTMsg;
			newGPTMsg["message"]["role"] = "assistant";
			newGPTMsg["message"]["content"] = gptResponse;

			ofLogNotice("ofApp") << "GPT: " << newGPTMsg;

			jResponse = newGPTMsg;

			//--

			textLastResponse = gptResponse;

			//TODO:
			subs.doBuildDataTextBlocks(textLastResponse);
			//subs.doSetTextSlideStart(textLastResponse);

			// Here textLastResponse is already catched 
			editorResponse.addText(textLastResponse);
			//ui.AddToLog("editorResponse.setTex");
		}
		else
		{
			bError = true;
			string errorMessage = "Error: " + ofxChatGPT::getErrorMessage(errorCode);
			ofLogError("ofApp") << "ofxChatGPT has an error: " << errorMessage;
		}
	}

	//--

#ifdef USE_WHISPER
	whisper.update();
#endif
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofClear(subs.getColorBg());

	//drawScene();

	subs.draw();

	//--

	if (!bGui) return;

	drawImGui();

	//--

#ifdef USE_WHISPER
	ofPushMatrix();
	ofTranslate(-15, ofGetHeight() * 0.7);
	whisper.draw();
	ofPopMatrix();
#endif
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
	// Bg: Blue when waiting. red if error. 
	{
		ofColor c;
		float v = glm::cos(10 * ofGetElapsedTimef());
		float a1 = ofMap(v, -1, 1, 100, 200, true);
		float a2 = ofMap(v, -1, 1, 8, 16, true);
		bool b = chatGpt.isWaiting();
		if (bError) c = ofColor(a1, 0, 0);
		else if (b) c = ofColor(0, 0, a1);
		else c = ofColor(a2);
		ofClear(c);
	}

	/*
	// Display the conversation on the screen.
	stringstream conversationText;
	// Iterate through the conversation messages and build the display text.
	for (const ofJson &message : chatGpt.getConversation()) {
		conversationText << message["role"] << ": " << message["content"] << "\n";
	}
	// Draw the conversation text on the screen.
	ofDrawBitmapStringHighlight("conversation:\n" + conversationText.str(), 20, 70);
	 */

	string s = "";
	int x, y, h, i;
	h = 20;
	x = 10;
	y = 20;

	i = 0;
	ofDrawBitmapStringHighlight("QUESTION: \n", x, y + (i++ * h));
	s = ofToString(jQuestion["message"]["role"]);
	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));
	s = ofToString(jQuestion["message"]["content"]);
	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));

	i = 0;
	y = ofGetHeight() / 2;
	ofDrawBitmapStringHighlight("RESPONSE: \n", x, y + (i++ * h));
	s = ofToString(jResponse["message"]["role"]);
	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));
	s = ofToString(jResponse["message"]["content"]);
	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));

	s = "";
	s += "Press 1-9 to ask for a MUSIC BAND.\n";
	s += "1 Jane's Addiction\n";
	s += "2 Fugazi\n";
	s += "3 Joy Division\n";
	s += "4 The Smiths\n";
	s += "5 Radio Futura\n";
	s += "6 John Frusciante\n";
	s += "7 Primus\n";
	s += "8 Kraftwerk\n";
	s += "9 Portishead\n\n";
	s += "Press SPACE to swap prompt.\n";
	s += "PROMPT #" + ofToString(iPrompt) + "\n";
	s += namePrompt + "\n\n";
	s += "Press ENTER to regenerate.";

	static ofBitmapFont f;
	auto bb = f.getBoundingBox(s, 0, 0);
	y = 20;
	x = ofGetWidth() / 2 - bb.getWidth() / 2;
	ofDrawBitmapStringHighlight(s, x, y);
}

//--------------------------------------------------------------
void ofApp::drawImGui()
{
	subs.drawGui();

	ui.Begin();
	{
		if (bGui)ImGui::SetNextWindowSize(ImVec2(230, 0), ImGuiCond_FirstUseEver);
		if (ui.BeginWindow(bGui))
		{
			string s;

			//ui.AddLabelHuge("SubtitleChatGPT", false, true);

			ui.AddMinimizerToggle();
			if (ui.isMaximized()) {
				ui.AddDebugToggle();
				ui.AddExtraToggle();
				ui.AddLogToggle();
			}

			ui.AddSpacingBigSeparated();

			//--

			ui.AddLabelHuge("Chat\nGpt", true, true);

			static ofParameter<bool> b{ "+",0 };
			if (ui.isMaximized()) ui.Add(b, OFX_IM_TOGGLE_ROUNDED_MINI);
			if (ui.isMaximized() && b)
			{
				ui.AddLabelBig("API KEY");
				ui.Add(keyApi, OFX_IM_TEXT_INPUT_NO_NAME);

				if (ui.AddButton("Setup"))
				{
					setupGpt();
				}
			}

			ui.AddSpacingSeparated();

			if (ui.AddButton("Send"))
			{
				setInputGpt(editorInput.getText(), bConversation);
			}
			//if (ui.AddButton("Get Response")) {
			//	//editorResponse.setText(textLastResponse);
			//}
			if (ui.AddButton("Random")) {
				doRandomInput();
			}
			if (ui.AddButton("Regenerate")) {
				doRegenerate();
			}

			ui.AddSpacingSeparated();

			ui.AddLabelHuge("PROMPT");

			ui.AddSpacing();
			if (ui.AddButton("Swap Prompt"))
			{
				doSwapPrompt();
			}
			ui.AddSpacing();
			ui.AddLabel(namePrompt);
			ui.PushFont(OFX_IM_FONT_BIG);
			ui.AddTooltip(strPrompt);
			ui.PopFont();

			ui.AddSpacingSeparated();

			//--

			ui.AddLabelHuge("EDITORS");

			ui.AddSpacing();
			ui.Add(editorInput.bGui, OFX_IM_TOGGLE_ROUNDED);
			ui.Add(editorResponse.bGui, OFX_IM_TOGGLE_ROUNDED);
			ui.Add(bGui_History, OFX_IM_TOGGLE_ROUNDED_MINI);

			ui.AddSpacingBigSeparated();

			//--

#ifdef USE_WHISPER
			drawImGuiWidgetsWhisper();
#endif
			//--

			ui.AddLabelHuge("Titles", true, true);

			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			if (subs.bGui) {
				ui.AddSpacing();
				ui.AddSpacingDouble();
				ui.PushFont(OFX_IM_FONT_BIG);
				s = "Random \nText!";
				s = ofToUpper(s);
				if (ui.AddButton(s, OFX_IM_BUTTON_BIG_XXXL_BORDER))
				{
					doPopulateText();
				}
				ui.PopFont();
			}

			ui.EndWindow();
		}

		//--

		// Editor Input
		editorInput.draw();

		//--

		if (ui.isMaximized() && ui.isExtraEnabled())
		{
			if (bGui_History)
			{
				//if (ui.isDebug())
				{
					// Gpt History
					if (ui.BeginWindow("GPT History", ImGuiWindowFlags_None))
					{
						if (ui.isDebug())ui.AddComboFontsSelector(fontI);
						ui.PushFont(SurfingFontTypes(fontI.get()));

						stringstream conversationText;

						//for (const ofJson& message : chatGpt.getConversation()) {
						//	conversationText << message["role"] << ": " << message["content"] << "\n";
						//}

						string s = "conversation:\n" + conversationText.str();

						ImGui::TextWrapped(s.c_str());

						ui.PopFont();
						ui.EndWindow();
					}
				}

				// Gpt last reply
				if (ui.BeginWindow("GPT Last Reply", ImGuiWindowFlags_None))
				{
					if (ui.isDebug()) ui.AddComboFontsSelector(fontR);
					ui.PushFont(SurfingFontTypes(fontR.get()));

					string s = textLastResponse;

					ImGui::TextWrapped(s.c_str());

					ui.PopFont();
					ui.EndWindow();
				}
			}

			if (ui.isDebug())
			{
				// Editor Response
				editorResponse.draw();
			}
		}

		//--

		subs.drawImGui();
	}
	ui.End();
}

//--------------------------------------------------------------
void ofApp::doPopulateText(string s)
{
	// auto generate a random text
	if (s == "")
	{
		s = ofxSurfingHelpers::getTextRandom();

		//workaround log fix
		s = " " + s;
	}

	////TODO:
	////trick
	//ui.ClearLogDefaultTags();
	//ofColor c = ofColor(subs.getColorText(), 255);
	//ui.AddLogTag(c);

	//ofLogNotice() << s;
	subs.doSetTextSlideStart(s);
	ui.AddToLog(s);

	// Spacing
	for (size_t i = 0; i < 10; i++)
	{
		ofLogNotice("ofApp") << "|";
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (ui.isOverInputText()) return; // skip when editing

	if (chatGpt.isWaiting()) return;

	if (key == '1') { strBandname = "Jane's Addiction"; doSendMessageToGpt(strBandname); }
	if (key == '2') { strBandname = "Fugazi"; doSendMessageToGpt(strBandname); }
	if (key == '3') { strBandname = "Joy Division"; doSendMessageToGpt(strBandname); }
	if (key == '4') { strBandname = "The Smiths";  doSendMessageToGpt(strBandname); }
	if (key == '5') { strBandname = "Radio Futura"; doSendMessageToGpt(strBandname); }
	if (key == '6') { strBandname = "John Frusciante"; doSendMessageToGpt(strBandname); }
	if (key == '7') { strBandname = "Primus"; doSendMessageToGpt(strBandname); }
	if (key == '8') { strBandname = "Kraftwerk"; doSendMessageToGpt(strBandname); }
	if (key == '9') { strBandname = "Portishead"; doSendMessageToGpt(strBandname); }

	//next prompt
	if (key == ' ') {
		doSwapPrompt();
	}

	//regenerate
	if (key == OF_KEY_RETURN) {
		doRegenerate();
	}

	//--

	if (key == 'g') { bGui = !bGui; }
	if (key == ' ') { doPopulateText(); }

	if (key == 'l') { subs.setToggleLive(); }
	if (key == 'e') { subs.setToggleEdit(); }
	//if (key == 'g') { subs.setToggleVisibleGui(); }

	//if (key == ' ') { subs.setTogglePlay(); }
	//if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }
	//if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	//if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	//if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };

	//--

	//switch (key)
	//{
	//case '1': {
	//	string path = ofToDataPath("text1.txt", true);
	//	editorInput.loadText(path);
	//	break;
	//}
	//case '2': {
	//	string path = ofToDataPath("text2.txt", true);
	//	editorInput.loadText(path);
	//	break;
	//}
	//case '3': {
	//	string str = "Garc�a Castell�n pone la X de Kitchen a Fern�ndez D�az\n y tapona la investigaci�n a Rajoy, \nla c�pula del PP y \nel CNI El juez \ndetermina que la decisi�n \nde espiar a B�rcenas con \nfondos reservados para evitar problemas judiciales \nal presidente y a Cospedal no \ntrascendi� del Ministerio del Interior.\nEl cierre de la instrucci�n llega \ncuando Anticorrupci�n apunta al CNI en \nel episodio del 'falso cura\n' e investiga una segunda Kitchen \nen la c�rcel";
	//	editorInput.setText(str);
	//	break;
	//}
	//}
}

//--------------------------------------------------------------
void ofApp::drawWidgets()
{
	if (editorInput.bExtra || !editorInput.bMenus) ui.AddSpacingSeparated();

	//float w = 200;
	float w = ui.getWidgetsWidth(4);
	float h = 2 * ui.getWidgetsHeightUnit();
	ImVec2 sz{ w, h };

	if (ui.AddButton("SEND", sz))
	{
		string s = editorInput.getText();
		ui.AddToLog(s, OF_LOG_NOTICE);
		setInputGpt(s, bConversation);

		editorInput.clearText();
	};
	ui.SameLine();
	if (ui.AddButton("CLEAR", sz))
	{
		editorInput.clearText();
	};

	ui.Add(bConversation);

	//editorInput.drawImGuiWidgetsFonts();

	ui.AddSpacingSeparated();

	//--

	// Catch from previous widget
	//bOver = ImGui::IsItemHovered();
	ImGuiIO& io = ImGui::GetIO();

	bool bMouseLeft = io.MouseClicked[0];
	bool bMouseRight = io.MouseClicked[1];
	float mouseWheel = io.MouseWheel;

	//bool bKeyReturn = ImGui::IsKeyboardKey(ImGuiKey_Enter);
	bool bKeyReturn = ImGui::IsKeyPressed(ImGuiKey_Enter);
	bool bCtrl = io.KeyCtrl;
	bool bShift = io.KeyShift;

	// Enter
	if (bCtrl && bKeyReturn)
	{
		string s = editorInput.getText();
		ui.AddToLog(s, OF_LOG_NOTICE);
		setInputGpt(s, bConversation);

		editorInput.clearText();
	}
};

//--------------------------------------------------------------
void ofApp::doSendMessageToGpt(string message) {

	ofxChatGPT::ErrorCode errorCode;

	bError = false;

	ofJson newUserMsg;
	newUserMsg["message"]["role"] = "user";
	newUserMsg["message"]["content"] = message;

	ofLogVerbose("ofApp") << "User: " << newUserMsg;

	jQuestion = newUserMsg;
	jResponse = ofJson();

	chatGpt.chatWithHistoryAsync(message);

	ui.AddToLog("doSendMessageToGpt()");
	ui.AddToLog(message);
}

//--------------------------------------------------------------
void ofApp::doRegenerate() {
	ofLogNotice("ofApp") << "doRegenerate()";
	ui.AddToLog("doRegenerate()");

	chatGpt.regenerateAsync();
}

//--------------------------------------------------------------
void ofApp::doRandomInput()
{
	ui.AddToLog("doRandomInput()");

	size_t sz = 9;
	float r = ofRandom(sz);
	if (r < 1) { strBandname = "Jane's Addiction"; }
	else if (r < 2) { strBandname = "Fugazi"; }
	else if (r < 3) { strBandname = "Joy Division"; }
	else if (r < 4) { strBandname = "The Smiths"; }
	else if (r < 5) { strBandname = "Radio Futura"; }
	else if (r < 6) { strBandname = "John Frusciante"; }
	else if (r < 7) { strBandname = "Primus"; }
	else if (r < 8) { strBandname = "Kraftwerk"; }
	else if (r < 9) { strBandname = "Portishead"; }
	string s = "";
	s = strBandname;
	doSendMessageToGpt(s);

	editorInput.setText(s);
	ui.AddToLog("editorInput.setText()");
	ui.AddToLog(s, OF_LOG_NOTICE);

	setInputGpt(editorInput.getText(), bConversation);

	//// Here textLastResponse is already catched 
	//editorResponse.setText(textLastResponse);
	//ui.AddToLog("editorResponse.setTex");
};

//--------------------------------------------------------------
void ofApp::doSwapPrompt() {
	if (iPrompt == 0) iPrompt = 1;
	else if (iPrompt == 1) iPrompt = 2;
	else if (iPrompt == 2) iPrompt = 0;
	setPrompt(iPrompt);
}

#ifdef USE_WHISPER

//--------------------------------------------------------------
void ofApp::doUpdatedWhisper()
{
	string s = whisper.getTextLast();
	ofLogNotice() << "doUpdatedWhisper(): " << s;
	doPopulateText(s);
}

//--------------------------------------------------------------
void ofApp::drawImGuiWidgetsWhisper()
{
	ui.Add(ui.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED);
	ui.Add(ui.bLog, OFX_IM_TOGGLE_BUTTON_ROUNDED);
	if (ui.bLog || whisper.bDebug) {
		ui.AddSpacing();
		if (ui.Add(whisper.vClear, OFX_IM_BUTTON)) {
			ui.ClearLog();
		};
	}
	ui.AddSpacingBigSeparated();

	ui.AddLabelHuge("ofxWhisper");
	ui.AddSpacing();
	ui.Add(whisper.bEnable, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
	ui.AddSpacing();

	if (ui.isMaximized())
	{
		ui.AddSpacing();
		ui.Add(whisper.bTimeStamps, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		ui.Add(whisper.bSpanish, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		s = "Uses another model\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.Add(whisper.bHighQuality, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		s = "Uses a bigger model\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.Add(whisper.step_ms);
		s = "Default is 500ms\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.Add(whisper.length_ms);
		s = "Default is 5000ms\n";
		s += "Requires app restart!";
		ui.AddTooltip(s);
		ui.AddSpacing();
		ui.Add(whisper.bDebug, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		if (whisper.bDebug) {
		}
		ui.AddSpacing();
		//ui.AddLabel(whisper.getTextLast());
	}

	ui.AddSpacingBigSeparated();
}

#endif