#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setPosition(float position) {
	if (indexModes == 0) positionExternal.set(position);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updatePosition(float position) {
	if (indexModes == 0) {
		positionExternal.set(position);
	}

	update();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::updateFrame(uint64_t frame) {
	//TODO: WIP:
	//not implemented yet
	//external link

#ifdef USE_TIME_CODE__SUBTITLES
	uint64_t t = timecode.millisForFrame(frame);

	// Force sync mode
	//if (!bPlayStandalone) bPlayStandalone = true;

	if (bPlayStandalone) {
		//tPlay = t;

		/*
		uint64_t tDiff = tPlay - t;
		if (tDiff > 0) {
		}
		else {
		}
		*/
	}
	if (bPlayForced) {
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
void ofxSurfingTextSubtitle::setTogglePlayForced() { bPlayForced = !bPlayForced; }

//--------------------------------------------------------------
bool ofxSurfingTextSubtitle::isPlaying() const {
	return bPlayStandalone.get();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setTogglePlay() {
	bool playStandalone = bPlayStandalone.get();
	bool playForced = bPlayForced.get();
	bool playManual = bPlayManual.get();

	auto mode = static_cast<ofxSurfingSubtitle::PlaybackMode>(indexModes.get());
	apiManager.togglePlay(mode, playStandalone, playForced, playManual);

	bPlayStandalone = playStandalone;
	bPlayForced = playForced;
	bPlayManual = playManual;

	//#ifdef USE_WIDGET__VIDEO_PLAYER
	//	if (bPlayStandalone) player.play();
	//	else player.stop();
	//#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::play() {
	bPlayStandalone = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::stop() {
	if (bPlayStandalone) bPlayStandalone = false;
	if (bPlayForced) bPlayForced = false;
	if (bPlayManual) bPlayManual = false;

	if (isAnimIn) isAnimIn = false;
	if (isAnimOut) isAnimOut = false;

	textCurrent = "";
	currentDialog = 0;
	positionExternal = 0;
	progressPlaySlide = 0;
	progressPlayFilm = 0;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::pause() {
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doBuildDataTextBlocks(string s, bool bNumbered) {
	ofLogNotice("ofxSurfingTextSubtitle") << "doBuildDataTextBlocks()";
	ofLogNotice("ofxSurfingTextSubtitle") << endl
										  << s;

	if (!bNumbered)
		dataTextBlocks = ofxSurfingHelpers::splitTextBlocks(s);
	else
		dataTextBlocks = ofxSurfingHelpers::splitTextBlocksNumbered(s);
	ofLogNotice("ofxSurfingTextSubtitle") << "Amount Blocks: " << dataTextBlocks.size();

	ofLogNotice("ofxSurfingTextSubtitle") << "Print Blocks";
	for (size_t i = 0; i < dataTextBlocks.size(); i++) {
		string s = "#" + ofToString(i) + ": " + dataTextBlocks[i];

#ifdef USE_IM_GUI__SUBTITLES
		ui->AddToLog(s);
#else
	#ifdef USE_OFX_GUI__SUBTITLES
		ofLogNotice("ofxSurfingTextSubtitle") << s;
	#endif
#endif
	}

	if (dataTextBlocks.size() > 0)
		bModeTextBlocks = true;
	else
		ofLogError("ofxSurfingTextSubtitle") << "blocks are empty";

	currentDialog.setMax(dataTextBlocks.size() - 1);

	//workflow
	currentDialog = 0;
	//bPlayStandalone = true;
	bPlayForced = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doSetTextSlideStartFile(string path) {
	string s = ofxSurfingHelpers::loadFileText(path);
	if (s != "") {
		doSetTextSlideStart(s);

		bLoadedFileText = true;
		path_Srt = "";
		name_Srt = "";
	} else {
		ofLogError("ofxSurfingTextSubtitle") << "Not valid file found: " << path;
		bLoadedFileText = false;
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doSetTextSlideStart(string s) {
	bool b = (indexModes == 3 && bPlayManual); // MANUAL

	lastTextSlideRaw = s; //store raw to allow hot upper/lower capitalize

	//TODO: should force?
	//// workflow
	//// A. Force mode manual
	//if (!b)
	//{
	//	if (indexModes != 3) indexModes = 3;
	//	if (!bPlayManual) bPlayManual = true;
	//}

	// workflow
	// B. Lock if wrong mode
	//if (b)
	{
		if (bCapitalize)
			textCurrent = ofToUpper(s);
		else
			textCurrent = s;

		progressPlaySlide = 0;
		ofLogNotice("ofxSurfingTextSubtitle") << "doSetTextSlideStart: " << textCurrent;

		//--

		// Fades

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

		tPlayStartSlide = ofGetElapsedTimeMillis();
		isSlidePlaying = true;
		durationPlaySlide = durationPlayForced;

		doRefreshDraw();
	}
	// workflow
	//else {
	//	ofLogWarning("ofxSurfingTextSubtitle") << "Ignoring doSetTextSlideStart bc not in MANUAL mode";
	//}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::notifySlideStartingFadeIn() {
	ofLogNotice("ofxSurfingTextSubtitle") << "notifySlideStartingFadeIn()";
	bStartingFadeOutNotified = false;

	ofxSurfingTextSubtitleEventArgs args;
	args.index = currentDialog.get();
	args.text = textCurrent;
	args.isTextBlocks = bModeTextBlocks;

	ofNotifyEvent(onSlideStartingFadein, args, this);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::notifySlideStartingFadeOut() {
	ofLogNotice("ofxSurfingTextSubtitle") << "notifySlideStartingFadeOut()";

	ofxSurfingTextSubtitleEventArgs args;
	args.index = currentDialog.get();
	args.text = textCurrent;
	args.isTextBlocks = bModeTextBlocks;

	ofNotifyEvent(onSlideStartingFadeOut, args, this);
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::startingSlide() {
	notifySlideStartingFadeIn();
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::endingSlide() {
	notifySlideStartingFadeOut();
}
