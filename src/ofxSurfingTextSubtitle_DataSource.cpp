#include "ofxSurfingTextSubtitle.h"

void ofxSurfingTextSubtitle::setupSubs(string _pathSrt) {
	path_Srt = _pathSrt;

	//ofFile file(path_Srt);
	ofFile file(ofToDataPath(path_Srt));

	name_Srt = file.getBaseName();

	ofLogNotice(__FUNCTION__) << " " << path_Srt;

	//bLoaded = ofFile::doesFileExist(path_Srt);
	//if (!bLoaded) {
	//	ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << path_Srt;
	//}

	subParserFactory = new SubtitleParserFactory(ofToDataPath(path_Srt));

	//bGui_List.setName(path_Srt);

	parser = subParserFactory->getParser();
	sub = parser->getSubtitles();
	if (sub.size() == 0) {
		ofLogError("ofxSurfingTextSubtitle") << "SUB object empty!";
	}

	//--

	if (!subParserFactory || sub.size() == 0) {
		bLoadedFileSubs = false;
		ofLogError("ofxSurfingTextSubtitle") << ".srt file not found: " << path_Srt;
	} else {
		bLoadedFileSubs = true;
		ofLogNotice("ofxSurfingTextSubtitle") << "Successfully loaded .srt file: " << path_Srt;
	}

	if (!bLoadedFileSubs) {
		ofLogError("ofxSurfingTextSubtitle") << "Incomplete initialization!";
		return;
	}

	//--

	currentDialog.setMax(sub.size() - 1);

	// pre read all subs on a vector
	buildDataSubs();

	//TODO: not sure if .srt file is loaded before the first frame!

	//--

	// duration taken from last subtitle end.
	// on external model could be get from the video duration!
	// get duration from srt subs if is not passed by the user,
	// by passing the real video film duration!
	if (tEndSubsFilm == 0) tEndSubsFilm = sub.back()->getEndTime();

	//TODO:
	//indexModes = indexModes.get();

	//workflow
	if (bModeTextBlocks) bModeTextBlocks = false;
	path_Text = "";
	name_Text = "";
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::buildDataSubs() {
	ofLogNotice(__FUNCTION__) << " " << path_Srt;

	//not used yet
	int i = 0;
	dataTextSubs.clear();
	for (SubtitleItem * element : sub) {
		string s = element->getDialogue();

		//TODO:
		//if (bCapitalize) s = ofToUpper(s);

		ofLogNotice("ofxSurfingTextSubtitle") << i++ << " " << s;
		dataTextSubs.push_back(s);
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doOpenFileSrt() {
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a .srt file.");

	//Check if the user opened a file
	if (openFileResult.bSuccess) {

		ofLogNotice("ofxSurfingTextSubtitle") << "User selected a file";

		//We have a file, check it and process it
		processOpenFileSrtSelection(openFileResult);
	} else {
		ofLogNotice("ofxSurfingTextSubtitle") << "User hit cancel";
	}

	//TODO:
	//refresh
	indexModes = indexModes.get();

	////workflow
	//if (bModeNoSrt) bModeNoSrt = false;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::processOpenFileSrtSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("ofxSurfingTextSubtitle") << "Name: " + openFileResult.getName();
	ofLogNotice("ofxSurfingTextSubtitle") << "Path: " + openFileResult.getPath();

	string path = openFileResult.getPath();
	ofFile file(openFileResult.getPath());

	if (file.exists()) {

		ofLogNotice("ofxSurfingTextSubtitle") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		//We only want
		if (fileExtension == "SRT") {
			//load subs
			setupSubs(path);

			//workflow
			//bPlayStandalone = true;
			//bPlayForced = true;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::doOpenFileText() {
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a .txt file.");

	//Check if the user opened a file
	if (openFileResult.bSuccess) {

		ofLogNotice("ofxSurfingTextSubtitle") << "User selected a file";

		//We have a file, check it and process it
		processOpenFileTextSelection(openFileResult);
	} else {
		ofLogNotice("ofxSurfingTextSubtitle") << "User hit cancel";
	}

	//TODO:
	//refresh
	indexModes = indexModes.get();

	////workflow
	//if (bModeNoSrt) bModeNoSrt = true;
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::processOpenFileTextSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("ofxSurfingTextSubtitle") << "Name: " + openFileResult.getName();
	ofLogNotice("ofxSurfingTextSubtitle") << "Path: " + openFileResult.getPath();

	string path = openFileResult.getPath();
	ofFile file(openFileResult.getPath());

	if (file.exists()) {
		ofLogNotice("ofxSurfingTextSubtitle") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		//We only want
		if (fileExtension == "TXT") {
			//load text
			string s = ofxSurfingHelpers::loadFileText(path);
			doBuildDataTextBlocks(s);

			//workflow
			//bPlayStandalone = true;
			//bPlayForced = true;

			path_Text = path;
			ofFile file(ofToDataPath(path));
			name_Text = file.getBaseName();

			bLoadedFileText = true;
			path_Srt = "";
			name_Srt = "";
		}
	} else {
		ofLogError("ofxSurfingTextSubtitle") << "Not valid file found";
		bLoadedFileText = false;
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setupTextBlocks(string path) {

	ofLogNotice("ofxSurfingTextSubtitle:setupTextBlocks") << "path: " + path;

	ofFile file(path);

	if (file.exists()) {
		ofLogNotice("ofxSurfingTextSubtitle:setupTextBlocks") << "The file exists - now checking the type via file extension";
		string fileExtension = ofToUpper(file.getExtension());

		// We only want
		if (fileExtension == "TXT") {
			// load text
			string s = ofxSurfingHelpers::loadFileText(path);
			doBuildDataTextBlocks(s);

			path_Text = path;
			ofFile file(ofToDataPath(path));
			name_Text = file.getBaseName();

			bModeTextBlocks = true;

			// workflow
			//bPlayStandalone = true;
			//bPlayForced = true;
			currentDialog = 0;

			bLoadedFileText = true;
			path_Srt = "";
			name_Srt = "";
		}
	} else {
		ofLogError("ofxSurfingTextSubtitle") << "Not valid file found";
		bLoadedFileText = false;
	}
}
