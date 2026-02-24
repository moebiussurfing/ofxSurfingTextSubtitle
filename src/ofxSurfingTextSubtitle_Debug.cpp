#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updateDebug() {

	// Calculate progress

	if (sub.size() == 0) {
		//ofLogVerbose("ofxSurfingTextSubtitle") << "SUB object empty!";
		progressPlayFilm = 0;
	} else {
		// Progress related to the elapsed player time
		if (indexModes == 1 && bPlayStandalone) {
			uint64_t t = ofGetElapsedTimeMillis() - tPlay;

			uint64_t ts;
			uint64_t te;

			// workaround:
			// fix this case
			if (currentDialog == 0) {
				ts = 0;
				te = sub[0]->getEndTime();
				progressPlaySlide = ofMap(t, ts, te, 0, 1, true);
			} else {
			}

			progressPlayFilm = ofMap(t, 0, tEndSubsFilm, 0, 1, true);
		}

		//else if (bPlayForced || !bPlayStandalone)
		// for any other modes
		else if (indexModes == 0 || indexModes == 2 || indexModes == 3) {
			progressPlayFilm = ofMap(currentDialog, 0, sub.size(), 0, 1); //TODO: wrong counting by slides..
		}
	}

	//--

	// Debug info
	{
#ifdef USE_WIDGET__SUBTITLES
		if (bDrawWidgetInfo) {
			string s = "";

			//--

			uint64_t t = 0;

			//--

			if (indexModes == 0 && bPlayExternal) {
				t = positionExternal * (float)tEndSubsFilm;
			}

			//--

			else if (indexModes == 1 && bPlayStandalone) {
	#ifdef USE_WIDGET__VIDEO_PLAYER
				if (player.playback.play) {
					t = ofGetElapsedTimeMillis() - tPlay;
				} else { //stopped
					t = tPlay;
				}
	#else
				t = ofGetElapsedTimeMillis() - tPlay;
	#endif
			}

			//--

			else if (indexModes == 2 && bPlayForced) {
	#ifdef USE_WIDGET__VIDEO_PLAYER
				if (player.playback.play) {
					t = ofGetElapsedTimeMillis() - tPlayForceFilm;
				} else { //stopped
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

			if (bDebug) {
				s += path_Srt; // .srt filename
				s += "\n";
			}

			//--

			// Display begin/end subs times
			if (bDebug) {
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
void ofxSurfingTextSubtitle::drawDebug() {
	if (!bGui) return; //workflow
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

		////TODO:
		//// improve getting the bigger rectangle..
		//if (0) {
		//	float diff = boxDrawnReal.getHeight() - bb.getHeight();
		//	bool b = (diff > 0);
		//	if (b) {
		//		bb.setHeight(boxDrawnReal.getHeight());
		//		bb.translateY(-diff / 2);
		//	}
		//}

		//--

		{
			int padx = 50;
			int pady = -50;
			//int pady = 50;

			//--

			// 1. Timeline
			{
				{
					// 1.1 Big line

					ofPushStyle();

					float lw1 = 3;
					float lh1 = 5;

					glm::vec2 p = (bTop.get() ? (bb.getTopLeft() + glm::vec2(0, -pady)) : (bb.getBottomLeft() + glm::vec2(0, pady)));

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

					float szt = 12; //triangles

					//--

					// 1.2 Fade Marks

					//TODO:
					// mark fade in phase end point with a vertical line
					// hard to do bc using speed instead of time duration in ms!

					if (bAnimatedFades && bAnimatedIn) {
						float d = 1.f / dtAnimIn;

						// estimated duration of the fade in phase,
						// bc we are applying this amount of the above
						// dt on every single frame!
						int t = (d / fps) * 1000;

						float r = 0;
						if (bPlayForced) {
							r = (t / (float)durationPlayForced); //ratio
						}
						if (bPlayStandalone || bPlayExternal || bPlayManual) {
							r = (t / (float)durationPlaySlide); //ratio
						}
						xIn = p.x + w * r;

						if (xIn != -1) {
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
					if (bAnimatedFades && bAnimatedOut) {
						ofSetLineWidth(lw1);

						// Fade out begins
						float r = 0.f;
						if (bPlayForced) {
							r = 1.f - (durationOut / (float)durationPlayForced); //ratio
						}
						if (bPlayStandalone || bPlayExternal || bPlayManual) {
							r = 1.f - (durationOut / (float)durationPlaySlide); //ratio
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

				if (bAnimatedFades && (bAnimatedIn || bAnimatedOut)) {
					ofSetLineWidth(1.f);
					ofSetColor(bTheme.get() ? colorDebugLight : colorDebugDark, 64);

					if (bTop)
						ofDrawLine(bb.getX(), bb.getY() - pady, bb.getX() + bb.getWidth(), bb.getY() - pady);
					else
						ofDrawLine(bb.getBottomLeft().x, bb.getBottomLeft().y + pady, bb.getBottomLeft().x + bb.getWidth(), bb.getBottomLeft().y + pady);
				}
			}

			//--

			// 2. Alpha Preview

			// Lateral Widget.
			// Vertical bar to show current opacity

			//if (alpha != 0)
			if (bAnimatedFades && (bAnimatedIn || bAnimatedOut)) {
				float xpadDebug = 10;
				float ypadDebug = 10;

				float x = 0;
				float y = 0;

				float x1 = 0;
				float x2 = 0;
				float y1 = 0;
				float y2 = 0;

				float w = 3;

				float h = bb.getHeight() - (2 * ypadDebug);

				static ofBitmapFont f;
				//float o = 0;//correct font
				float o = 3; //offset1 correct font
				float o2 = 10; //offset2

				glm::vec2 p;
				string s;

				//--

				if (bLeft)
					x = bb.getTopLeft().x - xpadDebug;
				else
					x = bb.getTopRight().x + xpadDebug;

				y = bb.getTopLeft().y + ypadDebug + h;

				s = "ALPHA";
				auto bb1 = f.getBoundingBox(s, 0, 0);
				x1 = bb1.getWidth() + o + o2;
				//x1 = bb1.getWidth() / 2 - o;

				ofDrawBitmapStringHighlight(s, x - x1, y - y1,
					(bTheme.get() ? 255 : 0), (bTheme.get() ? 0 : 255));

				//--

				if (bLeft) {
					p = bb.getTopLeft();
					x = p.x - xpadDebug;
				} else {
					p = bb.getTopRight();
					x = p.x + xpadDebug;
				}

				y = p.y + ypadDebug + h;

				// bar
				ofColor c = ofColor(colorTextFloat.get(), alpha * colorTextFloat.get().a * 255);
				ofSetColor(c);
				ofFill();
				ofRectangle r(x, y, w, -h * alpha);
				ofDrawRectangle(r);

				//--

				if ((bAnimatedIn && isAnimIn) || (bAnimatedOut && isAnimOut)) {
					if ((bAnimatedIn && isAnimIn))
						s = "IN";
					else if (bAnimatedOut && isAnimOut)
						s = "OUT";

					auto bb2 = f.getBoundingBox(s, 0, 0);

					//x2 = bb2.getWidth() / 2 - o;
					x2 = bb2.getWidth() + o + o2;
					if (bTop)
						y2 = ypadDebug + o2;
					else
						y2 = ypadDebug + h - 20;

					ofDrawBitmapStringHighlight(s, x - x2, y2,
						(bTheme.get() ? 255 : 0), (bTheme.get() ? 0 : 255));
				}
			}

			//----

			// Debug boxDrawnEstimated

			if (bDebug2) {
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
void ofxSurfingTextSubtitle::drawInsertionPoint(float _x, float _y, float _w, float _h) {
	float r = 2;

	ofPushStyle();

	ofFill();
	float v = ofMap(ofGetFrameNum() % 60, 0, 60, 0.25, 1);
	ofSetColor(bTheme ? colorDebugLight : colorDebugDark, v * 255);
	ofDrawCircle(_x, _y, r);

	if (_w != 0) { //an extra horizontal line
		ofNoFill();
		ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 64);
		ofDrawLine(_x - _w, _y, _x + _w, _y);
	}
	if (_h != 0) { //an extra vertical line
		ofNoFill();
		ofSetColor(bTheme ? colorDebugLight : colorDebugDark, 64);
		ofDrawLine(_x, _y - _h, _x, _y + _h);
	}

	ofPopStyle();
}

//--------------------------------------------------------------
