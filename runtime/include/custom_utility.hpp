#include <iostream>
#include <iterator>
#include <memory>

template<typename InputIterator>
static inline std::ostream& operator<<(
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
}

#if __cplusplus < 201400L
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>{new T(std::forward<Args>(args)...)};
    }
#else
    using std::make_unique;
#endif