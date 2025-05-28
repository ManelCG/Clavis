#include <extensions/StringHelper.h>

#include <algorithm>

namespace Clavis::StringHelper {
    std::string ToLower(const std::string &s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](const unsigned char c) { return std::tolower(c); });
        return result;
    }

}