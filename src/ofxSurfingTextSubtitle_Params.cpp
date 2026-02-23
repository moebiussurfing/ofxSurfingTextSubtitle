#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupParams() {
	ofLogNotice(__FUNCTION__);

	// force load default font
	setupFont();

	//--

	// Control
	bStop.set("Stop");
	bOpenSrt.set("Open Srt");
	bOpenText.set("Open Text");
	bDraw.set("DRAW", true);
	bLive.set("LIVE!", false);
	bEdit.set("Edit", true);
	bDebug.set("Debug", true);
	bExtra.set("Extra", false);
	bTop.set("Top Timeline", true);
	bLeft.set("Left Alpha", false);
#ifdef USE_WIDGET__SUBTITLES
	bDrawWidgetInfo.set("Info", true);
#endif
	currentDialog.set("Dialog", 0, 0, 0);
	bPlayStandalone.set("Play Standalone", false);
	bPlayForced.set("Play Forced", false);
	durationPlayForced.set("Duration Forced", 2000, 200, 10000);
	bPrev.set("<");
	bNext.set(">");
	//speedPlayForce.set("Speed", 0, 0, 1);
	bPlayExternal.set("Play External", false);
	positionExternal.set("Position", 0, 0, 1);
	bPlayManual.set("Play Manual", false);

	progressPlayFilm.set("% Film", 0, 0, 1); //full length progress
	progressPlaySlide.set("% Slide", 0, 0, 1); //slide progress

	bAutoScroll.set("Auto Scroll", true); //for ImGui only

	// Fade
	bAnimatedFades.set("Fades", true);
	bAnimatedIn.set("In", true);
	bAnimatedOut.set("Out", true);
	durationIn.set("Duration In", 1000, 1, 2000);
	durationOut.set("Duration Out", 1000, 1, 2000);
	progressIn.set("% In", 0, 0, 1);
	progressOut.set("% Out", 0, 0, 1);
	bResetFades.set("Reset Fades");

	// Style
	fSize.set("SizeR", 50, 5, (float)MAX_FONT_SIZE);
	fSizePrc.set("Size", 0.2, 0.1, 1.0f);
	fSpacing.set("Spacing", 0, -30, 30);
	fLineHeight.set("Height", 0.75, 0.5, 2.0);
	colorTextFloat.set("Color", ofFloatColor::white, ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	//colorTextShadow.set("ColorSw", ofFloatColor::black, ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	//offsetShadow.set("Offset", glm::vec2(0, 0), glm::vec2(-100, -100), glm::vec2(100, 100));
	colorBgFloat.set("ColorBg", ofFloatColor(50 / 255.f), ofFloatColor(0.f, 0.f), ofFloatColor(1.f, 1.f));
	fAlign.set("Align", 0, 0, 2);
	fAlign_str.set("Align ", "-1");
	bReset.set("Reset");
	bResetAll.set("Reset All");

	bCenteredV.set("y Centered", true);
	amountLinesTarget.set("Lines", 6, 1, 10);

	//bMinimize.set("Minimize", false);

	bCapitalize.set("Capitalize", false);
	bResponsive.set("Responsive", false);
	resizeResponsive.set("Resize", 0, 0, 1);
	yOffset.set("y Offset", 0, -1, 1);

	indexModes.set("Modes", 2, 0, 3);

	indexModes_Name.set("Name", "");
	indexModes_Name.setSerializable(false);

	box.bGui.makeReferenceTo(bEdit);

#ifdef USE_WIDGET__SUBTITLES
	boxInfo.bGui.makeReferenceTo(bEdit);
#endif

	//--

	//TODO: crashes when loading settings if uncommented!
	//bPlayStandalone.setSerializable(false);//!
	//bPlayForced.setSerializable(false);

	//fPath.setSerializable(false);
	fName.setSerializable(false);
	fAlign_str.setSerializable(false);
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
	params_Control.add(bOpenSrt);
	params_Control.add(bOpenText);
	params_Control.add(bDraw);
	params_Control.add(bLive);
	params_Control.add(bEdit);
	params_Control.add(bExtra);
	params_Control.add(bDebug);
	params_Control.add(bUseFbo);
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

	params_Control.add(bResetAll);

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
	params_Fade.add(bAnimatedFades);
	params_Fade.add(bAnimatedIn);
	params_Fade.add(bAnimatedOut);
	params_Fade.add(progressPlaySlide);

	params_FadeIn.setName("F In");
	params_FadeOut.setName("F Out");
	//params_FadeIn.add(speedFadeIn);
	params_FadeIn.add(progressIn); //TODO: should change speed mode to time duration. like on fade out!
	params_FadeIn.add(durationIn);

	//params_FadeOut.add(speedFadeOut);
	params_FadeOut.add(progressOut);
	params_FadeOut.add(durationOut);

	params_Fade.add(params_FadeIn);
	params_Fade.add(params_FadeOut);
	params_Fade.add(bResetFades);

	params_Style.setName("Style");
	params_Style.add(fPath);
	//params_Style.add(fName);
	params_Style.add(colorTextFloat);
	//#ifdef USE_SHADOW
	//	params_Style.add(colorTextShadow);
	//	params_Style.add(offsetShadow);
	//#endif
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
	params_Style.add(yOffset);
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
	params.add(bGui);

	ofAddListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

	//--

	//doReset();
	doResetAll();

	//--

	// Preset
	params_Preset.setName("ofxSurfingTextSubtitle_Presets");
	params_Preset.add(params_Style);
	params_Preset.add(params_Fade);
	params_Preset.add(bTheme); //extra

	//--

	// App Settings / session
	params_AppSettings.setName("ofxSurfingTextSubtitle");
	params_AppSettings.add(params_Control);
	params_AppSettings.add(params_Transport);
	params_AppSettings.add(bGui);

	//--

	//TODO:	gt.setPathGlobal(path_Global);
	settingsStore.setPaths(path_Global, path_SubtitlerSettings);
	gt.addGroup(params_AppSettings, settingsStore.getSettingsPath());
}

