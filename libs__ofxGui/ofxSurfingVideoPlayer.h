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
	void setupParams();
	void update();
	void drawVideo();
	void drawGui();

	//bool bLoaded = false;
	ofParameter<bool>  bLoaded{ "DoneLoaded", false };
	bool bScale = true;

	ofParameterGroup params_VideoPlayer{ "Video Player" };
	ofParameter<void> bOpenVideo{ "Open" };
	ofParameter<bool> bGui{ "VIDEO PLAYER", true };
	ofParameter<bool> bGui_VideoPlayer{ "VIDEO PLAYER ", true };
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
		return movie.isPlaying();
	}

	void setTogglePlay() {
		playback.play = !playback.play;
	}

	void play() {
		//movie.play();
		if (!playback.play) playback.play = true;
	}

	void pause() {
		//movie.setPaused(true);
		if (playback.play) playback.play = false;
	}

	void stop() {
		//movie.stop();
		playback.stop.trigger();
	}
};

