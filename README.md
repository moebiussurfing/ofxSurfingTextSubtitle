## OVERVIEW

Ready to use customizable `.SRT` text subtitle (and plain text) player for `openFrameworks`. 

## FEATURES

- Full `.SRT` file browser for exploring the dialogs.
- Fast `FontStash` render.
- Custom `paragraph styles`.
- Scalable and draggable `rectangle container`.
- All `settings are persistent`.
- Animated `fade In/Out` for transparency.
- `Transport time player` with four modes: 
	- **STANDALONE**  
	  - Dialogs following the original `SRT` times and durations.
	- **FORCED** 
	  - Customizable dialogs duration. Ignoring `SRT` times.
	- **EXTERNAL** 
	  - Time is controlled by a video player or linked to one external control.  
	- **MANUAL** 
	  - To work without loading any `SRT` file. Just pass the text slides manually.
- `Presets Engine` for all the settings. 
	
## SCREENCAST VIDEO

[VIDEO](https://youtu.be/kcObeooL3Pc)

<details>
  <summary>EXAMPLES / SCREENSHOTS</summary>

#### - example-Subs_ImGui / ImGui (Optional) 
![Screenshot](Examples/example-Subs_ImGui/Capture.PNG)

#### - example-Subs_ofxGui / ofxGui 
![Screenshot](Examples_ofxGui/example-Subs_ofxGui/Capture.PNG)

#### - example-Subs_ofxGui2 / ofxGui (Video Player) 
![Screenshot](Examples_ofxGui/example-Subs_ofxGui2/Capture.PNG)

#### - example-SubtitleWhisper / Whisper (Real-time audio to text) [ ADVANCED / WIP ]  
![Screenshot](Examples_Advanced/example-SubtitleWhisper/Capture.PNG)  
Requires this [FORK](https://github.com/moebiussurfing/ofxWhisper) of [ofxWhisper](https://github.com/roymacdonald/ofxWhisper) from [@roymacdonald](https://github.com/roymacdonald).  

#### - example-SubtitleChatGPT / [ ADVANCED / WIP ]  
![Screenshot](Examples_Advanced/example-SubtitleChatGPT/Capture.PNG)  


[VIDEO](https://youtu.be/G8iH-0UakN4)


</details>

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
- [ **BUG** ] Fix some words remove when building the paragraph.
- List and load different render fonts files on runtime.  
- macOS testing.

## TESTED SYSTEM
* **Windows 10** / **VS 2022** / **OF ~0.11**

## THANKS
* SRT parser: file `srtparser.h`. Taken from https://github.com/Jonathhhan/ofEmscriptenExamples. Thanks `@Jonathhhan`. Originally by: https://github.com/saurabhshri/simple-yet-powerful-srt-subtitle-parser-cpp.  
* [ofxPlaybackGui](https://github.com/roymacdonald/ofxPlaybackGui) from [@roymacdonald](https://github.com/roymacdonald) bundled into `/libs`. Optional to use only when using **ofxGui** as GUI. (Can be removed when using **ofxSurfingImGui**.)
* [ofxWhisper](https://github.com/roymacdonald/ofxWhisper) from [@roymacdonald](https://github.com/roymacdonald). Powered by [whisper.cpp](https://github.com/ggerganov/whisper.cpp).  

## LICENSE
**MIT License**