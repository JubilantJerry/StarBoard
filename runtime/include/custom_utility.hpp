#ifndef CUSTOM_UTILITY_HPP
#define CUSTOM_UTILITY_HPP

#include <iostream>
#include <iterator>
#include <vector>
#include <memory>
#include <type_traits>

template<typename InputIterator>
std::ostream& operator<<(
        std::ostream &stream,
        std::pair<InputIterator, InputIterator> range) {

    auto start = range.first;
    auto end = range.second;

    stream << "[";
    if (start != end) {
        stream << *start;
        ++start;
    }
    while (start != end) {
        stream << ", " << *start;
        ++start;
    }
    stream << "]";

    return stream;
}

template<typename T>
static inline std::ostream& operator<<(
        std::ostream &stream,
        std::vector<T> const &values) {

    stream << std::make_pair(values.begin(), values.end());
    return stream;
}

template<typename T>
static inline std::ostream& operator<<(
        std::ostream &stream,
        std::unique_ptr<T> const &ptr) {

    stream << '&' << *ptr;
    return stream;
}

#if __cplusplus < 201400L
    template<typename T>
    struct unique_ptr_select {
        typedef std::unique_ptr<T> single;
    };

    template<typename T>
    struct unique_ptr_select<T[]> {
        typedef std::unique_ptr<T[]> array;
    };

    template<typename T, size_t N>
    struct unique_ptr_select<T[N]> {
        typedef void array_illegal;
    };

    template<typename T, typename... Args>
    typename unique_ptr_select<T>::single make_unique(Args&&... args) {
        return std::unique_ptr<T>{new T(std::forward<Args>(args)...)};
    }

    template<typename T>
    typename unique_ptr_select<T>::array make_unique(std::size_t size) {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>{new U[size]()};
    }

    template<typename T, class... Args>
    typename unique_ptr_select<T>::array_illegal
        make_unique(Args&&... args) = delete;
#else
    using std::make_unique;
#endif

#endif