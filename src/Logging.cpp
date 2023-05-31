#include "Logging.hpp"
#include <iostream>

static std::string basename(const std::string &path)
{
    size_t pos = path.find_last_of("\\/");
    return (std::string::npos == pos) ? path : path.substr(pos + 1);
}

LogLine::LogLine(const char *severity_, const char *file_, int line_, const char *function_) : severity(severity_), line(line_), function(function_)
{
    filename = basename(file_);
}

LogLine::~LogLine()
{
    std::cout << stream.str() << std::endl;
}