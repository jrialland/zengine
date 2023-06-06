#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Blob.hpp"

class FsEntry {
    public:
    virtual bool exists() =0;
    virtual bool is_file()=0;
    virtual bool is_directory()=0;
    virtual std::vector<std::shared_ptr<FsEntry>> list()=0;
    virtual Blob read()=0;
    void write(const Blob& blob);
    virtual void write(const void* data, size_t size)=0;
    virtual bool is_readonly()=0;
    virtual long last_modified()=0;
};

class FileSystem
{
    public:
    static std::shared_ptr<FsEntry> get_entry(const std::string& uri);
    static bool is_filesystem_uri(const std::string& uri);
    static std::string get_extension(const std::string& uri);
};