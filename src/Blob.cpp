#include "Blob.hpp"

#include <cstring>
#include <stdexcept>

Blob::Blob() : ptr(nullptr), size(0), owned(false)
{
}

Blob::Blob(size_t size_) : size(size_), owned(true)
{
    ptr = malloc(size);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
}

Blob::Blob(void *ptr_, size_t size_, bool owned_) : ptr(ptr_), size(size_), owned(owned_)
{
}

Blob::Blob(Blob &&other)
{
    ptr = other.ptr;
    size = other.size;
    owned = other.owned;
    other.owned = false;
}

Blob &Blob::operator=(Blob &&other)
{
    ptr = other.ptr;
    size = other.size;
    owned = other.owned;
    other.owned = false;
    return *this;
}

Blob Blob::copy() const {
    Blob blob(size);
    memcpy(blob.ptr, ptr, size);
    return blob;
}

Blob::~Blob()
{
    if (owned)
    {
        free(ptr);
    }
}

void *Blob::get_ptr() const
{
    return ptr;
}

size_t Blob::get_size() const
{
    return size;
}

bool Blob::is_owned() const
{
    return owned;
}
