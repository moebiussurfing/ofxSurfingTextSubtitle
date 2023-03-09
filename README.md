## OVERVIEW

Ready to use customizable `.SRT` text subtitle (and plain text) player for `openFrameworks`. 

## FEATURES

- Fast `FontStash` render.
- Custom `Paragraph Styles`.
- Scalable and draggable `container`.
- `Persistent` settings.
- Full `srt` file browser for exploring the dialogs.
- Animated `Fade In/Out` for transparency.
- Transport `Player` with 4 Modes: 
	- STANDALONE: Dialogs following the original `srt` times and durations.
	- FORCED: Customizable dialogs duration. Ignoring `srt` times.
	- EXTERNAL: Time is controlled by a video player or linked to an external control.  
	- MANUAL: To work without loadgin any SRT file. Just pass the text slides manually.
- `Presets` Engine for styles (Optional). 
	
## SCREENCAST VIDEO

[VIDEO](https://youtu.be/kcObeooL3Pc)

## EXAMPLES / SCREENSHOTS

#### - Example-Subtitle / ofxGui 
![Screenshot](example-Subtitle/Capture.PNG)

#### - Example-Subtitle2 / ImGui (Optional) 
![Screenshot](example-Subtitle2/Capture.PNG)

#### - Example-Subtitle3 / ofxGui (Video Player) 
![Screenshot](example-Subtitle3/Capture.PNG)

#### - Example-SubtitleWhisper / Whisper / ImGui (real-time audio to text) [ADVANCED / WIP]  
![Screenshot](example-SubtitleWhisper/Capture.PNG)  
Requires this [FORK](https://github.com/moebiussurfing/ofxWhisper) of [ofxWhisper](https://github.com/roymacdonald/ofxWhisper) from [@roymacdonald](https://github.com/roymacdonald).  

[VIDEO](https://youtu.be/G8iH-0UakN4)


## USAGE

#### ofApp.h
```.cpp
#include "ofxSurfingTextSubtitle.h"
// Remember to comment to use ofxGui only and to disable ImGui!
// #define USE_IM_GUI__SUBTITLES

ofxSurfingTextSubtitle subs;
```

#### ofApp.cpp
```.cpp
void ofApp::setup() {
	subs.setup("Alphaville.srt");
}
void ofApp::update() {
	subs.update();
}
void ofApp::draw() {
	subs.draw();
	subs.drawGui();
}
```

## DEPENDENCIES
- [ofxFontStash](https://github.com/armadillu/ofxFontStash)
- [ofxSurfingBox](https://github.com/moebiussurfing/ofxSurfingBox)
- [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)
- **ofxGui** / oF core

### OPTIONAL
- [ofxSufingImGui](https://github.com/moebiussurfing/ofxSurfingImGui)
- [ofxWhisper](https://github.com/moebiussurfing/ofxWhisper) / FORK (Only for the example)

### TODO
- [**BUG**] Fix some words remove when building paragraph.
- List and load different render fonts files on runtime.  
- macOS testing.

## TESTED SYSTEM
* **Windows 10** / **VS 2017** / **OF ~0.11**

## THANKS
* SRT parser: file `srtparser.h`.  
Taken from https://github.com/Jonathhhan/ofEmscriptenExamples. Thanks `@Jonathhhan`.  
Originally by: https://github.com/saurabhshri/simple-yet-powerful-srt-subtitle-parser-cpp.  
* [ofxPlaybackGui](https://github.com/roymacdonald/ofxPlaybackGui) from [@roymacdonald](https://github.com/roymacdonald) bundled into `/libs`. Optional to use only when using **ofxGui** as GUI. (Can be removed when using **ofxSurfingImGui**.)
* [ofxWhisper](https://github.com/roymacdonald/ofxWhisper) from [@roymacdonald](https://github.com/roymacdonald). Powered by [whisper.cpp](https://github.com/ggerganov/whisper.cpp).  

## LICENSE
**MIT License**