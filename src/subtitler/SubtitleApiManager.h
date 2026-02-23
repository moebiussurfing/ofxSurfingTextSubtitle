#pragma once

#include "SubtitleState.h"

namespace ofxSurfingSubtitle {

/**
 * @brief Small API-oriented helpers used by the addon facade.
 */
class SubtitleApiManager {
public:
  void setVisible(bool& visibleFlag, bool value) const;
  void toggleVisible(bool& visibleFlag) const;

  void togglePlay(PlaybackMode mode,
    bool& playStandalone,
    bool& playForced,
    bool& playManual) const;

  static const char* getPlaybackModeName(PlaybackMode mode);
};

} // namespace ofxSurfingSubtitle
