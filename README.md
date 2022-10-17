## Screenshot
#### example-Subtitle / ofxGui 
![Screenshot](example-Subtitle/Capture.PNG)
#### example-Subtitle2 / ImGui (Optional) 
![Screenshot](example-Subtitle2/Capture.PNG)

## Features
- Fast `FontStash` render.
- Custom `paragraph Styles`.
- Scalable and `draggable container`.
- Transport `player` and browser.
- `Persistent` settings.
- `Presets` Engine (Optional). Look `example-Subtitle2`. Requires ImGui.
- Animated `Fade In`.

## Example example-Subtitle / ofxGui
#### ofApp.h
```.cpp
// Remember to commentent to use ofxGui only!
// #define USE_IM_GUI__SUBTTITTLES
#include "ofxSurfingTextSubtitle.h"
ofxSurfingTextSubtitle sub;
```
#### ofApp.cpp
```.cpp
void ofApp::setup() 
{
	sub.setup("Alphaville.srt");
}
void ofApp::update() 
{
	sub.update();
}
void ofApp::draw() 
{
	sub.draw();
	sub.drawGui();
}
```

## Dependencies
- [ofxFontStash](https://github.com/armadillu/ofxFontStash)
- [ofxSurfingBox](https://github.com/moebiussurfing/ofxSurfingBox)
- [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)
- ofxGui / Core

#### OPTIONAL
- [ofxSufingImGui](https://github.com/moebiussurfing/ofxSurfingImGui)

### TODO
- Pass time to link with an external video player.
- Load render fonts on runtime.

## Tested Systems
* **Windows 10** / **VS 2017** / **OF ~0.11**

## Thanks
`srtparser.h` taken from https://github.com/Jonathhhan/ofEmscriptenExamples.  
Thanks `@Jonathhhan`.

## License
**MIT License**