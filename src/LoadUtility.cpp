#include "LoadUtility.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

std::string LoadUtility::getExecutablePath() {
#if defined(_WIN32)
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::string(buffer);
#elif defined(__linux__)
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) throw std::runtime_error("Cannot get executable path");
    buffer[len] = '\0';
    return std::string(buffer);
#elif defined(__APPLE__)
    char buffer[1024];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0)
        throw std::runtime_error("Cannot get executable path");
    return std::string(buffer);
#endif
}

std::string LoadUtility::getProjectDir(int levelsUp) {
    std::string path = getExecutablePath();
    for (int i = 0; i < levelsUp; ++i) {
        auto pos = path.find_last_of("/\\");
        if (pos == std::string::npos) break;
        path = path.substr(0, pos);
    }
    return path;
}
