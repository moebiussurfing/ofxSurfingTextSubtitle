#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
ofxSurfingTextSubtitle::ofxSurfingTextSubtitle() {
	bGui.set("SUBTITLES", true);
	bGui_List.set("LIST", false);
	bGui_Paragraph.set("PARAGRAPH", true);

#ifndef USE_IM_GUI__SUBTITLES
	bGui_Internal.set("Gui Internal", true);
#endif
};

//--------------------------------------------------------------
ofxSurfingTextSubtitle::~ofxSurfingTextSubtitle() {
	exit();
};

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setup() {
	setup("");
	bModeNoSrt = true;

	// A.force mode
	if (indexModes != 3) indexModes = 3;
	if (!bPlayManual) bPlayManual = true;
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

	box.setup();

#ifdef USING_OFX_TIME_CODE
	timecode.setFPS(fps);
#endif

	//--

	// Internal Gui

#ifndef USE_IM_GUI__SUBTITLES
	if (bGui_InternalAllowed)
	{
		ofxSurfingHelpers::setThemeDarkMini_ofxGui();
		gui.setup(params);
		////gui.setup("TEXT SUBTITLE");
		//gui.setup(params.getName());
		//gui.add(params);

			//gui.minimize();
		gui.getGroup(params_Control.getName()).minimize();
		auto& guit = gui.getGroup(params_Transport.getName());
		guit.minimize();
		guit.getGroup(params_External.getName()).minimize();
		guit.getGroup(params_Forced.getName()).minimize();
		guit.getGroup(params_Standalone.getName()).minimize();
		auto& guif = gui.getGroup(params_Fade.getName());
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

	listeners.push(player.bLoaded.newListener([&](bool& b) {
		if (b && bLoadBothVideoAndSubs)
		{
			string s = player.path_Video.get();
			auto ss = ofSplitString(s, ".");
			string strunk = ss[0];
			string path = strunk + ".srt";
			if (ofFile::doesFileExist(path)) {
				ofLogNotice("ofxSurfingTextSubtitle") << path << " found. Trying to load it.";
				setupSubs(path);
			}
			else {
				ofLogError("ofxSurfingTextSubtitle") << "Not found an .srt file with same name that " << s;
			}
		}

		}));

	// create listeners for the buttons

	listeners.push(player.playback.play.newListener([&](bool& b) {
		ofLogNotice("ofxSurfingTextSubtitle") << "Play Paused pressed. Playing " << std::boolalpha << b << "\n";
	if (b) play();
	else stop();
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

	//--

	//startup();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::loadFont(string path)
{
	ofLogNotice("ofxSurfingTextSubtitle") << ("loadFont: ") << " " << path;

	if (fPath.get() != path) fPath.setWithoutEventNotifications(path);

	bool b = font.setup(fPath, 1.0, _textDimension, _mipmaps, 8, _dpiScale);
	if (!b) ofLogError("ofxSurfingTextSubtitle") << "Font file not found: " << fPath;

	if (b)
	{
		auto ss = ofSplitString(fPath, "/");
		if (ss.size() >= 2) fName = ss[1];
	}
}

#ifdef USE_IM_GUI__SUBTITLES
//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setUiPtr(ofxSurfingGui* _ui)
{
	ui = _ui;

#ifdef USE_PRESETS__SUBTITLES
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
}
#endif

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupFont()
{
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

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupParams()
{
	ofLogNotice(__FUNCTION__);

	// force load default font
	setupFont();

	//--

	// Control
	bStop.set("Stop");
	bOpen.set("Open");
	bDraw.set("DRAW", true);
	bLive.set("LIVE!", false);
	bEdit.set("Edit", true);
	bDebug.set("Debug", true);
	bTop.set("Top Timeline", true);
	bLeft.set("Left Alpha", false);
#ifdef USE_WIDGET__SUBTITLES
	bDrawWidgetInfo.set("Info", true);
#endif
	currentDialog.set("Dialog", 0, 0, 0);
	bPlayStandalone.set("Play Standalone", false);
	bPlayForced.set("Play Forced", false);
	durationPlayForced.set("Duration Forced", 2000, 100, 4000);
	bPrev.set("<", false);
	bNext.set(">", false);
	//speedPlayForce.set("Speed", 0, 0, 1);
	bPlayExternal.set("Play External", false);
	positionExternal.set("Position", 0, 0, 1);
	bPlayManual.set("Play Manual", false);

	progressPlayFilm.set("% Film", 0, 0, 1);//full length progress
	progressPlaySlide.set("% Slide", 0, 0, 1);//slide progress

	bAutoScroll.set("Auto Scroll", true);//for ImGui only

	// Fade
	bAnimatedIn.set("In", false);
	bAnimatedOut.set("Out", false);
	durationIn.set("Duration In", 1000, 1, 1000);
	durationOut.set("Duration Out", 1000, 1, 1000);
	progressIn.set("% In", 0, 0, 1);
	progressOut.set("% Out", 0, 0, 1);
	bResetFades.set("Reset Fades", false);

	// Style
	fSize.set("SizeR", 50, 5, (float)MAX_FONT_SIZE);
	fSizePrc.set("Size", 0.2, 0.1, 1.0f);
	fSpacing.set("Spacing", 0, -20, 50);
	fLineHeight.set("Height", 0.75, 0.5, 2.0);
	colorTextFloat.set("Color", ofFloatColor::white, ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	fColorShadow.set("ColorSw", ofFloatColor::black, ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	offsetShadow.set("Offset", glm::vec2(0, 0), glm::vec2(-100, -100), glm::vec2(100, 100));
	colorBgFloat.set("ColorBg", ofFloatColor::gray, ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	fAlign.set("Align", 0, 0, 2);
	fAlign_str.set("Align ", "-1");
	bReset.set("Reset", false);

	bCenteredV.set("y Centered", true);
	amountLinesTarget.set("Lines", 6, 1, 10);

	//bMinimize.set("Minimize", false);
	bMinimize.makeReferenceTo(ui->bMinimize);

	bCapitalize.set("Capitalize", false);
	bResponsive.set("Responsive", false);
	resizeResponsive.set("Resize", 0, 0, 1);

	indexModes.set("Modes", 0, 0, 3);

	indexModes_Name.set("Name", "");
	indexModes_Name.setSerializable(false);

	box.bGui.makeReferenceTo(bEdit);
	//box.bEdit.makeReferenceTo(bEdit);
//#ifdef USE_WIDGET__SUBTITLES
//	boxInfo.bGui.makeReferenceTo(bEdit);
//#endif

	//--

	//TODO: crashes when loading settings if uncommented!
	//bPlayStandalone.setSerializable(false);//!
	//bPlayForced.setSerializable(false);

	fName.setSerializable(false);
	//fPath.setSerializable(false);
	fAlign_str.setSerializable(false);
	bReset.setSerializable(false);
	bResetFades.setSerializable(false);
	bNext.setSerializable(false);
	bPrev.setSerializable(false);
	currentDialog.setSerializable(false);

	progressPlayFilm.setSerializable(false);
	progressPlaySlide.setSerializable(false);

	////we use index mode
	//bPlayStandalone.setSerializable(false);
	//bPlayExternal.setSerializable(false);
	//bPlayForced.setSerializable(false);

	progressIn.setSerializable(false);
	progressOut.setSerializable(false);

	//--

	params_Control.setName("Control");
	params_Control.add(bOpen);
	params_Control.add(bDraw);
	params_Control.add(bLive);
	params_Control.add(bEdit);
	params_Control.add(bDebug);
	params_Control.add(bTheme);
	params_Control.add(bTop);
	params_Control.add(bLeft);
#ifdef USE_WIDGET__SUBTITLES
	params_Control.add(bDrawWidgetInfo);
#endif
#ifdef USE_WIDGET__VIDEO_PLAYER
	params_Control.add(player.bGui_VideoPlayer);
	params_Control.add(bLoadBothVideoAndSubs);
#endif

#ifdef USE_IM_GUI__SUBTITLES
	params_Control.add(bGui_List);
	params_Control.add(bGui_Paragraph);
	params_Control.add(bMinimize);
#endif
	//params_Control.add(bGui);
	//params_Control.add(box.bEdit);

	params_Transport.setName("Transport");
	params_Transport.add(bStop);
	params_Transport.add(currentDialog);
	params_Transport.add(progressPlaySlide);
	params_Transport.add(indexModes);
	params_Transport.add(indexModes_Name);
	params_Standalone.add(bPlayStandalone);
	params_Transport.add(params_Standalone);
	params_Forced.add(bPlayForced);
	params_Forced.add(durationPlayForced);
	params_Forced.add(bPrev);
	params_Forced.add(bNext);
	params_Transport.add(params_Forced);
	params_External.add(bPlayExternal);
	params_External.add(positionExternal);
	params_Transport.add(bPlayManual);
	params_Transport.add(params_External);

	//params_Transport.add(progressPlayFilm);
	//params_Transport.add(speedPlayForce);

	// Fades
	params_Fade.setName("Fades");
	params_Fade.add(bAnimatedIn);
	params_Fade.add(bAnimatedOut);
	params_Fade.add(progressPlaySlide);
	params_FadeIn.setName("F In");
	params_FadeOut.setName("F Out");
	//params_FadeIn.add(speedFadeIn);
	params_FadeIn.add(progressIn);//TODO: should change speed mode to time duration. like on fade out!
	params_FadeIn.add(durationIn);
	//params_FadeOut.add(speedFadeOut);
	params_FadeOut.add(progressOut);
	params_FadeOut.add(durationOut);
	params_Fade.add(params_FadeIn);
	params_Fade.add(params_FadeOut);
	params_Fade.add(bResetFades);
	//params_Transport.add(params_Fade);

	params_Style.setName("Style");
	//params_Style.add(fName);
	params_Style.add(fPath);
	params_Style.add(colorTextFloat);
#ifdef USE_SHADOW
	params_Style.add(fColorShadow);
	params_Style.add(offsetShadow);
#endif
	params_Style.add(colorBgFloat);
	params_Style.add(bCapitalize);
	params_Style.add(fSizePrc);
	params_Style.add(fSpacing);
	params_Style.add(fLineHeight);
	params_Style.add(fAlign);
	params_Style.add(fAlign_str);
	params_Style.add(bCenteredV);
	params_Style.add(amountLinesTarget);
	params_Style.add(bResponsive);
	params_Style.add(resizeResponsive);
#ifdef USE_IM_GUI__SUBTITLES
	params_Style.add(bFine);
#endif
	params_Style.add(bReset);

	//params.setName(bGui.getName());//TODO: BUG: crashes
	params.setName("SUBTITLER");
	params.add(params_Control);
	params.add(params_Transport);
	params.add(params_Fade);
	params.add(params_Style);

	ofAddListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

	//--

	// Preset
	params_Preset.setName("Subtitler");
	params_Preset.add(params_Style);
	params_Preset.add(params_Fade);
	params_Preset.add(bTheme);//extra

	//--

	// App Settings / session
	params_AppSettings.setName("AppSettings");
	params_AppSettings.add(params_Control);
	params_AppSettings.add(params_Transport);
	gt.addGroup(params_AppSettings, path_SubtitlerSettings);
}

//--------------------------------------------------------------
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
		bLoaded = false;
		ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << path_Srt;
	}
	else {
		bLoaded = true;
		ofLogNotice("ofxSurfingTextSubtitle") << "Successfully loaded .srt file: " << path_Srt;
	}

	if (!bLoaded) {
		ofLogError("ofxSurfingTextSubtitle") << "Incomplete initialization!";
		return;
	}

	//--

	currentDialog.setMax(sub.size() - 1);

	// pre read all subs on a vector
	buildSubsData();

	//TODO: not sure if .srt file is loaded before the first frame!

	//--

	// duration taken from last subtitle end.
	// on external model could be get from the video duration!
	// get duration from srt subs if is not passed by the user, 
	// by passing the real video film duration!
	if (tEndSubsFilm == 0) tEndSubsFilm = sub.back()->getEndTime();


	//TODO:
	//indexModes = indexModes.get();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::buildSubsData()
{
	ofLogNotice(__FUNCTION__) << " " << path_Srt;

	//not used yet
	int i = 0;
	subsDataText.clear();
	for (SubtitleItem* element : sub)
	{
		string s = element->getDialogue();
		if (bCapitalize) s = ofToUpper(s);

		ofLogNotice("ofxSurfingTextSubtitle") << i++ << " " << s;
		subsDataText.push_back(s);
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::startup()
{
	ofLogNotice(__FUNCTION__);

//#ifdef USE_IM_GUI__SUBTITLES
//	this->setDisableGuiInternal(true);
//#endif

	//return;
	doReset();

	//TODO:
	//crash
	//return;
#ifdef USE_PRESETS__SUBTITLES
	//ofxSurfingHelpers::loadGroup(params_AppSettings, path_SubtitlerSettings);
#else
	ofxSurfingHelpers::loadGroup(params, path_SubtitlerSettings);
#endif

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.startup();
#endif

	bDoneStartup = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::exit() {
	ofLogNotice("ofxSurfingTextSubtitle") << "exit";
	ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

#ifdef USE_PRESETS__SUBTITLES
	//ofxSurfingHelpers::saveGroup(params_AppSettings, path_SubtitlerSettings);
#else
	ofxSurfingHelpers::saveGroup(params, path_SubtitlerSettings);
#endif

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.exit();
#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setPosition(float position)
{
	if (indexModes == 0) positionExternal.set(position);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updatePosition(float position)
{
	if (indexModes == 0) {
		positionExternal.set(position);
	}

	update();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updateFrame(uint64_t frame)
{
	//TODO: WIP: 
	//not implemented yet
	//external link 

#ifdef USE_TIME_CODE__SUBTITLES
	uint64_t t = timecode.millisForFrame(frame);

	// Force sync mode
	//if (!bPlayStandalone) bPlayStandalone = true;

	if (bPlayStandalone)
	{
		//tPlay = t;

		/*
		uint64_t tDiff = tPlay - t;
		if (tDiff > 0) {
		}
		else {
		}
		*/
	}
	if (bPlayForced)
	{
		//TODO:
		//should calculate which dialog correspond to this time,
		//then jump to that dialog / currentDialog
		//tPlayForce = t;
	}
#endif

	//--

	update();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updateFades()
{
	//TODO:
	// Fades are disabled
	if (!bAnimatedIn && !bAnimatedOut) 
	{
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
		if (bAnimatedIn && isAnimIn)
		{
			// make opacity grow 
			if (alpha < 1.f)
			{
				alpha += dtAnimIn;
				//cout << alpha << endl;

				//--

				/*
				uint64_t tSlide;
				if (bPlayForced) tSlide = ofGetElapsedTimeMillis() - tPlayForce;
				else if (bPlayStandalone || bPlayExternal) tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;
				*/

				//TODO:
				if (bPlayForced)
				{
					progressIn = ofClamp(alpha, 0, 1);//workaround to avoid deep calculations..
					//progressIn = ofMap(tSlide, 0, durationPlayForced, 0, 1, true);
				}
				else if (bPlayStandalone || bPlayExternal || bPlayManual)
				{
					progressIn = ofClamp(alpha, 0, 1);
					//progressIn = ofMap(tSlide, 0, durationPlaySlide, 0, 1, true);
				}
			}
			// stop if finished
			if (alpha >= 1.f)
			{
				alpha = 1;
				isAnimIn = false;
				progressIn = ofClamp(alpha, 0, 1);

				//isSlidePlaying = false;
			}
		}
		else
		{
			progressIn = 1;
			//isSlidePlaying = false;
		}
	}

	//--

	// Calculate Fade Out

	if (bAnimatedOut)
	{
		//if (bPlayStandalone || bPlayForced || bPlayExternal)
		{
			uint64_t tSlide = 0;

			if (bPlayForced) tSlide = ofGetElapsedTimeMillis() - tPlayForce;
			else if (bPlayStandalone || bPlayExternal || bPlayManual)
				tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;

			if (!isAnimOut) // checking if requires trig fade out
			{
				// trigs fade out start

				if (bPlayForced)
				{
					if (tSlide > durationPlayForced - durationOut)
					{
						isAnimOut = true;
						isSlidePlaying = true;
					}
				}
				else if (bPlayStandalone || bPlayExternal || bPlayManual)
				{
					if (tSlide > durationPlaySlide - durationOut)
					{
						isAnimOut = true;
						isSlidePlaying = true;
					}
				}
				progressOut = 1;
			}

			//else // fading out
			if (isAnimOut) // fading out
			{
				uint64_t t = tSlide - durationOut;

				if (bPlayForced)
				{
					progressOut = ofClamp(alpha, 0, 1);
					//progressOut = ofMap(t, 0, durationOut, 1, 0, true);
				}
				else if (bPlayStandalone || bPlayExternal || bPlayManual)
				{
					progressOut = ofClamp(alpha, 0, 1);
					//progressOut = ofMap(t, 0, durationOut, 1, 0, true);
				}
				//else progressOut = 1;

				if (alpha > 0.f)//decrease opacity 
				{
					alpha -= dtAnimOut;
				}
				if (alpha <= 0.f)//finished
				{
					alpha = 0.f;
					isAnimOut = false;
					//progressOut = 1;//?

					isSlidePlaying = false;
				}
			}
		}
	}
	else
	{
		progressOut = 1;
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::update()
{
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

	//TODO:
	//trick fix to example-SubtitleWhisper app
#ifdef USE_PRESETS__SUBTITLES
	if (presets.isChangedIndex())
	{
		ui->ClearLogDefaultTags();
		ofColor c = ofColor(getColorText(), 255);
		ui->AddLogTag(c);
	};
#endif

	//if (!bDraw) return;

	//--

	updateEngine();

	//--

	updateDebug();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updateEngine()
{
	//--

	updateFades();

	//--

	// Calculate progress

	uint64_t t = 0;

	if (indexModes == 0) // external
	{
		//if (bPlayExternal) t = positionExternal * (float)tEndSubsFilm;
		t = positionExternal * (float)tEndSubsFilm;
	}
	else if (indexModes == 1) // standalone
	{
		//if (bPlayStandalone) t = ofGetElapsedTimeMillis() - tPlay;
		t = ofGetElapsedTimeMillis() - tPlay;
	}
	else if (indexModes == 2) // forced
	{
	}
	else if (indexModes == 3) // manual
	{
		////if (bPlayExternal) t = positionExternal * (float)tEndSubsFilm;
		//t = positionExternal * (float)tEndSubsFilm;
	}

	//--

	if (sub.size() > 0)
	{
		// Pre start. 
		// Waiting first dialog in.
		// clear text before first dialog
		if (t < sub[0]->getStartTime())
		{
			textCurrent = "";
			isPrecoutingStart = true;
		}
		else
		{
			isPrecoutingStart = false;

			// find the dialog relative to current time
			int k = 0;
			for (SubtitleItem* element : sub)
			{
				if (t > element->getStartTime() && t <= element->getEndTime())
				{
					// To apply only once if currentDialog changed!
					if (currentDialog.get() != k)
					{
						// changed
						currentDialog = k; // will trig the callback!
						doUpdateSlidePlay(element);
					}
					break;//done found
				}
				k++;
			}
		}
	}

	//--

	if (indexModes == 0 || indexModes == 1) // external or standalone
	{
		if (isSlidePlaying && (bPlayStandalone || bPlayExternal))
		{
			uint64_t tp = ofGetElapsedTimeMillis() - tPlayStartSlide;
			progressPlaySlide = ofMap(tp, 0, durationPlaySlide, 0, 1, true);
		}
		else
		{
			progressPlaySlide = 0;
		}
	}

	//--

	if (indexModes == 0) // external
	{
		if (bPlayExternal)
		{
		}
	}
	else if (indexModes == 1) // standalone
	{
		//if(bPlayStandalone) {
		//	positionExternal = ofMap(ofGetElapsedTimeMillis() - tPlay, 0, tEndSubsFilm, 0, 1, false);
		//}
	}
	else if (indexModes == 2) // forced
	{
		if (bPlayForced)
		{
			uint64_t tf = ofGetElapsedTimeMillis() - tPlayForce;
			if (tf > durationPlayForced)
			{
				tPlayForce = ofGetElapsedTimeMillis();//restart timer. next slide
				currentDialog++;
			}
			progressPlaySlide = ofMap(tf, 0, durationPlayForced, 0, 1, true);

			//positionExternal = ofMap(ofGetElapsedTimeMillis() - tPlayForceFilm, 0, tEndSubsFilm, 0, 1, false);
		}
	}
	else if (indexModes == 3) // manual
	{
		if (bPlayManual)
		{
			if (isSlidePlaying)
			{
				uint64_t tp = ofGetElapsedTimeMillis() - tPlayStartSlide;
				progressPlaySlide = ofMap(tp, 0, durationPlaySlide, 0, 1, true);
			}
			else
			{
				progressPlaySlide = 0;
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updateDebug()
{
	//--

	// Calculate progress

	if (sub.size() == 0)
	{
		//ofLogVerbose("ofxSurfingTextSubtitle") << "SUB object empty!";
		progressPlayFilm = 0;
	}
	else
	{
		// Progress related to the elapsed player time
		if (indexModes == 1 && bPlayStandalone)
		{
			uint64_t t = ofGetElapsedTimeMillis() - tPlay;

			uint64_t ts;
			uint64_t te;

			// workaround:
			// fix this case
			if (currentDialog == 0)
			{
				ts = 0;
				te = sub[0]->getEndTime();
				progressPlaySlide = ofMap(t, ts, te, 0, 1, true);
			}
			else
			{
			}

			progressPlayFilm = ofMap(t, 0, tEndSubsFilm, 0, 1, true);
		}

		//else if (bPlayForced || !bPlayStandalone)
		// for any other modes
		else if (indexModes == 0 || indexModes == 2 || indexModes == 3)
		{
			progressPlayFilm = ofMap(currentDialog, 0, sub.size(), 0, 1);//TODO: wrong counting by slides..
		}
	}

	//--

	// Debug info
	{
#ifdef USE_WIDGET__SUBTITLES
		if (bDrawWidgetInfo)
		{
			string s = "";

			//--

			uint64_t t = 0;

			//--

			if (indexModes == 0 && bPlayExternal)
			{
				t = positionExternal * (float)tEndSubsFilm;
			}

			//--

			else if (indexModes == 1 && bPlayStandalone)
			{
#ifdef USE_WIDGET__VIDEO_PLAYER
				if (player.playback.play)
				{
					t = ofGetElapsedTimeMillis() - tPlay;
				}
				else {//stopped
					t = tPlay;
				}
#else
				t = ofGetElapsedTimeMillis() - tPlay;
#endif
			}

			//--

			else if (indexModes == 2 && bPlayForced)
			{
#ifdef USE_WIDGET__VIDEO_PLAYER
				if (player.playback.play)
				{
					t = ofGetElapsedTimeMillis() - tPlayForceFilm;
				}
				else {//stopped
					t = tPlayForceFilm;
				}
#else
				t = ofGetElapsedTimeMillis() - tPlayForceFilm;
#endif
			}

			//--

			// Convert formatting
#ifdef USING_OFX_TIME_CODE
			s += timecode.timecodeForMillis(t);
#else
			s += t / 1000.f + "''";
#endif
			s += "\n";

			if (bDebug)
			{
				s += path_Srt; // .srt filename
				s += "\n";
			}

			//--

			// Display begin/end subs times
			if (bDebug)
			{
				s += "\n";
				s += ofToString(currentDialog) + "/" + ofToString(sub.size() - 1);
				s += "\n\n";
				s += sub[currentDialog]->getStartTimeString();
				s += "\n";
				s += sub[currentDialog]->getEndTimeString();
				s += "\n";

				float duration = (sub[currentDialog]->getEndTime() - sub[currentDialog]->getStartTime()) / 1000.f;
				s += ofToString(duration, 2) + "s";
				s += "\n";
			}

			//--

			boxInfo.setText(s);
}
#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawGui() {
	if (!bGui) return;

	// info
#ifdef USE_WIDGET__SUBTITLES
	if (bDrawWidgetInfo) boxInfo.draw();
#endif

#ifndef USE_IM_GUI__SUBTITLES
	if (bGui_Internal && bGui_InternalAllowed) gui.draw();
#endif

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.drawGui();
#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::draw(ofRectangle view) {
	if (!bDraw) return;

	float x = view.getX();
	float y = view.getY();
	float w = view.getWidth();//TODO: not handled
	float h = view.getHeight();

	ofPushMatrix();
	ofTranslate(x, y);

	draw();

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawRaw()
{
	if (!bDraw) return;

	//--

	// Force box height

	if (bCenteredV || bResponsive)
	{
		float h = getOneLineHeight() + getSpacingBetweenLines();
		box.setHeight(amountLinesTarget * h);
	}
	else
	{
	}
	box.setLockH();

	//TODO:
	// 1st pass. Expected aprox amount lines of the last current drawn 
	// Will update amountLinesDrawn! 
	ofRectangle r = getTextBoxEstimate(textCurrent, box.getRectangle());

	//--

	// Will update boxDrawn
	// True. No drawing! 
	// Just calculate and spaciate lines!
	drawTextBox(textCurrent, box.getRectangle(), true);

	//--

	float _offset = 0;

	if (bCenteredV)
	{
		ofPushMatrix();

		_offset = -(boxDrawn.getHeight() * 0.5f) + (box.getHeight() * 0.5f);

		/*
		int _diff = amountLinesTarget - amountLinesDrawn;
		if (_diff < 0) {
			int _offsetLines = _diff / 2.f;
			float _h = getOneLineHeight() + getSpacingBetweenLines();
			_offset = _h * _offsetLines;
		}
		*/

		ofTranslate(0, _offset);
	}

	//--

	// The Real Real drawing!
	boxDrawnReal = drawTextBox(textCurrent, box.getRectangle(), false);
	boxDrawnReal.translateY(_offset);

	//--

	if (bCenteredV)
	{
		ofPopMatrix();

		// Mark Center Point
		/*
		// Will update amountLinesDrawn
		if (!bLive)
		{
			if (bDebug)
			//if (bDebug2)
			{
				float _x = box.getX();
				float _y = box.getY();

				_x += box.getWidth() / 2;
				_y += _offset;
				_y += r.getHeight() / 2;

				drawInsertionPoint(_x, _y, 8, 8);
			}
		}
		*/
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawDebug()
{
	if (!bGui) return;//workflow
	if (bLive) return;
	if (!bDebug) return;
	if (isPrecoutingStart) return;

	ofPushStyle();
	{
		/*
		if (bEdit && bDebug)
		{
			// A. Border
			//box.drawBorderBlinking();

			// B. Fill
			//ofPushStyle();
			//ofSetColor(colorBgFloat);
			//ofFill();
			//ofDrawRectangle(box.getRectangle());
			//ofPopStyle();
		}
		*/

		//--

		// Box
		// B. Attached to paragraph
		//ofRectangle bb = boxDrawnReal;
		// A. Attached to box
		ofRectangle bb = box.getRectangle();

		//TODO:
		// improve getting the bigger rectangle.. 
		if (1) {
			float diff = boxDrawnReal.getHeight() - bb.getHeight();
			bool b = (diff > 0);
			if (b) {
				bb.setHeight(boxDrawnReal.getHeight());
				bb.translateY(-diff / 2);
			}
		}

		//--

		{
			int padx = 50;
			int pady = 50;

			//--

			// 1. Timeline 
			{
				{
					// 1.1 Big line

					ofPushStyle();

					float lw1 = 3;
					float lh1 = 5;

					glm::vec2 p = (bTop.get() ?
						(bb.getTopLeft() + glm::vec2(0, -pady)) : (bb.getBottomLeft() + glm::vec2(0, pady)));

					float x = p.x;
					float y = p.y;

					float w = bb.getWidth() + 1;

					// Slide progress
					ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 255);
					ofSetLineWidth(lh1);

					float o = 0;
					//float o = 1;

					ofDrawLine(x - o, y, x - o + (w + o) * progressPlaySlide, y);

					// Phase fade in/out lines
					float sz = 6;

					float xIn = 0;
					float xOut1 = 0;
					float xOut2 = 0;

					float szt = 12;//triangles

					//--

					// 1.2 Fade Marks

					//TODO:
					// mark fade in phase end point with a vertical line
					// hard to do bc using speed instead of time duration in ms!

					if (bAnimatedIn)
					{
						float d = 1.f / dtAnimIn;

						// estimated duration of the fade in phase,
						// bc we are applying this amount of the above
						// dt on every single frame!
						int t = (d / fps) * 1000;

						float r = 0;
						if (bPlayForced)
						{
							r = (t / (float)durationPlayForced);//ratio
						}
						if (bPlayStandalone || bPlayExternal || bPlayManual)
						{
							r = (t / (float)durationPlaySlide);//ratio
						}
						xIn = p.x + w * r;

						if (xIn != -1)
						{
							// zero
							/*
							// triangle
							if (textCurrent != "") {
								ofFill();
								ofSetPolyMode(OF_POLY_WINDING_NONZERO);
								ofBeginShape();
								//float o = h / 2 - 0;
								float o = 0;
								ofVertex(p.x, p.y - o);
								ofVertex(p.x + szt, p.y - o - szt);
								ofVertex(p.x + szt, p.y - o);
								ofEndShape();
							}
							*/

							// triangle
							ofFill();
							ofSetPolyMode(OF_POLY_WINDING_NONZERO);
							ofBeginShape();
							// a
							//ofVertex(xIn - szt / 2, p.y + 0.7 * szt);
							//ofVertex(xIn + szt / 2, p.y + 0.7 * szt);
							//ofVertex(xIn, p.y);
							// b
							//ofVertex(xIn - szt / 2, p.y - 0.7 * szt);
							//ofVertex(xIn + szt / 2, p.y - 0.7 * szt);
							//ofVertex(xIn, p.y);
							// c
							//ofVertex(xIn - szt / 2, p.y - 0.7 * szt);
							//ofVertex(xIn + szt / 2, p.y - 0.7 * szt);
							//ofVertex(xIn, p.y);
							// d
							ofVertex(xIn - szt / 2, p.y + 0.7 * szt);
							ofVertex(xIn + szt / 2, p.y + 0.7 * szt);
							ofVertex(xIn, p.y);
							ofEndShape();

							//// line
							//ofSetLineWidth(lw1);
							//ofDrawLine(xIn + lw1 / 2, p.y - szt, xIn + lw1 / 2, p.y);
							////ofDrawLine(xIn, p.y - szt, xIn, p.y);
							////T
							////ofDrawLine(xIn, p.y - szt, xIn, p.y + szt);
							////ofDrawLine(xIn - szt / 2, p.y - szt, xIn + szt / 2, p.y - szt);

							////triangle
							//ofFill();
							//ofSetPolyMode(OF_POLY_WINDING_NONZERO);
							//ofBeginShape();
							//ofVertex(xIn - sz, p.y );
							//ofVertex(xIn, p.y -sz);
							//ofVertex(xIn, p.y +sz);
							////ofVertex(xIn - sz, p.y );
							//ofEndShape();
						}
					}

					//--

					// mark fade out phase start point with a vertical line
					// mark countdown start
					if (bAnimatedOut)
					{
						ofSetLineWidth(lw1);

						// Fade out begins
						float r = 0.f;
						if (bPlayForced)
						{
							r = 1.f - (durationOut / (float)durationPlayForced);//ratio
						}
						if (bPlayStandalone || bPlayExternal || bPlayManual)
						{
							r = 1.f - (durationOut / (float)durationPlaySlide);//ratio
						}
						xOut1 = p.x + w * r;

						//// line
						//ofDrawLine(xOut1, p.y - sz, xOut1, p.y + sz);

						// triangle
						ofFill();
						ofSetPolyMode(OF_POLY_WINDING_NONZERO);
						ofBeginShape();
						// a
						//float o = 0;
						//ofVertex(xOut1, p.y + o);
						//ofVertex(xOut1 - szt, p.y - szt);
						//ofVertex(xOut1 - szt, p.y + o);
						// b
						ofVertex(xOut1 - szt / 2, p.y);
						ofVertex(xOut1 + szt / 2, p.y);
						ofVertex(xOut1, p.y + 0.7 * szt);
						ofEndShape();

						//--

						/*
						// Fade out ends
						// Circle Point
						float pixPerMillis = 0;
						if (bPlayForced)
						{
							pixPerMillis = w / (float)durationPlayForced;//ratio
						}
						if (bPlayStandalone || bPlayExternal)
						{
							pixPerMillis = w / (float)durationPlaySlide;//ratio
						}
						float d = 1.f / dtAnimOut;
						int t = (d / fps) * 1000;//duration required to fade out until 0!
						xOut2 = xOut1 + t * pixPerMillis;
						if (xOut2 < p.x + w) ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 255);
						else ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 32);//attenuate if goes out of the  time line!
						//sz -= 2;
						//ofSetLineWidth(2);
						//ofDrawLine(xOut2, p.y - sz, xOut2, p.y + sz);
						ofFill();
						ofDrawCircle(xOut2, p.y, 5);

						//--

						// Make fatter the line for the full opacity middle section!
						// full alpha zone
						if (0)
						{
							int lw = 4;
							int off = 0;
							ofSetLineWidth(lw);
							ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 128);
							ofDrawLine(xIn, p.y + off, xOut1, p.y + off);
						}
						*/
					}

					ofPopStyle();
				}

				//--

				// 1.2 Draw main rule thin line

				if ((bAnimatedIn) || (bAnimatedOut))
				{
					ofSetLineWidth(1.f);
					ofSetColor(bTheme.get() ? colorDebugLight : colorDebugDark, 64);

					if (bTop) ofDrawLine(bb.getX(), bb.getY() - pady, bb.getX() + bb.getWidth(), bb.getY() - pady);
					else ofDrawLine(bb.getBottomLeft().x, bb.getBottomLeft().y + pady, bb.getBottomLeft().x + bb.getWidth(), bb.getBottomLeft().y + pady);
				}
			}

			//--

			// 2. Alpha Preview

			// Lateral Widget. 
			// Vertical bar to show current opacity

			if (alpha != 0)
				if ((bAnimatedIn || bAnimatedOut))
				{
					float pad2 = 60;

					float x = 0;
					float y = 0;

					float x1 = 0;
					float x2 = 0;
					float y1 = 6;
					float y2 = 0;

					float w = 3;
					float h = -bb.getHeight();

					static ofBitmapFont f;
					float o = 3;//correct font

					glm::vec2 p;
					string s;

					//--

					if (bLeft) x = bb.getTopLeft().x - pad2;
					else x = bb.getTopRight().x + pad2;

					y = bb.getTopLeft().y;

					s = "ALPHA";
					auto bb1 = f.getBoundingBox(s, 0, 0);
					x1 = bb1.getWidth() / 2 - o;

					ofDrawBitmapStringHighlight(s, x - x1, y - y1,
						(bTheme.get() ? 255 : 0), (bTheme.get() ? 0 : 255));

					//--

					if (bLeft)
					{
						p = bb.getTopLeft();
						x = p.x - pad2;
					}
					else
					{
						p = bb.getTopRight();
						x = p.x + pad2;
					}

					y = p.y + bb.getHeight();

					ofColor c = ofColor(colorTextFloat.get(), alpha * colorTextFloat.get().a * 255);
					ofSetColor(c);
					ofFill();
					ofRectangle r(x, y, w, h * alpha);
					ofDrawRectangle(r);

					//--

					if ((bAnimatedIn && isAnimIn) || (bAnimatedOut && isAnimOut))
					{
						if ((bAnimatedIn && isAnimIn)) s = "IN";
						else if (bAnimatedOut && isAnimOut) s = "OUT";

						auto bb2 = f.getBoundingBox(s, 0, 0);
						x2 = bb2.getWidth() / 2 - o;
						y2 = bb2.getHeight();
						ofDrawBitmapStringHighlight(s, x - x2, y + y2,
							(bTheme.get() ? 255 : 0), (bTheme.get() ? 0 : 255));
					}
				}

			//----

			// Debug boxDrawn

			if (bDebug2)
			{
				ofNoFill();
				ofSetColor(ofColor::yellow, 128);
				ofSetLineWidth(2);
				ofDrawRectangle(boxDrawnReal);
			}
		}
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::draw()
{

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.drawVideo();
#endif

	if (!bDraw) return;

	drawRaw();

	if (!bGui) return;

	//TODO: fix locking moving the box!
	//it happens when moving floating preview window sometimes.
	if (!bLive) box.draw();

	drawDebug();
}

//--

#ifdef USE_SHADOW
//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::drawTextBox(std::string _str, ofRectangle r, bool bNoDraw)
{
	// NOTE that uses only the rectangle width! 
	// Not the height!

	float _x = box.getX();
	float _y = box.getY();
	float _w = box.getWidth();
	float _h = box.getHeight();

	int _size = 1.f;
	int _align = fAlign.get();
	ofColor _color = colorTextFloat.get();
	ofColor _color2 = fColorShadow.get();

	float _x2 = _x + offsetShadow.get().x;
	float _y2 = _y + offsetShadow.get().y;

	// How much less or more lines that expected/targeted
	diff = amountLinesDrawn - amountLinesTarget;

	// Non responsive
	if (!bResponsive)
	{
		_size = fSize.get();
	}
	// Responsive
	else
	{
		// Using Engine
		{
			//TODO:
			// it's problematic bc when changing size, 
			// box relation changes too. 
			// must be a kind of manual algorithm. 
			// make font bigger to fit container
			// must adapt size. a bit complex..
			// also can be reduced to when there's too much lines..
			//float rMax = 1.25f;
			//_size = rMax * fSize.get();

			float ho = getOneLineHeight();
			float hb = box.getHeight();

			float rLimit;
			float rMax;
			float rMin;

			float r; // ratio
			// could use a variable ratio depending on the amount of lines

			float s = 1.f;//scaler

			//Default
			//_size = r * fSize.get();

			// A. Same lines than expected
			if (diff == 0)
			{
				sEngine = "A";

				// do nothing
				_size = fSize.get();
			}

			// B. More lines than expected
			else if (diff > 0)
			{
				sEngine = "B";

				rLimit = 0.5f;
				rMin = ofMap(resizeResponsive, 0, 1, 1.f, rLimit, true);
				r = ofMap(diff, 1, amountLinesTarget - 1, 1, rMin, true);

				//--

				_size = r * fSize.get();

				//--

				// Offset substract difference to align to top border.
				_y += _size - fSize.get();
			}

			// C. Less lines than expected
			else if (diff < 0)
			{
				sEngine = "C";

				//rLimit = 1.3f;
				rLimit = 2.5f;
				rMax = ofMap(resizeResponsive, 0, 1, 1.f, rLimit, true);
				r = ofMap(abs(diff), 0, amountLinesTarget - 1, 1, rMax, true);

				//--

				_size = r * fSize.get();

				//--

				//TODO:
				// Extra stuff
				if (amountLinesDrawn == 1) {
					_size *= 2.0f;
					//if(bCenteredV) _y -= fSize.get() * 0.2;
					_y -= fSize.get() * 0.2;
				}
				else if (amountLinesDrawn == 2) {
					_size *= 1.25f;
					if (bCenteredV) _y -= fSize.get() * 0.5;
				}

				//--

				// Offset substract difference to align to top border.
				//_y -= fSize.get()- _size;
				_y += _size - fSize.get();

				//if (diff < -1) _y -= getSpacingBetweenLines();
			}
		}
	}
	if (!bNoDraw)
	{
		if ((bAnimatedIn) || (bAnimatedOut))
		{
			_color = ofColor(_color, alpha * _color.a);
			_color2 = ofColor(_color2, alpha * _color2.a);
		}
	}

	// here upper border is aligned to the center horizontal
	_y += getOneLineHeight();

	// Fix. 
	// workaround to hard code calibrate.
	//_y += 2; 

	//------

	if (!bNoDraw) ofPushStyle();
	{
		int iPasses = 0;

#ifdef USE_SHADOW
		while (iPasses < 2)
#endif
		{
			if (!bNoDraw)
			{

#ifdef USE_SHADOW
				if (iPasses == 1) ofSetColor(_color);
				else if (iPasses == 0) ofSetColor(_color2);
#else
				ofSetColor(_color);
#endif
			}

			float __x = _x;
			float __y = _y;

#ifdef USE_SHADOW
			if (iPasses == 0) // shadow
			{
				__x = _x2;
				__y = _y2;
			}
			//else // regular
			//{
			//	__x = _x;
			//	__y = _y;
			//}
#endif

			int _numLines = 20;
			bool _wordsWereCropped = false;
			bool _bCenter = ((_align == 2) ? true : false);//centered

			/*get the final text formatting (by adding \n's) in the supplied string;
			BE ARWARE that using TRUE in here will modify your supplied string! */
			//bool _br = false;
			bool _br = bForceAddBreakLines;

			if (fAlign != 1) // left or center align
			{
				//TODO:
				//if(bNoDraw)
				boxDrawn = font.drawMultiLineColumn(
					_str,					/*string*/
					_size,					/*size*/
					//_x, _y,					/*where*/
					__x, __y,					/*where*/
					MAX(10, _w),			/*column width*/
					_numLines,				/*get back the number of lines*/
					bNoDraw,				/* if true, we wont draw (just get bbox back) */
					20,						/* max number of lines to draw, crop after that */
					_br,					/*get the final text formatting (by adding \n's) in the supplied string;
											BE ARWARE that using TRUE in here will modify your supplied string! */
					&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
					_bCenter				/*centered*/
				);
			}
			else // right align
			{
				//TODO: 
				//WIP trying to make work right align
				if (bForceAddBreakLines)
				{
					auto __bbox = font.drawMultiLineColumn(
						_str,					/*string*/
						_size,					/*size*/
						//_x, _y,					/*where*/
						__x, __y,					/*where*/
						MAX(10, _w),			/*column width*/
						_numLines,				/*get back the number of lines*/
						true,					/* if true, we wont draw (just get bbox back) */
						20,						/* max number of lines to draw, crop after that */
						_br,					/*get the final text formatting (by adding \n's) in the supplied string;
												BE ARWARE that using TRUE in here will modify your supplied string! */
						&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
						_bCenter				/*centered*/
					);

					_str = _str + " ";//fix last char
					//textCurrent = _str;
				}

				if (!bNoDraw)
				{
					boxDrawn = font.drawMultiLine(
						_str,					/*string*/
						_size,					/*size*/
						_x, _y,					/*where*/
						OF_ALIGN_HORZ_RIGHT,
						MAX(10, _w)				/*column width*/
					);
				}
				//TODO: workaround
				else
				{
#ifdef USE_SHADOW
					if (iPasses == 0)
#endif
					{
						boxDrawn = font.getBBox(
							_str,					/*string*/
							_size,					/*size*/
							//_x, _y,					/*where*/
							__x, __y,					/*where*/
							OF_ALIGN_HORZ_RIGHT,
							MAX(10, _w)				/*column width*/
						);
					}
				}
			}

#ifdef USE_SHADOW
			iPasses++;
#endif
		}
	}
	if (!bNoDraw) ofPopStyle();

	//--

	return boxDrawn;
}
#endif

#ifndef USE_SHADOW
//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::drawTextBox(std::string _str, ofRectangle r, bool bNoDraw)
{
	// NOTE that uses only the rectangle width! 
	// Not the height!

	float _x = box.getX();
	float _y = box.getY();
	float _w = box.getWidth();
	float _h = box.getHeight();

	int _size = 1.f;
	int _align = fAlign.get();
	ofColor _color = colorTextFloat.get();
	ofColor _color2 = fColorShadow.get();

	float _x2 = _x + offsetShadow.get().x;
	float _y2 = _y + offsetShadow.get().y;

	// How many less or more amount lines that expected/targeted
	diff = amountLinesDrawn - amountLinesTarget;

	// Non responsive
	if (!bResponsive)
	{
		_size = fSize.get();
	}
	// Responsive
	else
	{
		// Using Engine
		{
			//TODO:
			// it's problematic bc when changing size, 
			// box relation changes too. 
			// must be a kind of manual algorithm. 
			// make font bigger to fit container
			// must adapt size. a bit complex..
			// also can be reduced to when there's too much lines..
			//float rMax = 1.25f;
			//_size = rMax * fSize.get();

			float ho = getOneLineHeight();
			float hb = box.getHeight();

			float rLimit;
			float rMax;
			float rMin;

			float r; // ratio
			// could use a variable ratio depending on the amount of lines

			float s = 1.f;//scaler

			//Default
			//_size = r * fSize.get();

			// A. Same lines than expected
			if (diff == 0)
			{
				sEngine = "A";

				// do nothing
				_size = fSize.get();
			}

			// B. More lines than expected
			else if (diff > 0)
			{
				sEngine = "B";

				rLimit = 0.5f;
				rMin = ofMap(resizeResponsive, 0, 1, 1.f, rLimit, true);
				r = ofMap(diff, 1, amountLinesTarget - 1, 1, rMin, true);

				//--

				_size = r * fSize.get();

				//--

				// Offset substract difference to align to top border.
				_y += _size - fSize.get();
			}

			// C. Less lines than expected
			else if (diff < 0)
			{
				sEngine = "C";

				//rLimit = 1.3f;
				rLimit = 2.5f;
				rMax = ofMap(resizeResponsive, 0, 1, 1.f, rLimit, true);
				r = ofMap(abs(diff), 0, amountLinesTarget - 1, 1, rMax, true);

				//--

				_size = r * fSize.get();

				//--

				//TODO:
				// Extra stuff
				if (amountLinesDrawn == 1)
				{
					_size *= 2.0f;
					//if(bCenteredV) _y -= fSize.get() * 0.2;
					_y -= fSize.get() * 0.2;
				}
				else if (amountLinesDrawn == 2)
				{
					_size *= 1.25f;
					if (bCenteredV) _y -= fSize.get() * 0.5;
				}

				//--

				// Offset substract difference to align to top border.
				//_y -= fSize.get()- _size;
				_y += _size - fSize.get();

				//if (diff < -1) _y -= getSpacingBetweenLines();
			}
		}
	}
	if (!bNoDraw)
	{
		if ((bAnimatedIn) || (bAnimatedOut))
		{
			_color = ofColor(_color, alpha * _color.a);
			_color2 = ofColor(_color2, alpha * _color2.a);
		}
	}

	// here upper border is aligned to the center horizontal
	_y += getOneLineHeight();

	// Fix. 
	// workaround to hard code calibrate.
	//_y += 2; 

	//------

	if (!bNoDraw) ofPushStyle();
	{
		int iPasses = 0;

		{
			if (!bNoDraw)
			{
				ofSetColor(_color);
			}

			float __x = _x;
			float __y = _y;

			int _numLines = 20;
			bool _wordsWereCropped = false;
			bool _bCenter = ((_align == 2) ? true : false);//centered

			/*get the final text formatting (by adding \n's) in the supplied string;
			BE ARWARE that using TRUE in here will modify your supplied string! */
			//bool _br = false;
			bool _br = bForceAddBreakLines;

			if (fAlign != 1) // left or center align
			{
				//TODO:
				//if(bNoDraw)
				boxDrawn = font.drawMultiLineColumn(
					_str,					/*string*/
					_size,					/*size*/
					//_x, _y,					/*where*/
					__x, __y,					/*where*/
					MAX(10, _w),			/*column width*/
					_numLines,				/*get back the number of lines*/
					bNoDraw,				/* if true, we wont draw (just get bbox back) */
					20,						/* max number of lines to draw, crop after that */
					_br,					/*get the final text formatting (by adding \n's) in the supplied string;
											BE ARWARE that using TRUE in here will modify your supplied string! */
					&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
					_bCenter				/*centered*/
				);
			}
			else // right align
			{
				//TODO: 
				//WIP trying to make work right align
				if (bForceAddBreakLines)
				{
					auto __bbox = font.drawMultiLineColumn(
						_str,					/*string*/
						_size,					/*size*/
						//_x, _y,					/*where*/
						__x, __y,					/*where*/
						MAX(10, _w),			/*column width*/
						_numLines,				/*get back the number of lines*/
						true,					/* if true, we wont draw (just get bbox back) */
						20,						/* max number of lines to draw, crop after that */
						_br,					/*get the final text formatting (by adding \n's) in the supplied string;
												BE ARWARE that using TRUE in here will modify your supplied string! */
						&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
						_bCenter				/*centered*/
					);

					_str = _str + " ";//fix last char
					//textCurrent = _str;
				}

				if (!bNoDraw)
				{
					boxDrawn = font.drawMultiLine(
						_str,					/*string*/
						_size,					/*size*/
						_x, _y,					/*where*/
						OF_ALIGN_HORZ_RIGHT,
						MAX(10, _w)				/*column width*/
					);
				}
				//TODO: workaround
				else
				{
					{
						boxDrawn = font.getBBox(
							_str,					/*string*/
							_size,					/*size*/
							//_x, _y,					/*where*/
							__x, __y,					/*where*/
							OF_ALIGN_HORZ_RIGHT,
							MAX(10, _w)				/*column width*/
						);
					}
				}
			}
		}
	}
	if (!bNoDraw) ofPopStyle();

	//--

	return boxDrawn;
}
#endif

//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::getTextBoxEstimate(std::string _str, ofRectangle r)
{
	float _x = box.getX();
	float _y = box.getY();
	float _w = box.getWidth();
	float _h = box.getHeight();

	int _align;
	int _size;

	_size = fSize.get();
	_align = fAlign.get();

	_y += getOneLineHeight(); // here upper border is aligned to the center horizontal

	//------

	ofRectangle _bbox;
	int _numLines = 20;
	bool _wordsWereCropped = false;
	bool _bCenter = ((_align == 3) ? true : false);
	bool _br = false;

	_bbox = font.drawMultiLineColumn(_str,	/*string*/
		_size,					/*size*/
		_x, _y,					/*where*/
		MAX(10, _w),			/*column width*/
		_numLines,				/*get back the number of lines*/
		true,					/* if true, we wont draw (just get bbox back) */
		20,						/* max number of lines to draw, crop after that */
		_br,					/*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
		_bCenter				/*centered*/
	);

	amountLinesDrawn = _numLines; // amount lines of the last current drawn 

	return _bbox;
}

//--------------------------------------------------------------
float ofxSurfingTextSubtitle::getOneLineHeight(bool oneOnly)
{
	// Pre calculate line heights. without the spacing

	bool _br = false;

	// to get more precise value, we should use 3 lines, 
	// applying also lines-height and dividing by 3
	std::string _str1 = "T";// one lines

	int _align;
	int _size;

	_size = fSize.get();
	_align = fAlign.get();

	//-

	//ofRectangle boxDrawn;
	//boxDrawn = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	//float v2 = MAX(boxDrawn.getHeight(), 1) / 2.0f;
	//boxDrawn = font.drawMultiLine(_str3, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	//float v3 = MAX(boxDrawn.getHeight(), 1) / 3.0f;
	//boxDrawn = font.drawMultiLine(_str4, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	//float v4 = MAX(boxDrawn.getHeight(), 1) / 4.0f;

	//-

	ofRectangle _bbox;
	int _numLines = 20;
	bool _wordsWereCropped = false;
	bool _bCenter = ((_align == 3) ? true : false);
	float _w = 1000;
	int _x, _y;
	_x = 0;
	_y = 0;
	float v1, v2, v3, v4, v;

	_bbox = font.drawMultiLineColumn(_str1,	/*string*/
		_size,					/*size*/
		_x, _y,					/*where*/
		MAX(10, _w),			/*column width*/
		_numLines,				/*get back the number of lines*/
		true,					/* if true, we wont draw (just get bbox back) */
		20,						/* max number of lines to draw, crop after that */
		_br,					/*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
		_bCenter				/*centered*/
	);
	v1 = MAX(_bbox.getHeight(), 1);

	//-

	if (!oneOnly)
	{
		std::string _str2 = "T\nT";// two lines
		std::string _str3 = "T\nT\nT";// three lines
		std::string _str4 = "T\nT\nT\nT";// four lines

		//boxDrawn = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
		_bbox = font.drawMultiLineColumn(_str2,	/*string*/
			_size,					/*size*/
			_x, _y,					/*where*/
			MAX(10, _w),			/*column width*/
			_numLines,				/*get back the number of lines*/
			true,					/* if true, we wont draw (just get bbox back) */
			20,						/* max number of lines to draw, crop after that */
			_br,					/*get the final text formatting (by adding \n's) in the supplied string;
									BE ARWARE that using TRUE in here will modify your supplied string! */
			&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
			_bCenter				/*centered*/
		);
		v2 = MAX(_bbox.getHeight(), 1) / 2.0f;

		//-

		//boxDrawn = font.drawMultiLine(_str3, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
		_bbox = font.drawMultiLineColumn(_str3,	/*string*/
			_size,					/*size*/
			_x, _y,					/*where*/
			MAX(10, _w),			/*column width*/
			_numLines,				/*get back the number of lines*/
			true,					/* if true, we wont draw (just get bbox back) */
			20,						/* max number of lines to draw, crop after that */
			_br,					/*get the final text formatting (by adding \n's) in the supplied string;
									BE ARWARE that using TRUE in here will modify your supplied string! */
			&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
			_bCenter				/*centered*/
		);
		v3 = MAX(_bbox.getHeight(), 1) / 3.0f;

		//-

		//boxDrawn = font.drawMultiLine(_str4, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
		_bbox = font.drawMultiLineColumn(_str4,	/*string*/
			_size,					/*size*/
			_x, _y,					/*where*/
			MAX(10, _w),			/*column width*/
			_numLines,				/*get back the number of lines*/
			true,					/* if true, we wont draw (just get bbox back) */
			20,						/* max number of lines to draw, crop after that */
			_br,					/*get the final text formatting (by adding \n's) in the supplied string;
									BE ARWARE that using TRUE in here will modify your supplied string! */
			&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
			_bCenter				/*centered*/
		);
		v4 = MAX(_bbox.getHeight(), 1) / 4.0f;
	}

	// improve precision making more-than-one-line-size situations

	//-

	if (oneOnly) v = v1;
	else v = (v1 + v2 + v3 + v4) / 4.0f;

	//-

	return v;
}

//--------------------------------------------------------------
float ofxSurfingTextSubtitle::getSpacingBetweenLines()
{
	// Pre calculate line heights
	// to do that we measure a two lines height and subtract the space between the lines
	// that depends on spacing and height line.

	float spacing = -1;

	bool _br = false;

	// to get more precise value, we should use 3 lines, 
	// applying also lines-height and dividing by 3
	std::string _str1 = "T";// one lines

	int _align;
	int _size;

	_size = fSize.get();
	_align = fAlign.get();

	//-

	ofRectangle _bbox;
	int _numLines = 20;
	bool _wordsWereCropped = false;
	bool _bCenter = ((_align == 3) ? true : false);
	float _w = 1000;
	int _x, _y;
	_x = 0;
	_y = 0;
	float v1, v2;

	_bbox = font.drawMultiLineColumn(_str1,	/*string*/
		_size,					/*size*/
		_x, _y,					/*where*/
		MAX(10, _w),			/*column width*/
		_numLines,				/*get back the number of lines*/
		true,					/* if true, we wont draw (just get bbox back) */
		20,						/* max number of lines to draw, crop after that */
		_br,					/*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
		_bCenter				/*centered*/
	);
	v1 = MAX(_bbox.getHeight(), 1);

	//-

	std::string _str2 = "T\nT";// two lines

	//boxDrawn = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	_bbox = font.drawMultiLineColumn(_str2,	/*string*/
		_size,					/*size*/
		_x, _y,					/*where*/
		MAX(10, _w),			/*column width*/
		_numLines,				/*get back the number of lines*/
		true,					/* if true, we wont draw (just get bbox back) */
		20,						/* max number of lines to draw, crop after that */
		_br,					/*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
		_bCenter				/*centered*/
	);
	v2 = MAX(_bbox.getHeight(), 1);

	spacing = v2 - (2 * v1);

	return spacing;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawInsertionPoint(float _x, float _y, float _w, float _h)
{
	float r = 2;

	ofPushStyle();

	ofFill();
	float v = ofMap(ofGetFrameNum() % 60, 0, 60, 0.25, 1);
	ofSetColor(bTheme ? colorDebugLight : colorDebugDark, v * 255);
	ofDrawCircle(_x, _y, r);

	if (_w != 0) {//an extra horizontal line
		ofNoFill();
		ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 64);
		ofDrawLine(_x - _w, _y, _x + _w, _y);
	}
	if (_h != 0) {//an extra vertical line
		ofNoFill();
		ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 64);
		ofDrawLine(_x, _y - _h, _x, _y + _h);
	}

	ofPopStyle();
}

#ifdef USE_IM_GUI__SUBTITLES

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGui()
{
	if (!bGui) return;
	if (ui == NULL) {
		ofLogError("ofxSurfingTextSubtitle") << " ofxImGui is not instantiated. You should set the parent ui instance as reference!";
		ofLogError("ofxSurfingTextSubtitle") << " Usually: subs.setUiPtr(&ui);";
		return;
	}

	/*
	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;
	float w = 290;
	//float w = 250;
	ImVec2 size_max = ImVec2(w, -1);
	ImVec2 size_min = ImVec2(w, -1);
	ImGui::SetNextWindowSizeConstraints(size_min, size_max);
	*/

	if (ui->BeginWindow(bGui))//main
	{
		drawImGuiWidgets();

		ui->EndWindow();
	}

	//--

//#ifdef USE_PRESETS__SUBTITLES
	//presets.drawImGui(true);
//#endif

	//--

	drawImGuiWindowParagraph();

	//--

	if (bGui_List)
	{
		if (bGui_Paragraph) ui->setNextWindowAfterWindowNamed(bGui_Paragraph);
		else  ui->setNextWindowAfterWindowNamed(bGui);

		drawImGuiList();
	}

	//--

	//ui->AddSpacingSeparated();

#ifdef USE_PRESETS__SUBTITLES
	if (presets.bGui)
	{
		if (bGui_List) ui->setNextWindowAfterWindowNamed(bGui_List);
		else if (bGui_Paragraph) ui->setNextWindowAfterWindowNamed(bGui_Paragraph);
		//else if (bGui) ui->setNextWindowAfterWindowNamed(bGui);
		else ui->setNextWindowAfterWindowNamed(bGui);

		presets.drawImGui(true, false, false, false);
	}
#endif
	//--

#ifdef USE_WIDGET__VIDEO_PLAYER	
	if (ui->BeginWindow(player.bGui))
	{
		ui->AddLabelBig("VIDEO PLAYER");
		ui->Add(player.bOpenVideo, OFX_IM_BUTTON_SMALL);
		ui->Add(player.path_Video, OFX_IM_TEXT_DISPLAY);
		//ui->Add(player.bGui_VideoPlayer, OFX_IM_TOGGLE_ROUNDED_SMALL);
		ui->AddSpacingSeparated();

		ui->Add(player.bDraw_Video, OFX_IM_TOGGLE_ROUNDED_SMALL);
		ui->Add(player.volume, OFX_IM_HSLIDER_MINI);
		ui->Add(player.position, OFX_IM_HSLIDER_MINI);
		ui->AddSpacingSeparated();

		ofxImGuiSurfing::AddToggleNamed(player.playback.play, "Playing", "Play");
		//ui->Add(player.playback.play, OFX_IM_TOGGLE_SMALL);
		ui->Add(player.playback.stop, OFX_IM_BUTTON_SMALL);
		ui->Add(player.playback.backwards, OFX_IM_BUTTON_SMALL, 2, true);
		ui->Add(player.playback.forwards, OFX_IM_BUTTON_SMALL, 2);

		ui->EndWindow();
}
#endif	
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWindowParagraph()
{
	if (bGui && bGui_Paragraph) ui->setNextWindowAfterWindowNamed(bGui);

	//if (ui->BeginTree("PARAGRAPH", false, false))
	if (ui->BeginWindow(bGui_Paragraph))
	{
		string s;

		ui->Add(bFine, OFX_IM_TOGGLE_ROUNDED_MINI);
		ui->AddSpacing();
		//ui->PushWidth(0.7f);
		SurfingGuiTypes st = (bFine.get() ? OFX_IM_STEPPER : OFX_IM_DEFAULT);
		ui->Add(fSizePrc, st);
		ui->Add(fSpacing, st);
		ui->Add(fLineHeight, st);
		//ui->PopWidth();

		ui->AddSpacing();

		if (bMinimize) // minimized
		{
			ui->AddCombo(fAlign, names_Align);
			ui->AddTooltip("Align");
			ui->AddSpacingSeparated();

			ui->Add(bCenteredV, OFX_IM_TOGGLE_ROUNDED_MINI);
			s = "Y center on container.";
			ui->AddTooltip(s);
			ui->AddSpacingSeparated();
			ui->Add(bResponsive, OFX_IM_TOGGLE_ROUNDED_MINI);
			s = "Use responsive engine.";
			ui->AddTooltip(s);
			ui->AddSpacing();
			if (bResponsive)
			{
				ui->Add(resizeResponsive, OFX_IM_HSLIDER_SMALL);
				//s = "Rescale";
				ui->AddTooltip(resizeResponsive);
			}
			if (bCenteredV || bResponsive) ui->Add(amountLinesTarget, OFX_IM_STEPPER);

			ui->AddSpacingSeparated();

			ui->Add(bReset, OFX_IM_BUTTON_SMALL);
		}
		else // maximized
		{
			ui->AddCombo(fAlign, names_Align);
			ui->AddTooltip("Align");

			ui->AddSpacingSeparated();

			if (ui->BeginTree("EXTRA", false, false))
			{
				ui->AddSpacing();
				ui->Add(bCenteredV, OFX_IM_TOGGLE_ROUNDED_MINI);
				s = "Y center on container.";
				ui->AddTooltip(s);
				ui->AddSpacingSeparated();
				ui->Add(bResponsive, OFX_IM_TOGGLE_ROUNDED_MINI);
				s = "Use responsive engine.";
				ui->AddTooltip(s);
				if (bResponsive) {
					ui->Indent();
					ui->AddSpacing();
					ui->Add(resizeResponsive, OFX_IM_STEPPER);
					ui->Add(resizeResponsive, OFX_IM_HSLIDER_SMALL_NO_LABELS);
					s = "Rescale";
					ui->AddTooltip(s);
					ui->Unindent();
				}
				if (bCenteredV || bResponsive)
				{
					ui->AddSpacing();
					ui->Add(amountLinesTarget, OFX_IM_STEPPER);
					s = "Expented amount lines";
					ui->AddTooltip(s);
				}

				ui->AddSpacingSeparated();

				ui->Add(bReset, OFX_IM_BUTTON_SMALL);

				ui->AddSpacingSeparated();

				if (ui->BeginTree("CONTAINER", false, false))
				{
					ui->PushWidth(0.7f);

					//ui->AddLabel("Container");
					static float _x;
					//if (fAlign != 1) 
					{
						_x = box.getX();
						if (ImGui::SliderFloat("x", &_x, 0, ofGetWidth())) {
							box.setX(_x);
						}
					}
					//else {
					//	_x = ofGetWidth() - box.getX();
					//	if (ImGui::SliderFloat("x", &_x, 0, ofGetWidth())) {
					//		box.setX(ofGetWidth() - _x);
					//	}
					//}

					static float _y;
					_y = box.getY();
					if (ImGui::SliderFloat("y", &_y, 0, ofGetHeight())) {
						box.setY(_y);
					}

					static float _w;
					_w = box.getWidth();
					if (ImGui::SliderFloat("w", &_w, 0, ofGetWidth())) {
						box.setWidth(_w);
					}

					if (!bCenteredV && !bResponsive)
					{
						static float _h;
						_h = box.getHeight();
						if (ImGui::SliderFloat("h", &_h, 0, ofGetHeight())) {
							box.setHeight(_h);
						}
					}

					ui->PopWidth();

					ui->EndTree(false);
				}

				//--

				//if (0)
				if (!bMinimize)
				{
					ui->AddSpacingSeparated();

					//static bool bDebug3 = false;
					ui->AddToggle("Debug Advanced", bDebug2, OFX_IM_TOGGLE_ROUNDED_MINI);
					s = "Show internal vars from \nresponsive formatting engine.";
					ui->AddTooltip(s);
					ui->AddSpacing();
					ui->AddSpacing();

					if (bDebug2)
					{
						string s;
						ui->Indent();

						float ho = this->getOneLineHeight();
						s = "H Line: " + ofToString(ho);
						ui->AddLabel(s);
						float hb = box.getHeight();
						s = "H Box: " + ofToString(hb);
						ui->AddLabel(s);
						s = "Expected Lines: " + ofToString(amountLinesTarget);
						ui->AddLabel(s);
						s = "Drawn: " + ofToString(amountLinesDrawn);
						ui->AddLabel(s);
						s = "Diff: " + ofToString(diff);
						ui->AddLabel(s);
						s = "Used Engine: " + sEngine;
						ui->AddLabel(s);

						ui->Unindent();
					}
				}

				ui->EndTree(false);
			}
		}

		//ui->EndTree();
		ui->EndWindow();
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWidgets()
{
	// time label
	string sfile = "";
	string sdialog = "";
	string stime = "";
	string s;

	//--

	if (bLoaded)
	{
		// filename
		sfile = name_Srt;

		// Elapsed time 

		if (indexModes == 0) // external
		{
			if (bPlayExternal)
			{
				auto t = positionExternal * (float)tEndSubsFilm;
#ifdef USING_OFX_TIME_CODE
				stime = timecode.timecodeForMillis(t);
#else
				stime = t / 1000.f + "''";
#endif
			}
		}
		else if (indexModes == 1) // standalone
		{
			if (bPlayStandalone)
			{
				uint64_t t = ofGetElapsedTimeMillis() - tPlay;

#ifdef USING_OFX_TIME_CODE
				stime = timecode.timecodeForMillis(t);
#else
				stime += t / 1000.f;
#endif
			}
		}
		else if (indexModes == 2) // forced
		{
			if (bPlayForced)
			{
				stime = sub[currentDialog]->getStartTimeString();
			}
		}
		else if (indexModes == 3)  // manual
		{
			if (bPlayManual)
			{
			}
		}

		if (!bPlayStandalone && !bPlayForced && !bPlayExternal && !bPlayManual)
		{
			stime = sub[currentDialog]->getStartTimeString();
			//stime = timecode.timecodeForMillis(0);
		}

		// index
		sdialog = ofToString(currentDialog) + "/" + ofToString(sub.size() - 1);
	}

	//----

	ui->Add(bMinimize, OFX_IM_TOGGLE_ROUNDED_SMALL);
	if (!bMinimize) ui->Add(bKeys, OFX_IM_TOGGLE_ROUNDED_MINI);

	// manual
	if (indexModes != 3)
	{
		ui->AddSpacingSeparated();

		if (bLoaded)
		{
			if (!bMinimize)
			{
				ui->AddLabelBig(sfile);
				s = path_Srt;
				ui->AddTooltip(s);
			}
			ui->AddLabelBig(stime);

			ui->AddSpacing();

			ui->Add(progressPlayFilm, OFX_IM_PROGRESS_BAR);
			s = "Total Duration";
			ui->AddTooltip(s);

			ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR_NO_TEXT);
			s = "Dialog Duration";
			ui->AddTooltip(s);
		}
		else {
			ui->BeginBlinkText();
			ui->AddLabelBig("SRT FILE NOT LOADED!");
			ui->EndBlinkText();
			ui->AddSpacing();
			ui->Add(bOpen, OFX_IM_BUTTON_SMALL);
			s = "Open an SRT file";
			ui->AddTooltip(s);
		}
	}


	//TODO:
	if (bLoaded || (indexModes == 3) || bModeNoSrt)
	{
		ui->AddSpacingSeparated();

		//----

		//if (bMinimize)
		{
			ui->Add(bLive, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
			s = "Live Mode hides some stuff";
			if (!bLive)
			{
				ui->Add(bEdit, OFX_IM_TOGGLE_SMALL);
				s = "Edit container";
				ui->AddTooltip(s);
			}

			ui->AddSpacingSeparated();
		}

		if (ui->BeginTree("MAIN", false, false))
		{
			ui->AddSpacing();

			if (!bModeNoSrt)
			{
				if (!bMinimize) // maximized 
				{
					//if (!bLive) ui->AddSpacingSeparated();

					std::string n = "FILE";
					//std::string n = "SRT FILE";
					//std::string n = name_Srt;
					if (ui->BeginTree(n))
					{
						ui->AddLabel(name_Srt + ".srt");
						//ui->AddLabel("SRT FILE");

						ui->Add(bOpen, OFX_IM_BUTTON_SMALL);
						s = "Open an SRT file";
						ui->AddTooltip(s);

#ifdef USE_WIDGET__VIDEO_PLAYER
						ui->AddSpacingSeparated();
						ui->Add(player.bGui, OFX_IM_TOGGLE_ROUNDED);
#endif
						ui->EndTree(false);
					}

					ui->AddSpacingSeparated();
				}
			}

			ui->Add(bDraw, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
			ui->AddSpacing();

			ui->Add(bGui_Paragraph, OFX_IM_TOGGLE_ROUNDED);
#ifdef USE_PRESETS__SUBTITLES
			ui->Add(presets.bGui, OFX_IM_TOGGLE_ROUNDED);
#endif
			// not manual
			if (indexModes != 3)
			{
				ui->Add(bGui_List, OFX_IM_TOGGLE_ROUNDED);
			}

			if (!bLive) {
				ui->AddSpacingSeparated();
				ui->AddSpacing();
			}

			//--

			if (!bLive)
			{
				if (bModeNoSrt)
				{
					//ui->AddSpacing();
					ui->Add(bOpen, OFX_IM_BUTTON_SMALL);
					s = "Open an SRT file";
					ui->AddTooltip(s);
				}

				if (!bMinimize)
				{
					ui->AddSpacingSeparated();
					ui->AddSpacing();

					ui->Indent();
					{
						ui->Add(ui->bExtra, OFX_IM_TOGGLE_ROUNDED);
						if (ui->bExtra)
						{
							ui->Indent();
							{
								ui->AddSpacing();
								ui->Add(bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
								if (bDebug) {
									ui->Add(bTheme, OFX_IM_TOGGLE_ROUNDED_MINI);
									ui->Add(bTop, OFX_IM_TOGGLE_ROUNDED_MINI);
									ui->Add(bLeft, OFX_IM_TOGGLE_ROUNDED_MINI);

#ifndef USE_IM_GUI__SUBTITLES
									if (bEdit && bGui_InternalAllowed) ui->Add(bGui_Internal, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
#endif
#ifdef USE_WIDGET__SUBTITLES
									ui->Add(bDrawWidgetInfo, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
#endif
								}
							}
							ui->Unindent();
						}
					}
					ui->Unindent();
				}
			}

			ui->EndTree(false);
		}//main

		//--

		ui->AddSpacingSeparated();

		// maximized 
		//if (!bMinimize)
		{
			if (ui->BeginTree("TRANSPORT", false, false))
			{
				ui->AddSpacing();

				if (!bModeNoSrt)
				{
					ui->AddCombo(indexModes, names_Modes);
					string s = "Link Mode ";
					switch (indexModes)
					{
					case 0: s += "EXTERNAL\nLocal time linked \nto parent player."; break;
					case 1: s += "STANDALONE\nTime linked to \n.srt file times."; break;
					case 2: s += "FORCED\nLocal time linked \nto his timer."; break;
					case 3: s += "MANUAL\nIgnore SRT files \nand load any text."; break;
					}
					ui->AddTooltip(s);
				}

				if (indexModes == 0) // external
				{
					ui->Add(positionExternal, OFX_IM_HSLIDER_MINI);
					s = "Local time linked to parent player.";
					ui->AddTooltip(s);
					ui->Add(durationPlayForced, OFX_IM_HSLIDER_MINI);
					ui->AddTooltip(ofToString(durationPlayForced.get()) + "\nms");
				}
				else if (indexModes == 1) // standalone
				{
					ui->Add(bPlayStandalone, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
					s = "Time linked to .srt file times.";
					ui->AddTooltip(s);
					ui->Add(bStop, OFX_IM_BUTTON_SMALL);
					ui->Add(currentDialog, OFX_IM_HSLIDER_MINI_NO_NAME);
					s = "Dialog \n#" + ofToString(currentDialog.get());
				}
				else if (indexModes == 2) // forced
				{
					ui->Add(bPlayForced, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
					s = "Local time linked to his timer.";
					ui->AddTooltip(s);
					ui->Add(bStop, OFX_IM_BUTTON_SMALL);
					ui->Add(durationPlayForced, OFX_IM_HSLIDER_MINI);
					ui->AddTooltip(ofToString(durationPlayForced.get()) + "\nms");
					ui->Add(currentDialog, OFX_IM_HSLIDER_MINI_NO_NAME);
					s = "Dialog \n#" + ofToString(currentDialog.get());
					ui->AddTooltip(s);
					//ui->AddTooltip(currentDialog);

					ui->PushButtonRepeat();
					ui->Add(bPrev, OFX_IM_TOGGLE_SMALL, 2, true);
					ui->Add(bNext, OFX_IM_TOGGLE_SMALL, 2);
					ui->PopButtonRepeat();
				}
				else if (indexModes == 3) // manual
				{
					ui->Add(bPlayManual, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
					ui->Add(durationPlayForced, OFX_IM_HSLIDER_MINI);
					ui->AddTooltip(ofToString(durationPlayForced.get()) + "\nms");
				}

				//if (!bPlayExternal || indexModes != 2) ui->Add(bStop, OFX_IM_BUTTON_SMALL);

				ui->EndTree(false);
			}

			ui->AddSpacingSeparated();
		}

		//--

		if (ui->BeginTree("FADES", false, false))
		{
			string s;

			ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR);
			s = "Slide progress";
			ui->AddTooltip(s);
			ui->AddSpacing();

			if (bMinimize)
			{
				// in
				ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);
				s = "Fade In";
				ui->AddTooltip(s);

				// out
				ui->Add(bAnimatedOut, OFX_IM_TOGGLE_SMALL, 2);
				s = "Fade Out";
				ui->AddTooltip(s);

				if (bAnimatedIn) {
					ui->Add(progressIn, OFX_IM_PROGRESS_BAR_NO_TEXT);
					s = ofToString(durationIn.get(), 0);
					s += " ms";
					ui->AddTooltip(s);
				}
				if (bAnimatedOut) {
					ui->Add(progressOut, OFX_IM_PROGRESS_BAR_NO_TEXT);
					s = ofToString(durationOut.get(), 0);
					s += " ms";
					ui->AddTooltip(s);
				}
			}
			else
			{
				// in
				ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);
				s = "Fade In";
				ui->AddTooltip(s);
				if (bAnimatedIn)
				{
					ui->Add(progressIn, OFX_IM_PROGRESS_BAR_NO_TEXT);
					ui->Add(durationIn, OFX_IM_HSLIDER_MINI_NO_LABELS);
					ui->AddTooltip(durationIn);
				}
				ui->AddSpacing();

				// out
				ui->Add(bAnimatedOut, OFX_IM_TOGGLE_SMALL, 2, true);
				s = "Fade Out";
				ui->AddTooltip(s);
				if (bAnimatedOut)
				{
					ui->Add(progressOut, OFX_IM_PROGRESS_BAR_NO_TEXT);
					ui->Add(durationOut, OFX_IM_HSLIDER_MINI_NO_LABELS);
					ui->AddTooltip(durationOut);
				}
				ui->AddSpacing();

				ui->Add(bResetFades, OFX_IM_BUTTON_SMALL);
			}

			ui->EndTree(false);
		}

		ui->AddSpacingSeparated();

		if (ui->BeginTree("STYLE", false, false))
		{
			if (!bMinimize)
			{
				ui->Add(bCapitalize, OFX_IM_TOGGLE);
				ui->AddSpacing();

				if (ui->BeginTree("COLORS", false, false))
				{
					ui->PushWidth(0.7);
					static bool bShowBg = false;
					static float _alpha;
					_alpha = colorTextFloat.get().a;
					if (ImGui::SliderFloat("Alpha", &_alpha, 0, 1)) {
						ofFloatColor c = colorTextFloat.get();
						colorTextFloat.set(ofFloatColor(c.r, c.g, c.b, _alpha));
					}
					ui->PopWidth();

					ui->Add(colorTextFloat, OFX_IM_COLOR_NO_INPUTS);
					ui->Add(colorBgFloat, OFX_IM_COLOR_NO_INPUTS_NO_ALPHA);

					if (ui->AddButton("Swap")) {
						ofFloatColor c = colorTextFloat;
						colorTextFloat = colorBgFloat;
						colorBgFloat = c;
					}

					ui->EndTree(false);
				}
			}
			else
			{
				ui->Add(colorTextFloat, OFX_IM_COLOR_NO_INPUTS);
				ui->Add(colorBgFloat, OFX_IM_COLOR_NO_INPUTS_NO_ALPHA);
			}

			ui->EndTree(false);
		}

		//		//--
		//
		//		//ui->AddSpacingSeparated();
		//
		//#ifdef USE_PRESETS__SUBTITLES
		//		if (presets.bGui)
		//		{
		//			//ui->AddSpacingSeparated();
		//			ImGui::Spacing();
		//			ImGui::Separator();//fix
		//			ImGui::Spacing();
		//
		//			presets.drawImGui(false, false, true, false);
		//		}
		//#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiList()
{
	if (bGui_List)
	{
		//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_MEDIUM;
		/*
		float w = 200;
		ImVec2 size_max = ImVec2(-1, -1);
		ImVec2 size_min = ImVec2(w, -1);
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);
		*/
		/*
		ImVec2 width(300, 900);
		ImVec2 height(300, 900);
		ofxImGuiSurfing::SetWindowContraints(ImVec2(width.x, height.x), ImVec2(width.y, height.y));
		*/

		//--

		if (ui->BeginWindow(bGui_List, ImGuiWindowFlags_None))
		{
			if (!bMinimize) {
				//ui->AddLabelBig(path_Srt);
				ui->AddLabelBig(name_Srt + ".srt");
				ui->AddSpacing();

				ui->Add(progressPlayFilm, OFX_IM_PROGRESS_BAR);
				ui->AddSpacing();

				int track_item = currentDialog;
				ui->Add(bAutoScroll, OFX_IM_TOGGLE_ROUNDED_MINI);

				ui->AddSpacingBig();
				//ui->AddSpacingBigSeparated();
			}

			ImGui::BeginChild("_");
			{
				// iterate all srt dialogs
				for (int n = 0; n < subsDataText.size(); n++)
				{
					/*if (currentDialog == n)*/ ui->AddSeparated();

					//float h = ImGui::GetContentRegionAvail().y;
					float h = ui->getWidgetsHeightUnit();
					float w = ImGui::GetContentRegionAvail().x;
					//float w = ui->getWidgetsWidth(1);

					float w1 = 40;
					float w2 = w - w1;

					ImGui::Columns(2, "t", false);
					ImGui::SetColumnWidth(0, w1);
					ImGui::SetColumnWidth(1, w2);

					string s = ofToString(n);
					//ui->AddLabel(s);

					bool bDoUpdate = false;

					if (currentDialog == n) ui->BeginBorderFrame();

					if (ImGui::Button(s.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, h)))
					{
						//workflow
						if (bPlayExternal) bPlayForced = true;

						//currentDialog = n;
						bDoUpdate = true;
					}

					if (currentDialog == n) ui->EndBorderFrame();

					if (bDoUpdate) currentDialog = n;

					//if (!bMinimize) {
					//	ui->AddLabel(sub[n]->getStartTimeString());
					//	ui->AddLabel(sub[n]->getEndTimeString());
					//}

					ImGui::NextColumn();

					if (currentDialog == n)
					{
						//ImGui::ButtonEx(subsDataText[n].c_str(), ImVec2(w, 40));
						ui->AddLabelBig(subsDataText[n]);//bigger if selected

						if (bAutoScroll)
						{
							ImGui::SetScrollHereY(0.0f);
							//ImGui::SetScrollHereY(0.5f);
							//if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						}

						//--
					}
					else ui->AddLabelBig(subsDataText[n]);

					if (!bMinimize || currentDialog == n)
					{
						string s = sub[n]->getStartTimeString();
						s += "   ";
						s += sub[n]->getEndTimeString();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(s.c_str()).x
							- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);

						ui->AddLabel(s);
					}

					ImGui::Columns(1);

					//*if (currentDialog == n) ui->AddSeparated();
				}
			}
			ImGui::EndChild();

			ui->EndWindow();
		}
	}
}

#endif

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doReset() {
	ofLogNotice(__FUNCTION__);

	//colorTextFloat = ofColor(255, 255);
	fSpacing = 0;
	fLineHeight = 0.75;
	fSizePrc = 0.25;
	//fAlign = 0;

	amountLinesTarget = 6;
	bCenteredV = true;
	bResponsive = true;
	resizeResponsive = 0.5;
}

//--------------------------------------------------------------
std::string ofxSurfingTextSubtitle::getAlignNameFromIndex(int index) const {
	std::string n = "UNKNOWN";
	switch (index)
	{
	case 0:
		n = "LEFT";
		break;
	case 1:
		n = "RIGHT";
		break;
	case 2:
		n = "CENTER";
		break;
	}
	return n;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doResetFades() {
	//bAnimatedIn = true;
	//bAnimatedOut = true;
	durationPlayForced = 2000;
	durationOut = 250;
	durationIn = 250;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doUpdateSlidePlay(SubtitleItem* element) {

	tPlayStartSlide = ofGetElapsedTimeMillis();
	isSlidePlaying = true;
	auto ts = element->getStartTime();
	auto te = element->getEndTime();
	durationPlaySlide = te - ts;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::Changed(ofAbstractParameter& e)
{
	static bool bDISABLE_CALLBACKS = false;
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	// ignore
	if (name == progressPlaySlide.getName() ||
		name == progressPlayFilm.getName() ||
		name == progressIn.getName() ||
		name == progressOut.getName() ||
		name == positionExternal.getName())
		return;

	ofLogNotice("ofxSurfingTextSubtitle") << " Changed " << name << " : " << e;

	//--

	if (0) {}

	// Modes

	//TODO: fix crash
	//else if (name == indexModes.getName()) return;
	//else if (name == indexModes_Name.getName()) return;

	else if (name == indexModes.getName())
	{

#ifndef USE_IM_GUI__SUBTITLES
		if (bGui_Internal && bGui_InternalAllowed)
		{
			auto& guit = gui.getGroup(params_Transport.getName());
			guit.getGroup(params_External.getName()).minimize();
			guit.getGroup(params_Forced.getName()).minimize();
			guit.getGroup(params_Standalone.getName()).minimize();

			//workflow
			switch (indexModes)
			{
			case 0: guit.getGroup(params_External.getName()).maximize(); break;
			case 1: guit.getGroup(params_Standalone.getName()).maximize(); break;
			case 2: guit.getGroup(params_Forced.getName()).maximize(); break;
			}
		}
#endif

		//workflow
		switch (indexModes)
		{
			//case 0: bPlayExternal = true; break;
			//case 1: bPlayStandalone = true; break;
			//case 2: bPlayForced = true; break;

			//workflow
		case 0: indexModes_Name.setWithoutEventNotifications("EXTERNAL"); break;
		case 1: indexModes_Name.setWithoutEventNotifications("STANDALONE"); break;
		case 2: indexModes_Name.setWithoutEventNotifications("FORCED"); break;
		case 3: indexModes_Name.setWithoutEventNotifications("MANUAL"); break;
		}
	}

	// Dialog index
	else if (name == currentDialog.getName())
	{
		if (indexModes == 3 && bPlayManual) return;

		//return;//bypass

		// Avoid / clamp errors
		if (sub.size() == 0)
		{
			currentDialog.setWithoutEventNotifications(0);
			textCurrent = "NO_TEXT";
			ofLogError("ofxSurfingTextSubtitle") << "Not loaded subs file or it's empty or wrong format.";
			return;
		}
		if (currentDialog < 0)
		{
			currentDialog.setWithoutEventNotifications(0);//go to first
			//currentDialog.setWithoutEventNotifications(sub.size() - 1);//last. cycled
			return;
		}
		else if (currentDialog > sub.size() - 1)
		{
			currentDialog.setWithoutEventNotifications(0);//go to first
			textCurrent = "NO_TEXT";
			ofLogError("ofxSurfingTextSubtitle") << "Current sub index out of range!";
			return;
		}

		//--

		// Get dialog
		if (currentDialog.get() < sub.size())
		{
			string s = sub[currentDialog.get()]->getDialogue();
			if (bCapitalize) textCurrent = ofToUpper(s);
			else textCurrent = s;

			//TODO: fail
			/*
			if (indexModes==0)
			//if (bPlayExternal)
			{
				// get the position normalized
				auto t = sub[currentDialog.get()]->getStartTime();
				auto pos = t / (float)tEndSubsFilm;
				if (pos != positionExternal) positionExternal = pos;
				//positionExternal.setWithoutEventNotifications(pos);
			}
			*/
		}

		//--

		// Fades

		if (bAnimatedIn && bAnimatedOut)
		{
			isAnimIn = true;
			alpha = 0.f;
			if (isAnimOut) isAnimOut = false;
		}
		else if (bAnimatedIn && !bAnimatedOut)
		{
			isAnimIn = true;
			alpha = 0.f;
			//if (bPlayStandalone || bPlayForced) alpha = 1.f;//should force play
		}
		else if (!bAnimatedIn && bAnimatedOut) {
			alpha = 1.f;
		}
		else if (!bAnimatedIn && !bAnimatedOut) {
			alpha = 1.f;
		}

		/*
		if (bAnimatedOut)
		{
			isAnimOut = false;
		}
		*/

		if (bDebug && !bLive) ofLogNotice("ofxSurfingTextSubtitle") << textCurrent;
		else ofLogVerbose("ofxSurfingTextSubtitle") << textCurrent;
	}

	//--

	// TODO: for video link mode
	else if (name == bPlayExternal.getName())
	{
		if (bPlayExternal)
		{
			if (bPlayStandalone) bPlayStandalone = false;
			if (bPlayForced) bPlayForced = false;
			if (bPlayManual) bPlayManual = false;

			progressPlaySlide = 0;

			//workflow
			//if (indexModes != 0) indexModes.setWithoutEventNotifications(0);
			if (indexModes != 0) indexModes.set(0);
		}
		else {
		}
	}

	else if (name == bPlayStandalone.getName())
	{
		if (bPlayStandalone)
		{
			//bPlayExternal.setWithoutEventNotifications(false);
			bPlayForced.set(false);
			//bPlayForced.setWithoutEventNotifications(false);
			tPlay = ofGetElapsedTimeMillis();
			//currentDialog = 0;//crash
			currentDialog.setWithoutEventNotifications(0);

			progressPlaySlide = 0;

			if (bPlayExternal) bPlayExternal = false;
			if (bPlayManual) bPlayManual = false;

			//workflow
			//if (indexModes != 1) indexModes.setWithoutEventNotifications(1);
			if (indexModes != 1) indexModes.set(1);

		}
		else
		{
			//return;
			currentDialog.setWithoutEventNotifications(0);
			//currentDialog = 0;//crash

			if (bAnimatedIn && isAnimIn) isAnimIn = false;
			if (bAnimatedOut && isAnimOut) isAnimOut = false;
			//if (!bAnimatedIn && !bAnimatedOut) alpha = 1;//?

			progressPlaySlide = 0;
		}
	}

	else if (name == bPlayForced.getName())
	{
		if (bPlayForced)
		{
			bPlayStandalone.set(false);
			//bPlayStandalone.setWithoutEventNotifications(false);
			//bPlayExternal.setWithoutEventNotifications(false);

			tPlayForce = ofGetElapsedTimeMillis();
			tPlayForceFilm = ofGetElapsedTimeMillis();

			progressPlaySlide = 0;

			//bPlayExternal = false;
			if (bPlayExternal) bPlayExternal = false;
			if (bPlayManual) bPlayManual = false;

			//workflow
			if (indexModes != 2) indexModes.set(2);
			//if (indexModes != 2) indexModes.setWithoutEventNotifications(2);

		}
		else
		{
			tPlayForceFilm = 0;
			progressPlaySlide = 0;

			//isAnimIn = false;
			//isAnimOut = false;
			if (bAnimatedIn && isAnimIn) isAnimIn = false;
			if (bAnimatedOut && isAnimOut) isAnimOut = false;
			//alpha = 1;
		}
	}

	else if (name == bPlayManual.getName())
	{
		if (bPlayManual)
		{
			tPlayForce = ofGetElapsedTimeMillis();
			//tPlayForceFilm = ofGetElapsedTimeMillis();

			if (bPlayStandalone) bPlayStandalone = false;
			if (bPlayForced) bPlayForced = false;
			if (bPlayExternal) bPlayExternal = false;

			progressPlaySlide = 0;

			//workflow
			//if (indexModes != 0) indexModes.setWithoutEventNotifications(0);
			if (indexModes != 3) indexModes.set(3);
		}
		else {
		}
	}

	//--

	else if (name == bStop.getName())
	{
		this->stop();
	}

	//--

	// dt

	//else if (name == speedFadeIn.getName())
	//{
	//	dtAnimIn = ofMap(speedFadeIn, 0, 1, dt * DT_RATIO_MIN, dt * DT_RATIO_MAX, true);
	//}
	else if (name == durationIn.getName())
	{
		auto _numframes = durationIn / (float)(dt * 1000);
		float _margin = 1.f; // fade in increment per frame. from 0 to 1 or vice versa.
		dtAnimIn = _margin / (float)_numframes;
	}
	else if (name == durationOut.getName())
	{
		auto _numframes = durationOut / (float)(dt * 1000);
		float _margin = 1.f; // fade out increment per frame. from 0 to 1 or vice versa.
		dtAnimOut = _margin / (float)_numframes;
	}

	else if (name == bAnimatedIn.getName())
	{
		if (!bAnimatedIn.get()) {
			alpha = 1;
			isAnimIn = false;
			//	if (bAnimatedOut.get()) alpha = 1;
		}
	}

	else if (name == bAnimatedOut.getName())
	{
		if (bAnimatedOut.get()) {
			if (!bAnimatedIn.get()) alpha = 1;
		}
	}

	//--

	// next
	else if (name == bNext.getName() && bNext)
	{
		bNext.setWithoutEventNotifications(false);
		currentDialog++;

		//else {
		//TODO: fail
		/*
		if (bPlayExternal)
		{
			// get the position normalized
			auto t = sub[currentDialog.get()]->getStartTime();
			auto pos = t / (float)tEndSubsFilm;
			if (pos != positionExternal) positionExternal = pos;
			//positionExternal.setWithoutEventNotifications(pos);
		}
		*/
		//}
	}
	// prev
	else if (name == bPrev.getName() && bPrev)
	{
		bPrev.setWithoutEventNotifications(false);
		currentDialog--;
	}

	// edit 
	else if (name == bEdit.getName()) {
		box.bEdit = bEdit;
	}

	// debug
	else if (name == bDebug.getName()) {
		if (!bDebug) box.setBorderColor(ofColor(0, 0));
		else box.setBorderColor(ofColor(bTheme ? colorDebugLight : colorDebugDark, 64));
	}

	// theme
	else if (name == bTheme.getName())
	{
		box.setBorderColor(ofColor(bTheme ? colorDebugLight : colorDebugDark, 64));
	}

	//--

	// Font Styles

	else if (name == bCapitalize.getName())
	{
		if (bLoaded || (!bModeNoSrt && indexModes != 3))//not manual mode
		{
			if (bCapitalize) textCurrent = ofToUpper(sub[currentDialog.get()]->getDialogue());
			else textCurrent = sub[currentDialog.get()]->getDialogue();
			buildSubsData();
		}
		else if (bModeNoSrt && indexModes == 3)//manual mode
		{
			if (bCapitalize) textCurrent = ofToUpper(lastTextSlideRaw);
			else textCurrent = lastTextSlideRaw;
		}
	}

	else if (name == fSizePrc.getName())
	{
		fSize = fSizePrc * fSize.getMax();
		//fSize = fSizePrc * box.getWidth() / 5;
	}
	// spacing
	else if (name == fSpacing.getName())
	{
		font.setCharacterSpacing(fSpacing);
	}
	// height
	else if (name == fLineHeight.getName())
	{
		font.setLineHeight(fLineHeight);
	}
	// alignment
	else if (name == fAlign.getName())
	{
		fAlign_str = getAlignNameFromIndex(fAlign.get());
	}

	// reset style
	else if (name == bReset.getName() && bReset.get())
	{
		bReset = false;
		doReset();
	}
	// reset fades
	else if (name == bResetFades.getName() && bResetFades.get())
	{
		bResetFades = false;
		doResetFades();
	}

	//TODO:
	// box resize
	else if (name == amountLinesTarget.getName())
	{

	}

	// Open
	else if (name == bOpen.getName())
	{
		doOpenFile();

		if (bModeNoSrt) bModeNoSrt = false;
	}

	// Font
	else if (name == fPath.getName())
	{
		loadFont(fPath.get());
	}

	//--

	// workflow
	/*
	else if (name == bResponsive.getName())
	{
		if (bResponsive) bCenteredV = false;
	}

	else if (name == bCenteredV.getName())
	{
		if (bCenteredV) bResponsive = false;
	}
	*/
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::keyPressed(int key)
{
	if (!bKeys) return;

#ifdef USE_IM_GUI__SUBTITLES
	if (ui->bOverInputText.get()) return;
#endif

	if (key == 'l') { setToggleLive(); }
	if (key == 'g') { setToggleVisibleGui(); }
	if (key == 'e') { setToggleEdit(); }
	if (key == 'd') { setToggleDebug(); }
	if (key == '.') { stop(); }
	if (key == OF_KEY_TAB) { setToggleAlign(); }

	// Browse subs
	if (key == OF_KEY_RIGHT) { setSubtitleNext(); }
	if (key == OF_KEY_LEFT) { setSubtitlePrevious(); }
	if (key == OF_KEY_BACKSPACE) { setSubtitleRandomIndex(); };

	// Play both!
	//if (key == OF_KEY_RETURN) { setTogglePlayForced(); }
	if (key == ' ') { setTogglePlay(); }
}


//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doOpenFile()
{
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a .srt file.");

	//Check if the user opened a file
	if (openFileResult.bSuccess) {

		ofLogNotice("ofxSurfingTextSubtitle") << ("User selected a file");

		//We have a file, check it and process it
		processOpenFileSelection(openFileResult);
	}
	else {
		ofLogNotice("ofxSurfingTextSubtitle") << ("User hit cancel");
	}

	//TODO:
	indexModes = indexModes.get();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::processOpenFileSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("ofxSurfingTextSubtitle") << ("Name: " + openFileResult.getName());
	ofLogNotice("ofxSurfingTextSubtitle") << ("Path: " + openFileResult.getPath());

	string path = openFileResult.getPath();
	ofFile file(openFileResult.getPath());

	if (file.exists()) {

		ofLogNotice("ofxSurfingTextSubtitle") << ("The file exists - now checking the type via file extension");
		string fileExtension = ofToUpper(file.getExtension());

		//We only want 
		if (fileExtension == "SRT")
		{
			//load subs
			setupSubs(path);

			//workflow
			//bPlayStandalone = true;
			//bPlayForced = true;
		}
	}
}

//--

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setTogglePlayForced() { bPlayForced = !bPlayForced; }

//--------------------------------------------------------------
bool ofxSurfingTextSubtitle::isPlaying() const {
	return bPlayStandalone.get();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setTogglePlay()
{
	if (indexModes == 0) {
		// external
	}
	else if (indexModes == 1) {
		bPlayStandalone = !bPlayStandalone;
	}
	else if (indexModes == 2) {
		bPlayForced = !bPlayForced;
	}
	else if (indexModes == 3) {
		bPlayManual = !bPlayManual;
	}

	//#ifdef USE_WIDGET__VIDEO_PLAYER
	//	if (bPlayStandalone) player.play();
	//	else player.stop();
	//#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::play() {
	bPlayStandalone = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::stop() {
	if (bPlayStandalone) bPlayStandalone = false;
	if (bPlayForced) bPlayForced = false;
	if (bPlayManual) bPlayManual = false;

	if (isAnimIn) isAnimIn = false;
	if (isAnimOut) isAnimOut = false;

	textCurrent = "";
	currentDialog = 0;
	positionExternal = 0;
	progressPlaySlide = 0;
	progressPlayFilm = 0;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::pause() {
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doSetTextSlide(string s) {
	bool b = indexModes == 3 && bPlayManual;

	lastTextSlideRaw = s;//store raw to allow hot upper/lower capitalize

	// workflow
	// A.force mode
	if (!b)
	{
		if (indexModes != 3) indexModes = 3;
		if (!bPlayManual) bPlayManual = true;
	}

	// workflow
	// B. Lock if wrong mode
	//if (b)
	{
		if (bCapitalize) textCurrent = ofToUpper(s);
		else textCurrent = s;

		progressPlaySlide = 0;
		ofLogNotice("ofxSurfingTextSubtitle") << "doSetTextSlide: " << textCurrent;

		//--

		// Fades

		if (bAnimatedIn && bAnimatedOut)
		{
			isAnimIn = true;
			alpha = 0.f;
			if (isAnimOut) isAnimOut = false;
		}
		else if (bAnimatedIn && !bAnimatedOut)
		{
			isAnimIn = true;
			alpha = 0.f;
			//if (bPlayStandalone || bPlayForced) alpha = 1.f;//should force play
		}
		else if (!bAnimatedIn && bAnimatedOut) {
			alpha = 1.f;
		}
		else if (!bAnimatedIn && !bAnimatedOut) {
			alpha = 1.f;
		}

		tPlayStartSlide = ofGetElapsedTimeMillis();
		isSlidePlaying = true;
		durationPlaySlide = durationPlayForced;
	}
	// workflow
	//else {
	//	ofLogWarning("ofxSurfingTextSubtitle") << "Ignoring doSetTextSlide bc not in MANUAL mode";
	//}
}