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

	pathSrt = _pathSrt;

	setupParams();
	setupSubs();

#ifdef USE_WIDGET__SUBTITLES
	boxInfo.setMode(ofxSurfingBoxHelpText::TOP_RIGHT);
	boxInfo.setup();
#endif

	box.setRectConstraintMin(glm::vec2(200, 200));
	box.setBorderColor(colorDebug);
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

	gui.minimizeAll();

	//--

	//startup();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupParams() {
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

	//--

	// Control
	bDraw.set("Draw", true);
	bEdit.set("Edit", true);
	currentDialog.set("Dialog", 0, 0, 0);
	bPrev.set("<", false);
	bNext.set(">", false);
	bPlay.set("Play", false);
	bPlayForced.set("PlayForced", false);
	durationPlayForced.set("Duration", 2000, 1, 3000);
	//speedPlayForce.set("Speed", 0, 0, 1);
	//bExternal.set("External", false);

	progressPlayFilm.set("% Film", 0, 0, 1);//full length progress
	progressPlaySlide.set("% Slide", 0, 0, 1);//slide progress

	bAutoScroll.set("bAutoScroll", true);//for ImGui only

	// fade
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
	fSizePrc.set("Size", 0.5, 0.1, 1.0f);
	fSpacing.set("Spacing", 0, -20, 50);
	fLineHeight.set("Height", 0.75, 0.5, 2.0);
	fColorBg.set("ColorBg", ofColor::gray, ofColor(0, 0), ofColor(255, 255));
	fColor.set("Color", ofColor::white, ofColor(0, 0), ofColor(255, 255));
	fAlign.set("Align", 1, 1, 3);
	fAlign_str.set("Align ", "-1");
	bResetFont.set("Reset Style", false);

	bCentered.set("Centered", false);//TODO:

	box.bGui.makeReferenceTo(bEdit);
	//box.bEdit.makeReferenceTo(bEdit);
#ifdef USE_WIDGET__SUBTITLES
	boxInfo.bGui.makeReferenceTo(bEdit);
#endif

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

	//--

	params_Control.setName("Control");
	params_Control.add(bDraw);
	params_Control.add(bEdit);

#ifdef USE_IM_GUI__SUBTITLES
	params_Control.add(bGui_SrtFull);
#endif
	//params_Control.add(bGui);
	//params_Control.add(box.bEdit);

	params_Transport.setName("Transport");
	params_Transport.add(bPrev);
	params_Transport.add(bNext);
	params_Transport.add(currentDialog);
	params_Transport.add(progressPlayFilm);
	params_Transport.add(progressPlaySlide);
	params_Transport.add(bPlay);
	params_Transport.add(bPlayForced);
	params_Transport.add(durationPlayForced);
	//params_Transport.add(speedPlayForce);
	//params_Transport.add(bExternal);

	// fades
	params_Fade.setName("Fades");
	params_Fade.add(progressPlaySlide);
	params_FadeIn.setName("In");
	params_FadeOut.setName("Out");
	params_FadeIn.add(bAnimatedIn);
	params_FadeIn.add(speedFadeIn);
	params_FadeIn.add(progressIn);//TODO: should change speed mode to time duration. like on fade out!
	params_FadeOut.add(bAnimatedOut);
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
	params_Style.add(fColor);
	params_Style.add(fSizePrc);
	params_Style.add(fSpacing);
	params_Style.add(fLineHeight);
	params_Style.add(fAlign);
	params_Style.add(fAlign_str);
	params_Style.add(bCentered);
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
void ofxSurfingTextSubtitle::setupSubs() {
	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__);

	subParserFactory = new SubtitleParserFactory(ofToDataPath(pathSrt));

	if (!subParserFactory) ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << pathSrt;

	//bGui_SrtFull.setName(pathSrt);

	parser = subParserFactory->getParser();
	sub = parser->getSubtitles();
	if (sub.size() == 0) ofLogError("ofxSurfingTextSubtitle") << "SUB object empty!";

	currentDialog.setMax(sub.size() - 1);

	// pre read all subs on a vector
	//not used yet
	int i = 0;
	subsText.clear();
	for (SubtitleItem* element : sub)
	{
		string s = element->getDialogue();
		ofLogNotice("ofxSurfingTextSubtitle") << i++ << " " << s;
		subsText.push_back(s);
	}

	//TODO: not sure if srt file is loaded before the first frame!
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::startup() {
	//return;

	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__);
	ofxSurfingHelpers::loadGroup(params, "ofApp");
}

