#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	subs.setUiPtr(&ui);

	// Startup with no subs mode
	subs.setup();

#ifdef USE_WHISPER
	whisper.setup();
	whisper.vCallback.addListener(this, &ofApp::doUpdatedWhisper);
#endif

	bGui.set("ofApp_Whisper", true);

	ui.ClearLogDefaultTags();
	ui.AddLogTag(ofColor::white);
}

//--------------------------------------------------------------
void ofApp::update()
{
	string s = "example-SubtitleWhisper | " + ofToString(ofGetFrameRate(), 0) + "fps";
	ofSetWindowTitle(s);

	subs.update();

#ifdef USE_WHISPER
	whisper.update();
#endif
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofClear(subs.getColorBg());

	subs.draw();

	if (!bGui) return;

	subs.drawGui();

	ui.Begin();
	{
		ImGui::SetNextWindowSize(ImVec2(220, -1), ImGuiCond_Appearing);
		if (ui.BeginWindow(bGui))
		{
			string s;
#ifdef USE_WHISPER
			ui.Add(ui.bMinimize, OFX_IM_TOGGLE_BUTTON_ROUNDED);
			ui.Add(ui.bLog, OFX_IM_TOGGLE_BUTTON_ROUNDED);
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
				s = "Requires app restart!";
				ui.AddTooltip(s);
				ui.Add(whisper.bHighQuality, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
				ui.AddTooltip(s);
				ui.Add(whisper.bDebug, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
				if (whisper.bDebug) {
				}
				ui.AddSpacing();
				if (ui.Add(whisper.vClear, OFX_IM_BUTTON)) {
					ui.ClearLog();
				};
				ui.AddSpacing();
				//ui.AddLabel(whisper.getTextLast());
			}
#endif
			ui.AddSpacingBigSeparated();

			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			ui.AddSpacingDouble();
			ui.PushFont(OFX_IM_FONT_BIG);
			s = "Random Text!";
			s = ofToUpper(s);
			if (ui.AddButton(s, OFX_IM_BUTTON_BIG_XXXL_BORDER))
			{
				doPopulateText();
			}
			ui.PopFont();
			ui.EndWindow();
		}

		subs.drawImGui();
	}
	ui.End();

#ifdef USE_WHISPER
	ofPushMatrix();
	ofTranslate(-16, ofGetHeight() * 0.7);
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
		static int ilast = -1;
		float r = ofRandom(1.f);
		if (r < 0.25f && ilast != 0) {
			s = "Hello people, how are you today?";
			if (ofRandom(1) < 0.5) s += " And current frame is " + ofToString(ofGetFrameNum());
			ilast = 0;
		}
		else if (r < 0.5f && ilast != 1) {
			s = "Hello dude. Ready to wake up?";
			ilast = 1;
		}
		else if (r < 0.75f && ilast != 2) {
			s = "I go sleep now. Ready to wake up!";
			if (ofRandom(1) < 0.5) s += " Time is " + ofToString(ofGetTimestampString());
			ilast = 2;
		}
		else if (ilast != 3) {
			s = "Hey, hello! Im ready to go out";
			if (ofRandom(1) < 0.5) s += " Current time is " + ofToString(ofGetTimestampString());
			ilast = 3;
		}
		else {
			s = "How are you tonight? Bye bye people";
			ilast = 4;
		}

		//workaround fix
		s = " " + s;
	}

	//TODO:
	//trick
	ui.ClearLogDefaultTags();
	ofColor c = ofColor(subs.getColorText(), 255);
	ui.AddLogTag(c);

	ofLogNotice() << s;
	subs.doSetTextSlide(s);
	ui.AddToLog(s);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	//if (key == 'f') { ofSetFullscreen(true); }
	//if (key == 'f') { ofSetFullscreen(true); }

	if (key == 'g') { bGui = !bGui; }

	if (key == OF_KEY_F8 || key == ' ') { doPopulateText(); }

	//if (key == 'g') { subs.setToggleVisibleGui(); }
	if (key == 'l') { subs.setToggleLive(); }
	if (key == 'e') { subs.setToggleEdit(); }
	//if (key == ' ') { subs.setTogglePlay(); }
	//if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }
	//if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	//if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	//if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
}