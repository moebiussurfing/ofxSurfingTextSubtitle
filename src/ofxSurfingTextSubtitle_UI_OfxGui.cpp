#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawGui() {
	if (!bGui) {
		return;
	}

	// info
#ifdef USE_WIDGET__SUBTITLES
	if (bDrawWidgetInfo) boxInfo.draw();
#endif

#ifdef USE_OFX_GUI__SUBTITLES
	if (bGui_Internal && bGui_InternalAllowed) gui.draw();
#endif

	drawDebug();
}

