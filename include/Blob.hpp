#pragma once

#include <cstddef>
#include <type_traits>
#include <functional>

class Blob
{

private:
    void *ptr;
    size_t size;
    std::function<void(void *)> deleter;
public:

    Blob();
    Blob(size_t size);

    Blob(void *ptr, size_t size, std::function<void(void *)> deleter = [](void *) {});

    Blob(const Blob &other) = delete;
    Blob(Blob &&other);

    Blob &operator=(const Blob &other) = delete;
    Blob &operator=(Blob &&other);


    Blob copy() const;

    ~Blob();

    void *get_ptr() const;

    size_t get_size() const;

    template <typename T>
    T as() const
    {
        static_assert(std::is_pointer<T>::value, "T must be a pointer type");
        return (T)ptr;
    }
};