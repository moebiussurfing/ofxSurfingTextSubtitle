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

	//tester
	if (bAuto && (ofGetFrameNum() % (td * 60) == 0)) {
		doText();
	};
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

			ui.Add(ui.bDebugDebugger, OFX_IM_TOGGLE_BUTTON_ROUNDED);
			ui.AddSpacing();

			//--

			// Tester
			{
				ui.AddToggle("Debug", subs.bDebugPerformance);

				ui.AddLabel("TEST");
				ImVec2 sz{ ui.getWidgetsWidth(2), ui.getWidgetsHeightUnit() };
				ui.AddToggle("auto", bAuto, sz);
				ui.SameLine();
				if (ui.AddButton("DoText!", sz))
				{
					doText();
				};
				if (bAuto) ImGui::SliderInt("T", &td, 1, 10);
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

	if (key == ' ') 
	{
		//subs.setTogglePlay();
		
		// manual
		if (subs.getModePlayer() == 3) doText();
		else subs.setTogglePlay();
	}

	if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }

	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
}