#pragma once
#include <string>

class RailManager;

void GenerateStageFromAudio(const std::string& soundFilename, RailManager* railManager);
void DrawRailGeneratorDebugUI(const std::string& currentSoundFilename, RailManager* railManager);
