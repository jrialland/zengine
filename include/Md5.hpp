#pragma once

#include <string>
#include <vector>
#include <iostream>

/**
 * md5 implementation, not to be used in a cryptographic context
*/
class Md5Digest {
    void *context;
    public:
        Md5Digest();
        ~Md5Digest();
        void update(const void* data, size_t len);
        void digest(uint8_t hash[16]);
        std::string hexdigest();

        static std::string hexdigest(const void* data, size_t len);
        static std::string hexdigest(const std::string& data);
        static std::string hexdigest(std::istream& stream);
};