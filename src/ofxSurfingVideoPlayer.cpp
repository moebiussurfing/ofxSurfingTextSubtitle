#include "ofxSurfingVideoPlayer.h"

ofxSurfingVideoPlayer::ofxSurfingVideoPlayer() {
	//path_Video = "movies/fingers.mov";
};

ofxSurfingVideoPlayer::~ofxSurfingVideoPlayer() {

};

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::setup()
{
	setupGuiVideo();

	// Video
	//path_Video = ofToDataPath("Z:\\_DATA\\VIDEO\\Huxley_.mp4");
	//path_Video = (ofToDataPath("Z:\\_DATA\\VIDEO\\Huxley_.mp4", true));
	//path_Video = "movies/fingers.mov";
	//path_Video = (ofToDataPath("Z:\\_DATA\\VIDEO\\LET.mp4", true));
	//path_Video = (ofToDataPath("Z:\\_DATA\\\VIDEO\\fingers.mov", true));
	//setupVideo(path_Video, true);
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::setupVideo(string path, bool bAbs) {

	bLoaded = movie.load(ofToDataPath(path, bAbs));

	/*
	// Video
	// NOTE: compiled as Debug on Windows drops down too much the framerate.
	//bLoaded = movie.load("movies/fingers.mov");
	bLoaded = movie.load(ofToDataPath("Z:\\_DATA\\VIDEO\\Huxley_.mp4", true));
	//bLoaded = movie.load(ofToDataPath("Z:\\_DATA\\VIDEO\\LET.mp4", true));
	//bLoaded = movie.load(ofToDataPath("Z:\\_DATA\\\VIDEO\\fingers.mov", true));
	*/

	//--
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::startup()
{
	ofxSurfingHelpers::loadGroup(params_VideoPlayer, pathVideoPlayerSettings);
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::exit()
{
	ofxSurfingHelpers::saveGroup(params_VideoPlayer, pathVideoPlayerSettings);
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::setupGuiVideo()
{
	// playback widget
	playback.setup();

	guiPlayer.setup("VIDEO PLAYER");
	guiPlayer.add(bDraw_Video);
	guiPlayer.add(bOpenVideo);
	guiPlayer.add(path_Video);
	//guiPlayer.add(bEnableAudio);
	guiPlayer.add(volume);
	guiPlayer.add(position);
	guiPlayer.add(&playback);

	//auto p = glm::vec2(gui.getShape().getTopRight().x + 2, gui.getShape().getTopRight().y);
	//guiPlayer.setPosition(p.x, p.y);
	guiPlayer.setPosition(250, 10);

	//--

	// create listeners for the buttons

	listeners.push(playback.stop.newListener([&]() {
		ofLogNotice("ofxSurfingVideoPlayer") << "Stop pressed\n";
		//if (bPlay) 
		if (playback.play)
		{
			//pause();
			stop();
		}
		else {
			position.set(0);
			stop();
		}
		}));

	listeners.push(bOpenVideo.newListener([&]() {
		ofLogNotice("ofxSurfingVideoPlayer") << "Open Video\n";
		doOpenFile();
		}));

	listeners.push(playback.forwards.newListener([&]() {
		ofLogNotice("ofxSurfingVideoPlayer") << "Forwards pressed\n";
		doForwards();
		}));

	listeners.push(playback.backwards.newListener([&]() {
		ofLogNotice("ofxSurfingVideoPlayer") << "Backwards pressed\n";
		doBackwards();
		}));

	listeners.push(playback.rec.newListener([&]() {
		ofLogNotice("ofxSurfingVideoPlayer") << "Rec pressed\n";
		}));

	listeners.push(playback.play.newListener([&](bool& b) {
		ofLogNotice("ofxSurfingVideoPlayer") << "Play Paused pressed. Playing " << std::boolalpha << b << "\n";
		if (b) play();
		else pause();
		//else stop();
		}));

	listeners.push(volume.newListener([&](float& v) {
		ofLogNotice("ofxSurfingVideoPlayer") << "Volume:" << volume;
		movie.setVolume(volume);
		}));

	listeners.push(position.newListener([&](float& v) {
		ofLogVerbose("ofxSurfingVideoPlayer") << "Position:" << position;
		if (position == movie.getPosition()) return;
		movie.setPosition(v);
		}));

	listeners.push(path_Video.newListener([&](string& s) {
		ofLogNotice("ofxSurfingVideoPlayer") << "path_Video:" << path_Video;
		setupVideo(path_Video, true);
		}));

	//listeners.push(bEnableAudio.newListener([&](bool& b) {
	//	ofLogNotice("ofxSurfingVideoPlayer") << "bEnableAudio:" << std::boolalpha << bEnableAudio;
	//	//movie.audio(position);
	//	}));

	params_VideoPlayer.add(path_Video);
	params_VideoPlayer.add(bGui_VideoPlayer);
	params_VideoPlayer.add(bDraw_Video);
	params_VideoPlayer.add(volume);
	params_VideoPlayer.add(position);
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::updateVideo() {
	if (!bDraw_Video) movie.update();

	position.set(movie.getPosition());

	/*
	//TODO: WIP: link
	//float fps = movie.fps//not implemented?
	//float time = (movie.getDuration() / movie.getTotalNumFrames()) * movie.getCurrentFrame();//not implemented
	uint64_t frame = movie.getCurrentFrame();
	subs.update(frame);
	*/
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::drawGui() {
	if (bGui_VideoPlayer) guiPlayer.draw();
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::drawVideo() {
	if (!bLoaded)
		if (ofGetFrameNum() % 30 < 15)
			ofDrawBitmapStringHighlight("FILE NOT FOUND OR WRONG VIDEO FORMAT!", ofGetWidth() / 2, 20);

	if (!bDraw_Video) return;

	if (bScale) {
		ofRectangle r = ofGetCurrentViewport();
		ofRectangle rv(0, 0, movie.getWidth(), movie.getHeight());
		rv.scaleTo(r, OF_SCALEMODE_FILL);
		movie.draw(rv);
	}
	else movie.draw(0, 0);
}

////--------------------------------------------------------------
//void ofxSurfingVideoPlayer::play() {
//	play();
//	movie.play();
//}
//
////--------------------------------------------------------------
//void ofxSurfingVideoPlayer::stop() {
//	stop();
//	movie.stop();
//}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::doForwards()
{
	auto p = position.get();
	//auto cf = movie.getCurrentFrame();
	auto f = movie.getDuration() / movie.getTotalNumFrames(); // duration per frame
	auto i = f / movie.getDuration();
	//auto step = 30 / f;//frames
	auto step = 0.1f;
	position.set(p + step);
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::doBackwards()
{
	auto p = position.get();
	//auto cf = movie.getCurrentFrame();
	auto f = movie.getDuration() / movie.getTotalNumFrames(); // duration per frame
	auto i = f / movie.getDuration();
	//auto step = 30 / f;//frames
	auto step = 0.1f;
	position.set(p - step);
}


//--------------------------------------------------------------
void ofxSurfingVideoPlayer::dragEvent(ofDragInfo info) {
	/*
	if (info.files.size() > 0) {
		dragPt = info.position;

		draggedImages.assign(info.files.size(), ofImage());
		for (unsigned int k = 0; k < info.files.size(); k++) {
			draggedImages[k].load(info.files[k]);
		}
	}
	*/
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::processOpenFileSelection(ofFileDialogResult openFileResult) {

	ofLogNotice("ofxSurfingVideoPlayer") << ("getName(): " + openFileResult.getName());
	ofLogNotice("ofxSurfingVideoPlayer") << ("getPath(): " + openFileResult.getPath());

	string originalFileExtension;
	string path = openFileResult.getPath();
	ofFile file(openFileResult.getPath());

	if (file.exists()) {

		ofLogNotice("ofxSurfingVideoPlayer") << ("The file exists - now checking the type via file extension");
		string fileExtension = ofToUpper(file.getExtension());

		//We only want 
		if (fileExtension == "MP4" ||
			fileExtension == "MKV" ||
			fileExtension == "3GP" ||
			fileExtension == "AVI" ||
			fileExtension == "DIVX" ||
			fileExtension == "DV" ||
			fileExtension == "H264" ||
			fileExtension == "MKV" ||
			fileExtension == "MP4" ||
			fileExtension == "MPEG" ||
			fileExtension == "MPG" ||
			fileExtension == "MOV" ||
			fileExtension == "MXF" ||
			fileExtension == "OGG" ||
			fileExtension == "OGM" ||
			fileExtension == "OGV" ||
			fileExtension == "OMF" ||
			fileExtension == "QT" ||
			fileExtension == "WEBM" ||
			fileExtension == "WMV")
		{

			//Save the file extension to use when we save out
			originalFileExtension = fileExtension;

			//load video
			path_Video.set(path);
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingVideoPlayer::doOpenFile()
{
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a video file .mp4 or .mkv");

	//Check if the user opened a file
	if (openFileResult.bSuccess) {

		ofLogNotice("ofxSurfingVideoPlayer") << ("User selected a file");

		//We have a file, check it and process it
		processOpenFileSelection(openFileResult);
	}
	else {
		ofLogNotice("ofxSurfingVideoPlayer") << ("User hit cancel");
	}
}