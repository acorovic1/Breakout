#pragma once
#include <string>

namespace LoadUtility {
    std::string getExecutablePath();
    std::string getProjectDir(int levelsUp = 3);
}
