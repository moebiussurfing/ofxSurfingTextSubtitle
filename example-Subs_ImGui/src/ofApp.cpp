#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofxSurfingHelpers::setMonitorsLayout(0);

	subs.setUiPtr(&ui);

#ifndef USE_IM_GUI__SUBTITLES
	subs.setDisableGuiInternal(true);
#endif

	// SRT file path
	{
		path = "subs/Huxley.srt";
	}

	subs.setup(path);
}

//--------------------------------------------------------------
void ofApp::update() {
	string name = "example-Subs_ImGui";
	ofxSurfingHelpers::setWindowTitleDebugPerformance(name, true);

	subs.update();

	// Tester
	if (bAuto && (ofGetFrameNum() % (d * 60) == 0)) 
	{
		doLoadText();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(subs.getColorBg());

	//--

	// imageBg
	if (0) 
	{
		if (!image.isAllocated()) image.load("images/image2.png");
		ofxSurfingHelpers::drawImageFullScreenFit(image, OF_SCALEMODE_FILL, 0);
	}

	//--

	subs.draw();

	//--

	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui() {

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
				ui.AddLabel("TEST");

				ImVec2 sz{ ui.getWidgetsWidth(2), ui.getWidgetsHeightUnit() };
				ui.AddToggle("DoAuto", bAuto, sz);
				ui.SameLine();
				if (ui.AddButton("DoText", sz))
				{
					doLoadText();
				}
				if (bAuto) {
					ImGui::PushItemWidth(100);
					ImGui::SliderInt("T", &d, 1, 10);
					ImGui::PopItemWidth();
				}

				if (ui.AddButton("DoBlocks", sz))
				{
					doLoadTextBlocks();
				}
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
		if (subs.getModePlayer() == 3) doLoadText();
		else subs.setTogglePlay();
	}

	if (key == OF_KEY_RETURN) { subs.setTogglePlayForced(); }

	if (key == OF_KEY_LEFT) { subs.setSubtitlePrevious(); }
	if (key == OF_KEY_RIGHT) { subs.setSubtitleNext(); }
	if (key == OF_KEY_BACKSPACE) { subs.setSubtitleRandomIndex(); };
	}