#include "ofxSurfingTextSubtitle.h"

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
void ofxSurfingTextSubtitle::updateEngine() {

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
}

void ofxSurfingTextSubtitle::doSetSrtSlideStart(SubtitleItem * element) {

	tPlayStartSlide = ofGetElapsedTimeMillis();
	isSlidePlaying = true;
	auto ts = element->getStartTime();
	auto te = element->getEndTime();
	durationPlaySlide = te - ts;

	doRefreshDraw();
}
