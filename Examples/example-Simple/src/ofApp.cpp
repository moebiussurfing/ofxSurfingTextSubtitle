#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	// Optional
	string path = "files/srt/Huxley.srt"; // Srt file path
	subs.setup(path);

	subs.setUiPtr(&ui);
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	ofClear(subs.getColorBg());
	subs.draw();
	drawGui();
}

//--------------------------------------------------------------
void ofApp::drawGui()
{
	//if (!subs.isVisibleGui()) return;

	ui.Begin();
	{
		if (ui.BeginWindow("ofApp"))
		{
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			ui.EndWindow();
		}

		subs.drawImGui();
	}
	ui.End();

	subs.drawGui();
}
