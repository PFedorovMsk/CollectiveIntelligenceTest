#ifndef DEFINES_H
#define DEFINES_H

#include <cassert>
#include <vector>


template <typename T>
using Array = std::vector<T>;

template <typename T>
using Table = Array<Array<T>>;

using std::size_t;

enum class Answer { RIGHT = 1, NOT_RIGHT = 0, NO = -1 };


#endif // DEFINES_H
