#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
ofxSurfingTextSubtitle::ofxSurfingTextSubtitle() {
	bGui.set("SUBTITLES", true);
	bGui_List.set("LIST", false);
	bGui_Paragraph.set("PARAGRAPH", true);

	bUseFbo.set("Fbo", true);

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
	initFbo(w, h);
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

	//path_Srt = _pathSrt;

	setupParams();

	setupSubs(_pathSrt);

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

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.setup();

	// link both players

	listeners.push(player.bLoaded.newListener([&](bool & b) {
		if (b && bLoadBothVideoAndSubs) {
			string s = player.path_Video.get();
			auto ss = ofSplitString(s, ".");
			string strunk = ss[0];
			string path = strunk + ".srt";
			if (ofFile::doesFileExist(path)) {
				ofLogNotice("ofxSurfingTextSubtitle") << path << " found. Trying to load it.";
				setupSubs(path);
			} else {
				ofLogError("ofxSurfingTextSubtitle") << "Not found an .srt file with same name that " << s;
			}
		}
	}));

	// create listeners for the buttons

	listeners.push(player.playback.play.newListener([&](bool & b) {
		ofLogNotice("ofxSurfingTextSubtitle") << "Play Paused pressed. Playing " << std::boolalpha << b << "\n";
		if (b)
			play();
		else
			stop();
	}));

	listeners.push(player.playback.stop.newListener([&]() {
		ofLogNotice("ofxSurfingTextSubtitle") << "Stop pressed\n";
		stop();
	}));
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

	// Debugger
	#ifdef SURFING_IMGUI__USE_PROFILE_DEBUGGER
	T_CPU_SETUP_PTR(3);
	T_GPU_SETUP_PTR(5);
	#endif

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

void ofxSurfingTextSubtitle::setupSubs(string _pathSrt) {
	path_Srt = _pathSrt;

	//ofFile file(path_Srt);
	ofFile file(ofToDataPath(path_Srt));

	name_Srt = file.getBaseName();

	ofLogNotice(__FUNCTION__) << " " << path_Srt;

	//bLoaded = ofFile::doesFileExist(path_Srt);
	//if (!bLoaded) {
	//	ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << path_Srt;
	//}

	subParserFactory = new SubtitleParserFactory(ofToDataPath(path_Srt));

	//bGui_List.setName(path_Srt);

	parser = subParserFactory->getParser();
	sub = parser->getSubtitles();
	if (sub.size() == 0) {
		ofLogError("ofxSurfingTextSubtitle") << "SUB object empty!";
	}

	//--

	if (!subParserFactory || sub.size() == 0) {
		bLoadedFileSubs = false;
		ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << path_Srt;
	} else {
		bLoadedFileSubs = true;
		ofLogNotice("ofxSurfingTextSubtitle") << "Successfully loaded .srt file: " << path_Srt;
	}

	if (!bLoadedFileSubs) {
		ofLogError("ofxSurfingTextSubtitle") << "Incomplete initialization!";
		return;
	}

	//--

	currentDialog.setMax(sub.size() - 1);

	// pre read all subs on a vector
	buildDataSubs();

	//TODO: not sure if .srt file is loaded before the first frame!

	//--

	// duration taken from last subtitle end.
	// on external model could be get from the video duration!
	// get duration from srt subs if is not passed by the user,
	// by passing the real video film duration!
	if (tEndSubsFilm == 0) tEndSubsFilm = sub.back()->getEndTime();

	//TODO:
	//indexModes = indexModes.get();

	//workflow
	if (bModeTextBlocks) bModeTextBlocks = false;
	path_Text = "";
	name_Text = "";
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::buildDataSubs() {
	ofLogNotice(__FUNCTION__) << " " << path_Srt;

	//not used yet
	int i = 0;
	dataTextSubs.clear();
	for (SubtitleItem * element : sub) {
		string s = element->getDialogue();

		//TODO:
		//if (bCapitalize) s = ofToUpper(s);

		ofLogNotice("ofxSurfingTextSubtitle") << i++ << " " << s;
		dataTextSubs.push_back(s);
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::initFbo() {
	int w = ofGetWidth();
	int h = ofGetHeight();
	ofLogNotice(__FUNCTION__) << w << "," << h;

	ofFboSettings fboSettings; // Declare an instance of FBO settings
	fboSettings.width = w;
	fboSettings.height = h;
	fboSettings.internalformat = GL_RGBA;
	fboSettings.useDepth = true;
	fboSettings.useStencil = true;
	fbo.allocate(fboSettings);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::initFbo(int w, int h) {
	ofLogNotice(__FUNCTION__) << w << "," << h;

	ofFboSettings fboSettings; // Declare an instance of FBO settings
	fboSettings.width = w;
	fboSettings.height = h;
	fboSettings.internalformat = GL_RGBA;
	fboSettings.useDepth = true;
	fboSettings.useStencil = true;
	fbo.allocate(fboSettings);
}

void ofxSurfingTextSubtitle::updateFades() {
	//TODO:
	// Fades are disabled
	if (!bAnimatedFades || (!bAnimatedIn && !bAnimatedOut)) {
		alpha = 1.f;
		progressIn = 1;
		progressOut = 1;
		isAnimIn = false;
		isAnimOut = false;

		return;
	}

	//--

	// Calculate Fade In

	//if (bPlayStandalone || bPlayForced || bPlayExternal)
	{
		if (bAnimatedFades && bAnimatedIn && isAnimIn) {
			// make opacity grow
			if (alpha < 1.f) {
				alpha += dtAnimIn;
				//cout << alpha << endl;

				//--

				/*
				uint64_t tSlide;
				if (bPlayForced) tSlide = ofGetElapsedTimeMillis() - tPlayForce;
				else if (bPlayStandalone || bPlayExternal) tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;
				*/

				//TODO:
				if (bPlayForced) {
					progressIn = ofClamp(alpha, 0, 1); //workaround to avoid deep calculations..
					//progressIn = ofMap(tSlide, 0, durationPlayForced, 0, 1, true);
				} else if (bPlayStandalone || bPlayExternal || bPlayManual) {
					progressIn = ofClamp(alpha, 0, 1);
					//progressIn = ofMap(tSlide, 0, durationPlaySlide, 0, 1, true);
				}
			}
			// stop if finished
			if (alpha >= 1.f) {
				alpha = 1;
				isAnimIn = false;
				progressIn = ofClamp(alpha, 0, 1);

				//isSlidePlaying = false;
			}
		} else {
			progressIn = 1;
			//isSlidePlaying = false;
		}
	}

	//--

	// Calculate Fade Out

	if (bAnimatedFades && bAnimatedOut) {
		//if (bPlayStandalone || bPlayForced || bPlayExternal)
		{
			uint64_t tSlide = 0;

			if (bPlayForced)
				tSlide = ofGetElapsedTimeMillis() - tPlayForce;
			else if (bPlayStandalone || bPlayExternal || bPlayManual)
				tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;

			if (!isAnimOut) // checking if requires trig fade out
			{
				// trigs fade out start

				if (bPlayForced) {
					if (tSlide > durationPlayForced - durationOut) {
						isAnimOut = true;
						if (!bStartingFadeOutNotified) {
							bStartingFadeOutNotified = true;
							notifySlideStartingFadeOut();
						}
						isSlidePlaying = true;
					}
				} else if (bPlayStandalone || bPlayExternal || bPlayManual) {
					if (tSlide > durationPlaySlide - durationOut) {
						isAnimOut = true;
						if (!bStartingFadeOutNotified) {
							bStartingFadeOutNotified = true;
							notifySlideStartingFadeOut();
						}
						isSlidePlaying = true;
					}
				}
				progressOut = 1;
			}

			//else // fading out
			if (isAnimOut) // fading out
			{
				uint64_t t = tSlide - durationOut;

				if (bPlayForced) {
					progressOut = ofClamp(alpha, 0, 1);
					//progressOut = ofMap(t, 0, durationOut, 1, 0, true);
				} else if (bPlayStandalone || bPlayExternal || bPlayManual) {
					progressOut = ofClamp(alpha, 0, 1);
					//progressOut = ofMap(t, 0, durationOut, 1, 0, true);
				}
				//else progressOut = 1;

				if (alpha > 0.f) //decrease opacity
				{
					alpha -= dtAnimOut;
				}
				if (alpha <= 0.f) //finished
				{
					alpha = 0.f;
					isAnimOut = false;
					//progressOut = 1;//?

					isSlidePlaying = false;
				}
			}
		}
	} else {
		progressOut = 1;
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::update() {
#ifdef SURFING_IMGUI__USE_PROFILE_DEBUGGER
	T_CPU_START_PTR(0, "update");
#endif

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

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.update();
#endif

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

	//--

#ifdef SURFING_IMGUI__USE_PROFILE_DEBUGGER
	T_CPU_END_PTR(0);
#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updateEngine() {
#ifdef SURFING_IMGUI__USE_PROFILE_DEBUGGER
	T_CPU_START_PTR(1, "Engine");
#endif

	//--

	if (box.isChanged()) {
		ofLogNotice("ofxSurfingTextSubtitle") << "Box container changed";
		initFbo();
		doRefreshDraw();
	}

	//--

	updateFades();

	//--

	// Calculate progress

	uint64_t t = 0;

	if (indexModes == 0) // EXTERNAL
	{
		//if (bPlayExternal) t = positionExternal * (float)tEndSubsFilm;
		t = positionExternal * (float)tEndSubsFilm;
	} else if (indexModes == 1) // STANDALONE
	{
		//if (bPlayStandalone) t = ofGetElapsedTimeMillis() - tPlay;
		t = ofGetElapsedTimeMillis() - tPlay;
	} else if (indexModes == 2) // FORCED
	{
	} else if (indexModes == 3) // MANUAL
	{
		////if (bPlayExternal) t = positionExternal * (float)tEndSubsFilm;
		//t = positionExternal * (float)tEndSubsFilm;
	}

	//--

	// 1. Mode text blocks

	if (bModeTextBlocks) {
		////TODO:
		//tPlayStartSlide = ofGetElapsedTimeMillis();
		//isSlidePlaying = true;
	} else {
		// 2. Mode Srt

		if (sub.size() > 0) {
			// Fix. Pre start.
			// Waiting first dialog in.
			// clear text before first dialog
			if (t < sub[0]->getStartTime()) {
				textCurrent = "";
				isPrecoutingStart = true;
			} else {
				isPrecoutingStart = false;

				// find the dialog relative to current time
				int k = 0;
				for (SubtitleItem * element : sub) {
					if (t > element->getStartTime() && t <= element->getEndTime()) {
						// To apply only once if currentDialog changed!
						if (currentDialog.get() != k) {
							// Changed
							currentDialog = k; // will trig the callback!
							doSetSrtSlideStart(element);
						}
						break; //done found
					}
					k++;
				}
			}
		}
	}

	//--

	if (indexModes == 0 || indexModes == 1) // external or standalone
	{
		if (isSlidePlaying && (bPlayStandalone || bPlayExternal)) {
			uint64_t tp = ofGetElapsedTimeMillis() - tPlayStartSlide;
			progressPlaySlide = ofMap(tp, 0, durationPlaySlide, 0, 1, true);
		} else {
			progressPlaySlide = 0;
		}
	}

	//--

	if (indexModes == 0) // EXTERNAL
	{
		if (bPlayExternal) {
		}
	} else if (indexModes == 1) // STANDALONE
	{
		//if(bPlayStandalone) {
		//	positionExternal = ofMap(ofGetElapsedTimeMillis() - tPlay, 0, tEndSubsFilm, 0, 1, false);
		//}
	} else if (indexModes == 2) // FORCED
	{
		if (bPlayForced) {
			uint64_t tf = ofGetElapsedTimeMillis() - tPlayForce;
			if (tf > durationPlayForced) {
				tPlayForce = ofGetElapsedTimeMillis(); //restart timer. next slide
				currentDialog++;
			}
			progressPlaySlide = ofMap(tf, 0, durationPlayForced, 0, 1, true);

			//positionExternal = ofMap(ofGetElapsedTimeMillis() - tPlayForceFilm, 0, tEndSubsFilm, 0, 1, false);
		}
	} else if (indexModes == 3) // MANUAL
	{
		if (bPlayManual) {
			if (isSlidePlaying) {
				uint64_t tp = ofGetElapsedTimeMillis() - tPlayStartSlide;
				progressPlaySlide = ofMap(tp, 0, durationPlaySlide, 0, 1, true);
			} else {
				progressPlaySlide = 0;
			}
		}
	}
#ifdef SURFING_IMGUI__USE_PROFILE_DEBUGGER
	T_CPU_END_PTR(1);
#endif
}

void ofxSurfingTextSubtitle::doSetSrtSlideStart(SubtitleItem * element) {

	tPlayStartSlide = ofGetElapsedTimeMillis();
	isSlidePlaying = true;
	auto ts = element->getStartTime();
	auto te = element->getEndTime();
	durationPlaySlide = te - ts;

	doRefreshDraw();
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
void ofxSurfingTextSubtitle::doOpenFileSrt() {
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a .srt file.");

	//Check if the user opened a file
	if (openFileResult.bSuccess) {

		ofLogNotice("ofxSurfingTextSubtitle") << "User selected a file";

		//We have a file, check it and process it
		processOpenFileSrtSelection(openFileResult);
	} else {
		ofLogNotice("ofxSurfingTextSubtitle") << "User hit cancel";
	}

	//TODO:
	//refresh
	indexModes = indexModes.get();

	////workflow
	//if (bModeNoSrt) bModeNoSrt = false;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::processOpenFileSrtSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("ofxSurfingTextSubtitle") << "Name: " + openFileResult.getName();
	ofLogNotice("ofxSurfingTextSubtitle") << "Path: " + openFileResult.getPath();

	string path = openFileResult.getPath();
	ofFile file(openFileResult.getPath());

	if (file.exists()) {

		ofLogNotice("ofxSurfingTextSubtitle") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		//We only want
		if (fileExtension == "SRT") {
			//load subs
			setupSubs(path);

			//workflow
			//bPlayStandalone = true;
			//bPlayForced = true;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doOpenFileText() {
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a .txt file.");

	//Check if the user opened a file
	if (openFileResult.bSuccess) {

		ofLogNotice("ofxSurfingTextSubtitle") << "User selected a file";

		//We have a file, check it and process it
		processOpenFileTextSelection(openFileResult);
	} else {
		ofLogNotice("ofxSurfingTextSubtitle") << "User hit cancel";
	}

	//TODO:
	//refresh
	indexModes = indexModes.get();

	////workflow
	//if (bModeNoSrt) bModeNoSrt = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::processOpenFileTextSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("ofxSurfingTextSubtitle") << "Name: " + openFileResult.getName();
	ofLogNotice("ofxSurfingTextSubtitle") << "Path: " + openFileResult.getPath();

	string path = openFileResult.getPath();
	ofFile file(openFileResult.getPath());

	if (file.exists()) {
		ofLogNotice("ofxSurfingTextSubtitle") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		//We only want
		if (fileExtension == "TXT") {
			//load text
			string s = ofxSurfingHelpers::loadFileText(path);
			doBuildDataTextBlocks(s);

			//workflow
			//bPlayStandalone = true;
			//bPlayForced = true;

			path_Text = path;
			ofFile file(ofToDataPath(path));
			name_Text = file.getBaseName();

			bLoadedFileText = true;
			path_Srt = "";
			name_Srt = "";
		}
	} else {
		ofLogError("ofxSurfingTextSubtitle") << "Not valid file found";
		bLoadedFileText = false;
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupTextBlocks(string path) {

	ofLogNotice("ofxSurfingTextSubtitle:setupTextBlocks") << "path: " + path;

	ofFile file(path);

	if (file.exists()) {
		ofLogNotice("ofxSurfingTextSubtitle:setupTextBlocks") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		// We only want
		if (fileExtension == "TXT") {
			// load text
			string s = ofxSurfingHelpers::loadFileText(path);
			doBuildDataTextBlocks(s);

			path_Text = path;
			ofFile file(ofToDataPath(path));
			name_Text = file.getBaseName();

			bModeTextBlocks = true;

			// workflow
			//bPlayStandalone = true;
			//bPlayForced = true;
			currentDialog = 0;

			bLoadedFileText = true;
			path_Srt = "";
			name_Srt = "";
		}
	} else {
		ofLogError("ofxSurfingTextSubtitle") << "Not valid file found";
		bLoadedFileText = false;
	}
}
