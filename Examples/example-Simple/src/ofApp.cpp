#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofxSurfingHelpers::moveWindowToMyLeftMonitor();
	ofxSurfingHelpers::moveWindowToMyRightMonitor();

	// Set the initial SRT file path. 
	// But you can load other SRT or TXT files on runtime.
	string path = "files/srt/Huxley.srt";
	subs.setup(path);

	subs.setUiPtr(&ui); // ui must be instantiated in ofApp. 
	// Not internally handled on the addon yet.
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofClear(subs.getColorBg()); // the addon handles a Bg color ready to use.

	subs.draw(); // draws the text

	drawGui(); // gui
}

//--------------------------------------------------------------
void ofApp::drawGui()
{
	ui.Begin();
	{
		if (ui.BeginWindow("ofApp"))
		{
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED_MEDIUM);
			ui.DrawWidgetsGlobalScaleMini();
			ui.EndWindow();
		}

		subs.drawImGui();
	}
	ui.End();

	// gui for debug, container or hud 
	subs.drawGui();
}
