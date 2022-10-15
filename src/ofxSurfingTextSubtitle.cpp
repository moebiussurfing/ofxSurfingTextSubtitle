#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::setup() {
	sz = 50;
	font.load(OF_TTF_MONO, sz);
	string path = "Alphaville.ENG.srt";
	//string path = "audio.wav.srt";
	subParserFactory = new SubtitleParserFactory(ofToDataPath(path));
	parser = subParserFactory->getParser();

	//to get subtitles
	sub = parser->getSubtitles();
	int i = 0;

	for (SubtitleItem* element : sub)
	{
		cout << i++ << " " << element->getDialogue() << endl;

		//if (element->getStartTime() && element->getEndTime() ) {
		//	ofDrawBitmapString(element->getDialogue(), 50, 50);
		//}
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::draw() {
	/*
	for (SubtitleItem* element : sub)
	{
		if (element->getStartTime() <= (ofGetElapsedTimeMillis() + 35000) && element->getEndTime() >= (ofGetElapsedTimeMillis() + 35000)) {
			ofDrawBitmapString(element->getDialogue(), 50, 50);
		}
	}
	*/

	static int i = 0;
	if (ofGetFrameNum() % 10 == 0) i++;
	if (i >= sub.size() - 1) i = 0;

	string s;
	s = ofToString(i) + "/" + ofToString(sub.size() - 1);
	font.drawString(s, 0, ofGetHeight() / 2 + sz / 2);
	s = sub[i]->getDialogue();
	font.drawString(s, 0, ofGetHeight() / 2 + sz / 2 + (sz + 10));
	s = ofToString(sub[i]->getStartTimeString());
	font.drawString(s, 0, ofGetHeight() / 2 + sz / 2 + 2 * (sz + 10));
	//ofDrawBitmapString(s, 50, 50);

}
