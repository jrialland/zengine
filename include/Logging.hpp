#pragma once

#include <string>
#include <sstream>

#include <fmt/core.h>

struct LogLine {
    std::ostringstream stream;
    std::string severity;
    std::string filename;
    std::string function;
    int line=0;
    LogLine(const char* severity, const char *file="", int line=0, const char *function="");
    ~LogLine();
};

#define LOG(severity) LogLine(#severity, __FILE__, __LINE__, __FUNCTION__).stream
