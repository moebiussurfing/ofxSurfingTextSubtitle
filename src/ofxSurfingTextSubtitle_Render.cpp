#include "ofxSurfingTextSubtitle.h"

void ofxSurfingTextSubtitle::draw(ofRectangle view) {
	if (!bDraw) return;

	float x = view.getX();
	float y = view.getY();
	float w = view.getWidth(); //TODO: not handled
	float h = view.getHeight();

	ofPushMatrix();
	ofTranslate(x, y);

	draw();

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawRaw() {
	if (!bDraw) return;

	//--

	// Force box height

	if (bCenteredV || bResponsive) {
		float h = getLineHeightUnit();
		//float h = getOneLineHeight() + getSpacingBetweenLines();

		//TODO:
		//box.setHeight(amountLinesTarget * h);
	} else {
	}
	//TODO:
	//box.setLockH();

	//TODO:
	// 1st pass. Expected approx amount lines of the last current drawn
	// Will update amountLinesDrawn!
	if (bDoRefreshNoDraw) {
		//bypass bc below there's another drawTextBox/ noDraw call!
		//bDoRefreshNoDraw = false;//do once
		getTextBoxEstimate(textCurrent, box.getRectangle());
	}

	//--

	bool bNoDraw;

	// 1. No draw
	// Will update boxDrawnEstimated
	// True. No drawing!
	// Just calculate and distribute lines!
	bNoDraw = true;

	if (bDoRefreshNoDraw) {
		drawTextBox(textCurrent, box.getRectangle(), bNoDraw);
		bDoRefreshNoDraw = false; //do once
		//boxDrawnEstimated will not be refreshed until next slide or box container changes!
	} else {
		//workaround to refresh. will be auto set last frozen value if not..
	}

	//--

	float _offset = 0;

	if (bCenteredV) {
		ofPushMatrix();

		_offset = -(boxDrawnEstimated.getHeight() * 0.5f) + (box.getHeight() * 0.5f);

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

	// 2. Draw

	// The Real Real drawing!

	bNoDraw = false;
	boxDrawnReal = drawTextBox(textCurrent, box.getRectangle(), bNoDraw);
	boxDrawnReal.translateY(_offset);

	//--

	if (bCenteredV) {
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

void ofxSurfingTextSubtitle::draw() {
	ofPushStyle();

	//---

	bool b = (bUseFbo && fbo.isAllocated());
	if (b) {
		if (bDoRefreshFboCapture) {
			bDoRefreshFboCapture = false;

			fbo.begin();

			//draw into fbo with full alpha
			if (bEnableColorBg) ofClear(colorBgFloat.get());
			ofSetColor(colorTextFloat.get());
			drawRaw();

			fbo.end();
		}

		int a = 255;
		if (bAnimatedFades && (bAnimatedIn || bAnimatedOut)) {
			a *= alpha;
		}

		//---

		// draw fbo with alpha
		ofSetColor(255, a);
		fbo.draw(0, 0);

	} else {
		drawRaw();
	}

	//---

	ofPopStyle();

	//----

	if (!bGui) return;

	//TODO: fix locking moving the box!
	//it happens when moving floating preview window sometimes.
	if (!bLive) box.draw();

	//drawDebug();
}

//--

//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::drawTextBox(std::string _str, ofRectangle r, bool bNoDraw) {

	//--

	// NOTE that uses only the rectangle width!
	// Not the height!

	float _x = box.getX();
	float _y = box.getY();
	float _w = box.getWidth();
	float _h = box.getHeight();

	float _yOffset = 0;
	if (yOffset != 0) _yOffset = ofMap(yOffset, -1, 1, -_h / 2, _h / 2, true);

	int _size = 1.f;
	int _align = fAlign.get();
	ofColor _colorText = colorTextFloat.get();
	//ofColor _colorTextShadow = colorTextShadow.get();
	//float _x2 = _x + offsetShadow.get().x;
	//float _y2 = _y + offsetShadow.get().y;

	// How many less or more amount lines that expected/targeted
	diff = amountLinesDrawn - amountLinesTarget;

	// Non responsive
	if (!bResponsive) {
		_size = fSize.get();
	}
	// Responsive
	else {
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

			float ho = oneLineHeight;
			float hb = box.getHeight();

			float rLimit;
			float rMax;
			float rMin;

			float r; // ratio
			// could use a variable ratio depending on the amount of lines

			float s = 1.f; //scaler

			//Default
			//_size = r * fSize.get();

			// A. Same lines than expected
			if (diff == 0) {
				sEngine = "A";

				// do nothing
				_size = fSize.get();
			}

			// B. More lines than expected
			else if (diff > 0) {
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
			else if (diff < 0) {
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
				} else if (amountLinesDrawn == 2) {
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
	if (!bNoDraw) {
		if (bUseFbo) {
		} else {
			if (bAnimatedFades && (bAnimatedIn || bAnimatedOut)) {
				_colorText = ofColor(_colorText, alpha * _colorText.a);
				//_colorTextShadow = ofColor(_colorTextShadow, alpha * _colorTextShadow.a);
			}
		}
	}

	// here upper border is aligned to the center horizontal
	_y += oneLineHeight;
	//_y += getOneLineHeight();

	// Fix.
	// workaround to hard code calibrate.
	//_y += 2;

	//--

	_y += _yOffset;

	//------

	if (!bNoDraw) ofPushStyle();
	{
		if (!bNoDraw) {
			ofSetColor(_colorText);
		}

		float __x = _x;
		float __y = _y;

		int _numLines = -1; //will be reported after draw
		bool _wordsWereCropped = false; //will be reported after draw
		bool _bCenter = ((_align == 2) ? true : false); //centered

		/*get the final text formatting (by adding \n's) in the supplied string;
		BE ARWARE that using TRUE in here will modify your supplied string! */

		//bool _br = false;
		bool _br = bForceAddBreakLines;

		if (fAlign != 1) // left or center align
		{
			boxDrawnEstimated = font.drawMultiLineColumn(
				_str, /*string*/
				_size, /*size*/
				__x, __y, /*where*/
				MAX(10, _w), /*column width*/
				_numLines, /*get back the number of lines*/
				bNoDraw, /* if true, we wont draw (just get bbox back) */
				0, /* max number of lines to draw, crop after that */
				_br, /*get the final text formatting (by adding \n's) in the supplied	string;
										BE ARWARE that using TRUE in here will modify your supplied string! */
				&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
				_bCenter /*centered*/
			);
		} else // right align
		{
			//TODO:
			//WIP trying to make work right align
			if (bForceAddBreakLines) {
				auto __bbox = font.drawMultiLineColumn(
					_str, /*string*/
					_size, /*size*/
					__x, __y, /*where*/
					MAX(10, _w), /*column width*/
					_numLines, /*get back the number of lines*/
					true, /* if true, we wont draw (just get bbox back) */
					0, /* max number of lines to draw, crop after that */
					_br, /*get the final text formatting (by adding \n's) in the supplied string;
											BE ARWARE that using TRUE in here will modify your supplied string! */
					&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
					_bCenter /*centered*/
				);

				_str = _str + " "; //fix last char
				//textCurrent = _str;
			}

			if (!bNoDraw) {
				boxDrawnEstimated = font.drawMultiLine(
					_str, /*string*/
					_size, /*size*/
					_x, _y, /*where*/
					OF_ALIGN_HORZ_RIGHT,
					MAX(10, _w) /*column width*/
				);
			}
			//TODO: workaround
			else {
				boxDrawnEstimated = font.getBBox(
					_str, /*string*/
					_size, /*size*/
					__x, __y, /*where*/
					OF_ALIGN_HORZ_RIGHT,
					MAX(10, _w) /*column width*/
				);
			}
		}
	}
	if (!bNoDraw) ofPopStyle();

	//--

	return boxDrawnEstimated;
}

//--------------------------------------------------------------
ofRectangle ofxSurfingTextSubtitle::getTextBoxEstimate(std::string _str, ofRectangle r) {
	float _x = box.getX();
	float _y = box.getY();
	float _w = box.getWidth();
	float _h = box.getHeight();

	int _align;
	int _size;

	_size = fSize.get();
	_align = fAlign.get();

	// here upper border is aligned to the center horizontal
	//_y += getOneLineHeight();
	_y += oneLineHeight;

	//------

	ofRectangle _bbox;
	int _numLines = 20;
	bool _wordsWereCropped = false;
	bool _bCenter = ((_align == 3) ? true : false);
	bool _br = false;

	_bbox = font.drawMultiLineColumn(_str, /*string*/
		_size, /*size*/
		_x, _y, /*where*/
		MAX(10, _w), /*column width*/
		_numLines, /*get back the number of lines*/
		true, /* if true, we wont draw (just get bbox back) */
		0, /* max number of lines to draw, crop after that */
		_br, /*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
		_bCenter /*centered*/
	);

	amountLinesDrawn = _numLines; // amount lines of the last current drawn

	return _bbox;
}

//--------------------------------------------------------------
float ofxSurfingTextSubtitle::getLineHeightUnit() {
	return oneLineHeight + spacingBetweenLines;
	//return oneLineHeight + getSpacingBetweenLines();
	//return getOneLineHeight() + getSpacingBetweenLines();
}

//--------------------------------------------------------------
float ofxSurfingTextSubtitle::getOneLineHeight(bool oneOnly) {
	// Pre calculate line heights. without the spacing

	bool _br = false;

	// to get more precise value, we should use 3 lines,
	// applying also lines-height and dividing by 3
	//std::string _str1 = "T";// one line

	int _align;
	int _size;

	_size = fSize.get();
	_align = fAlign.get();

	//--

	ofRectangle _bbox;
	int _numLines = 20;
	bool _wordsWereCropped = false;
	bool _bCenter = ((_align == 3) ? true : false);
	float _w = 1000;
	int _x, _y;
	_x = 0;
	_y = 0;
	float v1, v2, v3, v4, v;

	_bbox = font.drawMultiLineColumn(_str1, /*string*/
		_size, /*size*/
		_x, _y, /*where*/
		MAX(10, _w), /*column width*/
		_numLines, /*get back the number of lines*/
		true, /* if true, we wont draw (just get bbox back) */
		0, /* max number of lines to draw, crop after that */
		_br, /*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
		_bCenter /*centered*/
	);
	v1 = MAX(_bbox.getHeight(), 1);

	//-

	if (!oneOnly) {
		//boxDrawnEstimated = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
		_bbox = font.drawMultiLineColumn(_str2, /*string*/
			_size, /*size*/
			_x, _y, /*where*/
			MAX(10, _w), /*column width*/
			_numLines, /*get back the number of lines*/
			true, /* if true, we wont draw (just get bbox back) */
			0, /* max number of lines to draw, crop after that */
			_br, /*get the final text formatting (by adding \n's) in the supplied string;
									BE ARWARE that using TRUE in here will modify your supplied string! */
			&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
			_bCenter /*centered*/
		);
		v2 = MAX(_bbox.getHeight(), 1) / 2.0f;

		//-

		//boxDrawnEstimated = font.drawMultiLine(_str3, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
		_bbox = font.drawMultiLineColumn(_str3, /*string*/
			_size, /*size*/
			_x, _y, /*where*/
			MAX(10, _w), /*column width*/
			_numLines, /*get back the number of lines*/
			true, /* if true, we wont draw (just get bbox back) */
			0, /* max number of lines to draw, crop after that */
			_br, /*get the final text formatting (by adding \n's) in the supplied string;
									BE ARWARE that using TRUE in here will modify your supplied string! */
			&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
			_bCenter /*centered*/
		);
		v3 = MAX(_bbox.getHeight(), 1) / 3.0f;

		//-

		//boxDrawnEstimated = font.drawMultiLine(_str4, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
		_bbox = font.drawMultiLineColumn(_str4, /*string*/
			_size, /*size*/
			_x, _y, /*where*/
			MAX(10, _w), /*column width*/
			_numLines, /*get back the number of lines*/
			true, /* if true, we wont draw (just get bbox back) */
			0, /* max number of lines to draw, crop after that */
			_br, /*get the final text formatting (by adding \n's) in the supplied string;
									BE ARWARE that using TRUE in here will modify your supplied string! */
			&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
			_bCenter /*centered*/
		);
		v4 = MAX(_bbox.getHeight(), 1) / 4.0f;
	}

	// improve precision making more-than-one-line-size situations

	//-

	if (oneOnly)
		v = v1;
	else
		v = (v1 + v2 + v3 + v4) / 4.0f;

	//-

	return v;
}

//--------------------------------------------------------------
float ofxSurfingTextSubtitle::getSpacingBetweenLines() {
	// Pre calculate line heights
	// to do that we measure a two lines height and subtract the space between the lines
	// that depends on spacing and height line.

	float spacing = -1;

	bool _br = false;

	// to get more precise value, we should use 3 lines,
	// applying also lines-height and dividing by 3
	std::string _str1 = "T"; // one lines

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

	_bbox = font.drawMultiLineColumn(_str1, /*string*/
		_size, /*size*/
		_x, _y, /*where*/
		MAX(10, _w), /*column width*/
		_numLines, /*get back the number of lines*/
		true, /* if true, we wont draw (just get bbox back) */
		0, /* max number of lines to draw, crop after that */
		_br, /*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
		_bCenter /*centered*/
	);
	v1 = MAX(_bbox.getHeight(), 1);

	//-

	std::string _str2 = "T\nT"; // two lines

	//boxDrawnEstimated = font.drawMultiLine(_str2, _size, 0, 0, OF_ALIGN_HORZ_LEFT, 1000);
	_bbox = font.drawMultiLineColumn(_str2, /*string*/
		_size, /*size*/
		_x, _y, /*where*/
		MAX(10, _w), /*column width*/
		_numLines, /*get back the number of lines*/
		true, /* if true, we wont draw (just get bbox back) */
		0, /* max number of lines to draw, crop after that */
		_br, /*get the final text formatting (by adding \n's) in the supplied string;
								BE ARWARE that using TRUE in here will modify your supplied string! */
		&_wordsWereCropped, /* this bool will b set to true if the box was to small to fit all text*/
		_bCenter /*centered*/
	);
	v2 = MAX(_bbox.getHeight(), 1);

	spacing = v2 - (2 * v1);

	return spacing;
}
