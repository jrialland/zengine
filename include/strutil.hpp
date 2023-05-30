#pragma once

#include <string>
#include <algorithm>
#include <vector>

namespace strutil
{

    std::string to_lower(const std::string &str);

    std::string to_upper(const std::string &str);

    std::string trim(const std::string &str);

    std::string trim_left(const std::string &str);
    
    std::string trim_right(const std::string &str);

    std::string replace_first(const std::string &str, const std::string &from, const std::string &to);

    std::string replace_all(const std::string& str, const std::string& from, const std::string& to);

    std::vector<std::string> split(const std::string& str, const std::string& delim);

}