#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setup(string _pathSrt) {

	pathSrt = _pathSrt;

	setupParams();
	setupSubs();

	ofxSurfingHelpers::setThemeDarkMini_ofxGui();
	gui.setup(params);

	boxInfo.setup();

	box.setBorderColor(colorDebug);
	box.setup();

	startup();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupParams() {

	float _dpiScale = 2.0;
	bool _mipmaps = true;
	int _textDimension = 2048;

	fName = "ExtendedBold";

	// search some fonts alternatives
	fPath = "fonts/GTAmerica-ExtendedBold.ttf";
	if (!ofFile::doesFileExist(fPath.get())) {
		fPath = "assets/fonts/JetBrainsMono-ExtraBold.ttf";
		if (!ofFile::doesFileExist(fPath.get())) {
			fPath = OF_TTF_SANS;
		}
	}

	bool b = font.setup(fPath, 1.0, _textDimension, _mipmaps, 8, _dpiScale);
	if (!b) ofLogError("ofxSurfingTextSubtitle") << "Font file not found: " << fPath;

	bDraw.set("Draw", true);
	bPlay.set("Play", false);
	bDebug.set("Debug", false);
	bAuto.set("Auto", false);
	speedAuto.set("Speed", 0.5, 0, 1);

	// style
	fSize.set("Size", 50, 5, (float)MAX_FONT_SIZE);
	fSizePrc.set("Size Prc", 0.5, 0.1, 1.0);
	fSpacing.set("Spacing", 0, -20, 50);
	fLineHeight.set("Height", 0.75, 0.5, 2.0);
	fColor.set("Color", ofColor::white, ofColor(0, 0), ofColor(255, 255));
	fAlign.set("Align", 1, 1, 3);
	fAlign_str.set("Align ", "-1");
	bResetFont.set("Reset", false);

	params.setName("Text Subtitle");
	params.add(bGui);
	params.add(bDraw);
	params.add(bPlay);
	params.add(bAuto);
	params.add(speedAuto);
	params.add(bDebug);
	//params.add(box.bGui);
	box.bGui.makeReferenceTo(bDebug);

	params_Style.setName("Style");
	params_Style.add(fName);
	params_Style.add(fColor);
	params_Style.add(fSizePrc);
	params_Style.add(fLineHeight);
	params_Style.add(fSpacing);
	params_Style.add(fAlign);
	params_Style.add(fAlign_str);
	params_Style.add(bResetFont);
	params.add(params_Style);

	//bPlay.setSerializable(false);
	//bAuto.setSerializable(false);
	fPath.setSerializable(false);
	fName.setSerializable(false);
	fAlign_str.setSerializable(false);
	bResetFont.setSerializable(false);

	ofAddListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

	refreshFontStyles();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupSubs() {

	subParserFactory = new SubtitleParserFactory(ofToDataPath(pathSrt));

	if (!subParserFactory) ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << pathSrt;

	parser = subParserFactory->getParser();
	sub = parser->getSubtitles();

	int i = 0;
	subsText.clear();

	for (SubtitleItem* element : sub)
	{
		string s = element->getDialogue();
		ofLogNotice("ofxSurfingTextSubtitle") << i++ << " " << s;
		subsText.push_back(s);
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::startup() {
	ofxSurfingHelpers::load(params);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::refreshFontStyles() { // refresh font styles after any change
	ofLogNotice("ofxSurfingTextSubtitle") << "refreshFontStyles";

	fAlign_str = getAlignNameFromIndex(fAlign.get());

	font.setCharacterSpacing(fSpacing);
	font.setLineHeight(fLineHeight);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::exit() {
	ofLogNotice("ofxSurfingTextSubtitle") << "exit";
	ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);
	ofxSurfingHelpers::save(params);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawGui() {
	if (!bGui_Internal) return;

	// info
	boxInfo.draw();

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

	// text
	ofRectangle r = drawTextBox(textCurrent, box.getRectangle());
	boxhMax = r.getHeight();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::update() {
	if (!bDraw) return;

	if (bPlay && !bAuto)
	{
		if (bAuto) bAuto = false;

		uint64_t t = ofGetElapsedTimeMillis() - tPlay;
		int k = 0;

		for (SubtitleItem* element : sub)
		{
			if (t > element->getStartTime() && t <= element->getEndTime())
			{
				textCurrent = element->getDialogue();
				currentSub = k;
				break;
			}
			k++;
		}
	}

	if (bAuto && !bPlay)
	{
		int n = ofMap(speedAuto, 0, 1, 100, 5);
		if (ofGetFrameNum() % n == 0) currentSub++;
		if (currentSub >= sub.size() - 1) currentSub = 0;
		textCurrent = sub[currentSub]->getDialogue();
	}

	// info
	string s = "";
	s += pathSrt;
	s += "\n";
	s += ofToString(currentSub) + "/" + ofToString(sub.size() - 1);
	s += "\n";
	s += sub[currentSub]->getStartTimeString();
	s += "\n";
	if (bPlay)
	{
		uint64_t t = ofGetElapsedTimeMillis() - tPlay;
		s += ofToString(t / 1000);
		s += "\n";
	}
	boxInfo.setText(s);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::draw() {
	if (!bDraw) return;

	// container
	///*
	if (box.getHeight() < boxhMax) {
		box.setHeight(boxhMax);
	}
	//*/

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
	ofLogNotice("ofxSurfingTextSubtitle") << name << " : " << e;

	if (false) {}

	else if (name == bAuto.getName())
	{
		if (bAuto) {
			bPlay = false;
		}
	}

	else if (name == bPlay.getName())
	{
		if (bPlay) {
			bAuto = false;
			currentSub = 0;
			tPlay = ofGetElapsedTimeMillis();
		}
	}

	// font styles
	else if (name == fSizePrc.getName())
	{
		fSize = fSizePrc * fSize.getMax();
		//refreshFontStyles();
	}

	// font size, spacing, line height
	else if (name == fSpacing.getName() || name == fLineHeight.getName())
	{
		refreshFontStyles();
	}

	// alignment
	else if (name == fAlign.getName())
	{
		fAlign_str = getAlignNameFromIndex(fAlign.get());
	}

	else if (name == bResetFont.getName() && bResetFont.get())
	{
		bAttending = true;

		bResetFont = false;
		fSizePrc = 0.5;
		fSpacing = 0;
		fLineHeight = 0.75;
		fColor = ofColor(255, 255);
		bAttending = false;

		refreshFontStyles();
	}
}

//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::drawTextBox(std::string _str, ofRectangle r) {
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
void ofxSurfingTextSubtitle::drawInsertionPoint(float _x, float _y, float _w) {
	ofPushStyle();
	ofFill();
	ofSetColor(colorDebug, (ofGetFrameNum() % 60) / 60. * 255);
	ofDrawCircle(_x, _y, 2);
	ofSetColor(colorDebug, 64);
	ofDrawLine(_x - 10, _y, _x + _w, _y);
	ofPopStyle();
}