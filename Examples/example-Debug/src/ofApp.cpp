#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofxSurfingHelpers::setMonitorsLayout(1, true, true);

	// Optional
	path = "files/srt/Huxley.srt"; // Srt file path
	subs.setup(path);

	subs.setUiPtr(&ui);
}

//--------------------------------------------------------------
void ofApp::update() {
	string name = "example-Subs_ImGui";
	ofxSurfingHelpers::setWindowTitleDebugPerformance(name, true);

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

	//TODO:
	// imageBg
	// to test how fbo's alpha behave
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
void ofApp::drawGui()
{
	if (!subs.isVisibleGui()) return;

	subs.drawGui();

	ui.Begin();
	{
		IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL;

		if (ui.BeginWindow("ofApp"))
		{
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			ui.AddSpacingSeparated();

			//--

			/*
			ui.AddLabelBig("Theme");
			ofxImGuiSurfing::AddToggleRoundedButtonNamed(ui.bThemeUiAlt, "Day", "Night");

			string s;
			if (!ui.bThemeUiAlt) s = THEME_NAME_NIGHT;
			else s = THEME_NAME_DAY;
			ui.AddLabel(s);

			ui.AddSpacingSeparated();
			*/

			//--

			static bool bTester = false;
			ui.AddToggle("TESTER", bTester);

			// Tester
			if (bTester)
			{
				ui.BeginChild(ImVec2{ -1, 200 });

				ui.AddLabelBig("TEST", 1, 1);
				ui.AddSpacing();

				ImVec2 sz{ ui.getWidgetsWidth(2), 2 * ui.getWidgetsHeightUnit() };
				ui.AddToggle("DoAuto", bAuto, sz);
				ui.AddTooltip("Automated timer.");
				ui.SameLine();

				if (ui.AddButton("DoText", sz))
				{
					doLoadText();
				}
				ui.AddTooltip("Populate a random text block.");

				if (bAuto)
				{
					ImGui::PushItemWidth(100);
					ImGui::SliderInt("T", &d, 1, 10);
					ImGui::PopItemWidth();
				}
				if (ui.AddButton("DoBlocks", sz))
				{
					doLoadTextBlocks();
				}
				ui.AddTooltip("Populate a big text separated in many blocks.");

				ui.EndChild();
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
	if (key == 'g')
	{
#ifdef USE_PRESETS
		presets.setVisibleGui(subs.getVisibleGui());
#endif
	}

}