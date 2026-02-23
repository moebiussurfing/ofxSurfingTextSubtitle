#pragma once

#include <string>

namespace ofxSurfingSubtitle {

/**
 * @brief Centralizes settings path composition for the subtitle addon.
 */
class SubtitleSettingsStore {
public:
  void setPaths(const std::string& globalPath, const std::string& settingsFileName);

  const std::string& getGlobalPath() const;
  const std::string& getSettingsFileName() const;

  std::string getSettingsPath() const;

private:
  std::string globalPath_{"ofxSurfingTextSubtitle"};
  std::string settingsFileName_{"ofxSurfingTextSubtitle_Settings.json"};
};

} // namespace ofxSurfingSubtitle
