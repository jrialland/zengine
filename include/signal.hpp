#pragma once

#include <vector>
#include <functional>

namespace util
{
    template <typename... Args>
    class signal
    {
    private:
        std::vector<std::function<void(Args...)>> callbacks;

    public:
        void operator()(Args... args)
        {
            for (auto &callback : callbacks)
            {
                callback(args...);
            }
        }

        void connect(std::function<void(Args...)> callback)
        {
            callbacks.push_back(callback);
        }

        template <typename T>
        void connect(T *instance, void (T::*callback)(Args...))
        {
            callbacks.push_back([=](Args... args)
                                { (instance->*callback)(args...); });
        }

        void disconnect(std::function<void(Args...)> callback)
        {
            callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), callback), callbacks.end());
        }

        void disconnect(void (*callback)(Args...))
        {
            callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), callback), callbacks.end());
        }

        template <typename T>
        void disconnect(T *instance, void (T::*callback)(Args...))
        {
            callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), [=](Args... args)
                                        { (instance->*callback)(args...); }),
                            callbacks.end());
        }

        void clear()
        {
            callbacks.clear();
        }
    };
} // namespace util