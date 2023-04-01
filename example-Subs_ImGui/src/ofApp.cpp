#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofxSurfingHelpers::SurfSetMyMonitor(0);

	subs.setUiPtr(&ui);

#ifndef USE_IM_GUI__SUBTITLES
	subs.setDisableGuiInternal(true);
#endif

	// SRT file path
	{
		path = "subs/Huxley.srt";
		//path = "subs/Alphaville.srt";
		//path = "subs/spanish.srt";
	}

	subs.setup(path);
}

//--------------------------------------------------------------
void ofApp::update() {
	string name = "example-Subs_ImGui";
	ofxSurfingHelpers::SurfSetWindowTitleDebugPerformance(name);

	subs.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(subs.getColorBg());

	subs.draw();

	//--

	subs.drawGui();
	ui.Begin();
	{
		if (ui.BeginWindow("ofApp"))
		{
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED);
			ui.AddSpacingSeparated();

			//--

			// Tester
			{
				ui.AddToggle("Debug", subs.bDebugPerformance);

				// lambda function to random populate slides, loading different text files.
				static auto doText = [&]() {
					string s;
					static bool b = false;
					if (b) {
						b = !b;
						if (ofRandom(1) < 0.5)
							s = subs.loadFileText("files/text1.txt");
						else
							s = subs.loadFileText("files/text2.txt");
					}
					else {
						b = !b;
						if (ofRandom(1) < 0.5)
							s = subs.loadFileText("files/text3.txt");
						else
							s = subs.loadFileText("files/text4.txt");
					}
					subs.doSetTextSlide(s);
				};

				ImVec2 sz{ ui.getWidgetsWidth(2), ui.getWidgetsHeightUnit() };

				//ui.AddLabelBig("TEST");
				static bool bAuto = false;
				static int td = 2;
				ImGui::SliderInt("T", &td, 1, 10);
				ui.AddToggle("auto", bAuto, sz);
				ui.SameLine();

				if (bAuto && (ofGetFrameNum() % (td * 60) == 0)) {
					doText();
				};

				if (ui.AddButton("DoText!", sz))
				{
					doText();
				};
			}

			//--

			ui.EndWindow();
		}

		subs.drawImGui();
	}
	ui.End();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') {
		subs.setToggleVisibleGui();
#ifdef USE_PRESETS
		presets.setVisibleGui(subs.getVisibleGui());
#endif
	}

	if (key == 'l') { subs.setToggleLive(); }
	if (key == 'e') { subs.setToggleEdit(); }

	if (key == ' ') { subs.setTogglePlay(); }
	if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }

	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
}