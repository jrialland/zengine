#include "FileSystem.hpp"
#include "strutil.hpp"

#include <fstream>
#include <filesystem>

#include <dlfcn.h>

//#include "minizip-ng/mz_zip.h"

namespace fs = std::filesystem;

class DiskFsEntry : public FsEntry
{

    fs::path path;

public:
    DiskFsEntry(const std::string &path_) : path(path_) {}

    bool exists() override
    {
        return fs::exists(path);
    }

    bool is_file() override
    {
        return fs::is_regular_file(path);
    }

    bool is_directory() override
    {
        return fs::is_directory(path);
    }

    std::vector<std::shared_ptr<FsEntry>> list() override
    {
        std::vector<std::shared_ptr<FsEntry>> result;
        for (auto &entry : fs::directory_iterator(path))
        {
            result.push_back(std::make_shared<DiskFsEntry>(entry.path().string()));
        }
        return result;
    }

    Blob read() override
    {
        std::ifstream file(path.string().c_str(), std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error("Failed to open file '" + path.string() + "' for reading");
        }
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        Blob result(size);
        file.read(reinterpret_cast<char *>(result.get_ptr()), size);
        return result;
    }

    void write(const Blob &blob) override
    {
        std::ofstream file(path.string().c_str(), std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Failed to open file '" + path.string() + "' for writing");
        }
        file.write(reinterpret_cast<const char *>(blob.get_ptr()), blob.get_size());
    }

    virtual bool is_readonly() override
    {
        return false;
    }

    virtual long last_modified() override
    {
        return fs::last_write_time(path).time_since_epoch().count();
    }
};

class SymbolFsEntry : public FsEntry
{
    std::string symbol;
    void *resolved;
    size_t size;

public:
    SymbolFsEntry(const std::string &symbol_) : symbol(symbol_), size(0)
    {
        auto comma = symbol.find(",");
        if (comma != std::string::npos)
        {
            size = std::stoul(symbol.substr(comma + 1));
            symbol = symbol.substr(0, comma);
        }

        dlopen(nullptr, RTLD_NOW | RTLD_GLOBAL);
        resolved = dlsym(RTLD_DEFAULT, symbol.c_str());
        if (!resolved)
        {
            dlclose(RTLD_DEFAULT);
            throw std::runtime_error("Symbol not found");
        }

        if (size == 0)
        {
            void *end = dlsym(RTLD_DEFAULT, (symbol + "_end").c_str());
            if (!end)
            {
                dlclose(RTLD_DEFAULT);
                throw std::runtime_error("Symbol not found");
            }
            size = reinterpret_cast<size_t>(end) - reinterpret_cast<size_t>(resolved);
        }

        dlclose(RTLD_DEFAULT);
    }

    bool exists() override
    {
        return true;
    }

    bool is_file() override
    {
        return true;
    }

    bool is_directory() override
    {
        return false;
    }

    std::vector<std::shared_ptr<FsEntry>> list() override
    {
        return {};
    }

    Blob read() override
    {
        return Blob(resolved, size);
    }

    void write(const Blob &blob) override
    {
        throw std::runtime_error("read-only");
    }

    virtual bool is_readonly() override
    {
        return true;
    }

    virtual long last_modified() override
    {
        return 0;
    }
};

std::shared_ptr<FsEntry> FileSystem::get_entry(const std::string &uri)
{

    size_t exclam = uri.find("!");
    if (exclam != std::string::npos)
    {
        // treat as zip file
        auto zipFile = get_entry(uri.substr(7, exclam - 7));
        if (!zipFile->is_file())
        {
            throw std::runtime_error("Zip file not found");
        }
        throw std::runtime_error("Zip file not implemented");
        // return std::make_shared<ZipFsEntry>(zipFile, uri.substr(exclam + 1));
    }

    // 'normal' disk file
    if (uri.starts_with("file://"))
    {
        std::string path = uri.substr(7);
        path = strutil::replace_all(path, "/", std::to_string(fs::path::preferred_separator));
        return std::make_shared<DiskFsEntry>(path);
    }

    if (uri.starts_with("symbol://"))
    {
        return std::make_shared<SymbolFsEntry>(uri.substr(9));
    }

    if (uri.starts_with("assets://"))
    {
        return std::make_shared<DiskFsEntry>("assets/" + uri.substr(9));
    }

    if (uri.starts_with("tmp://"))
    {
        return std::make_shared<DiskFsEntry>("tmp/" + uri.substr(6));
    }

    throw std::runtime_error("Unsupported URI scheme");
}

bool FileSystem::is_filesystem_uri(const std::string& uri) {
    return uri.starts_with("file://") || uri.starts_with("symbol://") || uri.starts_with("assets://") || uri.starts_with("tmp://");
}