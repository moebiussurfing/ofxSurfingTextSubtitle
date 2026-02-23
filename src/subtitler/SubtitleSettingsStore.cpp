#include "SubtitleSettingsStore.h"

namespace ofxSurfingSubtitle {

void SubtitleSettingsStore::setPaths(const std::string& globalPath,
  const std::string& settingsFileName) {
  globalPath_ = globalPath;
  settingsFileName_ = settingsFileName;
}

const std::string& SubtitleSettingsStore::getGlobalPath() const {
  return globalPath_;
}

const std::string& SubtitleSettingsStore::getSettingsFileName() const {
  return settingsFileName_;
}

std::string SubtitleSettingsStore::getSettingsPath() const {
  return globalPath_ + "/" + settingsFileName_;
}

} // namespace ofxSurfingSubtitle
