#include "strutil.hpp"

namespace strutil {

    std::string to_lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    std::string to_upper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::string trim(const std::string& str) {
        std::string result = str;
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), result.end());
        return result;
    }

    std::string trim_left(const std::string& str) {
        std::string result = str;
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        return result;
    }

    std::string trim_right(const std::string& str) {
        std::string result = str;
        result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), result.end());
        return result;
    }

    std::string replace_first(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t start_pos = 0;
        if((start_pos = result.find(from, start_pos)) != std::string::npos) {
            result.replace(start_pos, from.length(), to);
        }
        return result;
    }

    std::string replace_all(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t start_pos = 0;
        while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
            result.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return result;
    }

    
    std::string remove_all(const std::string& str, const std::string& remove) {
        std::string result = str;
        size_t start_pos = 0;
        while ((start_pos = result.find(remove, start_pos)) != std::string::npos) {
            result.erase(start_pos, remove.length());
        }
        return result;
    }

    std::vector<std::string> split(const std::string& str, const std::string& delim) {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = 0;
        while ((end = str.find(delim, start)) != std::string::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + delim.length();
        }
        result.push_back(str.substr(start, end));
        return result;
    }

    bool starts_with(const std::string& str, const std::string& prefix) {
        return str.rfind(prefix, 0) == 0;
    }

    bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.rfind(suffix) == (str.size() - suffix.size());
    }

} // namespace strutil
