#include "SubtitleApiManager.h"

namespace ofxSurfingSubtitle {

void SubtitleApiManager::setVisible(bool& visibleFlag, bool value) const {
  visibleFlag = value;
}

void SubtitleApiManager::toggleVisible(bool& visibleFlag) const {
  visibleFlag = !visibleFlag;
}

void SubtitleApiManager::togglePlay(PlaybackMode mode,
  bool& playStandalone,
  bool& playForced,
  bool& playManual) const {
  switch (mode) {
  case PlaybackMode::External:
    break;
  case PlaybackMode::Standalone:
    playStandalone = !playStandalone;
    break;
  case PlaybackMode::Forced:
    playForced = !playForced;
    break;
  case PlaybackMode::Manual:
    playManual = !playManual;
    break;
  }
}

const char* SubtitleApiManager::getPlaybackModeName(PlaybackMode mode) {
  switch (mode) {
  case PlaybackMode::External:
    return "EXTERNAL";
  case PlaybackMode::Standalone:
    return "STANDALONE";
  case PlaybackMode::Forced:
    return "FORCED";
  case PlaybackMode::Manual:
    return "MANUAL";
  }

  return "UNKNOWN";
}

} // namespace ofxSurfingSubtitle
