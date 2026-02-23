#pragma once

#include "SubtitleState.h"

namespace ofxSurfingSubtitle {

/**
 * @brief Small API-oriented helpers used by the addon facade.
 */
class SubtitleApiManager {
public:
  void setVisible(bool& visibleFlag, bool value) const { visibleFlag = value; }
  void toggleVisible(bool& visibleFlag) const { visibleFlag = !visibleFlag; }

  void togglePlay(PlaybackMode mode,
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

  static const char* getPlaybackModeName(PlaybackMode mode) {
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
};

} // namespace ofxSurfingSubtitle
