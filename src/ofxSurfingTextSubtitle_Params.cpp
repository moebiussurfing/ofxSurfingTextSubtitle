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
	bEnableColorBg.set("Enable Bg", true);
	fAlign.set("Align", 0, 0, 2);
	fAlign_str.set("Align ", "-1");
	bReset.set("Reset");
	bResetAll.set("Reset All");
	//vSave.set("Save");

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

#ifdef USE_IM_GUI__SUBTITLES
	params_Control.add(bGui_List);
	params_Control.add(bGui_Paragraph);
	params_Control.add(bMinimize);
#endif

	//params_Control.add(bGui);
	//params_Control.add(box.bEdit);

	params_Control.add(bResetAll);
	//params_Control.add(vSave);

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
	params_Style.add(bEnableColorBg);
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
void ofxSurfingTextSubtitle::doResetAll() {
	ofLogNotice(__FUNCTION__);

	colorTextFloat.set(ofFloatColor::white);
	colorBgFloat.set(ofFloatColor(50 / 255.f));
	bEnableColorBg = true;

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
