#pragma once

#include "ofMain.h"
#include "srtparser.h"

class ofxSurfingTextSubtitle {
	
	public:
		
		void setup();
		void draw();

		SubtitleParserFactory* subParserFactory;
		SubtitleParser* parser;
		std::vector<SubtitleItem*> sub;
		ofTrueTypeFont font;
		int sz;
};
