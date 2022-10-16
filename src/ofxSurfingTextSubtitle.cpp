#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setup(string _pathSrt) {

	pathSrt = _pathSrt;

	setupParams();
	setupSubs();

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();
	gui.setup(params);

#ifdef USE_WIDGET__SUBTTITTLES
	boxInfo.setMode(ofxSurfingBoxHelpText::TOP_RIGHT);
	boxInfo.setup();
#endif

	box.setBorderColor(colorDebug);
	box.setup();

#ifdef USING_OFX_TIME_CODE
	timecode.setFPS(fps);
#endif

	startup();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupParams() {

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

	// Control
	bDraw.set("Draw", true);
	bDebug.set("Debug", true);
	currentLine.set("Line", 0, 0, 0);
	//bExternal.set("External", false);
	bPrev.set("<", false);
	bNext.set(">", false);
	bPlay.set("Play", false);
	bPlayForce.set("PlayForce", false);
	speedForce.set("Speed", 0, 0, 1);

	bAnimated.set("FadeIn", false);
	speedFade.set("SpeedIn", 0, 0, 1);
	bAnimatedOut.set("FadeOut", false);
	speedFadeOut.set("SpeedOut", 0, 0, 1);

	// Style
	fSize.set("SizeR", 50, 5, (float)MAX_FONT_SIZE);
	fSizePrc.set("Size", 0.5, 0.1, 1.0);
	fSpacing.set("Spacing", 0, -20, 50);
	fLineHeight.set("Height", 0.75, 0.5, 2.0);
	fColorBg.set("ColorBg", ofColor::gray, ofColor(0, 0), ofColor(255, 255));
	fColor.set("Color", ofColor::white, ofColor(0, 0), ofColor(255, 255));
	fAlign.set("Align", 1, 1, 3);
	fAlign_str.set("Align ", "-1");
	bCentered.set("Centered", false);//TODO:
	bResetFont.set("Reset", false);

	box.bGui.makeReferenceTo(bDebug);
	//box.bEdit.makeReferenceTo(bDebug);
#ifdef USE_WIDGET__SUBTTITTLES
	boxInfo.bGui.makeReferenceTo(bDebug);
#endif

	//--

	bPlay.setSerializable(false);
	bPlayForce.setSerializable(false);

	fName.setSerializable(false);
	fPath.setSerializable(false);
	fAlign_str.setSerializable(false);
	bResetFont.setSerializable(false);
	bNext.setSerializable(false);
	bPrev.setSerializable(false);
	currentLine.setSerializable(false);

	//--

	params_Control.setName("Control");
	params_Control.add(bDraw);
	params_Control.add(bDebug);
	params_Control.add(bGui_ViewFull);
	//params_Control.add(bGui);
	//params_Control.add(box.bEdit);

	params_Transport.setName("Transport");
	params_Transport.add(currentLine);
	params_Transport.add(bPrev);
	params_Transport.add(bNext);
	params_Transport.add(bPlay);
	params_Transport.add(bPlayForce);
	params_Transport.add(speedForce);
	//params_Transport.add(bExternal);

	params_Fade.setName("Fade");
	params_Fade.add(bAnimated);
	params_Fade.add(speedFade);
	//params_Fade.add(bAnimatedOut);
	//params_Fade.add(speedFadeOut);
	params_Transport.add(params_Fade);

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

	//params.setName(bGui.getName());
	params.setName("Text Subtitle");
	params.add(params_Control);
	params.add(params_Transport);
	params.add(params_Style);

	ofAddListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

	//refreshFontStyles();

	// Preset
	params_Preset.setName("Subtitler");
	params_Preset.add(params_Style);
	params_Preset.add(params_Fade);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupSubs() {
	subParserFactory = new SubtitleParserFactory(ofToDataPath(pathSrt));

	if (!subParserFactory) ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << pathSrt;

	//bGui_ViewFull.setName(pathSrt);

	parser = subParserFactory->getParser();
	sub = parser->getSubtitles();
	if (sub.size() == 0) ofLogError("ofxSurfingTextSubtitle") << "SUB object empty!";

	currentLine.setMax(sub.size() - 1);

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
void ofxSurfingTextSubtitle::drawGui() {
	if (!bGui) return;

	// info
#ifdef USE_WIDGET__SUBTTITTLES
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

		// Center Point
		//TODO:
		if (0)
		{
			float _x = box.getX();
			float _y = box.getY();
			_x += box.getWidth() / 2;
			_y += box.getHeight() / 2;
			//_y += r.getHeight() / 2;
			drawInsertionPoint(_x, _y);
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::update() {
	if (!bDraw) return;

	if (bAnimated && isAnim)
	{
		if (alpha < 1.f) {
			static float dt = 1 / 60.f;
			dtAnim = ofMap(speedFade, 0, 1, dt * 0.4f, dt * 7, true);

			alpha += dtAnim;
		}
		if (alpha >= 1.f) {
			alpha = 1;
			isAnim = false;
		}
	}
	else
	{
		alpha = 1.f;
	}

	//--

	if (bPlay && !bPlayForce)
	{
		//if (bPlayForce) bPlayForce = false;

		uint64_t t = ofGetElapsedTimeMillis() - tPlay;

		int k = 0;
		// find the dialog relative to current time
		for (SubtitleItem* element : sub)
		{
			if (t > element->getStartTime() && t <= element->getEndTime())
			{
				static int currentLine_ = -1;
				currentLine_ = k;
				//apply only if changed!
				if (currentLine != currentLine_) 
				{
					currentLine = currentLine_;
				}
				break;
			}
			k++;
		}
	}

	if (bPlayForce && !bPlay)
	{
		int n = ofMap(speedForce, 0, 1, 100, 5);
		if (ofGetFrameNum() % n == 0) currentLine++;
	}

	//if (bExternal) {
	//}

	//--

	// Debug info
#ifdef USE_WIDGET__SUBTTITTLES
	string s = "";
	s += pathSrt;
	s += "\n\n";
	s += ofToString(currentLine) + "/" + ofToString(sub.size() - 1);
	s += "\n";
	s += sub[currentLine]->getStartTimeString();
	s += "\n";
	if (bPlay)
	{
		/*
		const int SPEED_SCALE = 5;
		float speed = ofMap(speedForce, -1, 1, 0.25f, (float)SPEED_SCALE);
		uint64_t t = ofGetElapsedTimeMillis() * speed - tPlay;
		s += timecode.timecodeForMillis(t);
		//s += ofToString(t / 1000);
		s += "\n";
		*/

		uint64_t t = ofGetElapsedTimeMillis() - tPlay;

#ifdef USING_OFX_TIME_CODE
		s += timecode.timecodeForMillis(t);
#else
		s += t / 1000.f;
#endif
		//s += "\n";
}
	boxInfo.setText(s);
#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::draw() {
	if (!bDraw) return;

	// container
	//workflow auto scale
	/*
	if (box.getHeight() < boxhMax) {
		box.setHeight(boxhMax);
	}
	*/
	box.draw();

	// debug
	if (bDebug) box.drawBorderBlinking();

	drawRaw();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::Changed(ofAbstractParameter& e)
{
	static bool bAttending = false;
	if (bAttending) return;

	string name = e.getName();
	ofLogNotice("ofxSurfingTextSubtitle") << (__FUNCTION__) << " " << name << " : " << e;

	if (false) {}

	// set sub index
	else if (name == currentLine.getName())
	{
		//return;

		if (sub.size() == 0)
		{
			currentLine.setWithoutEventNotifications(0);
			textCurrent = "NO_TEXT";
			ofLogError("ofxSurfingTextSubtitle") << "Not loaded subs file or it's empty or wrong format.";
			return;
		}

		// clamp
		if (currentLine < 0)
		{
			currentLine.setWithoutEventNotifications(0);//first
			//currentLine.setWithoutEventNotifications(sub.size() - 1);//last
		}
		else if (currentLine > sub.size() - 1) currentLine.setWithoutEventNotifications(0);//first

		// get dialog
		if (currentLine.get() < sub.size())
		{
			textCurrent = sub[currentLine.get()]->getDialogue();
		}
		else
		{
			ofLogError("ofxSurfingTextSubtitle") << "Current sub index out of range!";
			textCurrent = "NO_TEXT";
		}

		if (bAnimated)
		{
			isAnim = true;
			alpha = 0.f;
		}
	}

	else if (name == bPlay.getName())
	{
		if (bPlay.get())
		{
			//bExternal.setWithoutEventNotifications(false);
			bPlayForce.setWithoutEventNotifications(false);
			tPlay = ofGetElapsedTimeMillis();
			currentLine = 0;
		}
		else
		{
			currentLine = 0;
		}
	}

	else if (name == bPlayForce.getName())
	{
		if (bPlayForce) {
			bPlay.setWithoutEventNotifications(false);
			//bExternal.setWithoutEventNotifications(false);
		}
	}

	// next
	else if (name == bNext.getName() && bNext)
	{
		bNext.setWithoutEventNotifications(false);
		currentLine++;
	}
	// prev
	else if (name == bPrev.getName() && bPrev)
	{
		bPrev.setWithoutEventNotifications(false);
		currentLine--;
	}

	else if (name == bDebug.getName()) {
		box.bEdit = bDebug;
	}

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
	// font size, spacing, line height
	else if (name == fSpacing.getName())
	{
		font.setCharacterSpacing(fSpacing);
	}
	else if (name == fLineHeight.getName())
	{
		font.setLineHeight(fLineHeight);
	}
	// alignment
	else if (name == fAlign.getName())
	{
		fAlign_str = getAlignNameFromIndex(fAlign.get());
	}
	else if (name == bCentered.getName())
	{
		if (bCentered) {
			boxhMax = 0;
		}
	}
	else if (name == bResetFont.getName() && bResetFont.get())
	{
		doResetFont();
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

	if (bAnimated && isAnim)
	{
		_color = ofColor(_color, alpha * _color.a);
	}

	_y += getOneLineHeight(); // here upper border is aligned to the center horizontal

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
	//if (bDebug && !bRaw)
	if (bDebug)
	{
		ofPushStyle();

		// 1. prepare
		ofColor c;
		c = colorDebug;
		int _period = 60; // one second
		bool b = ofGetFrameNum() % _period > _period / 2;
		ofSetColor(ofColor(c, (b ? 64 : 32)));
		ofNoFill();
		ofSetLineWidth(1.0f);

		// 2. blink box
		ofDrawRectangle(_bbox);

		// 3. anchor
		drawInsertionPoint(_x, _y, 50);

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

	// improve precision making more line-size situations

	//-

	if (oneOnly) v = v1;
	else v = (v1 + v2 + v3 + v4) / 4.0f; // ?? this is strange

	//-

	return v;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawInsertionPoint(float _x, float _y, float _w)
{
	float r = 3;

	ofPushStyle();
	ofFill();
	float v = ofMap((ofGetFrameNum() % 60) / 60.f, 0, 60, 0.25, 1);
	ofSetColor(colorDebug, v * 255);
	ofDrawCircle(_x, _y, r);

	if (_w != 0) {//an extra line
		ofNoFill();
		ofSetColor(colorDebug, 64);
		ofDrawLine(_x - 10, _y, _x + _w, _y);
	}

	ofPopStyle();
}

#ifdef USE_IM_GUI__SUBTTITTLES

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

	if (bGui_ViewFull)
	{
		//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_MEDIUM;
		/*
		float w = 200;
		ImVec2 size_max = ImVec2(-1, -1);
		ImVec2 size_min = ImVec2(w, -1);
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);
		*/

		if (ui->BeginWindow(bGui_ViewFull, ImGuiWindowFlags_None))
		{
			ui->AddLabelBig(pathSrt);
			ui->AddSpacingBigSeparated();

			ImGui::BeginChild("_");
			for (int n = 0; n < subsText.size(); n++)
			{
				float w = ImGui::GetContentRegionAvail().x;
				float w1 = 40;
				float w2 = w - w1;
				ImGui::Columns(2, "t");
				//ImGui::SetCursorPosX(w100 * 0.3f);
				ImGui::SetColumnWidth(0, w1);
				ImGui::SetColumnWidth(1, w2);
				ui->AddLabel(ofToString(n));

				ImGui::NextColumn();
				ui->AddLabel(subsText[n]);
				//ui->AddLabelBig(subsText[n]);

				ImGui::Columns(1);
			}
			ImGui::EndChild();
			ui->EndWindow();
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWidgets()
{
	//ui->AddGroup(subs.params);

	ui->Add(ui->bMinimize, OFX_IM_TOGGLE_ROUNDED_SMALL);
	ui->AddSpacingSeparated();

	ui->Add(bDraw, OFX_IM_TOGGLE_ROUNDED);

	// time label
	string s = "";
	string s2 = "";
	{
		if (!ui->bMinimize) {
			s += pathSrt;
			s += "\n";
		}
		if (bPlay) {
			uint64_t t = ofGetElapsedTimeMillis() - tPlay;

#ifdef USING_OFX_TIME_CODE
			s2 += timecode.timecodeForMillis(t);
#else
			s2 += t / 1000.f;
#endif
		}
		else {
			s2 += sub[currentLine]->getStartTimeString();
	}
		s2 += "\n";
		s += ofToString(currentLine) + "/" + ofToString(sub.size() - 1);
}

	if (!ui->bMinimize) {
		if (bDraw) {
			ui->Add(bGui_ViewFull, OFX_IM_TOGGLE_ROUNDED_SMALL);
			ui->Add(bDebug, OFX_IM_TOGGLE_ROUNDED_SMALL);
			//if(bDebug) ui->Add(box.bEdit, OFX_IM_TOGGLE_ROUNDED_MINI);
			if (bDebug) ui->Add(bGui_Internal, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
		}

		ui->AddSpacingSeparated();
		ui->AddLabelBig("TRANSPORT");
		ui->AddSpacing();
		ui->Add(bPlay, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bPlayForce, OFX_IM_TOGGLE_SMALL, 2);
		if (bPlayForce) { ui->Add(speedForce); }
		ui->AddLabelBig(s2);
		ui->AddLabel(s);
		ui->Add(currentLine, OFX_IM_HSLIDER_MINI_NO_NAME);
		ui->PushButtonRepeat();
		ui->Add(bPrev, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bNext, OFX_IM_TOGGLE_SMALL, 2);
		ui->PopButtonRepeat();
		ui->AddSpacingSeparated();
	}
	else {//minimized
		ui->AddSpacingSeparated();
		//ui->AddLabelBig("TRANSPORT");		
		ui->AddSpacing();
		ui->Add(bPlay, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bPlayForce, OFX_IM_TOGGLE_SMALL, 2);
		if (bPlayForce) { ui->Add(speedForce); }
		ui->AddLabelBig(s2);
		ui->AddLabel(s);
		ui->Add(currentLine, OFX_IM_HSLIDER_MINI_NO_NAME);
		ui->PushButtonRepeat();
		ui->Add(bPrev, OFX_IM_TOGGLE_SMALL, 2, true);
		ui->Add(bNext, OFX_IM_TOGGLE_SMALL, 2);
		ui->PopButtonRepeat();
		ui->AddSpacing();
	}

	//if (!ui->bMinimize) 
	{
		if (!ui->bMinimize) ui->AddLabelBig("FADE");
		ui->Add(bAnimated, OFX_IM_TOGGLE_SMALL, 2, true);
		if (bAnimated) {
			ui->Add(speedFade, OFX_IM_HSLIDER_MINI_NO_LABELS, 2);
			static ofParameter<float>v{ "v", 0, 0, 1 };
			v.setWithoutEventNotifications(alpha);
			ui->Add(v, OFX_IM_PROGRESS_BAR_NO_TEXT, 2);
		}
		ui->AddSpacingSeparated();
	}

	//ui->AddGroup(params_Style);
	if (!ui->bMinimize) {
		ui->AddLabelBig("STYLE");
		ui->Add(fName);
		ui->Add(fColorBg, OFX_IM_COLOR_NO_ALPHA);
	}
	ui->Add(fColor, OFX_IM_COLOR);
	ui->AddSpacing();
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
}

#endif