#pragma once

#include <string>

namespace ofxSurfingSubtitle {

/**
 * @brief Centralizes settings path composition for the subtitle addon.
 */
class SubtitleSettingsStore {
public:
  void setPaths(const std::string& globalPath, const std::string& settingsFileName) {
    globalPath_ = globalPath;
    settingsFileName_ = settingsFileName;
  }

  const std::string& getGlobalPath() const { return globalPath_; }
  const std::string& getSettingsFileName() const { return settingsFileName_; }

  std::string getSettingsPath() const {
    return globalPath_ + "/" + settingsFileName_;
  }

private:
  std::string globalPath_{"ofxSurfingTextSubtitle"};
  std::string settingsFileName_{"ofxSurfingTextSubtitle_Settings.json"};
};

} // namespace ofxSurfingSubtitle
