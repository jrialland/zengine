#include "Blob.hpp"

#include <cstring>
#include <stdexcept>

Blob::Blob() : ptr(nullptr), size(0), deleter([](void *) {})
{
}

Blob::Blob(size_t size_) : size(size_), deleter([](void *ptr) { free(ptr); })
{
    ptr = malloc(size);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
}

Blob::Blob(void *ptr_, size_t size_, std::function<void(void *)> deleter_) : ptr(ptr_), size(size_), deleter(deleter_)
{
}

Blob::Blob(Blob &&other)
{
    ptr = other.ptr;
    size = other.size;
    deleter = other.deleter;
    other.deleter = [](void *) {};
}

Blob &Blob::operator=(Blob &&other)
{
    ptr = other.ptr;
    size = other.size;
    deleter = other.deleter;
    other.deleter = [](void *) {};
    return *this;
}

Blob Blob::copy() const {
    Blob blob(size);
    memcpy(blob.ptr, ptr, size);
    return blob;
}

Blob::~Blob()
{
    deleter(ptr);
}

void *Blob::get_ptr() const
{
    return ptr;
}

size_t Blob::get_size() const
{
    return size;
}
