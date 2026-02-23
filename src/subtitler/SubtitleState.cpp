#include "SubtitleState.h"

namespace ofxSurfingSubtitle {

void FadeState::resetRuntime() {
  animatingIn = false;
  animatingOut = false;
  fadeOutNotified = false;

  alpha = 1.0f;
  progressIn = 1.0f;
  progressOut = 1.0f;
}

void PlaybackState::resetRuntime() {
  slidePlaying = false;
  tPlay = 0;
  tPlayStartSlide = 0;
  tPlayForce = 0;
  tPlayForceFilm = 0;

  progressFilm = 0.0f;
  progressSlide = 0.0f;
}

void SubtitleState::resetRuntime() {
  fade.resetRuntime();
  playback.resetRuntime();

  text.currentText.clear();
  text.lastTextRaw.clear();
  text.preCountingStart = false;
}

} // namespace ofxSurfingSubtitle
