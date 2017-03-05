#ifndef DEFAULTS_H
#define DEFAULTS_H

#define DEFAULT_MIN_LENGTH 8
#define DEFAULT_MAX_LENGTH 63
#define DEFAULT_SESSION_NAME "session"
#define SESSION_EXTENSION ".egglist"
#define DEFAULT_THREAD_COUNT 2
#define DEFAULT_DICTIONARY_FILE "dictionary.txt"
// 1GB
#define DEFAULT_MAX_TOTAL_SIZE 1073741824
// 200MB
#define DEFAULT_MAX_FILE_SIZE 209715200

#include <string>
#include <algorithm>
#include <iterator>

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

// TODO move to utils
struct StringCompare {
    bool operator()(const std::string &first, const std::string &second) {
        return first.size() < second.size();
    }
};

struct StringFilter : public std::unary_function<std::string, bool> {
    StringFilter(const uint &minLength, const uint &maxLength) : minLength(minLength), maxLength(maxLength) {}
    bool operator()(const std::string &s) const {
        return s.length() >= minLength && s.length() <= maxLength;
    }

    const uint minLength;
    const uint maxLength;
};

#endif //DEFAULTS_H
