## OVERVIEW

Ready to use customizable `.SRT` text subtitle player for `openFrameworks`. 

## FEATURES

- Fast `FontStash` render.
- Custom `Paragraph Styles`.
- Scalable and draggable `container`.
- `Persistent` settings.
- Full `srt` file browser for exploring the dialogs.
- Animated `Fade In/Out` for transparency.
- Transport `Player` with 3 Modes: 
	- Standalone: Dialogs following the original `srt` times and durations.
	- Forced: Customizable dialogs duration. Ignoring `srt` times.
	- External: Time is controlled by a video player or linked to an external control. 
- `Presets` Engine for styles (Optional). 
	(Look `example-Subtitle2`. Requires ImGui.)
	
## SCREENCAST VIDEO

[![](https://markdown-videos.deta.dev/youtube/kcObeooL3Pc)](https://youtu.be/kcObeooL3Pc)
<!-- [![](https://img.youtube.com/vi/kcObeooL3Pc/maxresdefault.jpg)](https://www.youtube.com/watch?v=kcObeooL3Pc) -->

## SCREENSHOTS

### Example-Subtitle / ofxGui 
![Screenshot](example-Subtitle/Capture.PNG)

### Example-Subtitle2 / ImGui (Optional) 
![Screenshot](example-Subtitle2/Capture.PNG)

### Example-Subtitle3 / ofxGui (Video Player) 
![Screenshot](example-Subtitle3/Capture.PNG)

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
- ofxGui / oF core

### OPTIONAL
- [ofxSufingImGui](https://github.com/moebiussurfing/ofxSurfingImGui)

### TODO
<<<<<<< HEAD
- Load render fonts on runtime.
=======
- Load render fonts files on runtime.
>>>>>>> develop

## TESTED SYSTEM
* **Windows 10** / **VS 2017** / **OF ~0.11**

## THANKS
* SRT parser: file `srtparser.h` taken from https://github.com/Jonathhhan/ofEmscriptenExamples.  
Thanks `@Jonathhhan`.  
Originally by https://github.com/saurabhshri/simple-yet-powerful-srt-subtitle-parser-cpp.  
* [ofxPlaybackGui](https://github.com/roymacdonald/ofxPlaybackGui) from [@roymacdonald](https://github.com/roymacdonald) bundled into `/libs`. Optional to use only when using ofxGui as GUI. (Can be removed when using ofxSurfingImGui.)

## LICENSE
**MIT License**
