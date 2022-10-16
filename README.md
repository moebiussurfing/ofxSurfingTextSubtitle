### WIP
- Pass time to link with an external player.
- Add basic animation/fade.

## Screenshot
#### ofxGui 
![Screenshot](example-Subtitle/Capture.PNG)
#### ImGui (Optional) 
![Screenshot](example-Subtitle2/Capture.PNG)

## Features
- Fast FontStash render.
- Custom Styles.
- Scalable and draggable container.
- Transport player and browser.
- Persistent settings.
- Presets Engine.

## Example
#### ofApp.h
```.cpp
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
- ofxFontStash
- ofxSurfingBox
- ofxSurfingHelpers
- ofxGui
#### OPTIONAL
- ofxSufingImGui

## Tested Systems
* **Windows 10** / **VS 2017** / **OF ~0.11**

## Thanks
`srtparser.h` taken from https://github.com/Jonathhhan/ofEmscriptenExamples. Thanks @Jonathhhan.

## License
**MIT License**