#include "ofxSurfingTextSubtitle.h"

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawGui() {
#ifdef SURFING_IMGUI__USE_PROFILE_DEBUGGER
	T_GPU_START_PTR(3, "Gui");

	if (!bGui) {
		T_GPU_END_PTR(3);
		return;
	}
#else
	if (!bGui) {
		return;
	}
#endif

	// info
#ifdef USE_WIDGET__SUBTITLES
	if (bDrawWidgetInfo) boxInfo.draw();
#endif

#ifdef USE_OFX_GUI__SUBTITLES
	if (bGui_Internal && bGui_InternalAllowed) gui.draw();
#endif

#ifdef USE_WIDGET__VIDEO_PLAYER
	player.drawGui();
#endif

#ifdef SURFING_IMGUI__USE_PROFILE_DEBUGGER
	T_GPU_END_PTR(3);
#endif
}

