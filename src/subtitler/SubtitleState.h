#pragma once

#include <cstdint>
#include <string>

namespace ofxSurfingSubtitle {

enum class PlaybackMode : std::uint8_t {
  External = 0,
  Standalone = 1,
  Forced = 2,
  Manual = 3
};

struct FadeState {
  bool animateAll = true;
  bool animateIn = true;
  bool animateOut = true;
  bool animatingIn = false;
  bool animatingOut = false;
  bool fadeOutNotified = false;

  float alpha = 1.0f;
  float progressIn = 1.0f;
  float progressOut = 1.0f;
  float deltaIn = 1.0f;
  float deltaOut = 1.0f;

  void resetRuntime() {
    animatingIn = false;
    animatingOut = false;
    fadeOutNotified = false;

    alpha = 1.0f;
    progressIn = 1.0f;
    progressOut = 1.0f;
  }
};

struct PlaybackState {
  PlaybackMode mode = PlaybackMode::Forced;

  bool playExternal = false;
  bool playStandalone = false;
  bool playForced = false;
  bool playManual = false;
  bool slidePlaying = false;
  bool cycled = false;

  std::uint64_t tPlay = 0;
  std::uint64_t tPlayStartSlide = 0;
  std::uint64_t tPlayForce = 0;
  std::uint64_t tPlayForceFilm = 0;
  std::uint64_t durationPlaySlide = 0;
  std::uint64_t durationFilm = 0;

  float progressFilm = 0.0f;
  float progressSlide = 0.0f;

  void resetRuntime() {
    slidePlaying = false;
    tPlay = 0;
    tPlayStartSlide = 0;
    tPlayForce = 0;
    tPlayForceFilm = 0;

    progressFilm = 0.0f;
    progressSlide = 0.0f;
  }
};

struct TextState {
  std::string currentText;
  std::string lastTextRaw;

  bool modeTextBlocks = false;
  bool loadedSubs = false;
  bool loadedText = false;
  bool preCountingStart = false;
};

struct SubtitleState {
  FadeState fade;
  PlaybackState playback;
  TextState text;

  void resetRuntime() {
    fade.resetRuntime();
    playback.resetRuntime();

    text.currentText.clear();
    text.lastTextRaw.clear();
    text.preCountingStart = false;
  }
};

} // namespace ofxSurfingSubtitle
