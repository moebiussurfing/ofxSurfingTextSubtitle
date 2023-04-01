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
	string s = "example-Subs_ImGui " + ofToString(ofGetFrameRate(), 0) + " Fps";
	ofSetWindowTitle(s);

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
		if (ui.BeginWindow("ofApp")) {
			ui.Add(subs.bGui, OFX_IM_TOGGLE_BUTTON_ROUNDED);

			if (ui.AddButton("Do Text!"))
			{
				string s;
				static bool b = false;
				if (b) {
					b = !b;
					s = load("files/text3.txt");
					//s = load("files/text1.txt");
				}
				else {
					b = !b;
					s = load("files/text1.txt");
					//s = load("files/text2.txt");
				}

				subs.doSetTextSlide(s);
			}

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

//--------------------------------------------------------------
string ofApp::load(string path)
{
	string p = ofToDataPath(path, true);
	//string p = ofFilePath::getAbsolutePath(path);

	string text = "";

	char* fileToEdit;
	fileToEdit = (char*)(p.c_str());

	//-

	ofLogNotice("ofApp") << "load ifstream fileToEdit: " << ofToString(fileToEdit);

	std::ifstream t(fileToEdit);
	if (t.good())
	{
		string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		text = str;

		ofLogNotice("ofApp") << "loaded file: " << ofToString(fileToEdit);
	}
	else {
		ofLogNotice("ofApp") << "file not found! " << ofToString(fileToEdit);
	}

	return text;
}