//--------------------------------------------------------------
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

				doRefreshDraw();
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

		doRefreshDraw();
	} else if (name == durationOut.getName()) {
		auto _numframes = durationOut / (float)(dt * 1000);
		float _margin = 1.f; // fade out increment per frame. from 0 to 1 or vice versa.
		dtAnimOut = _margin / (float)_numframes;

		doRefreshDraw();
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

		doRefreshDraw();
	}

	else if (name == fSizePrc.getName()) {
		fSize = fSizePrc * fSize.getMax();
		//fSize = fSizePrc * box.getWidth() / 5;

		doRefreshDraw();
	}
	// spacing
	else if (name == fSpacing.getName()) {
		font.setCharacterSpacing(fSpacing);

		doRefreshDraw();
	}
	// height
	else if (name == fLineHeight.getName()) {
		font.setLineHeight(fLineHeight);

		doRefreshDraw();
	}
	// alignment
	else if (name == fAlign.getName()) {
		fAlign_str = getAlignNameFromIndex(fAlign.get());

		doRefreshDraw();
	}

	// reset all
	else if (name == bResetAll.getName()) {
		doResetAll();
	}
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
		doRefreshDraw();
	} else if (name == colorBgFloat.getName()) {
		doRefreshDraw();
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

//--------------------------------------------------------------

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doResetAll() {
	ofLogNotice(__FUNCTION__);

	colorTextFloat.set(ofFloatColor::white);
	colorBgFloat.set(ofFloatColor(50 / 255.f));

	currentDialog = 0;
	bDraw.set(true);
	bLive.set(false);
	bExtra.set(false);
	bEdit.set(true);
	bDebug.set(true);
	bTop.set(true);
	bLeft.set(false);

	doReset();
	doResetFades();

	//--

	//// A. Force default mode FORCED
	//if (indexModes != 2) indexModes = 2;
	//if (!bPlayForced) bPlayForced = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doReset() {
	ofLogNotice(__FUNCTION__);

	fSpacing = 0;
	fLineHeight = 0.75;
	fSizePrc = 0.25;
	fAlign = 0;

	amountLinesTarget = 6;
	bCenteredV = true;
	bResponsive = true;
	resizeResponsive = 0.5;
	yOffset = 0;
}

//--------------------------------------------------------------
std::string ofxSurfingTextSubtitle::getAlignNameFromIndex(int index) const {
	std::string n = "UNKNOWN";
	switch (index) {
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

	doRefreshDraw();
}

//--------------------------------------------------------------
