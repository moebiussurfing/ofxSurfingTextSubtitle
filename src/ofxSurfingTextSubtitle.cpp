#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
ofxSurfingTextSubtitle::ofxSurfingTextSubtitle() {
	bGui.set("SUBTITLES", true);
	bGui_List.set("LIST", false);
	bGui_Paragraph.set("PARAGRAPH", true);

	settingsStore.setPaths(path_Global, path_SubtitlerSettings);

#ifndef USE_IM_GUI__SUBTITLES
	bGui_Internal.set("Gui Internal", true);
#endif

	ofAddListener(ofEvents().update, this, &ofxSurfingTextSubtitle::update);
	ofAddListener(ofEvents().keyPressed, this, &ofxSurfingTextSubtitle::keyPressed);
	ofAddListener(ofEvents().windowResized, this, &ofxSurfingTextSubtitle::windowResized);
};

//--------------------------------------------------------------
ofxSurfingTextSubtitle::~ofxSurfingTextSubtitle() {
	//delete dataTextPtr;

	ofRemoveListener(ofEvents().update, this, &ofxSurfingTextSubtitle::update);
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingTextSubtitle::keyPressed);
	ofRemoveListener(ofEvents().windowResized, this, &ofxSurfingTextSubtitle::windowResized);

	exit();
};

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::windowResized(ofResizeEventArgs & args) {
	int w = args.width;
	int h = args.height;
	//initFbo(w, h);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setup() {
	setup("");
	//bModeNoSrt = true;

	//// A. Force default mode
	//if (indexModes != 3) indexModes = 3;
	//if (!bPlayManual) bPlayManual = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setup(string _pathSrt) {
	ofLogNotice(__FUNCTION__) << "path: " << _pathSrt;

	//path_Srt = _pathSrt;

	setupParams();

	ofFile file(ofToDataPath(_pathSrt));
	std::string ext = file.getExtension();
	if (ext == "srt") {
		ofLogNotice(__FUNCTION__) << " Extension is .srt" << _pathSrt;
		setupSubs(_pathSrt);
	}
	//TODO: fix text file format txt
	else if (ext == "txt") {
		ofLogNotice(__FUNCTION__) << " Extension is " << _pathSrt;
		//setupTextBlocks(_pathSrt);
		//processOpenFileTextSelection(file);
		if (file.exists()) {
			ofLogNotice("ofxSurfingTextSubtitle") << "The file exists - now checking the type via file extension";
			string fileExtension = ofToUpper(file.getExtension());

			////We only want
			//if (fileExtension == "TXT") {
			//	//load text
				string s = ofxSurfingHelpers::loadFileText(_pathSrt);
				doBuildDataTextBlocks(s);

			//	//workflow
			//	//bPlayStandalone = true;
			//	//bPlayForced = true;

				path_Text = _pathSrt;
				ofFile file(ofToDataPath(_pathSrt));
				name_Text = file.getBaseName();

				bLoadedFileText = true;
				path_Srt = "";
				name_Srt = "";
			//}
		}
	} else {
		ofLogWarning(__FUNCTION__) << " Extension is unknown. Requires .srt or .txt: " << _pathSrt;
	}

	//--

#ifdef USE_WIDGET__SUBTITLES
	boxInfo.setFontSize(12);
	boxInfo.setMode(ofxSurfingBoxHelpText::TOP_RIGHT);
	boxInfo.setup();
#endif

	box.setRectConstraintMin(glm::vec2(200, 200));
	box.setBorderColor(ofColor(bTheme ? colorDebugLight : colorDebugDark, 64));

	box.setName("BoxContainer");
	box.setPath(path_Global);
	box.setup();
	box.setType(BOX_TYPE::TYPE_BAR_VERTICAL); //force
	//box.setType(BOX_TYPE::TYPE_RECTANGLE);//force

#ifdef USING_OFX_TIME_CODE
	timecode.setFPS(fps);
#endif

	//--

	// Internal Gui

#ifdef USE_OFX_GUI__SUBTITLES
	if (bGui_InternalAllowed) {
		ofxSurfingHelpers::setThemeDarkMini_ofxGui();
		gui.setup(params);
		////gui.setup("TEXT SUBTITLE");
		//gui.setup(params.getName());
		//gui.add(params);

		//gui.minimize();
		gui.getGroup(params_Control.getName()).minimize();
		auto & guit = gui.getGroup(params_Transport.getName());
		guit.minimize();
		guit.getGroup(params_External.getName()).minimize();
		guit.getGroup(params_Forced.getName()).minimize();
		guit.getGroup(params_Standalone.getName()).minimize();
		auto & guif = gui.getGroup(params_Fade.getName());
		guif.minimize();
		guif.getGroup(params_FadeIn.getName()).minimize();
		guif.getGroup(params_FadeOut.getName()).minimize();
		gui.getGroup(params_Style.getName()).minimize();
	}
#endif

	//--

	//	//crash
	//#ifdef USE_IM_GUI__SUBTITLES
	//	bMinimize.makeReferenceTo(ui->bMinimize);
	//#endif

	bDoneSetup = true;

	//--

	//startup();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::loadFont(string path) {
	ofLogNotice("ofxSurfingTextSubtitle") << ("loadFont: ") << " " << path;

	if (fPath.get() != path) fPath.setWithoutEventNotifications(path);

	bool b = font.setup(fPath, 1.0, _textDimension, _mipmaps, 8, _dpiScale);
	if (!b) ofLogError("ofxSurfingTextSubtitle") << "Font file not found: " << fPath;

	if (b) {
		auto ss = ofSplitString(fPath, "/");
		if (ss.size() >= 2) fName = ss[1];
	}
}

