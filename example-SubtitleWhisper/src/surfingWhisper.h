#pragma once

#include "ofMain.h"

#include "ofxWhisper.h"
#include <deque>

class surfingWhisper {

public:
	surfingWhisper() {};
	~surfingWhisper() {};

	ofParameter<void> vCallback{ "vCallback" };
	ofParameter<bool> bDebug{ "Debug", false };

private:
	ofSoundStream soundStream;
	ofxWhisper whisper;
	deque<string> textQueue;

public:
	void setup()
	{
		ofxWhisperSettings whisperSettings;
		/// Using the default settings which are. Uncomment any line below and change value if needed

		// //n_threads "number of threads to use during computation\n", ;
		// whisperSettings.n_threads  = std::min(4, (int32_t) std::thread::hardware_concurrency());        
		 //whisperSettings.n_threads  = std::min(8, (int32_t) std::thread::hardware_concurrency());        
		// 
		// whisperSettings.step_ms    = 3000; //step_ms "audio step size in milliseconds\n",             ;
		// whisperSettings.length_ms  = 10000; //length_ms "audio length in milliseconds\n",                ;
		// whisperSettings.keep_ms    = 200; //keep_ms "audio to keep from previous step in ms\n",      ;
		// whisperSettings.capture_id = -1; //capture_id "capture device ID\n",                           ;
		// whisperSettings.max_tokens = 32; //max_tokens "maximum number of tokens per audio chunk\n",    ;
		// whisperSettings.audio_ctx  = 0; //audio_ctx "audio context size (0 - all)\n",                ;
		// whisperSettings.vad_thold    = 0.6f; //"voice activity detection threshold\n"
		// whisperSettings.freq_thold   = 100.0f; //"high-pass frequency cutoff\n"
		// whisperSettings.speed_up      = false; //"speed up audio by x2 (reduced accuracy)\n",
		// whisperSettings.translate     = false; //"translate from source language to english\n",
		// whisperSettings.print_special = false; //"print special tokens\n",
		// whisperSettings.no_context    = true; //"keep context between audio chunks\n",
		// whisperSettings.no_timestamps = false; 
		// whisperSettings.language  = "en"; //"spoken language\n",

		// customize
		if (1) {
			//https://github.com/ggerganov/whisper.cpp/tree/master/examples/stream
			whisperSettings.step_ms = 500; //step_ms "audio step size in milliseconds\n",             ;
			whisperSettings.length_ms = 5000; //length_ms "audio length in milliseconds\n",                ;

			whisperSettings.no_timestamps = true; // ignored

			whisperSettings.language = "es"; //"spoken language\n",
			whisperSettings.translate = false; //"translate from source language to english\n",

			whisperSettings.model = "models/ggml-base.bin"; //"model path\n",
			// whisperSettings.model = "models/ggml-base.en.bin"; //"model path\n",
		}

		whisper.setup(whisperSettings);

		//--

		ofSoundStreamSettings settings;
		soundStream.printDeviceList();

#ifdef TARGET_WIN32
		// set your device to the correct api. otherwise it might not work.
		// windows is a bit quirky about this.
		//auto devices = soundStream.getDeviceList(ofSoundDevice::Api::MS_DS);
		auto devices = soundStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);

		for (size_t i = 0; i < devices.size(); i++) {
			cout << i << "  : " << devices[i].name << endl;
		}

		size_t d = 7;//WASAPI
		//size_t d = 9;//DS

		// remember to choose the correct input device.
		settings.setInDevice(devices[d]);

		settings.setApi(ofSoundDevice::Api::MS_WASAPI);
		//settings.setApi(ofSoundDevice::Api::MS_DS);

		cout << endl << "Connecting to device #" << d << " " << devices[d].name << endl;
#else
		auto devices = soundStream.getMatchingDevices("default");
		if (!devices.empty()) {
			settings.setInDevice(devices[0]);
		}
#endif   

		settings.setInListener(whisper.audio_input.get());
		settings.sampleRate = 48000;
		settings.numOutputChannels = 0;
		//settings.numInputChannels = 2;
		settings.numInputChannels = 1;
		settings.bufferSize = 1024;
		soundStream.setup(settings);

		//soundStream.;
	};

	void update() {
		string newText = "";
		while (whisper.textChannel.tryReceive(newText)) {
			textQueue.push_back(newText);

			textLast = newText;
			vCallback.trigger();
		}

		size_t sz = 20;
		while (textQueue.size() > sz) {
			textQueue.pop_front();
		}
	};

	void draw() {
		if (!bDebug) return;

		ofBitmapFont bf;
		float y = 20;
		for (auto& t : textQueue) {
			auto r = bf.getBoundingBox(t, 0, 0);
			ofDrawBitmapStringHighlight(t, 20, y);
			y += r.height + 8;
		}
	};

	string getTextLast() const {
		return textLast;
	};

private:
	string textLast = "";
};