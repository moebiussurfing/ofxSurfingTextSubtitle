#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
#if 1
	//w.doReset();
	//ofxSurfingHelpers::setMonitorsLayout(1, true, true);
#endif

	//--

	//ui.setImGuiViewPort(true);
	ui.setup();

	//--

	textInput.makeReferenceTo(bigTextInput.textInput);
	eTextInput = textInput.newListener([this](string& s)
		{
			ui.AddToLog("Prompt Input: " + s, OF_LOG_WARNING);
			//doGptSendMessage(s);

			doAttendCallbackTextInput();
		});

	// Optional customizations
	// Custom path for multi-instances 
	// avoid "collide folders".
	// or to organize bin/data
	// bigTextInput.setPathGlobal("Gpt");
	// bigTextInput.setName("Prompt");
	// Change the hint text:
	// bigTextInput.setHint("Type search");
	// Change the submit button text:
	 //bigTextInput.setSubmit("Send");

	ui.setDisableStartupResetLayout();

	//--

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

#ifdef USE_EDITOR_INPUT
	editorInput.setup("Input");
	editorInput.setCustomFonts(ui.getFontsPtr(), ui.getFontsNames());

	//// custom keyword
	//editorInput.addKeyword("\"user\":");
	//editorInput.addKeyword("\"assistant\":");

	// Advanced
	callback_t myFunctionDraw = std::bind(&ofApp::drawWidgets, this);
	//std::function<void()> myFunctionDraw = std::bind(&ofApp::drawWidgets, this);
	editorInput.setDrawWidgetsFunction(myFunctionDraw);
#endif

	//-

#ifdef USE_EDITOR_RESPONSE
	editorResponse.setup("Response");
	editorResponse.setCustomFonts(ui.getFontsPtr(), ui.getFontsNames());
#endif

	//--

	//params.add(bigTextInput.bGui);
	//params.add(apiKey);
	params.add(subs.bGui);
	params.add(fontI);
	params.add(fontR);
	params.add(bConversation);
	params.add(bModeHistory);
	params.add(bGui_GptHistory);
	params.add(bGui_GptLastReply);

	//--

	startup();
}

//--------------------------------------------------------------
void ofApp::startup()
{
	ofxSurfingHelpers::load(params);

	setupGpt();

	//ui.ClearLogDefaultTags();
}

//--------------------------------------------------------------
void ofApp::setupGpt()
{
	//if (apiKey.get() == "")
	//{
	//	ui.AddToLog("No settled API key to run Setup GPT", OF_LOG_ERROR);
	//	return;
	//}

	{
		ofSetLogLevel(OF_LOG_VERBOSE);
		ui.setLogLevel(OF_LOG_VERBOSE);

		ui.AddToLog("-------------------- Setup GPT --------------------", OF_LOG_WARNING);

		//--

		// GPT

		// Load file settings

		ofFile f;
		if (f.doesFileExist("GptChat_ConfigKey.json")) {
			ofJson configJson = ofLoadJson("GptChat_ConfigKey.json");
			//will fail if file do not exist
			if (configJson.find("apiKey") != configJson.end())
				apiKey = configJson["apiKey"].get<string>();
			if (configJson.find("model") != configJson.end())
				model = configJson["model"].get<string>();
		}
		else {
			apiKey = "your-api-key";

			// Model
			model = "gpt-4";
			//model = "gpt-3.5-turbo";
		}

		//fix


		//--

		ofxChatGPT chatGpt_;
		chatGpt_.setup(apiKey.get());
		vector<string> models;

		ofxChatGPT::ErrorCode errorCode;
		tie(models, errorCode) = chatGpt_.getModelList();

		ui.AddToLog("Available OpenAI GPT models:");
		for (auto m : models)
		{
			if (ofIsStringInString(m, "gpt"))
			{
				ui.AddToLog(m, OF_LOG_WARNING);
			}
		}

		//--

		// Initialize
		chatGpt.setup(model, apiKey);
	}

	//--

	//// 1st query
	//if (0)
	//{
	//	string userMessage = "Hello, are you ChatGPT?";
	//	ui.AddToLog("User: " + userMessage);
	//	string assistantMessage = chatGpt.chatWithHistory(userMessage);
	//	ui.AddToLog("GPT: " + assistantMessage);
	//
	//	userMessage = "Can you answer anything?";
	//	ui.AddToLog("User: " + userMessage);
	//	assistantMessage = chatGpt.chatWithHistory(userMessage);
	//	ui.AddToLog("GPT: " + assistantMessage);
	//
	//	userMessage = "That's amazing, I can answer anything too. can i ask you a question?";
	//	ui.AddToLog("User: " + userMessage);
	//	assistantMessage = chatGpt.chatWithHistory(userMessage);
	//	ui.AddToLog("GPT: " + assistantMessage);
	//}

	//--

	// Create prompts
	prompts.clear();
	prompts.push_back(std::pair<std::string, std::string> { "10 short sentences.", GPT_Prompt_0() });
	prompts.push_back(std::pair<std::string, std::string> { "10 words list.", GPT_Prompt_1() });
	prompts.push_back(std::pair<std::string, std::string> { "10 Similar bands", GPT_Prompt_2() });
	prompts.push_back(std::pair<std::string, std::string> { "Default", GPT_Prompt_3() });

	setPrompt(0);
}

