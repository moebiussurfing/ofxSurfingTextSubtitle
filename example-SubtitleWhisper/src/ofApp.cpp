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
		ImGui::SetNextWindowSize(ImVec2(230, 0), ImGuiCond_Appearing);
		if (ui.BeginWindow(bGui))
		{
			string s;
#ifdef USE_WHISPER
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
#endif
			ui.AddSpacingBigSeparated();

			ui.AddLabelHuge("ofxSurfing\nTextSubtitle");
			ui.AddSpacing();
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
	if (key == 'g') { bGui = !bGui; }
	if (key == ' ') { doPopulateText(); }

	//if (key == 'g') { subs.setToggleVisibleGui(); }
	if (key == 'l') { subs.setToggleLive(); }
	if (key == 'e') { subs.setToggleEdit(); }
	//if (key == ' ') { subs.setTogglePlay(); }
	//if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }
	//if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	//if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	//if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
}