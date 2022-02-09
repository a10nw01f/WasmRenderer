#pragma once

#include <type_traits>
#include <initializer_list>

template<class... Ts>
struct TypeList{};

template<class L, class... Ts>
void ForEach(L&& func, Ts&&... args)
{
    return (void)(
        std::initializer_list<int>{
            (func(std::forward<Ts>(args)),0)...
        });
}

template<class T, int N>
constexpr int ArraySize(T(&)[N])
{
    return N;
}
