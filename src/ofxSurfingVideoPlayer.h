#pragma once
#include "ofMain.h"
#include "ofxPlaybackGui.h"
#include "ofxSurfingHelpers.h"

class ofxSurfingVideoPlayer
{
public:
	ofxSurfingVideoPlayer();
	~ofxSurfingVideoPlayer();

	ofVideoPlayer movie;

	void doOpenFile();
	void processOpenFileSelection(ofFileDialogResult openFileResult);
	void dragEvent(ofDragInfo dragInfo);

	void setup();
	void startup();
	void exit();

	void setupVideo(string path, bool bAbs = true);
	void setupGuiVideo();
	void updateVideo();
	void drawVideo();
	void drawGui();

	bool bLoaded = false;
	bool bScale = true;

	ofParameterGroup params_VideoPlayer{ "Video Player" };
	ofParameter<void> bOpenVideo{ "Open" };
	ofParameter<bool> bGui_VideoPlayer{ "VIDEO PLAYER", true };
	ofParameter<bool> bDraw_Video{ "Draw Video", true };
	//ofParameter<bool> bEnableAudio{ "Enable Audio", true };
	ofParameter<float> volume{ "Volume", 0.5, 0, 1 };
	ofParameter<float> position{ "Position", 0, 0, 1 };
	ofParameter <string> path_Video{ "Path", "" };

	//void play();
	//void stop();

	ofxPanel guiPlayer;
	ofxPlaybackGui playback;
	ofEventListeners listeners;
	string pathVideoPlayerSettings = "VideoPlayer.json";

	void doForwards();
	void doBackwards();

	//--

	// Transport

public:

	bool isPlaying() const {
		movie.isPlaying();
	}

	void setTogglePlay() {
		movie.stop();
	}

	void play() {
		movie.play();
	}

	void stop() {
		movie.stop();
	}

	void pause() {
		movie.setPaused(true);
	}
};

