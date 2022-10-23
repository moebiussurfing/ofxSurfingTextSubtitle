#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
ofxSurfingTextSubtitle::ofxSurfingTextSubtitle() {
	bGui.set("SUBTITLES", true);
	bGui_SrtFull.set("SRT", false);
	bGui_Internal.set("Gui Internal", true);
};

//--------------------------------------------------------------
ofxSurfingTextSubtitle::~ofxSurfingTextSubtitle() {
	exit();
};

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
	box.setBorderColor(bTheme ? colorDebugLight : colorDebugDark);
	box.setup();

#ifdef USING_OFX_TIME_CODE
	timecode.setFPS(fps);
#endif

	//--

	// Internal Gui
	ofxSurfingHelpers::setThemeDarkMini_ofxGui();
	gui.setup(params);
	////gui.setup("TEXT SUBTITLE");
	//gui.setup(params.getName());
	//gui.add(params);

	//gui.minimize();
	gui.getGroup(params_Control.getName()).minimize();
	gui.getGroup(params_Transport.getName()).minimize();
	auto& gf = gui.getGroup(params_Fade.getName());
	gf.minimize();
	gf.getGroup(params_FadeIn.getName()).minimize();
	gf.getGroup(params_FadeOut.getName()).minimize();
	gui.getGroup(params_Style.getName()).minimize();

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

	//startup();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupFont()
{
	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__);

	float _dpiScale = 2.0;
	bool _mipmaps = true;
	int _textDimension = 2048;

	fName = "-1";
	//fName = "ExtendedBold";

	// search some fonts alternatives
	fPath = "fonts/GTAmerica-ExtendedBlack.ttf";
	if (!ofFile::doesFileExist(fPath.get())) {
		fPath = FONT_FILES_PATH + ofToString(FONT_FILE_BIG);
		if (!ofFile::doesFileExist(fPath.get())) {
			fPath = FONT_FILES_PATH + ofToString(FONT_FILE_SMALL);
			if (!ofFile::doesFileExist(fPath.get())) {
				fPath = FONT_FILES_PATH + ofToString(FONT_FILE_ALT1);
				if (!ofFile::doesFileExist(fPath.get())) {
					fPath = FONT_FILES_PATH + ofToString(FONT_FILE_ALT2);
					if (!ofFile::doesFileExist(fPath.get())) {
						fPath = OF_TTF_SANS;
					}
				}
			}
		}
	}

	bool b = font.setup(fPath, 1.0, _textDimension, _mipmaps, 8, _dpiScale);
	if (!b) ofLogError("ofxSurfingTextSubtitle") << "Font file not found: " << fPath;

	if (b) {
		auto ss = ofSplitString(fPath, "/");
		if (ss.size() >= 2) fName = ss[1];
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupParams()
{
	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__);

	setupFont();

	//--

	// Control
	indexModes.set("Modes", 0, 0, 2);
	bCapitalize.set("Capitalize", false);
	bMinimize.set("Minimize", false);
	bFontResponsive.set("FontResponsive", false);
	bStop.set("Stop");
	bOpen.set("Open");
	bDraw.set("Draw", true);
	bLive.set("Live", false);
	bEdit.set("Edit", true);
	bDebug.set("Debug", true);
	bTop.set("Top", false);
#ifdef USE_WIDGET__SUBTITLES
	bDrawWidgetInfo.set("Info", true);
#endif
	currentDialog.set("Dialog", 0, 0, 0);
	bPrev.set("<", false);
	bNext.set(">", false);
	bPlay.set("Play", false);
	bPlayForced.set("PlayForced", false);
	durationPlayForced.set("DurationForced", 2000, 1, 4000);
	//speedPlayForce.set("Speed", 0, 0, 1);
	bPlayExternal.set("PlayExternal", false);
	tPosition.set("Position", 0, 0, 1);

	progressPlayFilm.set("% Film", 0, 0, 1);//full length progress
	progressPlaySlide.set("% Slide", 0, 0, 1);//slide progress

	bAutoScroll.set("Auto Scroll", true);//for ImGui only

	// Fade
	bAnimatedIn.set("Enable In", false);
	speedFadeIn.set("SpeedIn", 0, 0, 1);
	bAnimatedOut.set("Enable Out", false);
	speedFadeOut.set("SpeedOut", 0, 0, 1);
	countDownOut.set("CountDown", 1000, 1, 1000);
	progressIn.set("% In", 0, 0, 1);
	progressOut.set("% Out", 0, 0, 1);
	bResetFades.set("Reset Fades", false);

	// Style
	fSize.set("SizeR", 50, 5, (float)MAX_FONT_SIZE);
	fSizePrc.set("Size", 0.2, 0.1, 1.0f);
	fSpacing.set("Spacing", 0, -20, 50);
	fLineHeight.set("Height", 0.75, 0.5, 2.0);
	//fColorBg.set("ColorBg", ofColor::gray, ofColor(0, 0), ofColor(255, 255));
	//fColorTxt.set("Color", ofColor::white, ofColor(0, 0), ofColor(255, 255));
	fColorBg.set("ColorBg", ofFloatColor::gray, ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	fColorTxt.set("Color", ofFloatColor::white, ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	fAlign.set("Align", 1, 1, 3);
	fAlign_str.set("Align ", "-1");
	bResetFont.set("Reset Style", false);

	bvCentered.set("V Centered", true);
	amountLinesTargetCentered.set("Lines", 6, 1, 10);

	box.bGui.makeReferenceTo(bEdit);
	//box.bEdit.makeReferenceTo(bEdit);
//#ifdef USE_WIDGET__SUBTITLES
//	boxInfo.bGui.makeReferenceTo(bEdit);
//#endif

	//--

	//TODO: crashes when loading settings if uncommented!
	//bPlay.setSerializable(false);//!
	//bPlayForced.setSerializable(false);

	fName.setSerializable(false);
	fPath.setSerializable(false);
	fAlign_str.setSerializable(false);
	bResetFont.setSerializable(false);
	bResetFades.setSerializable(false);
	bNext.setSerializable(false);
	bPrev.setSerializable(false);
	currentDialog.setSerializable(false);

	progressPlayFilm.setSerializable(false);
	progressPlaySlide.setSerializable(false);

	//we use index mode
	bPlay.setSerializable(false);
	bPlayExternal.setSerializable(false);
	bPlayForced.setSerializable(false);

	//--

	params_Control.setName("Control");
	params_Control.add(bOpen);
	params_Control.add(bDraw);
	params_Control.add(bLive);
	params_Control.add(bEdit);
	params_Control.add(bDebug);
	params_Control.add(bTheme);
	params_Control.add(bTop);
#ifdef USE_WIDGET__SUBTITLES
	params_Control.add(bDrawWidgetInfo);
#endif
#ifdef USE_WIDGET__VIDEO_PLAYER
	params_Control.add(player.bGui_VideoPlayer);
	params_Control.add(bLoadBothVideoAndSubs);
#endif

#ifdef USE_IM_GUI__SUBTITLES
	params_Control.add(bGui_SrtFull);
	params_Control.add(bMinimize);
#endif
	//params_Control.add(bGui);
	//params_Control.add(box.bEdit);

	params_Transport.setName("Transport");
	params_Transport.add(currentDialog);
	params_Transport.add(progressPlaySlide);
	params_Transport.add(bStop);
	params_Transport.add(indexModes);
	params_Transport.add(bPlay);
	params_Transport.add(bPlayForced);
	params_Transport.add(durationPlayForced);
	params_Transport.add(bPrev);
	params_Transport.add(bNext);
	params_Transport.add(bPlayExternal);
	params_Transport.add(tPosition);

	//params_Transport.add(progressPlayFilm);
	//params_Transport.add(speedPlayForce);

	// Fades
	params_Fade.setName("Fades");
	params_Fade.add(bAnimatedIn);
	params_Fade.add(bAnimatedOut);
	params_Fade.add(progressPlaySlide);
	params_FadeIn.setName("In");
	params_FadeOut.setName("Out");
	params_FadeIn.add(speedFadeIn);
	params_FadeIn.add(progressIn);//TODO: should change speed mode to time duration. like on fade out!
	params_FadeOut.add(speedFadeOut);
	params_FadeOut.add(progressOut);
	params_FadeOut.add(countDownOut);
	params_Fade.add(params_FadeIn);
	params_Fade.add(params_FadeOut);
	params_Fade.add(bResetFades);
	//params_Transport.add(params_Fade);

	params_Style.setName("Style");
	params_Style.add(fName);
	params_Style.add(fColorBg);
	params_Style.add(fColorTxt);
	params_Style.add(fSizePrc);
	params_Style.add(fSpacing);
	params_Style.add(fLineHeight);
	params_Style.add(fAlign);
	params_Style.add(fAlign_str);
	params_Style.add(bvCentered);
	params_Style.add(amountLinesTargetCentered);
	params_Style.add(bCapitalize);
	params_Style.add(bFontResponsive);
#ifdef USE_IM_GUI__SUBTITLES
	params_Style.add(bFine);
#endif
	params_Style.add(bResetFont);

	params.setName(bGui.getName());//TODO: BUG: crashes
	//params.setName("TEXT SUBTITLE");
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
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupSubs(string _pathSrt) {
	path_Srt = _pathSrt;

	ofFile file(ofToDataPath(path_Srt));
	name_Srt = file.getBaseName();

	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__) << path_Srt;

	subParserFactory = new SubtitleParserFactory(ofToDataPath(path_Srt));

	if (!subParserFactory) ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << path_Srt;

	//bGui_SrtFull.setName(path_Srt);

	parser = subParserFactory->getParser();
	sub = parser->getSubtitles();
	if (sub.size() == 0) ofLogError("ofxSurfingTextSubtitle") << "SUB object empty!";

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
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::buildSubsData()
{
	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__) << path_Srt;

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
	//return;
	doResetFont();

	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__);
	ofxSurfingHelpers::loadGroup(params, path_SubtitlerSettings);

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.startup();
#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::exit() {
	ofLogNotice("ofxSurfingTextSubtitle") << "exit";
	ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

	ofxSurfingHelpers::saveGroup(params, path_SubtitlerSettings);

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.exit();
#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updatePos(float position)
{
	tPosition.set(position);
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
	//if (!bPlay) bPlay = true;

	if (bPlay)
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
	// Calculate Fade In

	if (bPlay || bPlayForced || bPlayExternal)
	{
		if (bAnimatedIn && isAnimIn)
		{
			// make opacity grow 
			if (alpha < 1.f)
			{
				static float dt = 1 / fps;
				dtAnim = ofMap(speedFadeIn, 0, 1, dt * 0.4f, dt * 7, true);
				alpha += dtAnim;

				//--

				/*
				uint64_t tSlide;
				if (bPlayForced) tSlide = ofGetElapsedTimeMillis() - tPlayForce;
				else if (bPlay || bPlayExternal) tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;
				*/

				//TODO:
				if (bPlayForced)
				{
					progressIn = ofClamp(alpha, 0, 1);//workaround to avoid deep calculations..
					//progressIn = ofMap(tSlide, 0, durationPlayForced, 0, 1, true);
				}
				else if (bPlay || bPlayExternal)
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
		if (bPlay || bPlayForced || bPlayExternal)
		{
			uint64_t tSlide = 0;

			if (bPlayForced) tSlide = ofGetElapsedTimeMillis() - tPlayForce;
			else if (bPlay || bPlayExternal) tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;

			if (!isAnimOut) // checking if requires trig fade out
			{
				// trigs fade out start

				if (bPlayForced)
				{
					if (tSlide > durationPlayForced - countDownOut)
					{
						isAnimOut = true;
						isSlidePlaying = true;
					}
				}
				else if (bPlay || bPlayExternal)
				{
					if (tSlide > durationPlaySlide - countDownOut)
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
				uint64_t t = tSlide - countDownOut;

				if (bPlayForced)
				{
					progressOut = ofClamp(alpha, 0, 1);
					//progressOut = ofMap(t, 0, countDownOut, 1, 0, true);
				}
				else if (bPlay || bPlayExternal)
				{
					progressOut = ofClamp(alpha, 0, 1);
					//progressOut = ofMap(t, 0, countDownOut, 1, 0, true);
				}
				//else progressOut = 1;

				if (alpha > 0.f)//decrease opacity 
				{
					static float dt = 1.f / fps;
					dtAnim = ofMap(speedFadeOut, 0, 1, dt * 0.4f, dt * 7.f, true);
					alpha -= dtAnim;
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
	if (ofGetFrameNum() == 1) startup();

	//--

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.update();
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

	{
		uint64_t t = 0;

		if (bPlay)
		{
			if (!bPlayExternal)
			{
				t = ofGetElapsedTimeMillis() - tPlay;
			}
		}

		if (bPlayExternal)
		{
			t = tPosition * (float)tEndSubsFilm;
		}

		// find the dialog relative to current time
		int k = 0;
		for (SubtitleItem* element : sub)
		{
			if (t > element->getStartTime() && t <= element->getEndTime())
			{
				// To apply only once if currentDialog changed!
				if (currentDialog.get() != k)
				{
					//changed
					currentDialog = k;//will trig the callback!
					doUpdateSlidePlay(element);
				}
				break;//done found
			}
			k++;
		}

		//--

		if (isSlidePlaying && (bPlay || bPlayExternal))
		{
			uint64_t tp = ofGetElapsedTimeMillis() - tPlayStartSlide;
			progressPlaySlide = ofMap(tp, 0, durationPlaySlide, 0, 1);
		}
		else
		{
			progressPlaySlide = 0;
		}
	}

	//--

	// bPlayForced

	if (bPlayForced)
	{
		uint64_t tf = ofGetElapsedTimeMillis() - tPlayForce;
		if (tf > durationPlayForced) {
			tPlayForce = ofGetElapsedTimeMillis();//restart timer. next slide
			currentDialog++;
		}
		progressPlaySlide = ofMap(tf, 0, durationPlayForced, 0, 1);
	}

	if (bPlayExternal) {

	}
	else {
		if (bPlayForced) {
			tPosition = ofMap(ofGetElapsedTimeMillis() - tPlayForceFilm, 0, tEndSubsFilm, 0, 1, false);
		}
		if (bPlay) {
			tPosition = ofMap(ofGetElapsedTimeMillis() - tPlay, 0, tEndSubsFilm, 0, 1, false);
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
		ofLogVerbose("ofxSurfingTextSubtitle") << "SUB object empty!";
		progressPlayFilm = 0;
	}
	else
	{
		if (bPlay) // progress related to the elapsed player time
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
				progressPlaySlide = ofMap(t, ts, te, 0, 1);
			}
			else
			{
			}

			progressPlayFilm = ofMap(t, 0, tEndSubsFilm, 0, 1);
		}

		//else if (bPlayForced || !bPlay)
		else // for any other modes
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

			if (bPlay)
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

			if (bPlayForced)
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

			if (bPlayExternal)
			{
				t = tPosition * (float)tEndSubsFilm;
			}

			//--

			// convert formatting
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

			// display begin/end subs times
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

	if (!bGui_Internal) return;
	gui.draw();

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

	// v centered disabled
	if (!bvCentered)
	{
		drawTextBox(textCurrent, box.getRectangle(), true);
	}

	// v centered enabled
	else
	{
		ofRectangle r = getTextBox(textCurrent, box.getRectangle());
		float _offset = 0;

		//--

		ofPushMatrix();
		{
			// Force box height
			float h = getOneLineHeight(true) + getSpacingBetweenLines();
			int n = amountLinesTargetCentered;
			//h -= getSpacingBetweenLines();
			box.setHeight(n * h);

			// Translate
			//if (amountLinesDrawn <= amountLinesTargetCentered)
			{
				int _offsetLines = (amountLinesTargetCentered - amountLinesDrawn) / 2.f;
				_offset = (getOneLineHeight() + getSpacingBetweenLines()) * _offsetLines;
				ofTranslate(0, _offset);
			}

			// Draw
			drawTextBox(textCurrent, box.getRectangle(), true);
		}
		ofPopMatrix();

		//--

		// Mark Center Point
		if (!bLive)
		{
			if (bDebug && bDebug2)
			{
				float _x = box.getX();
				float _y = box.getY();

				_x += box.getWidth() / 2;
				_y += _offset;
				_y += r.getHeight() / 2;

				drawInsertionPoint(_x, _y, 8, 8);
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawDebug()
{
	if (!bLive)
	{
		if (bEdit && bDebug)
		{
			//fill
			//ofPushStyle();
			//ofSetColor(fColorBg);
			//ofFill();
			//ofDrawRectangle(box.getRectangle());
			//ofPopStyle();

			//box.setBorderColor(bTheme ? colorDebugLight : colorDebugDark);
			box.drawBorderBlinking();
		}

		// debug
			// alpha preview
		if (bDebug && (bAnimatedIn || bAnimatedOut) && (isAnimIn || isAnimOut))
		{
			bool bCenter = true;//or left

			float x;
			float y;
			float pad;
			float w = 78;//hard coded to text widget
			float h;//bar
			glm::vec2 p;

			// top
			if (bTop)
			{
				p = box.getRectangle().getTopLeft();
				h = 6;
				pad = 0;
				if (bCenter)x = p.x + box.getWidth() / 2.f - w / 2;
				else x = p.x;
				//else x = p.x - 1;
				y = p.y - pad - h - 1;
				//y = p.y - pad - h - 2;
			}
			// bottom
			else
			{
				p = box.getRectangle().getBottomLeft();
				h = 6;
				pad = -4;
				//x = p.x - 1;
				if (bCenter)x = p.x + box.getWidth() / 2.f - w / 2;//center
				else x = p.x;//left
				y = p.y + 15 + pad + h;
			}

			//filled
			ofPushStyle();
			ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 255);
			ofFill();
			ofDrawRectangle(x, y, w * alpha, h);
			//border
			ofNoFill();
			ofDrawRectangle(x, y, w, h);
			ofPopStyle();

			float h2 = 15;
			//x += 3;
			x += 4;
			y -= h;
			y += 3;
			string s = (bAnimatedIn && isAnimIn) ? "IN" : "  ";
			s += "    ";
			s += (bAnimatedOut && isAnimOut) ? "OUT" : "   ";
			if (bTheme) ofDrawBitmapStringHighlight(s, x, y, 255, 0);
			else ofDrawBitmapStringHighlight(s, x, y);
		}

		//--

		// Timeline: 

		// thin line
		if (bDebug && (bPlay || bPlayForced || bPlayExternal))
		{
			ofPushStyle();

			float lw1 = 3;
			float h = 5;
			//float h = 3;
			auto p = bTop.get() ? box.getRectangle().getTopLeft() : box.getRectangle().getBottomLeft();
			float w = box.getRectangle().getWidth() + 1;
			float x = p.x;
			float y = p.y;
			//float y = p.y - h / 2;

			// slide progress
			ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 255);
			ofSetLineWidth(h);
			ofDrawLine(x - 1, y, x - 1 + (w + 1) * MIN(progressPlaySlide, 1), y);
			//float y = p.y - h;
			//ofDrawRectangle(x, y, w * MIN(progressPlaySlide, 1), h);

			// Phase fade in/out lines
			float sz = 6;
			float dt = 1 / fps;

			float xIn = 0;
			float xOut1 = 0;
			float xOut2 = 0;

			float szt = 12;

			//TODO:
			// mark fade in phase end point with a vertical line
			// hard to do bc using speed instead of time duration in ms!

			if (bAnimatedIn)
			{
				dtAnim = ofMap(speedFadeIn, 0, 1, dt * 0.4f, dt * 7, true);
				float d = 1.f / dtAnim;

				// estimated duration of the fade in phase,
				// bc we are applying this amount of the above
				// dt on every single frame!
				int t = (d / fps) * 1000;

				float r;
				if (bPlayForced)
				{
					r = (t / (float)durationPlayForced);//ratio
				}
				if (bPlay || bPlayExternal)
				{
					r = (t / (float)durationPlaySlide);//ratio
				}
				xIn = p.x + w * r;

				if (xIn != -1)
				{
					// zero
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

					// line
					ofSetLineWidth(lw1);
					ofDrawLine(xIn + lw1 / 2, p.y - szt, xIn + lw1 / 2, p.y);
					//ofDrawLine(xIn, p.y - szt, xIn, p.y);
					//T
					//ofDrawLine(xIn, p.y - szt, xIn, p.y + szt);
					//ofDrawLine(xIn - szt / 2, p.y - szt, xIn + szt / 2, p.y - szt);

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

				// fade out begins
				float r = 0.f;
				if (bPlayForced)
				{
					r = 1.f - (countDownOut / (float)durationPlayForced);//ratio
				}
				if (bPlay || bPlayExternal)
				{
					r = 1.f - (countDownOut / (float)durationPlaySlide);//ratio
				}
				xOut1 = p.x + w * r;

				//// line
				//ofDrawLine(xOut1, p.y - sz, xOut1, p.y + sz);

				// triangle
				ofFill();
				ofSetPolyMode(OF_POLY_WINDING_NONZERO);
				ofBeginShape();
				float o = 0;
				ofVertex(xOut1, p.y + o);
				ofVertex(xOut1 - szt, p.y - szt);
				ofVertex(xOut1 - szt, p.y + o);
				ofEndShape();

				//--

				// fade out ends
				float pixPerMillis = 0;
				if (bPlayForced)
				{
					pixPerMillis = w / (float)durationPlayForced;//ratio
				}
				if (bPlay || bPlayExternal)
				{
					pixPerMillis = w / (float)durationPlaySlide;//ratio
				}
				dtAnim = ofMap(speedFadeOut, 0, 1, dt * 0.4f, dt * 7.f, true);
				float d = 1.f / dtAnim;
				int t = (d / fps) * 1000;//duration required to fade out until 0!
				xOut2 = xOut1 + t * pixPerMillis;
				if (xOut2 < p.x + w) ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 255);
				else ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 32);//attenuate if goes out of the  time line!
				//sz -= 2;
				//ofSetLineWidth(2);
				//ofDrawLine(xOut2, p.y - sz, xOut2, p.y + sz);
				ofFill();
				ofDrawCircle(xOut2, p.y, 3);

				//--

				// Make fatter the line for the full opacity middle section!
				// full alpha zone
				int lw = 4;
				int off = 0;
				ofSetLineWidth(lw);
				ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 128);
				ofDrawLine(xIn, p.y + off, xOut1, p.y + off);
			}

			ofPopStyle();
		}
	}
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

	if (!bLive) box.draw();

	drawDebug();
}

//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::drawTextBox(std::string _str, ofRectangle r, bool bRaw)
{
	// NOTE that uses only the rectangle width! not the height!

	//TODO:
	//bRaw not used. remove?

	float _x = box.getX();
	float _y = box.getY();
	float _w = box.getWidth();
	float _h = box.getHeight();

	//ofFloatColor _color;
	ofColor _color;

	int _align;
	int _size;

	if (!bFontResponsive) 
	{ 
		_size = fSize.get(); 
	}
	else 
	{
		if (bvCentered) {};

		int diff = 0;
		if (amountLinesDrawn >= amountLinesTargetCentered)
		{
			_size = fSize.get();
		}
		else
		{
			diff = amountLinesTargetCentered - amountLinesDrawn;
			_size = ofMap(diff, 0, amountLinesTargetCentered, fSize.get(), amountLinesTargetCentered * fSize.get() * (amountLinesTargetCentered - diff), true);
		}
	} 

	_align = fAlign.get();

	_color = fColorTxt.get();

	//if ((bAnimatedIn && isAnimIn) || (bAnimatedOut && isAnimOut))
	if ((bAnimatedIn) || (bAnimatedOut))
	{
		//_color = ofFloatColor(_color.r, _color.g, _color.b, alpha * _color.a);
		_color = ofColor(_color, alpha * _color.a);
	}

	_y += getOneLineHeight(); // here upper border is aligned to the center horizontal
	//_y += 2; // fix. workaround to hard code calibrate.

	//------

	ofRectangle _bbox;

	ofPushStyle();
	{
		ofSetColor(_color); // don't do nothing. only applies the style

		int _numLines = 20;
		bool _wordsWereCropped = false;
		bool _bCenter = ((_align == 3) ? true : false);//centered

		/*get the final text formatting (by adding \n's) in the supplied string;
		BE ARWARE that using TRUE in here will modify your supplied string! */
		//bool _br = false;
		bool _br = bForceAddBreakLines;

		if (fAlign != 2) // all (left-center) except right align
		{
			_bbox = font.drawMultiLineColumn(
				_str,					/*string*/
				_size,					/*size*/
				_x, _y,					/*where*/
				MAX(10, _w),			/*column width*/
				_numLines,				/*get back the number of lines*/
				false,					/* if true, we wont draw (just get bbox back) */
				20,						/* max number of lines to draw, crop after that */
				_br,					/*get the final text formatting (by adding \n's) in the supplied string;
										BE ARWARE that using TRUE in here will modify your supplied string! */
				&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
				_bCenter				/*centered*/
			);
		}
		else // right aligned
		{
			//TODO: WIP trying to make work right align
			if (bForceAddBreakLines)
			{
				auto __bbox = font.drawMultiLineColumn(
					_str,					/*string*/
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

				_str = _str + " ";//fix last char
				//textCurrent = _str;
			}

			_bbox = font.drawMultiLine(
				_str,					/*string*/
				_size,					/*size*/
				_x, _y,					/*where*/
				OF_ALIGN_HORZ_RIGHT,
				MAX(10, _w)				/*column width*/
			);
		}
	}
	ofPopStyle();

	//------

	// draw debug
	//if (bEdit && !bRaw)
	if (!bLive)
	{
		if (bGui && bDebug && bDebug2)
		{
			ofPushStyle();

			// 1. prepare
			ofColor c;
			c = bTheme ? colorDebugLight : colorDebugDark;
			int _period = 60; // one second
			bool b = ofGetFrameNum() % _period > _period / 2;
			ofSetColor(ofColor(c, (b ? 48 : 24)));
			ofNoFill();
			ofSetLineWidth(1.0f);

			// 2. blink box
			ofDrawRectangle(_bbox);

			// 3. anchor
			//drawInsertionPoint(_x, _y, 15);//useful to debug ofxFontStash

			ofPopStyle();
		}
	}

	return _bbox;
}

//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::getTextBox(std::string _str, ofRectangle r)
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
float ofxSurfingTextSubtitle::getOneLineHeight(bool oneOnly) {
	bool _br = false;

	// Pre calculate line heights

	// to get more precise value, we should use 3 lines, 
	// applying also lines-height and dividing by 3
	std::string _str1 = "T";// one lines

	int _align;
	int _size;

	_size = fSize.get();
	_align = fAlign.get();

	//-

	//ofRectangle _bbox;
	//_bbox = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	//float v2 = MAX(_bbox.getHeight(), 1) / 2.0f;
	//_bbox = font.drawMultiLine(_str3, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	//float v3 = MAX(_bbox.getHeight(), 1) / 3.0f;
	//_bbox = font.drawMultiLine(_str4, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	//float v4 = MAX(_bbox.getHeight(), 1) / 4.0f;

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

		//_bbox = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
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

		//_bbox = font.drawMultiLine(_str3, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
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

		//_bbox = font.drawMultiLine(_str4, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
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

	//_bbox = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
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

	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;
	float w = 290;
	//float w = 250;
	ImVec2 size_max = ImVec2(w, -1);
	ImVec2 size_min = ImVec2(w, -1);
	ImGui::SetNextWindowSizeConstraints(size_min, size_max);

	if (ui->BeginWindow(bGui))
	{
		drawImGuiWidgets();

		ui->EndWindow();
	}

	drawImGuiSrtFull();

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
void ofxSurfingTextSubtitle::drawImGuiWidgets()
{
	// time label
	string sfile = "";
	string sdialog = "";
	string stime = "";
	string s;
	{
		// filename
		sfile = name_Srt;
		//sfile = path_Srt;

		// elapsed time 
		if (bPlay) {
			uint64_t t = ofGetElapsedTimeMillis() - tPlay;

#ifdef USING_OFX_TIME_CODE
			stime = timecode.timecodeForMillis(t);
#else
			stime += t / 1000.f;
#endif
		}
		if (bPlayForced) {
			stime = sub[currentDialog]->getStartTimeString();
		}
		if (bPlayExternal)
		{
			auto t = tPosition * (float)tEndSubsFilm;
#ifdef USING_OFX_TIME_CODE
			stime = timecode.timecodeForMillis(t);
#else
			stime = t / 1000.f + "''";
#endif
		}

		// index
		sdialog = ofToString(currentDialog) + "/" + ofToString(sub.size() - 1);
	}

	//----

	ui->Add(bMinimize, OFX_IM_TOGGLE_ROUNDED_SMALL);
	ui->AddSpacingSeparated();

	if (!bMinimize) ui->AddLabelBig(sfile);
	ui->AddLabelHuge(stime);
	ui->AddSpacing();

	ui->Add(progressPlayFilm, OFX_IM_PROGRESS_BAR);
	s = "Total Duration";
	ui->AddTooltip(s);

	ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR_NO_TEXT);
	s = "Dialog Duration";
	ui->AddTooltip(s);

	ui->AddSpacingSeparated();

	ui->Add(bDraw, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
	if (!bMinimize) ui->Add(bGui_SrtFull, OFX_IM_TOGGLE_ROUNDED_SMALL);
	ui->Add(bLive, OFX_IM_TOGGLE_ROUNDED_SMALL);

	if (!bLive)
	{
		ui->Indent();
		ui->Add(bEdit, OFX_IM_TOGGLE_ROUNDED_SMALL);
		ui->Add(bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
		if (bDebug) {
			ui->Indent();
			ui->Add(bTheme, OFX_IM_TOGGLE_ROUNDED_MINI);
			ui->Add(bTop, OFX_IM_TOGGLE_ROUNDED_MINI);
			if (bEdit) ui->Add(bGui_Internal, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
#ifdef USE_WIDGET__SUBTITLES
			ui->Add(bDrawWidgetInfo, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
#endif
			ui->Unindent();
		}
		ui->Unindent();
		ui->AddSpacing();
	}

	ui->AddSpacingSeparated();

	if (!bMinimize) // maximized 
	{
		ui->AddLabelBig("SRT FILE");

		std::string n = name_Srt;
		if (ui->BeginTree(n))
		{
			ui->Add(bOpen, OFX_IM_BUTTON_SMALL);

#ifdef USE_WIDGET__VIDEO_PLAYER
			ui->AddSpacingSeparated();
			ui->Add(player.bGui, OFX_IM_TOGGLE_ROUNDED);
#endif
			ui->EndTree();
		}

		ui->AddSpacingSeparated();
	}

	//--

	// maximized 

	if (!bMinimize)
	{
		if (ui->BeginTree("TRANSPORT", false, false))
		{
			//ui->AddLabelBig("TRANSPORT");
			ui->AddSpacing();

			ui->AddCombo(indexModes, names_Modes);
			//if (!bPlayExternal) ui->Add(bStop, OFX_IM_BUTTON_SMALL);

			if (bPlay) {
				//ui->Add(bPlay, OFX_IM_TOGGLE_SMALL_BORDER_BLINK);
			}
			if (bPlayForced) {
				//ui->Add(bPlayForced, OFX_IM_TOGGLE_SMALL_BORDER_BLINK);
				ui->Add(durationPlayForced, OFX_IM_HSLIDER_MINI);
				ui->Add(currentDialog, OFX_IM_HSLIDER_MINI_NO_NAME);
				ui->PushButtonRepeat();
				ui->Add(bPrev, OFX_IM_TOGGLE_SMALL, 2, true);
				ui->Add(bNext, OFX_IM_TOGGLE_SMALL, 2);
				ui->PopButtonRepeat();
			}
			ui->EndTree();
		}

		ui->AddSpacingSeparated();
	}

	if (ui->BeginTree("FADES", false, false))
	{
		if (bMinimize)
		{
			// in
			ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);

			// out
			ui->Add(bAnimatedOut, OFX_IM_TOGGLE_SMALL, 2);
			if (bAnimatedIn) ui->Add(progressIn, OFX_IM_PROGRESS_BAR_NO_TEXT);
			if (bAnimatedOut) ui->Add(progressOut, OFX_IM_PROGRESS_BAR_NO_TEXT);
		}
		else
		{
			// in
			ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);
			ui->Add(speedFadeIn, OFX_IM_HSLIDER_MINI_NO_LABELS, 2);
			ui->AddTooltip(speedFadeIn);
			if (bAnimatedIn) ui->Add(progressIn, OFX_IM_PROGRESS_BAR_NO_TEXT);
			ui->AddSpacing();

			// out
			ui->Add(bAnimatedOut, OFX_IM_TOGGLE_SMALL, 2, true);
			ui->Add(speedFadeOut, OFX_IM_HSLIDER_MINI_NO_LABELS, 2);
			ui->AddTooltip(speedFadeOut);
			if (bAnimatedOut) {
				ui->Add(progressOut, OFX_IM_PROGRESS_BAR_NO_TEXT);
				ui->Add(countDownOut, OFX_IM_HSLIDER_MINI_NO_LABELS);
				ui->AddTooltip(countDownOut);
			}
			ui->AddSpacing();

			ui->Add(bResetFades, OFX_IM_BUTTON_SMALL);
		}

		ui->EndTree();
	}

	ui->AddSpacingSeparated();

	if (ui->BeginTree("STYLE", false, false))
	{
		if (!bMinimize)
		{
			if (ui->BeginTree("COLORS", false, false)) {
				static bool bShowBg = false;
				static float _alpha;
				_alpha = fColorTxt.get().a;
				if (ImGui::SliderFloat("Alpha", &_alpha, 0, 1)) {
					ofFloatColor c = fColorTxt.get();
					fColorTxt.set(ofFloatColor(c.r, c.g, c.b, _alpha));
				}

				//ui->Add(fName);
				//ui->AddSpacing();

				ui->Add(fColorTxt, OFX_IM_COLOR);

				ui->AddToggle("Bg", bShowBg, OFX_IM_TOGGLE_ROUNDED_MINI);
				if (bShowBg) ui->Add(fColorBg, OFX_IM_COLOR_NO_ALPHA);

				ui->EndTree();
			}
		}

		if (ui->BeginTree("PARAGRAPH", false, false))
		{
			if (bMinimize)
			{
				ui->Add(fColorTxt, OFX_IM_COLOR_NO_INPUTS);
			}

			ui->Add(bFine, OFX_IM_TOGGLE_ROUNDED_MINI);
			SurfingGuiTypes st = (bFine.get() ? OFX_IM_STEPPER : OFX_IM_DEFAULT);
			ui->Add(fSizePrc, st);
			ui->Add(fSpacing, st);
			ui->Add(fLineHeight, st);
			ui->AddSpacing();

			if (!bMinimize)
			{
				ui->AddCombo(fAlign, names_Align);
				ui->Add(bFontResponsive, OFX_IM_TOGGLE_ROUNDED_MINI);
				ui->Add(bvCentered, OFX_IM_TOGGLE_ROUNDED_MINI);
				ui->Add(amountLinesTargetCentered, OFX_IM_STEPPER);
				ui->AddSpacing();

				ui->AddLabel("Container");
				static float _x;
				//if (fAlign != 2) 
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
				if (ImGui::SliderFloat("Width", &_w, 0, ofGetWidth())) {
					box.setWidth(_w);
				}
				/*
				static float _h;
				_h = box.getHeight();
				if (ImGui::SliderFloat("Height", &_h, 0, ofGetHeight())) {
					box.setHeight(_h);
				}
				*/

				ui->AddSpacing();
			}
			ui->Add(bCapitalize, OFX_IM_TOGGLE_ROUNDED_MINI);
			ui->Add(bResetFont, OFX_IM_BUTTON_SMALL);

			ui->EndTree();
		}

		ui->EndTree();
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiSrtFull()
{
	if (bGui_SrtFull)
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

		if (ui->BeginWindow(bGui_SrtFull, ImGuiWindowFlags_None))
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
						//currentDialog = n;
						bDoUpdate = true;
					}

					if (currentDialog == n) ui->EndBorderFrame();

					if (bDoUpdate) currentDialog = n;

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

						/*
						static ImVec2 size(100.0f, 100.0f);
						static ImVec2 offset(30.0f, 30.0f);
						ImGui::DragFloat2("size", (float*)&size, 0.5f, 1.0f, 200.0f, "%.0f");
						ImGui::TextWrapped("(Click and drag to scroll)");

						HelpMarker(
							"(Left) Using ImGui::PushClipRect():\n"
							"Will alter ImGui hit-testing logic + ImDrawList rendering.\n"
							"(use this if you want your clipping rectangle to affect interactions)\n\n"
							"(Center) Using ImDrawList::PushClipRect():\n"
							"Will alter ImDrawList rendering only.\n"
							"(use this as a shortcut if you are only using ImDrawList calls)\n\n"
							"(Right) Using ImDrawList::AddText() with a fine ClipRect:\n"
							"Will alter only this specific ImDrawList::AddText() rendering.\n"
							"This is often used internally to avoid altering the clipping rectangle and minimize draw calls.");

						for (int n = 0; n < 3; n++)
						{
							if (n > 0)
								ImGui::SameLine();

							ImGui::PushID(n);
							ImGui::InvisibleButton("##canvas", size);
							if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
							{
								offset.x += ImGui::GetIO().MouseDelta.x;
								offset.y += ImGui::GetIO().MouseDelta.y;
							}
							ImGui::PopID();
							if (!ImGui::IsItemVisible()) // Skip rendering as ImDrawList elements are not clipped.
								continue;

							const ImVec2 p0 = ImGui::GetItemRectMin();
							const ImVec2 p1 = ImGui::GetItemRectMax();
							const char* text_str = "Line 1 hello\nLine 2 clip me!";
							const ImVec2 text_pos = ImVec2(p0.x + offset.x, p0.y + offset.y);
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							switch (n)
							{
							case 0:
								ImGui::PushClipRect(p0, p1, true);
								draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
								draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
								ImGui::PopClipRect();
								break;
							case 1:
								draw_list->PushClipRect(p0, p1, true);
								draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
								draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
								draw_list->PopClipRect();
								break;
							case 2:
								ImVec4 clip_rect(p0.x, p0.y, p1.x, p1.y); // AddText() takes a ImVec4* here so let's convert.
								draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
								draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), text_pos, IM_COL32_WHITE, text_str, NULL, 0.0f, &clip_rect);
								break;
							}
						}
						*/

						/*
						static ImVec2 pos(10, 10);
						static ImVec2 size(40.0f, 40.0f);
						const ImRect bb(pos, pos + size);

						ItemSize(size, style.FramePadding.y);

						bool hovered, held;
						bool pressed = ButtonBehavior(bb, 1, &hovered, &held, ImGuiButtonFlags_None);
						*/

						/*
						h = 100;
						w = ImGui::GetContentRegionAvail().x;

						//ImRect bb(text_pos, text_pos + text_size);

						ImVec2 p = ImGui::GetCursorPos();
						static ImVec2 pos(p.x, p.y);
						static ImVec2 size(40.0f, 40.0f);

						static ImVec2 p0(pos.x, pos.y);
						static ImVec2 p1(pos.x + size.x, pos.y + size.y);

						//const ImVec2 p0 = ImGui::GetItemRectMin();
						//const ImVec2 p1 = ImGui::GetItemRectMax();

						const ImVec2 text_pos = ImVec2(p0.x + offset.x, p0.y + offset.y);

						ImDrawList* draw_list = ImGui::GetWindowDrawList();
						draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
						draw_list->AddText(text_pos, IM_COL32_WHITE, "text_pos");

						ImGui::InvisibleButton("butt", ImVec2(w, h));
						*/

						//--
					}
					else ui->AddLabel(subsDataText[n]);

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
void ofxSurfingTextSubtitle::doResetFont() {
	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__);

	//TODO BUG: crash
	//return;

	fSpacing = 0;
	fLineHeight = 0.75;
	fSizePrc = 0.25;
	fAlign = 1;
	amountLinesTargetCentered = 6;
	bvCentered = true;
	//fColorTxt = ofColor(255, 255);
}

//--------------------------------------------------------------
std::string ofxSurfingTextSubtitle::getAlignNameFromIndex(int index) const {
	std::string n = "UNKNOWN";
	switch (index)
	{
	case 0:
		n = "IGNORE";
		break;
	case 1:
		n = "LEFT";
		break;
	case 2:
		n = "RIGHT";
		break;
	case 3:
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
	countDownOut = 500;
	speedFadeIn = .15;
	speedFadeOut = .5;
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
		name == progressOut.getName())
		return;

	ofLogNotice("ofxSurfingTextSubtitle") << " Changed " << name << " : " << e;

	//--

	if (false) {}

	// modes
	else if (name == indexModes.getName())
	{
		switch (indexModes)
		{
		case 0: bPlayExternal = true; break;
		case 1: bPlay = true; break;
		case 2: bPlayForced = true; break;
		}
	}

	// set sub index
	else if (name == currentDialog.getName())
	{
		//return;//bypass

		// clamp errors
		if (sub.size() == 0)
		{
			currentDialog.setWithoutEventNotifications(0);
			textCurrent = "NO_TEXT";
			ofLogError("ofxSurfingTextSubtitle") << "Not loaded subs file or it's empty or wrong format.";
			return;
		}
		if (currentDialog < 0)
		{
			currentDialog.setWithoutEventNotifications(0);//first
			//currentDialog.setWithoutEventNotifications(sub.size() - 1);//last. cycled
			return;
		}
		else if (currentDialog > sub.size() - 1)
		{
			currentDialog.setWithoutEventNotifications(0);//first
			textCurrent = "NO_TEXT";
			ofLogError("ofxSurfingTextSubtitle") << "Current sub index out of range!";
			return;
		}

		//--

		// Get dialog
		if (currentDialog.get() < sub.size())
		{
			buildSubsData();

			if (bCapitalize) textCurrent = ofToUpper(sub[currentDialog.get()]->getDialogue());
			else textCurrent = sub[currentDialog.get()]->getDialogue();

			//TODO: fail
			/*
			if (bPlayExternal)
			{
				// get the position normalized
				auto t = sub[currentDialog.get()]->getStartTime();
				auto pos = t / (float)tEndSubsFilm;
				if (pos != tPosition) tPosition = pos;
				//tPosition.setWithoutEventNotifications(pos);
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
			//if (bPlay || bPlayForced) alpha = 1.f;//should force play
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

		ofLogNotice("ofxSurfingTextSubtitle") << textCurrent;
	}

	//--

	// TODO: for video link mode
	else if (name == bPlayExternal.getName())
	{
		if (bPlayExternal)
		{
			if (bPlay)bPlay = false;
			if (bPlayForced)bPlayForced = false;

			progressPlaySlide = 0;
		}
		else {
		}
	}

	else if (name == bStop.getName())
	{
		this->stop();
	}

	else if (name == bPlay.getName())
	{
		if (bPlay)
		{
			//bPlayExternal.setWithoutEventNotifications(false);
			bPlayForced.set(false);
			//bPlayForced.setWithoutEventNotifications(false);
			tPlay = ofGetElapsedTimeMillis();
			//currentDialog = 0;//crash
			currentDialog.setWithoutEventNotifications(0);

			progressPlaySlide = 0;
			bPlayExternal = false;
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
			bPlay.set(false);
			//bPlay.setWithoutEventNotifications(false);
			//bPlayExternal.setWithoutEventNotifications(false);

			tPlayForce = ofGetElapsedTimeMillis();
			tPlayForceFilm = ofGetElapsedTimeMillis();
			progressPlaySlide = 0;
			bPlayExternal = false;
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

	//--

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
			if (pos != tPosition) tPosition = pos;
			//tPosition.setWithoutEventNotifications(pos);
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
		else box.setBorderColor(bTheme ? colorDebugLight : colorDebugDark);
	}

	// theme
	else if (name == bTheme.getName())
	{
		box.setBorderColor(bTheme ? colorDebugLight : colorDebugDark);
	}

	//--

	// Font Styles

	else if (name == bCapitalize.getName())
	{
		if (bCapitalize) textCurrent = ofToUpper(sub[currentDialog.get()]->getDialogue());
		else textCurrent = sub[currentDialog.get()]->getDialogue();
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
	// v center
	else if (name == bvCentered.getName())
	{
	}

	// reset style
	else if (name == bResetFont.getName() && bResetFont.get())
	{
		bResetFont = false;
		doResetFont();
	}
	// reset fades
	else if (name == bResetFades.getName() && bResetFades.get())
	{
		bResetFades = false;
		doResetFades();
	}

	//TODO:
	// box resize
	else if (name == amountLinesTargetCentered.getName())
	{

	}

	// Open
	else if (name == bOpen.getName())
	{
		doOpenFile();
	}
}

/*
//--------------------------------------------------------------
void ofxSurfingTextSubtitle::keyPressed(int key)
{
}
*/

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
			//bPlay = true;
			bPlayForced = true;
		}
	}
}

//--

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setTogglePlayForced() { bPlayForced = !bPlayForced; }

//--------------------------------------------------------------
bool ofxSurfingTextSubtitle::isPlaying() const {
	return bPlay.get();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setTogglePlay() {
	bPlay = !bPlay;
#ifdef USE_WIDGET__VIDEO_PLAYER
	if (bPlay) player.play();
	else player.stop();
#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::play() {
	bPlay = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::stop() {
	if (bPlay) bPlay = false;
	if (bPlayForced) bPlayForced = false;

	if (isAnimIn) isAnimIn = false;
	if (isAnimOut) isAnimOut = false;

	textCurrent = "";
	currentDialog = 0;
	tPosition = 0;
	progressPlaySlide = 0;
	progressPlayFilm = 0;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::pause() {
}