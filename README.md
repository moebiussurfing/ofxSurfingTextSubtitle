## Overview
Draw a subtitle text lines from a `.srt` file.

##### WIP
- Pass time to link with external player.

## Screenshot
![Screenshot](example-Subtitle/Capture.PNG)

## Features
- Fast FontStash render.
- Custom Styles.
- Scalable draw rectangle container.
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

void ofApp::draw() 
{
	sub.draw();
}
```

## Dependencies
- ofxFontStash
- ofxGui
- ofxSurfingBox
- ofxSurfingHelpers

## Tested Systems
* **Windows 10** / **VS 2017** / **OF ~0.11**

## License
**MIT License**