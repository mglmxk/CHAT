#pragma once

#include <tuple>
#include <type_traits>

template <typename T, typename = void>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(Args...)> {
    using return_type = R;
    static constexpr size_t arity = sizeof...(Args);
    
    template <size_t i>
    using arg_type = std::tuple_element_t<i, std::tuple<Args...>>;
};

template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};

template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R(Args...)> {};

template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)> {};

template <typename T>
struct function_traits<T, std::void_t<decltype(&T::operator())>>
    : function_traits<decltype(&T::operator())> {};