/*
//--------------------------------------------------------------
void ofxSurfingTextSubtitle::refreshFontStyles() { // refresh font styles after any change
	ofLogNotice("ofxSurfingTextSubtitle") << "refreshFontStyles";

	fAlign_str = getAlignNameFromIndex(fAlign.get());

	font.setCharacterSpacing(fSpacing);
	font.setLineHeight(fLineHeight);
}
*/

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::exit() {
	ofLogNotice("ofxSurfingTextSubtitle") << "exit";
	ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

	ofxSurfingHelpers::saveGroup(params, "ofApp");
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::update(uint64_t frame)
{
	//TODO: WIP: external link 
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
void ofxSurfingTextSubtitle::update()
{
	//TODO:
	// Delayed startup to avoid crashes
	if (ofGetFrameNum() == 1) startup();

	//--

	//if (!bDraw) return;

	//--

	// Calculate Fade In

	if (bPlay || bPlayForced)
	{
		if (bAnimatedIn && isAnimIn)
		{
			if (alpha < 1.f) {//make opacity grow 
				static float dt = 1 / fps;
				dtAnim = ofMap(speedFadeIn, 0, 1, dt * 0.4f, dt * 7, true);
				alpha += dtAnim;

				//-

				uint64_t tSlide;
				if (bPlayForced) tSlide = ofGetElapsedTimeMillis() - tPlayForce;
				else if (bPlay) tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;

				//TODO:
				if (bPlayForced) {
					progressIn = ofClamp(alpha, 0, 1);//workaround to avoid deep calculations..
					//progressIn = ofMap(tSlide, 0, durationPlayForced, 0, 1, true);
				}
				else if (bPlay) {
					progressIn = ofClamp(alpha, 0, 1);
					//progressIn = ofMap(tSlide, 0, durationPlaySlide, 0, 1, true);
				}
			}
			if (alpha >= 1.f) {//stop if finished
				alpha = 1;
				isAnimIn = false;
				progressIn = ofClamp(alpha, 0, 1);
			}
		}
		else {
			progressIn = 1;
		}
	}

	//--

	// Calculate Fade Out

	if (bAnimatedOut)
	{
		if (bPlay || bPlayForced)
		{
			uint64_t tSlide;
			if (bPlayForced) tSlide = ofGetElapsedTimeMillis() - tPlayForce;
			else if (bPlay) tSlide = ofGetElapsedTimeMillis() - tPlayStartSlide;

			if (!isAnimOut) // checking if requires trig fade out
			{
				// trigs fade out

				if (bPlayForced)
				{
					if (tSlide > durationPlayForced - countDownOut)
					{
						isAnimOut = true;
					}
				}
				else if (bPlay)
				{
					if (tSlide > durationPlaySlide - countDownOut)
					{
						isAnimOut = true;
					}
				}
				progressOut = 1;
			}
			//else // fading out
			if (isAnimOut) // fading out
			{
				uint64_t t = tSlide - countDownOut;
				if (bPlayForced) {
					progressOut = ofClamp(alpha, 0, 1);
					//progressOut = ofMap(t, 0, countDownOut, 1, 0, true);
				}
				else if (bPlay) {
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
				}

				//cout << progressOut << endl;
				//cout << tSlide << endl;
			}
		}
	}
	else
	{
		progressOut = 1;
	}

	//--

	// Calculate progress

	// bPlay

	if (bPlay)
	{
		uint64_t t = ofGetElapsedTimeMillis() - tPlay;

		int k = 0;

		// find the dialog relative to current time
		for (SubtitleItem* element : sub)
		{
			if (t > element->getStartTime() && t <= element->getEndTime())
			{
				static int currentLine_ = -1;
				currentLine_ = k;

				// To apply only once if currentDialog changed!
				if (currentDialog != currentLine_)
				{
					currentDialog = currentLine_;//will trig the callback!

					doUpdateSlidePlay(element);
				}
				break;
			}
			k++;
		}

		uint64_t tp = ofGetElapsedTimeMillis() - tPlayStartSlide;
		progressPlaySlide = ofMap(tp, 0, durationPlaySlide, 0, 1);
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

	//if (bExternal) {
	//}

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
			else {
			}

			progressPlayFilm = ofMap(t, 0, sub.back()->getEndTime(), 0, 1);
		}
		//else if (bPlayForced || !bPlay)
		else // for any other modes
		{
			progressPlayFilm = ofMap(currentDialog, 0, sub.size(), 0, 1);
		}
	}

	//--

	// Debug info
	{
#ifdef USE_WIDGET__SUBTITLES
		string s = "";
		s += pathSrt;//srt filename

		if (bPlay)
		{
			/*
			//TODO: add tweakeable speed
			const int SPEED_SCALE = 5;
			float speed = ofMap(speedPlayForce, -1, 1, 0.25f, (float)SPEED_SCALE);
			uint64_t t = ofGetElapsedTimeMillis() * speed - tPlay;
			s += timecode.timecodeForMillis(t);
			//s += ofToString(t / 1000);
			s += "\n";
			*/

			s += "\n";
			uint64_t t = ofGetElapsedTimeMillis() - tPlay;
#ifdef USING_OFX_TIME_CODE
			s += timecode.timecodeForMillis(t);
#else
			s += t / 1000.f;
#endif
		}

		s += "\n\n";
		s += ofToString(currentDialog) + "/" + ofToString(sub.size() - 1);
		s += "\n\n";
		s += sub[currentDialog]->getStartTimeString();
		s += "\n";
		s += sub[currentDialog]->getEndTimeString();
		s += "\n";

		boxInfo.setText(s);
#endif
		}
	}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawGui() {
	if (!bGui) return;

	// info
#ifdef USE_WIDGET__SUBTITLES
	boxInfo.draw();
#endif

	if (!bGui_Internal) return;
	gui.draw();
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

/*
//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawRaw(ofRectangle view) {
	if (!bDraw) return;

	// text
	ofRectangle r = drawTextBox(textCurrent, box.getRectangle());
	//boxhMax = r.getHeight();
}
*/

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawRaw() {
	if (!bDraw) return;

	if (!bCentered)
	{
		drawTextBox(textCurrent, box.getRectangle(), true);
	}
	else
	{
		ofRectangle r = getTextBox(textCurrent, box.getRectangle());

		offset = glm::vec2(0, -r.getHeight() / 2.f);
		offset += glm::vec2(0, boxhMax / 2.f);
		//offset = offset + glm::vec2(0, box.getHeight() / 2.f);

		if (r.getHeight() > boxhMax)
		{
			boxhMax = r.getHeight();
		}

		// draw
		ofPushMatrix();
		ofTranslate(offset);

		drawTextBox(textCurrent, box.getRectangle(), true);

		ofPopMatrix();

		//--

		//TODO: workaround
		if (bCentered && box.isChanged()) boxhMax = 0;

		//TODO:
		// Center Point
		if (bEdit && bCentered)
		{
			float _x = box.getX();
			float _y = box.getY();

			_x += box.getWidth() / 2;
			_y += boxhMax / 2.f;
			//_y += box.getHeight() / 2;
			//_y += r.getHeight() / 2;

			drawInsertionPoint(_x, _y, 8, 8);
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::draw() {
	if (!bDraw) return;

	drawRaw();

	// container
	//workflow auto scale
	/*
	if (box.getHeight() < boxhMax) {
		box.setHeight(boxhMax);
	}
	*/
	box.draw();

	if (bEdit) box.drawBorderBlinking();

	// debug

	// alpha preview
	if (bEdit && (bAnimatedIn || bAnimatedOut))
	{
		bool bTop = false;

		float x;
		float y;
		float pad;
		float w = 79;//hard coded to text widget
		float h;//bar
		glm::vec2 p;

		if (bTop) {//top
			p = box.getRectangle().getTopLeft();
			h = 6;
			pad = 0;
			x = p.x - 1;
			y = p.y - pad - h;
		}
		else {//bottom
			p = box.getRectangle().getBottomLeft();
			h = 6;
			pad = -4;
			x = p.x - 1;
			y = p.y + 15 + pad + h;
		}

		ofPushStyle();
		ofSetColor(0, 255);
		ofFill();
		ofDrawRectangle(x, y, w * alpha, h);
		ofNoFill();
		ofSetColor(0, 255);
		ofDrawRectangle(x + 1, y, w - 1, h);
		ofPopStyle();

		float h2 = 15;
		x += 4;
		y -= h;
		y += 3;
		string s = (bAnimatedIn && isAnimIn) ? "IN" : "  ";
		s += "    ";
		s += (bAnimatedOut && isAnimOut) ? "OUT" : "   ";
		ofDrawBitmapStringHighlight(s, x, y);
	}

	// slide progress
	// bottom thin line
	if (bEdit && (bPlay || bPlayForced))
	{
		ofPushStyle();

		float lw1 = 3;
		float h = 3;
		auto p = box.getRectangle().getBottomLeft();
		float w = box.getRectangle().getWidth() + 1;
		float x = p.x;
		float y = p.y - h / 2;
		ofSetColor(0, 255);
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
			if (bPlayForced) {
				r = (t / (float)durationPlayForced);//ratio
			}
			if (bPlay) {
				r = (t / (float)durationPlaySlide);//ratio
			}
			xIn = p.x + w * r;

			if (xIn != -1) {
				ofSetLineWidth(lw1);
				ofDrawLine(xIn, p.y - sz, xIn, p.y + sz);
			}
		}

		//--

		// mark fade out phase start point with a vertical line
		// mark countdown 
		if (bAnimatedOut)
		{
			ofSetLineWidth(lw1);

			float r = 0.f;
			if (bPlayForced) {
				r = 1.f - (countDownOut / (float)durationPlayForced);//ratio
			}
			if (bPlay) {
				r = 1.f - (countDownOut / (float)durationPlaySlide);//ratio
			}
			xOut1 = p.x + w * r;
			ofDrawLine(xOut1, p.y - sz, xOut1, p.y + sz);

			//--

			float pixPerMillis = 0;
			if (bPlayForced) {
				pixPerMillis = w / (float)durationPlayForced;//ratio
			}
			if (bPlay) {
				pixPerMillis = w / (float)durationPlaySlide;//ratio
			}
			dtAnim = ofMap(speedFadeOut, 0, 1, dt * 0.4f, dt * 7.f, true);
			float d = 1.f / dtAnim;
			int t = (d / fps) * 1000;//duration required to fade out until 0!
			xOut2 = xOut1 + t * pixPerMillis;
			if (xOut2 < p.x + w) ofSetColor(0, 255);
			else ofSetColor(0, 32);//attenuate if goes out of the  time line!
			ofSetLineWidth(2);
			sz -= 2;
			ofDrawLine(xOut2, p.y - sz, xOut2, p.y + sz);

			//--

			// Make fatter the line for the full opacity section!

			int lw = 4;
			int off = 0;
			ofSetLineWidth(lw);
			ofSetColor(0, 64);
			ofDrawLine(xIn, p.y + off, xOut1, p.y + off);
		}

		ofPopStyle();
	}
}

//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::drawTextBox(std::string _str, ofRectangle r, bool bRaw) {
	float _x = box.getX();
	float _y = box.getY();
	float _w = box.getWidth();
	float _h = box.getHeight();

	ofColor _color;
	int _align;
	int _size;

	_size = fSize.get();
	_align = fAlign.get();

	_color = fColor.get();

	//if ((bAnimatedIn && isAnimIn) || (bAnimatedOut && isAnimOut))
	if ((bAnimatedIn) || (bAnimatedOut))
	{
		_color = ofColor(_color, alpha * _color.a);
	}

	_y += getOneLineHeight(); // here upper border is aligned to the center horizontal
	//_y += 2; // fix. workaround to hard code calibrate.

	//------

	ofPushStyle();
	ofSetColor(_color); // don't do nothing. only applies the style

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
		false,					/* if true, we wont draw (just get bbox back) */
		20,						/* max number of lines to draw, crop after that */
		_br,					/*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped,		/* this bool will b set to true if the box was to small to fit all text*/
		_bCenter				/*centered*/
	);

	ofPopStyle();

	//------

	// draw debug
	//if (bEdit && !bRaw)
	if (bEdit)
	{
		ofPushStyle();

		// 1. prepare
		ofColor c;
		c = colorDebug;
		int _period = 60; // one second
		bool b = ofGetFrameNum() % _period > _period / 2;
		ofSetColor(ofColor(c, (b ? 48 : 24)));
		ofNoFill();
		ofSetLineWidth(1.0f);

		// 2. blink box
		ofDrawRectangle(_bbox);

		// 3. anchor
		//drawInsertionPoint(_x, _y, 15);//useful to debug ofxFontStash

		/*
		// 4. elastic bar
		// TODO: not working bc rectangle is not positioned, just storing w and h
		//_y = textBox[debugIndex].getRectangle().getBottom();
		_y = _bbox.getBottom() - 7;

		if (debugIndex != -1)
		{
			_x = columnUnits * canvasColumnUnitWidth - 16;

			ofFill();

			float _hb = 25;
			float _w = ofMap(animatorElastic.getValue(), 0, 1, 1, _hb);
			float _ww = 12;
			float _yy = -6 + _y + _hb * 0.5;

			// 4.1 bg box
			ofSetColor(0, 0, 0, 255);
			ofDrawRectangle(_x - 4, _y - 15, 16, 48);

			// 4.2 text
			std::string s;
			s = ofToString(debugIndex);
			//s += "\n";
			//s += ofToString(textBox[debugIndex].getSpacer(), 1);
			ofDrawBitmapStringHighlight(s, _x, _y);

			// 4.3 bar
			ofSetColor(255, 220);
			ofDrawRectangle(_x - 2, _yy, _ww, _w);
		}
		*/

		ofPopStyle();
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

	return _bbox;
}

//--------------------------------------------------------------
float ofxSurfingTextSubtitle::getOneLineHeight(bool oneOnly) {
	bool _br = false;

	// to get more precise value, we should use 3 lines, 
	// applying also lines-height and dividing by 3
	std::string _str1 = "T";// one lines
	std::string _str2 = "T\nT";// two lines
	std::string _str3 = "T\nT\nT";// three lines
	std::string _str4 = "T\nT\nT\nT";// four lines

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
	else v = (v1 + v2 + v3 + v4) / 4.0f; // ?? this is weird

	//-

	v += 2; // fix. workaround to hard code calibrate.

	return v;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawInsertionPoint(float _x, float _y, float _w, float _h)
{
	float r = 2;

	ofPushStyle();
	ofFill();
	float v = ofMap(ofGetFrameNum() % 60, 0, 60, 0.25, 1);
	ofSetColor(colorDebug, v * 255);
	ofDrawCircle(_x, _y, r);

	if (_w != 0) {//an extra horizontal line
		ofNoFill();
		ofSetColor(colorDebug, 64);
		ofDrawLine(_x - _w, _y, _x + _w, _y);
	}
	if (_h != 0) {//an extra vertical line
		ofNoFill();
		ofSetColor(colorDebug, 64);
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
	ImVec2 size_max = ImVec2(w, -1);
	ImVec2 size_min = ImVec2(w, -1);
	ImGui::SetNextWindowSizeConstraints(size_min, size_max);

	if (ui->BeginWindow(bGui))
	{
		drawImGuiWidgets();
		ui->EndWindow();
	}

	drawImGuiSrtFull();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWidgets()
{
	//ui->AddGroup(subs.params);

	ui->Add(ui->bMinimize, OFX_IM_TOGGLE_ROUNDED_SMALL);
	ui->AddSpacingSeparated();

	ui->Add(bDraw, OFX_IM_TOGGLE_ROUNDED);
	ui->Add(bEdit, OFX_IM_TOGGLE_ROUNDED_SMALL);

	// time label
	string s1 = "";
	string s2 = "";
	{
		// filename
		if (!ui->bMinimize) {
			s1 += pathSrt;
			//s1 += "\n";
		}

		// time
		if (bPlay) {
			uint64_t t = ofGetElapsedTimeMillis() - tPlay;

#ifdef USING_OFX_TIME_CODE
			s2 += timecode.timecodeForMillis(t);
#else
			s2 += t / 1000.f;
#endif
		}
		else {
			s2 += sub[currentDialog]->getStartTimeString();
		}
		s2 += "\n";

		// index
		s2 += ofToString(currentDialog) + "/" + ofToString(sub.size() - 1);
		//s2 += " " + ofToString(progressPlayFilm * 100, 0) + ofToString("'%'");//TODO: fix
		//s2 += "\n";
	}

	//--

	if (!ui->bMinimize) // maximized 
	{
		if (bDraw) {
			//ui->Add(bEdit, OFX_IM_TOGGLE_ROUNDED_SMALL);
			ui->Add(bGui_SrtFull, OFX_IM_TOGGLE_ROUNDED_SMALL);
			if (bEdit) ui->Add(bGui_Internal, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
			//if(bEdit) ui->Add(box.bEdit, OFX_IM_TOGGLE_ROUNDED_MINI);
		}

		ui->AddSpacingSeparated();
		ui->AddLabelBig("TRANSPORT");
		ui->AddSpacing();
		ui->Add(bPlay, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bPlayForced, OFX_IM_TOGGLE_SMALL, 2);

		if (bPlay) {
			ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR_NO_TEXT);
		}
		if (bPlayForced) {
			ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR_NO_TEXT);
			//ui->Add(speedPlayForce);
		}
		ui->AddSpacing();

		ui->AddLabel(s1);
		ui->AddLabelBig(s2);
		ui->Add(currentDialog, OFX_IM_HSLIDER_MINI_NO_NAME);
		ui->PushButtonRepeat();
		ui->Add(bPrev, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bNext, OFX_IM_TOGGLE_SMALL, 2);
		ui->PopButtonRepeat();
		ui->AddSpacingSeparated();
	}
	else //minimized
	{
		ui->AddSpacingSeparated();
		//ui->AddLabelBig("TRANSPORT");		
		ui->AddSpacing();
		ui->Add(bPlay, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bPlayForced, OFX_IM_TOGGLE_SMALL, 2);

		if (bPlay) {
			ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR_NO_TEXT);
		}
		if (bPlayForced) {
			//ui->Add(speedPlayForce);
			ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR_NO_TEXT);
		}
		ui->AddSpacing();

		ui->AddLabelBig(s2);
		ui->Add(currentDialog, OFX_IM_HSLIDER_MINI_NO_NAME);
		ui->PushButtonRepeat();
		ui->Add(bPrev, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bNext, OFX_IM_TOGGLE_SMALL, 2);
		ui->PopButtonRepeat();
		ui->AddSpacing();
	}

	if (ui->BeginTree("FADES", false, false))
	{
		if (ui->bMinimize)
		{
			// in
			ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);
			// out
			ui->Add(bAnimatedOut, OFX_IM_TOGGLE_SMALL, 2);
			if (bAnimatedIn) ofxImGuiSurfing::ProgressBar2(alpha);
			if (bAnimatedOut) ui->Add(progressOut, OFX_IM_PROGRESS_BAR_NO_TEXT);
		}
		else
		{
			//ui->AddLabelBig("FADE");

			// in
			ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);
			ui->Add(speedFadeIn, OFX_IM_HSLIDER_MINI_NO_LABELS, 2);
			ui->AddTooltip(speedFadeIn);
			if (bAnimatedIn) {
				ofxImGuiSurfing::ProgressBar2(alpha);
				//static ofParameter<float>v{ "v", 0, 0, 1 };
				//v.setWithoutEventNotifications(alpha);
				//ui->Add(v, OFX_IM_PROGRESS_BAR_NO_TEXT, 2);
			}
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

	if (!ui->bMinimize) ui->AddSpacingSeparated();

	//ui->AddGroup(params_Style);
	if (!ui->bMinimize) {
		ui->AddLabelBig("STYLE");
		ui->Add(fName);
		ui->AddSpacing();

		if (ui->BeginTree("COLORS", false, false)) {
			ui->Add(fColorBg, OFX_IM_COLOR_NO_ALPHA);
			ui->Add(fColor, OFX_IM_COLOR);
			ui->EndTree();
		}
	}
	//else {
	//	ui->Add(fColor, OFX_IM_COLOR_NO_INPUTS);
	//}

	if (ui->BeginTree("PARAGRAPH", false, false)) {
		if (ui->bMinimize)
		{
			ui->Add(fColor, OFX_IM_COLOR_NO_INPUTS);
		}

		static ofParameter<bool> bFine{ "Fine", false };
		ui->Add(bFine, OFX_IM_TOGGLE_ROUNDED_MINI);
		SurfingGuiTypes st = (bFine.get() ? OFX_IM_STEPPER : OFX_IM_DEFAULT);
		ui->Add(fSizePrc, st);
		ui->Add(fSpacing, st);
		ui->Add(fLineHeight, st);
		ui->AddSpacing();
		if (!ui->bMinimize) {
			static vector<string>names{ "IGNORE","LEFT","RIGHT","CENTER" };
			ui->AddCombo(fAlign, names);
			//ui->Add(fAlign, OFX_IM_DEFAULT);
			//ui->Add(fAlign_str);
			ui->Add(bCentered, OFX_IM_TOGGLE_ROUNDED_MINI);
			ui->AddSpacing();
		}
		ui->Add(bResetFont, OFX_IM_BUTTON_SMALL);
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
			ui->AddLabelBig(pathSrt);
			ui->AddSpacing();

			ui->Add(progressPlayFilm, OFX_IM_PROGRESS_BAR);
			ui->AddSpacing();

			int track_item = currentDialog;
			ui->Add(bAutoScroll, OFX_IM_TOGGLE_ROUNDED_MINI);

			ui->AddSpacingBigSeparated();

			ImGui::BeginChild("_");

			// iterate all srt dialogs
			for (int n = 0; n < subsText.size(); n++)
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
					//ImGui::ButtonEx(subsText[n].c_str(), ImVec2(w, 40));
					ui->AddLabelBig(subsText[n]);//bigger if selected

					if (bAutoScroll)
					{
						ImGui::SetScrollHereY(0.5f);
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
				else ui->AddLabel(subsText[n]);

				ImGui::Columns(1);

				//*if (currentDialog == n) ui->AddSeparated();
			}

			ImGui::EndChild();

			ui->EndWindow();
		}
	}
}

#endif

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doResetFont() {

	//TODO BUG: crash
	//return;

	fSpacing = 0;
	fLineHeight = 0.75;
	//fSizePrc = 0.25;
	//fColor = ofColor(255, 255);
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
	bAnimatedIn = true;
	bAnimatedOut = true;
	countDownOut = 1000;
	speedFadeIn = .1;
	speedFadeOut = .25;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doUpdateSlidePlay(SubtitleItem* element) {

	tPlayStartSlide = ofGetElapsedTimeMillis();
	auto ts = element->getStartTime();
	auto te = element->getEndTime();
	durationPlaySlide = te - ts;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::Changed(ofAbstractParameter& e)
{
	static bool bAttending = false;
	if (bAttending) return;

	string name = e.getName();

	//ignore
	if (name == progressPlaySlide.getName() ||
		name == progressPlayFilm.getName() ||
		name == progressIn.getName() ||
		name == progressOut.getName())
		return;

	ofLogNotice("ofxSurfingTextSubtitle") << " Changed " << name << " : " << e;

	//--

	if (false) {}

	// set sub index
	else if (name == currentDialog.getName())
	{
		//return;

		if (sub.size() == 0)
		{
			currentDialog.setWithoutEventNotifications(0);
			textCurrent = "NO_TEXT";
			ofLogError("ofxSurfingTextSubtitle") << "Not loaded subs file or it's empty or wrong format.";
			return;
		}

		// clamp
		if (currentDialog < 0)
		{
			currentDialog.setWithoutEventNotifications(0);//first
			//currentDialog.setWithoutEventNotifications(sub.size() - 1);//last
		}
		else if (currentDialog > sub.size() - 1) currentDialog.setWithoutEventNotifications(0);//first

		// get dialog
		if (currentDialog.get() < sub.size())
		{
			textCurrent = sub[currentDialog.get()]->getDialogue();
		}
		else
		{
			ofLogError("ofxSurfingTextSubtitle") << "Current sub index out of range!";
			textCurrent = "NO_TEXT";
		}

		if (bAnimatedIn)
		{
			isAnimIn = true;
			alpha = 0.f;
		}
		if (bAnimatedOut)
		{
			isAnimOut = false;
		}
		if (bAnimatedOut && !bAnimatedIn) {
			alpha = 1.f;
		}

		ofLogNotice("ofxSurfingTextSubtitle") << textCurrent;
	}

	//--
	
	else if (name == bPlay.getName())
	{
		if (bPlay)
		{
			//bExternal.setWithoutEventNotifications(false);
			bPlayForced.set(false);
			//bPlayForced.setWithoutEventNotifications(false);
			tPlay = ofGetElapsedTimeMillis();
			//currentDialog = 0;//crash
			currentDialog.setWithoutEventNotifications(0);
		}
		else
		{
			//return;
			currentDialog.setWithoutEventNotifications(0);
			//currentDialog = 0;//crash

			if (bAnimatedIn && isAnimIn) isAnimIn = false;
			if (bAnimatedOut && isAnimOut) isAnimOut = false;
			//if (!bAnimatedIn && !bAnimatedOut) alpha = 1;//?
		}
	}
	
	else if (name == bPlayForced.getName())
	{
		if (bPlayForced) 
		{
			bPlay.set(false);
			//bPlay.setWithoutEventNotifications(false);
			//bExternal.setWithoutEventNotifications(false);

			tPlayForce = ofGetElapsedTimeMillis();
			progressPlaySlide = 0;
		}
		else 
		{
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
	}
	// prev
	else if (name == bPrev.getName() && bPrev)
	{
		bPrev.setWithoutEventNotifications(false);
		currentDialog--;
	}

	// edit and debug
	else if (name == bEdit.getName()) {
		box.bEdit = bEdit;
	}

	// TODO: for video link mode
	//else if (name == bExternal.getName())
	//{
	//	if (bExternal) 
	//	{
	//	}
	//}

	//--

	// Font Styles

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
	else if (name == bCentered.getName())
	{
		if (bCentered) {
			boxhMax = 0;
		}
	}
	// reset style
	else if (name == bResetFont.getName() && bResetFont.get())
	{
		bResetFont = false;
		doResetFont();

		/*
		//TODO: crashes..
		bAttending = true;
		bResetFont.setWithoutEventNotifications(false);
		doResetFont();
		bAttending = false;
		*/
	}
	// reset fades
	else if (name == bResetFades.getName() && bResetFades.get())
	{
		bResetFades = false;
		doResetFades();
	}
}