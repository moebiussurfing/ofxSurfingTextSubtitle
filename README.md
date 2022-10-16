## Overview
Draws text lines from a `.srt` subtitle file.

#### WIP
- Pass time to link with an external player.
- Add basic animation/fade.

## Screenshot
#### ofxGui 
![Screenshot](example-Subtitle/Capture.PNG)
#### ImGui 
![Screenshot](example-Subtitle2/Capture.PNG)

## Features
- Fast FontStash render.
- Custom Styles.
- Scalable draw rectangle width container.
- Persistent settings.

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
- ofxGui
- ofxSurfingBox
- ofxSurfingHelpers
#### OPTIONAL
- ofxSufingImGui
- ofxImGui

## Tested Systems
* **Windows 10** / **VS 2017** / **OF ~0.11**

## Thanks
`srtparser.h` taken from https://github.com/Jonathhhan/ofEmscriptenExamples. Thanks @Jonathhhan.

## License
**MIT License**