#ifdef USE_IM_GUI__SUBTITLES
//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setUiPtr(ofxSurfingGui * _ui) {
	if (!bDoneSetup) {
		bDoneSetup = true;
		setup();
	}

	//--

	ui = _ui;

	#ifdef USE_PRESETS__SUBTITLES
	//startup
	presets.bGui = false;

	presets.setUiPtr(_ui);
	presets.setPathGlobal("ofxSurfingTextSubtitle");
	presets.setPath("ofxSurfingTextSubtitle");
	presets.AddGroup(params_Preset);

		////TODO:
		////trick fix to example-SubtitleWhisper app
		//plistener = presets.index.newListener([this](int i) {
		//	{
		//		ui->ClearLogDefaultTags();
		//		ofColor c = ofColor(getColorText(), 255);
		//		ui->AddLogTag(c);
		//	}
		//	});
	#endif

	bMinimize.makeReferenceTo(ui->bMinimize);
}
#endif

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupFont() {
	ofLogNotice(__FUNCTION__);

	fName = "-1";
	//fName = "ExtendedBold";

	// search some fonts alternatives
	string _fPath = "fonts/GTAmerica-ExtendedBlack.ttf";
	if (!ofFile::doesFileExist(_fPath)) {
		_fPath = FONT_FILES_PATH + ofToString(FONT_FILE_BIG);
		if (!ofFile::doesFileExist(_fPath)) {
			_fPath = FONT_FILES_PATH + ofToString(FONT_FILE_SMALL);
			if (!ofFile::doesFileExist(_fPath)) {
				_fPath = FONT_FILES_PATH + ofToString(FONT_FILE_ALT1);
				if (!ofFile::doesFileExist(_fPath)) {
					_fPath = FONT_FILES_PATH + ofToString(FONT_FILE_ALT2);
					if (!ofFile::doesFileExist(_fPath)) {
						_fPath = OF_TTF_SANS;
					}
				}
			}
		}
	}

	loadFont(_fPath);

	//bool b = font.setup(fPath, 1.0, _textDimension, _mipmaps, 8, _dpiScale);
	//if (!b) ofLogError("ofxSurfingTextSubtitle") << "Font file not found: " << fPath;

	//if (b) {
	//	auto ss = ofSplitString(fPath, "/");
	//	if (ss.size() >= 2) fName = ss[1];
	//}
}

//void ofxSurfingTextSubtitle::initFbo() {
//	int w = ofGetWidth();
//	int h = ofGetHeight();
//	ofLogNotice(__FUNCTION__) << w << "," << h;
//
//	ofFboSettings fboSettings; // Declare an instance of FBO settings
//	fboSettings.width = w;
//	fboSettings.height = h;
//	fboSettings.internalformat = GL_RGBA;
//	fboSettings.useDepth = true;
//	fboSettings.useStencil = true;
//	fbo.allocate(fboSettings);
//}
//
////--------------------------------------------------------------
//void ofxSurfingTextSubtitle::initFbo(int w, int h) {
//	ofLogNotice(__FUNCTION__) << w << "," << h;
//
//	ofFboSettings fboSettings; // Declare an instance of FBO settings
//	fboSettings.width = w;
//	fboSettings.height = h;
//	fboSettings.internalformat = GL_RGBA;
//	fboSettings.useDepth = true;
//	fboSettings.useStencil = true;
//	fbo.allocate(fboSettings);
//}

void ofxSurfingTextSubtitle::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::update() {

	if (!bDoneSetup) {
		bDoneSetup = true;
		setup();
	}

	//--

	//TODO:
	// Delayed startup to avoid crashes
	if (!bDoneStartup) startup();
	//if (ofGetFrameNum() == 0) startup();
	//if (ofGetFrameNum() == 1) startup();

	//--

	//	//TODO:
	//	//trick improve to example-SubtitleWhisper app
	//	//set the text color for the log..
	//#ifdef USE_PRESETS__SUBTITLES
	//	if (presets.isChangedIndex())
	//	{
	//		ui->ClearLogDefaultTags();
	//		ofColor c = ofColor(getColorText(), 255);
	//		ui->AddLogTag(c);
	//	};
	//#endif

	//if (!bDraw) return;

	//--

	updateEngine();

	updateDebug();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::keyPressed(ofKeyEventArgs & eventArgs) {
	if (!bKeys) return;

	const int & key = eventArgs.key;
	keyPressed(key);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::keyPressed(int key) {
	if (!bKeys) return;

	ofLogNotice("ofxSurfingTextSubtitle") << "keyPressed:" << char(key);

#ifdef USE_IM_GUI__SUBTITLES
	if (ui->bOverInputText.get()) return;
#endif

	if (key == 'g') {
		setToggleVisibleGui();
	}
	if (key == 'l') {
		setToggleLive();
	}
	if (key == 'e') {
		setToggleEdit();
	}
	if (key == 'd') {
		setToggleDebug();
	}
	if (key == '.') {
		stop();
	}
	if (key == OF_KEY_TAB) {
		setToggleAlign();
	}

	// Browse subs
	if (key == OF_KEY_RIGHT) {
		setSubtitleNext();
	}
	if (key == OF_KEY_LEFT) {
		setSubtitlePrevious();
	}
	if (key == OF_KEY_BACKSPACE) {
		setSubtitleRandomIndex();
	};

	//if (key == 'A' || key == 'a') doStartAnimStarting();
	//if (key == 'A' || key == 'a') doStartAnimEnding();

	// Play both!
	//if (key == OF_KEY_RETURN) { setTogglePlayForced(); }
	if (key == ' ') {
		setTogglePlay();
	}
}

//--

//--------------------------------------------------------------
