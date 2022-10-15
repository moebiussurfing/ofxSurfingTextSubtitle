## Overview
**ofxAddonTemplateCustomApp** is an addon template for openFrameworks to make other addons.

## Screenshot
![Screenshot](docs/readme_images/Capture1.PNG?raw=true "Screenshot")

## Features
- Can have some app modes  (2 by default). So it can have multiple behaviour modes. (Edit/Use).

## Usage
- Copy the folder ```/ofxFboMixerBlend```.
- Rename the folder to the name of your addon.

## Code Example
#### ofApp.h
```.cpp
#include "ofxSurfingUndoHelper.h"
ofParameterGroup params {"Params"};
```

#### ofApp.cpp
```.cpp
void ofApp::setup()
{
}

void ofApp::draw()
{
}

void ofApp::keyPressed(int key)
{
    if (key == 's')
    {
    }
}
```

## Dependencies
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)  
* [ofxScaleDragRect](https://github.com/moebiussurfing/ofxScaleDragRect)

## Tested Systems
* **Windows 10** / **VS 2017** / **OF ~0.11**
* **macOS**. **High Sierra** / **Xcode9** & **Xcode10** / **OF ~0.11**

## License
**MIT License**