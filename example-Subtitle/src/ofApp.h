#pragma once

#include "ofMain.h"
#include "ofxSurfingTextSubtitle.h"

class ofApp : public ofBaseApp{
	
	public:
		
		void setup();
		void draw();

		ofxSurfingTextSubtitle sub;
};
