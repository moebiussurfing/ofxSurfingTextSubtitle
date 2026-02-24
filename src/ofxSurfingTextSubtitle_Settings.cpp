#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::startup() {
	ofLogNotice(__FUNCTION__);

	//--

	initFbo();

	//--

	//#ifdef USE_IM_GUI__SUBTITLES
	//	this->setDisableGuiInternal(true);
	//#endif

	////doReset();
	//doResetAll();

	//--

	//gt.setPathGlobal(path_Global);
	//gt.addGroup(params_AppSettings, path_Global + "/" + path_SubtitlerSettings);

#ifdef USE_PRESETS__SUBTITLES
	//ofxSurfingHelpers::loadGroup(params_AppSettings, path_Global+path_SubtitlerSettings);
#else
	ofxSurfingHelpers::loadGroup(params, settingsStore.getSettingsPath());
#endif

	//--

	//TODO: do once
	oneLineHeight = getOneLineHeight();
	spacingBetweenLines = getSpacingBetweenLines();

	//--

	bDoneStartup = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::exit() {
	ofLogNotice("ofxSurfingTextSubtitle") << "exit";
	ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingTextSubtitle::Changed);

#ifdef USE_PRESETS__SUBTITLES
	//ofxSurfingHelpers::saveGroup(params_AppSettings, path_Global+path_SubtitlerSettings);
#else
	ofxSurfingHelpers::saveGroup(params, settingsStore.getSettingsPath());
#endif
}
