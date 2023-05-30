#pragma once

#include <cstddef>

class Blob
{

private:
    void *ptr;
    size_t size;
    bool owned;

public:
    Blob(size_t size);

    Blob(void *ptr, size_t size, bool owned = false);

    Blob(const Blob &other) = delete;
    Blob(Blob &&other);

    Blob &operator=(const Blob &other) = delete;
    Blob &operator=(Blob &&other);

    ~Blob();

    void *get_ptr() const;

    size_t get_size() const;

    bool is_owned() const;

    template <typename T>
    T as() const
    {
        return (T)ptr;
    }
};