#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::exit()
{
	ofxSurfingHelpers::save(params);
}

//--------------------------------------------------------------
void ofApp::setInputGPT(string s)
{
	//textLastResponse = chatGPT.chatWithHistory(s);
	chatGPT.chatWithHistory(s);

	textLastResponse = chatGPT.chat(s);
	//textLastResponse = chatGPT.chat(s, std::function<void(std::string)>(myCallback));
};

//--------------------------------------------------------------
void ofApp::setup()
{
	params.add(keyAPI);

	subs.setUiPtr(&ui);
	subs.setup(); // Startup with no subs mode

#ifdef USE_WHISPER
	whisper.setup();
	whisper.vCallback.addListener(this, &ofApp::doUpdatedWhisper);
#endif

	bGui.set("example-SubtitleChatGPT", true);

	//ui.ClearLogDefaultTags();
	//ui.AddLogTag(ofColor::white);

	editorInput.setup("Input");
	//editorInput.loadText(ofToDataPath("text1.txt", true));

	editorReply.setup("Response");

	startup();
}

//--------------------------------------------------------------
void ofApp::startup()
{
	ofxSurfingHelpers::load(params);

	setupGPT();

	//ui.ClearLogDefaultTags();

}

//--------------------------------------------------------------
void ofApp::update()
{
	string s = "example-SubtitleChatGPT | " + ofToString(ofGetFrameRate(), 0) + "fps";
	ofSetWindowTitle(s);

	subs.update();

#ifdef USE_WHISPER
	whisper.update();
#endif
}

#ifdef USE_WHISPER
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
void ofApp::draw()
{
	ofClear(subs.getColorBg());

	subs.draw();

	if (!bGui) return;

	subs.drawGui();

	ui.Begin();
	{
		ImGui::SetNextWindowSize(ImVec2(230, 0), ImGuiCond_FirstUseEver);
		if (ui.BeginWindow(bGui))
		{
			string s;

			ui.AddLabelHuge("example-SubtitleChatGPT");

			ui.AddMinimizerToggle();
			ui.AddLogToggle();
			ui.AddSpacingSeparated();

			//--

			ui.AddLabelHuge("Chat GPT");
			ui.AddSpacing();
			ui.Add(keyAPI, OFX_IM_TEXT_INPUT_NO_NAME);
			if (ui.AddButton("Setup")) {
				setupGPT();
			}
			if (ui.AddButton("Set Input")) {
				setInputGPT(editorInput.getText());
			}
			if (ui.AddButton("Get Response")) {
				editorReply.setText(textLastResponse);
			}

			ui.AddSpacingSeparated();

			//--

#ifdef USE_WHISPER
			drawImGuiWidgetsWhisper();
#endif
			//--

			ui.AddLabelHuge("Text Titles");
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			if (subs.bGui) {
				ui.AddSpacing();
				ui.AddSpacingDouble();
				ui.PushFont(OFX_IM_FONT_BIG);
				s = "Random Text!";
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
		ui.PushFont(OFX_IM_FONT_BIG);
		editorInput.draw();
		ui.PopFont();

		//--

		if (ui.BeginWindow("GPT History", ImGuiWindowFlags_None))
		{
			string s = "";
			stringstream conversationText;
			for (const ofJson& message : chatGPT.getConversation()) {
				conversationText << message["role"] << ": " << message["content"] << "\n";
			}
			//ofDrawBitmapString("conversation:\n" + conversationText.str(), 20, 70);
			s = "conversation:\n" + conversationText.str();

			ui.AddLabel(s);

			ui.EndWindow();
		}

		//--

		if (ui.BeginWindow("GPT Last Reply", ImGuiWindowFlags_None))
		{
			ui.AddLabelBig(textLastResponse);
			ui.EndWindow();
		}

		// Editor Input
		ui.PushFont(OFX_IM_FONT_BIG);
		editorReply.draw();
		ui.PopFont();

		//--

		subs.drawImGui();
	}
	ui.End();

	//--

#ifdef USE_WHISPER
	ofPushMatrix();
	ofTranslate(-15, ofGetHeight() * 0.7);
	whisper.draw();
	ofPopMatrix();
#endif
}

#ifdef USE_WHISPER
//--------------------------------------------------------------
void ofApp::doUpdatedWhisper()
{
	string s = whisper.getTextLast();
	ofLogNotice() << "doUpdatedWhisper(): " << s;
	doPopulateText(s);
}
#endif

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

	ofLogNotice() << s;
	subs.doSetTextSlide(s);
	ui.AddToLog(s);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (ui.isOverInputText()) return; // skip when editing

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

	switch (key)
	{
	case '1': {
		string path = ofToDataPath("text1.txt", true);
		editorInput.loadText(path);
		break;
	}
	case '2': {
		string path = ofToDataPath("text2.txt", true);
		editorInput.loadText(path);
		break;
	}
	case '3': {
		string str = "Garc�a Castell�n pone la X de Kitchen a Fern�ndez D�az\n y tapona la investigaci�n a Rajoy, \nla c�pula del PP y \nel CNI El juez \ndetermina que la decisi�n \nde espiar a B�rcenas con \nfondos reservados para evitar problemas judiciales \nal presidente y a Cospedal no \ntrascendi� del Ministerio del Interior.\nEl cierre de la instrucci�n llega \ncuando Anticorrupci�n apunta al CNI en \nel episodio del 'falso cura\n' e investiga una segunda Kitchen \nen la c�rcel";
		editorInput.setText(str);
		break;
	}
	}
}

//--------------------------------------------------------------
void ofApp::setupGPT() {
	if (keyAPI.get() == "") {
		ui.AddToLog("No settled API key to run Setup GPT", OF_LOG_ERROR);
		return;
	}
	ui.AddToLog("Setup GPT");

	chatGPT.setup(keyAPI.get());

	ofSetLogLevel(OF_LOG_VERBOSE);
	ui.setLogLevel(OF_LOG_VERBOSE);

	ui.AddToLog("GPT Model list");
	for (auto model : chatGPT.getModelList()) {
		ui.AddToLog(model);
	}

	if (0) {
		string userMessage = "Hello, are you ChatGPT?";
		ui.AddToLog("User: " + userMessage);
		string assistantMessage = chatGPT.chatWithHistory(userMessage);
		ui.AddToLog("GPT: " + assistantMessage);

		userMessage = "Can you answer anything?";
		ui.AddToLog("User: " + userMessage);
		assistantMessage = chatGPT.chatWithHistory(userMessage);
		ui.AddToLog("GPT: " + assistantMessage);

		userMessage = "That's amazing, I can answer anything too. can i ask you a question?";
		ui.AddToLog("User: " + userMessage);
		assistantMessage = chatGPT.chatWithHistory(userMessage);
		ui.AddToLog("GPT: " + assistantMessage);
	}
}