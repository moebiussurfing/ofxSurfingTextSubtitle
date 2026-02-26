#include "ofxSurfingTextSubtitle.h"

void ofxSurfingTextSubtitle::Changed(ofAbstractParameter & e) {
	static bool bDISABLE_CALLBACKS = false;
	if (bDISABLE_CALLBACKS) return;

	string name = e.getName();

	// ignore
	if (name == progressPlaySlide.getName() || name == progressPlayFilm.getName() || name == progressIn.getName() || name == progressOut.getName() ||
		//name == indexModes.getName() ||
		//name == currentDialog.getName() ||
		name == positionExternal.getName())
		return;

	// exclude logging
	if (name != indexModes.getName() && name != currentDialog.getName())
		ofLogNotice("ofxSurfingTextSubtitle") << "Changed " << name << " : " << e;

	//--

	if (0) {
	}

	//--

	// Modes

	//TODO: fix crash
	//else if (name == indexModes.getName()) return;
	//else if (name == indexModes_Name.getName()) return;

	else if (name == indexModes.getName()) {
		//static int indexModes_ = -1;
		//if (indexModes != indexModes_) {
		//	indexModes_ = indexModes;
		//}
		//else return;//not changed

		ofLogNotice("ofxSurfingTextSubtitle") << "Changed " << name << " : " << e;

#ifdef USE_OFX_GUI__SUBTITLES
		if (bGui_Internal && bGui_InternalAllowed) {
			auto & guit = gui.getGroup(params_Transport.getName());
			guit.getGroup(params_External.getName()).minimize();
			guit.getGroup(params_Forced.getName()).minimize();
			guit.getGroup(params_Standalone.getName()).minimize();

			//workflow
			switch (indexModes) {
			case 0:
				guit.getGroup(params_External.getName()).maximize();
				break;
			case 1:
				guit.getGroup(params_Standalone.getName()).maximize();
				break;
			case 2:
				guit.getGroup(params_Forced.getName()).maximize();
				break;
			}
		}
#endif

		//workflow
		switch (indexModes) {
			//case 0: bPlayExternal = true; break;
			//case 1: bPlayStandalone = true; break;
			//case 2: bPlayForced = true; break;

			//workflow
		case 0:
			indexModes_Name.setWithoutEventNotifications("EXTERNAL");
			break;
		case 1:
			indexModes_Name.setWithoutEventNotifications("STANDALONE");
			break;
		case 2:
			indexModes_Name.setWithoutEventNotifications("FORCED");
			break;
		case 3: {
			indexModes_Name.setWithoutEventNotifications("MANUAL");
			//workflow
			if (bGui_List) bGui_List = false;
			break;
		}
		}
	}

	//--

	// Dialog / text block index

	else if (name == currentDialog.getName()) {
		currentDialog.setWithoutEventNotifications(ofClamp(currentDialog.get(), currentDialog.getMin(), currentDialog.getMax()));

		//static int currentDialog_ = -1;
		//if (currentDialog != currentDialog_) {
		//	currentDialog_ = currentDialog;
		//}
		//else return;//not changed

		ofLogNotice("ofxSurfingTextSubtitle") << "Changed " << name << " : " << e;

		//if (indexModes == 3 && bPlayManual) return; // MANUAL

		//--

		// 1. Mode text blocks

		if (bModeTextBlocks) {
			if (dataTextBlocks.size() > 0) {
				if (currentDialog.get() < dataTextBlocks.size()) {
					if (currentDialog.get() == dataTextBlocks.size() - 1) {
						if (bCycled)
							currentDialog.setWithoutEventNotifications(0);

						else {
							//TODO:
							// stop
							if (bPlayForced) bPlayForced = false;
							//else if (bPlayStandalone) bPlayStandalone = false;
							//else if (bPlayExternal) bPlayExternal = false;
							//else if (bPlayManual) bPlayManual = false;

							currentDialog.set(0);

							lastTextSlideRaw = "";
							textCurrent = "";

							return;
						}
					}

					string s = dataTextBlocks[currentDialog.get()];
					lastTextSlideRaw = s;

					if (bCapitalize)
						textCurrent = ofToUpper(s);
					else
						textCurrent = s;

					doSetTextSlideStart(textCurrent);
				}
			}
		} else {
			// 2. Mode srt

			//TODO:
			//if (indexModes == 3 && bPlayManual) return; // MANUAL

			// Avoid / clamp errors
			if (sub.size() == 0) {
				currentDialog.setWithoutEventNotifications(0);
				textCurrent = "NO_TEXT";
				ofLogError("ofxSurfingTextSubtitle") << "Not loaded subs file or it's empty or wrong format.";

				return;
			}
			if (currentDialog < 0) {
				currentDialog.setWithoutEventNotifications(0); //go to first
				//currentDialog.setWithoutEventNotifications(sub.size() - 1);//last. cycled

				return;
			} else if (currentDialog > sub.size() - 1) {
				//cycled
				//go to first
				if (bCycled)
					currentDialog.setWithoutEventNotifications(0);
				else {
					//TODO:
					// stop
					if (bPlayForced)
						bPlayForced = false;
					else if (bPlayStandalone)
						bPlayStandalone = false;
					else if (bPlayExternal)
						bPlayExternal = false;
					//else if (bPlayManual) bPlayManual = false;

					currentDialog.set(0);

					lastTextSlideRaw = "";
					textCurrent = "";

					//return;
				}
				textCurrent = "NO_TEXT";
				ofLogError("ofxSurfingTextSubtitle") << "Current sub index out of range!";

				return;
			}

			//--

			// Get dialog
			if (currentDialog.get() < sub.size()) {
				string s = sub[currentDialog.get()]->getDialogue();

				//TODO:
				lastTextSlideRaw = s;

				if (bCapitalize)
					textCurrent = ofToUpper(s);
				else
					textCurrent = s;

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

				//doRefreshDraw();
			}
		}

		//--

		// Fades

		// start
		if (bAnimatedIn && bAnimatedOut) {
			isAnimIn = true;
			notifySlideStartingFadeIn();
			alpha = 0.f;
			if (isAnimOut) isAnimOut = false;
		} else if (bAnimatedIn && !bAnimatedOut) {
			isAnimIn = true;
			notifySlideStartingFadeIn();
			alpha = 0.f;
			//if (bPlayStandalone || bPlayForced) alpha = 1.f;//should force play
		} else if (!bAnimatedIn && bAnimatedOut) {
			alpha = 1.f;
		} else if (!bAnimatedIn && !bAnimatedOut) {
			alpha = 1.f;
		}

		/*
		if (bAnimatedOut)
		{
			isAnimOut = false;
		}
		*/

		if (bDebug && !bLive)
			ofLogNotice("ofxSurfingTextSubtitle") << textCurrent;
		else
			ofLogVerbose("ofxSurfingTextSubtitle") << textCurrent;
	}

	//--

	// TODO: for video link mode
	else if (name == bPlayExternal.getName()) {
		if (bPlayExternal) {
			if (bPlayStandalone) bPlayStandalone = false;
			if (bPlayForced) bPlayForced = false;
			if (bPlayManual) bPlayManual = false;

			progressPlaySlide = 0;

			//workflow
			//if (indexModes != 0) indexModes.setWithoutEventNotifications(0);
			if (indexModes != 0) indexModes.set(0);
		} else {
		}
	}

	else if (name == bPlayStandalone.getName()) {
		if (bPlayStandalone) {
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

		} else {
			//return;
			currentDialog.setWithoutEventNotifications(0);
			//currentDialog = 0;//crash

			if (bAnimatedIn && isAnimIn) isAnimIn = false;
			if (bAnimatedOut && isAnimOut) isAnimOut = false;
			//if (!bAnimatedIn && !bAnimatedOut) alpha = 1;//?

			progressPlaySlide = 0;
		}
	}

	else if (name == bPlayForced.getName()) {
		if (bPlayForced) {
			if (bPlayStandalone) bPlayStandalone.set(false);
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

		} else {
			tPlayForceFilm = 0;
			progressPlaySlide = 0;

			//isAnimIn = false;
			//isAnimOut = false;
			if (bAnimatedIn && isAnimIn) isAnimIn = false;
			if (bAnimatedOut && isAnimOut) isAnimOut = false;
			//alpha = 1;
		}
	}

	else if (name == bPlayManual.getName()) {
		if (bPlayManual) {
			tPlayForce = ofGetElapsedTimeMillis();
			//tPlayForceFilm = ofGetElapsedTimeMillis();

			if (bPlayStandalone) bPlayStandalone = false;
			if (bPlayForced) bPlayForced = false;
			if (bPlayExternal) bPlayExternal = false;

			progressPlaySlide = 0;

			//workflow
			//if (indexModes != 0) indexModes.setWithoutEventNotifications(0);
			if (indexModes != 3) indexModes.set(3);
		} else {
		}
	}

	//--

	else if (name == bStop.getName()) {
		this->stop();
	}

	//--

	// dt

	//else if (name == speedFadeIn.getName())
	//{
	//	dtAnimIn = ofMap(speedFadeIn, 0, 1, dt * DT_RATIO_MIN, dt * DT_RATIO_MAX, true);
	//}
	else if (name == durationIn.getName()) {
		auto _numframes = durationIn / (float)(dt * 1000);
		float _margin = 1.f; // fade in increment per frame. from 0 to 1 or vice versa.
		dtAnimIn = _margin / (float)_numframes;

		//doRefreshDraw();
	} else if (name == durationOut.getName()) {
		auto _numframes = durationOut / (float)(dt * 1000);
		float _margin = 1.f; // fade out increment per frame. from 0 to 1 or vice versa.
		dtAnimOut = _margin / (float)_numframes;

		//doRefreshDraw();
	}

	else if (name == bAnimatedIn.getName()) {
		if (!bAnimatedIn.get()) {
			alpha = 1;
			isAnimIn = false;
			//	if (bAnimatedOut.get()) alpha = 1;
		}
	}

	else if (name == bAnimatedOut.getName()) {
		if (bAnimatedOut.get()) {
			if (!bAnimatedIn.get()) alpha = 1;
		}
	}

	//--

	// next
	else if (name == bNext.getName()) {
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
	else if (name == bPrev.getName()) {
		currentDialog--;
	}

	// edit
	else if (name == bEdit.getName()) {
		box.bEdit = bEdit;
	}

	// debug
	else if (name == bDebug.getName()) {
		if (!bDebug)
			box.setBorderColor(ofColor(0, 0));
		else
			box.setBorderColor(ofColor(bTheme ? colorDebugLight : colorDebugDark, 64));
	}

	// theme
	else if (name == bTheme.getName()) {
		box.setBorderColor(ofColor(bTheme ? colorDebugLight : colorDebugDark, 64));
	}

	//--

	// Font Styles

	else if (name == bCapitalize.getName()) {
		if (bCapitalize)
			textCurrent = ofToUpper(lastTextSlideRaw);
		else
			textCurrent = lastTextSlideRaw;

		//doRefreshDraw();
	}

	else if (name == fSizePrc.getName()) {
		fSize = fSizePrc * fSize.getMax();
		//fSize = fSizePrc * box.getWidth() / 5;

		//doRefreshDraw();
	}
	// spacing
	else if (name == fSpacing.getName()) {
		font.setCharacterSpacing(fSpacing);

		//doRefreshDraw();
	}
	// height
	else if (name == fLineHeight.getName()) {
		font.setLineHeight(fLineHeight);

		//doRefreshDraw();
	}
	// alignment
	else if (name == fAlign.getName()) {
		fAlign_str = getAlignNameFromIndex(fAlign.get());

		//doRefreshDraw();
	}

	// reset all
	else if (name == bResetAll.getName()) {
		doResetAll();
	}
//	else if (name == vSave.getName()) {
//		ofLogNotice("ofxSurfingTextSubtitle") << "save";
//		ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);
//
//#ifdef USE_PRESETS__SUBTITLES
//		//ofxSurfingHelpers::saveGroup(params_AppSettings, path_Global+path_SubtitlerSettings);
//#else
//		ofxSurfingHelpers::saveGroup(params, settingsStore.getSettingsPath());
//#endif
//	}
	// reset style
	else if (name == bReset.getName()) {
		doReset();
	}
	// reset fades
	else if (name == bResetFades.getName()) {
		doResetFades();
	}

	//TODO:
	// box resize
	else if (name == amountLinesTarget.getName()) {

	}

	// Open srt
	else if (name == bOpenSrt.getName()) {
		doOpenFileSrt();
	}

	// Open text
	else if (name == bOpenText.getName()) {
		doOpenFileText();
	}

	// Font
	else if (name == fPath.getName()) {
		loadFont(fPath.get());
	}

	// Fbo/redraw engine
	else if (name == colorTextFloat.getName()) {
		//doRefreshDraw();
	} else if (name == colorBgFloat.getName()) {
		//doRefreshDraw();
	}

	else if (name == bLive.getName()) {
		//workflow
		if (bLive) bEdit = false;
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