//--------------------------------------------------------------
void ofApp::setPrompt(int index)
{
	ui.AddToLog("setPrompt(" + ofToString(index) + ")", OF_LOG_WARNING);

	if (index > prompts.size() - 1) {
		ui.AddToLog("Index " + ofToString(index) + " out of range!", OF_LOG_ERROR);
		return;
	}

	iPrompt = index;
	namePrompt = ofToString(prompts[iPrompt].first);
	strPrompt = prompts[iPrompt].second;

	chatGpt.setSystemMessage(strPrompt);

	ui.AddToLog("Prompt: " + namePrompt, OF_LOG_VERBOSE);
	ui.AddToLog(strPrompt, OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void ofApp::update()
{
	string s = "example-SubtitleChatGPT | " + ofToString(ofGetFrameRate(), 0) + "fps";
	ofSetWindowTitle(s);

	//--

	if (chatGpt.hasMessage())
	{
		doGptGetMessage();
	}

	//--

#ifdef USE_WHISPER
	whisper.update();
#endif
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// Bg
	{
		int bgMin = 100;

		// Blink when waiting. 
		// Red if error.
		if (bGptError) {
			float v = glm::cos(10 * ofGetElapsedTimef());
			float a1 = ofMap(v, -1, 1, 100, 200, true);
			ofColor c = ofColor(a1, 0, 0);
			ofClear(c);
		}
		else if (bGptWaiting) {
			ofColor c = bigTextInput.getColor();
			auto br = c.getBrightness();
			float v = glm::cos(10 * ofGetElapsedTimef());
			float a1 = ofMap(v, -1, 1, 100, 150, true);
			c.setBrightness(a1);
			ofClear(c);
		}
		else {
			// Flash when submit
			if (v > 0) v -= 0.05f;
			else v = 0;

			// Use color from subtitler when no flash
			if (v > 0) ofClear(bgMin + (255 - bgMin) * v);
			else ofClear(subs.getColorBg());
		}
	}

	//drawScene();

	subs.draw();

	//--

	//if (!bGui) return;
	drawImGui();

	//--

#ifdef USE_WHISPER
	ofPushMatrix();
	ofTranslate(-15, ofGetHeight() * 0.7);
	whisper.draw();
	ofPopMatrix();
#endif
}

////--------------------------------------------------------------
//void ofApp::drawScene()
//{
//	// Bg: Blue when waiting. red if error. 
//	{
//		ofColor c;
//		float v = glm::cos(10 * ofGetElapsedTimef());
//		float a1 = ofMap(v, -1, 1, 100, 200, true);
//		float a2 = ofMap(v, -1, 1, 8, 16, true);
//		bool b = chatGpt.isWaiting();
//		if (bError) c = ofColor(a1, 0, 0);
//		else if (b) c = ofColor(0, 0, a1);
//		else c = ofColor(a2);
//		ofClear(c);
//	}
//
//	/*
//	// Display the conversation on the screen.
//	stringstream conversationText;
//	// Iterate through the conversation messages and build the display text.
//	for (const ofJson &message : chatGpt.getConversation()) {
//		conversationText << message["role"] << ": " << message["content"] << "\n";
//	}
//	// Draw the conversation text on the screen.
//	ofDrawBitmapStringHighlight("conversation:\n" + conversationText.str(), 20, 70);
//	 */
//
//	string s = "";
//	int x, y, h, i;
//	h = 20;
//	x = 10;
//	y = 20;
//
//	i = 0;
//	ofDrawBitmapStringHighlight("QUESTION: \n", x, y + (i++ * h));
//	s = ofToString(jQuestion["message"]["role"]);
//	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));
//	s = ofToString(jQuestion["message"]["content"]);
//	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));
//
//	i = 0;
//	y = ofGetHeight() / 2;
//	ofDrawBitmapStringHighlight("RESPONSE: \n", x, y + (i++ * h));
//	s = ofToString(jResponse["message"]["role"]);
//	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));
//	s = ofToString(jResponse["message"]["content"]);
//	ofDrawBitmapStringHighlight(s, x, y + (i++ * h));
//
//	s = "";
//	s += "Press 1-9 to ask for a MUSIC BAND.\n";
//	s += "1 Jane's Addiction\n";
//	s += "2 Fugazi\n";
//	s += "3 Joy Division\n";
//	s += "4 The Smiths\n";
//	s += "5 Radio Futura\n";
//	s += "6 John Frusciante\n";
//	s += "7 Primus\n";
//	s += "8 Kraftwerk\n";
//	s += "9 Portishead\n\n";
//	s += "Press SPACE to swap prompt.\n";
//	s += "PROMPT #" + ofToString(iPrompt) + "\n";
//	s += namePrompt + "\n\n";
//	s += "Press ENTER to regenerate.";
//
//	static ofBitmapFont f;
//	auto bb = f.getBoundingBox(s, 0, 0);
//	y = 20;
//	x = ofGetWidth() / 2 - bb.getWidth() / 2;
//	ofDrawBitmapStringHighlight(s, x, y);
//}

//--------------------------------------------------------------
void ofApp::drawImGui()
{
	subs.drawGui();

	ui.Begin();
	{
		//--

		// TextInput

		bigTextInput.draw(ui);

		//--

		if (bGui) ImGui::SetNextWindowSize(ImVec2(230, 0), ImGuiCond_FirstUseEver);
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

			ui.AddLabelHuge("ChatGPT");

			static ofParameter<bool> b{ "SERVER",0 };
			if (ui.isMaximized()) ui.Add(b, OFX_IM_TOGGLE_ROUNDED_MINI);
			if (ui.isMaximized() && b)
			{
				ui.AddLabelBig("API KEY");
				ui.Add(apiKey, OFX_IM_TEXT_INPUT_NO_NAME);

				if (ui.AddButton("Setup"))
				{
					setupGpt();
				}
			}
			ui.AddSpacingSeparated();

			ui.PushFont(OFX_IM_FONT_BIG);
			{
#ifdef USE_EDITOR_INPUT
				if (ui.AddButton("Send"))
				{
					doGptSendMessage(editorInput.getText(), bConversation);
				}
#endif
				if (ui.AddButton("Random", OFX_IM_BUTTON_BIG)) {
					doRandomInput();
				}
			}
			ui.PopFont();

			if (ui.AddButton("Regenerate")) {
				doRegenerate();
			}

			ui.AddSpacingSeparated();

			ui.AddLabelHuge("Prompt");
			ui.AddSpacing();

			ui.Add(bigTextInput.bGui, OFX_IM_TOGGLE_ROUNDED);
			if (ui.isMaximized()) ui.Add(bigTextInput.bGui_Config, OFX_IM_TOGGLE_ROUNDED_SMALL);
			ui.AddSpacing();

			if (ui.AddButton("Swap Prompt", OFX_IM_BUTTON))
			{
				doSwapPrompt();
			}
			ui.AddSpacing();

			ui.AddLabel(namePrompt);

			//ui.PushFont(OFX_IM_FONT_BIG);
			//ui.AddTooltip(strPrompt);
			//ui.PopFont();

			ui.AddSpacingSeparated();

			//--


#ifdef USE_EDITOR_INPUT
			ui.AddLabelHuge("EDITORS");
			ui.AddSpacing();
			ui.Add(editorInput.bGui, OFX_IM_TOGGLE_ROUNDED);
#endif
			//if (subs.bExtra)
			if (ui.isMaximized())
			{
				ui.Add(bModeHistory, OFX_IM_CHECKBOX);
				ui.Add(bGui_GptHistory, OFX_IM_TOGGLE_ROUNDED_MINI);
				ui.Add(bGui_GptLastReply, OFX_IM_TOGGLE_ROUNDED_MINI);
#ifdef USE_EDITOR_RESPONSE
				ui.Add(editorResponse.bGui, OFX_IM_TOGGLE_ROUNDED_MINI);
#endif
				ui.AddSpacingBigSeparated();

				s = errorCodesNames[indexErrorCode.get()];
				ui.AddLabelBig(s);

				s = gptErrorMessage;
				ui.AddLabelBig(s);

				//TODO: spinning
				ui.Add(bGptWaiting, OFX_IM_CHECKBOX);

				ui.AddSpacingBigSeparated();
			}

			//--

#ifdef USE_WHISPER
			drawImGuiWidgetsWhisper();
#endif
			//--

			ui.AddLabelHuge("Titles");
			ui.AddSpacing();

			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			if (subs.bGui)
			{
				ui.AddSpacing();
				ui.AddSpacingDouble();

				//ui.AddLabelBig("Tester", false, true);
				static bool bTester = 0;
				ImGui::Checkbox("Tester", &bTester);
				ui.PushFont(OFX_IM_FONT_BIG);
				{
					if (bTester) {
						s = "Random\nText!";
						s = ofToUpper(s);
						if (ui.AddButton(s, OFX_IM_BUTTON_BIG))
						{
							doPopulateText();
						}
						s = "Do\nBlock!";
						s = ofToUpper(s);
						if (ui.AddButton(s, OFX_IM_BUTTON_BIG))
						{
							doPopulateTextBlocks();
						}
						s = "Clear\nList";
						s = ofToUpper(s);
						if (ui.AddButton(s, OFX_IM_BUTTON_BIG))
						{
							doClearList();
						}
					}

					ui.PopFont();
				}
			}
			ui.EndWindow();
			}
		if (bGui) {

			//--

			// Editor Input

#ifdef USE_EDITOR_INPUT
			editorInput.drawImGui();
#endif

			//--

			//if (ui.isMaximized() && ui.isExtraEnabled())
			if (ui.isExtraEnabled())
			{
				//if (bModeHistory)
				{
					//if (ui.isDebug())
					{
						// Gpt History
						if (ui.BeginWindow(bGui_GptHistory, ImGuiWindowFlags_None))
						{
							if (ui.isDebug()) ui.AddComboFontsSelector(fontI);

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
					if (ui.BeginWindow(bGui_GptLastReply, ImGuiWindowFlags_None))
					{
						if (ui.isDebug()) ui.AddComboFontsSelector(fontR);

						ui.PushFont(SurfingFontTypes(fontR.get()));

						string s = textLastResponse;

						ImGui::TextWrapped(s.c_str());

						ui.PopFont();

						ui.EndWindow();
					}
				}

				//if (ui.isDebug())
				{

#ifdef USE_EDITOR_RESPONSE
					// Editor Response
					editorResponse.drawImGui();
#endif
				}
			}

			//--

			subs.drawImGui();
		}
		}
	ui.End();
	}

//--------------------------------------------------------------
void ofApp::doPopulateText(string s)
{
	//workflow
	doClearList();

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

// Function to process a full file and split into blocks/slides.
//--------------------------------------------------------------
void ofApp::doPopulateTextBlocks() {
	string path = "files/txt/text2.txt";
	subs.setupTextBlocks(path);
}
//--------------------------------------------------------------
void ofApp::doClearList() {
	subs.doClearList();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (ui.isOverInputText()) return; // skip when editing

	if (chatGpt.isWaiting()) return;

	if (key == '1') { strBandname = "Jane's Addiction"; doGptSendMessage(strBandname); }
	if (key == '2') { strBandname = "Fugazi"; doGptSendMessage(strBandname); }
	if (key == '3') { strBandname = "Joy Division"; doGptSendMessage(strBandname); }
	if (key == '4') { strBandname = "The Smiths";  doGptSendMessage(strBandname); }
	if (key == '5') { strBandname = "Radio Futura"; doGptSendMessage(strBandname); }
	if (key == '6') { strBandname = "John Frusciante"; doGptSendMessage(strBandname); }
	if (key == '7') { strBandname = "Primus"; doGptSendMessage(strBandname); }
	if (key == '8') { strBandname = "Kraftwerk"; doGptSendMessage(strBandname); }
	if (key == '9') { strBandname = "Portishead"; doGptSendMessage(strBandname); }

	// next prompt
	if (key == ' ') { doSwapPrompt(); }

	// regenerate
	if (key == OF_KEY_RETURN) { doRegenerate(); }

	//--

	if (key == 'd') bigTextInput.bDebug = !bigTextInput.bDebug;

	if (key == 'g') {
		bGui = !bGui;

		//workflow
		if (!bGui && bigTextInput.bGui_Config) bigTextInput.bGui_Config = 0;
	}

	if (key == ' ') { doRegenerate(); }

	//if (key == ' ') { doPopulateText(); }
	if (key == 'l') { subs.setToggleLive(); }
	if (key == 'e') { subs.setToggleEdit(); }
	//if (key == 'g') { subs.setToggleVisibleGui(); }

	//if (key == ' ') { subs.setTogglePlay(); }
	//if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }
	//if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	//if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	//if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };

	//--

#ifdef USE_EDITOR_INPUT
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
#endif
}

//--------------------------------------------------------------
void ofApp::drawWidgets()
{
	//--

#ifdef USE_EDITOR_INPUT
	//if (editorInput.bExtra || !editorInput.bMenus) ui.AddSpacingSeparated();

	//float w = 200;
	float w = ui.getWidgetsWidth(4);
	float h = 2 * ui.getWidgetsHeightUnit();
	ImVec2 sz{ w, h };

	if (ui.AddButton("SEND", sz))
	{
		string s = editorInput.getText();
		ui.AddToLog(s, OF_LOG_NOTICE);
		doGptSendMessage(s, bConversation);

		//workflow
		//editorInput.clearText();
	};
	ui.SameLine();
	if (ui.AddButton("CLEAR", sz))
	{
		editorInput.clearText();
	};

	ui.Add(bConversation);

	//editorInput.drawImGuiWidgetsFonts();

	ui.AddSpacingSeparated();
#endif

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
#ifdef USE_EDITOR_INPUT
		string s = editorInput.getText();
		ui.AddToLog(s, OF_LOG_NOTICE);
		doGptSendMessage(s, bConversation);
		editorInput.clearText();
#endif
}
}

//--------------------------------------------------------------
void ofApp::doGptSendMessage(string s, bool bWithHistory)
{
	doGptSendMessage(s);
}

//--------------------------------------------------------------
void ofApp::doGptSendMessage(string message) {

	// Clear
	
	ofxChatGPT::ErrorCode errorCode;
	bGptError = false;

	ofJson newUserMsg;
	newUserMsg["message"]["role"] = "user";
	newUserMsg["message"]["content"] = message;

	ofLogVerbose("ofApp") << "User: " << newUserMsg;

	jQuestion = newUserMsg;
	jResponse = ofJson();

	bGptWaiting = 1;

	textLastResponse = "";
#ifdef USE_EDITOR_RESPONSE
	editorResponse.clearText();//workflow
#endif

	ui.AddToLog("doGptSendMessage()", OF_LOG_WARNING);
	ui.AddToLog(message);

	//--

	// Submit

	//TODO: mode
	if (bModeHistory) chatGpt.chatWithHistoryAsync(message);
	else chatGpt.chatAsync(message);
}

//--------------------------------------------------------------
void ofApp::doRegenerate() {
	ui.AddToLog("doRegenerate()", OF_LOG_WARNING);

	chatGpt.regenerateAsync();
}

//--------------------------------------------------------------
void ofApp::doGptGetMessage()
{
	ui.AddToLog("doGptGetMessage()", OF_LOG_WARNING);

	string strGptResponse;
	ofxChatGPT::ErrorCode errorCode;
	tie(strGptResponse, errorCode) = chatGpt.getMessage();

	indexErrorCode = getErrorCodeByCode(errorCode);
	if (indexErrorCode > 0 && indexErrorCode < 9) bGptError = true;
	else bGptError = false;

	bGptWaiting = 0;

	if (errorCode == ofxChatGPT::Success)
	{
		// Get response

		ofLogNotice("ofApp") << "ofxChatGPT Success.";
		bGptError = false;
		gptErrorMessage = "";

		ofJson newGPTMsg;
		newGPTMsg["message"]["role"] = "assistant";
		newGPTMsg["message"]["content"] = strGptResponse;

		ofLogNotice("ofApp") << "GPT: " << newGPTMsg;

		jResponse = newGPTMsg;

		//--

		// Process response

		for (auto& content : newGPTMsg["content"]) {
			strGptResponse += content.get<std::string>() + "\n";
		}
		//ofLogNotice("ofxSurfingTextSubtitle") << "strGptResponse:" << strGptResponse;

		//--

		textLastResponse = ofxSurfingHelpers::removeNumbersStartingLines(strGptResponse);
		ofLogNotice("ofxSurfingTextSubtitle") << endl << textLastResponse;

		//--

		// Build slides
		subs.doBuildDataText(textLastResponse);

		/*
		//TODO:
		//there's no new line \n marks. so we assume the blocks will be numbered 1., 2., 3. etc
		size_t sz = ofxSurfingHelpers::countNewlines(textLastResponse);
		bool b = true;
		//b = sz == 0;
		if (b) {
			subs.doBuildDataTextBlocks(textLastResponse, true);
		}
		else {//we found \n tags. so we assume blocks ends with \n.
			subs.doBuildDataTextBlocks(textLastResponse);
		}
		*/

		//--

#ifdef USE_EDITOR_RESPONSE
		// Here textLastResponse is already catched 
		//editorResponse.clearText();//workflow
		editorResponse.addText(textLastResponse);
#endif
	}
	else
	{
		indexErrorCode = getErrorCodeByCode(errorCode);
		bGptError = true;
		gptErrorMessage = "Error: " + ofxChatGPT::getErrorMessage(errorCode);
		ofLogError("ofApp") << "ofxChatGPT has an error: " << gptErrorMessage;
		ui.AddToLog(gptErrorMessage, OF_LOG_ERROR);
	}
}

//--------------------------------------------------------------
void ofApp::doRandomInput()
{
	ui.AddToLog("doRandomInput()", OF_LOG_WARNING);

	//workflow
	doClearList();

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
	ui.AddToLog(s, OF_LOG_NOTICE);

	bigTextInput.setText(s);
	doGptSendMessage(s);

#ifdef USE_EDITOR_INPUT
	editorInput.setText(s);
	ui.AddToLog("editorInput.setText()");
	ui.AddToLog(s, OF_LOG_NOTICE);
	doGptSendMessage(editorInput.getText(), bConversation);
#endif
};

//--------------------------------------------------------------
void ofApp::doSwapPrompt() {
	if (iPrompt == 0) iPrompt = 1;
	else if (iPrompt == 1) iPrompt = 2;
	else if (iPrompt == 2) iPrompt = 3;
	else if (iPrompt == 3) iPrompt = 0;
	setPrompt(iPrompt);
}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofLogVerbose() << "exit()";

	ofxSurfingHelpers::save(params);

	ofJson configJson;
	configJson["apiKey"] = apiKey;
	configJson["model"] = model;
	ofSavePrettyJson("GptChat_ConfigKey.json", configJson);
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

//--------------------------------------------------------------
int ofApp::getErrorCodeByCode(ofxChatGPT::ErrorCode errorCode)
{
	int i = -1;

	switch (errorCode)
	{
	case ofxChatGPT::Success:
		i = 0;
		break;
	case ofxChatGPT::InvalidAPIKey:
		i = 1;
		break;
	case ofxChatGPT::NetworkError:
		i = 2;
		break;
	case ofxChatGPT::ServerError:
		i = 3;
		break;
	case ofxChatGPT::RateLimitExceeded:
		i = 4;
		break;
	case ofxChatGPT::TokenLimitExceeded:
		i = 5;
		break;
	case ofxChatGPT::InvalidModel:
		i = 6;
		break;
	case ofxChatGPT::BadRequest:
		i = 7;
		break;
	case ofxChatGPT::Timeout:
		i = 8;
		break;
	case ofxChatGPT::UnknownError:
		i = 9;
		break;
	default:
		i = 9;
		break;
	}

	return i;
}

//--------------------------------------------------------------
void ofApp::doAttendCallbackTextInput()
{
	//workflow
	//clear
	doClearList();
#ifdef USE_EDITOR_RESPONSE
	editorResponse.clearText();
#endif

	//// will be called when submitted text changed!
	//text = bigTextInput.getText();
	//ofLogNotice(__FUNCTION__) << text;
	//ofSetWindowTitle(text);

	//ui.AddToLog("TextInput: " + textInput.get(), OF_LOG_NOTICE);

	v = 1;

	string s = textInput.get();
	doGptSendMessage(s);
}