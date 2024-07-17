#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#define seq(x, y) (!strcmp(x, y))

template <typename T>
inline std::string vecunpack(std::vector<T> v)
{
    std::string s;
    for (T& t : v) s += t + " ";
    return s;
}

#endif //UTIL_H
