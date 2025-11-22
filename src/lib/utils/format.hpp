#ifndef SRC_LIB_FORMAT_HPP
#define SRC_LIB_FORMAT_HPP

#include <string>
#include <sstream>
#include <iomanip>

// Custom format helper for hex values
inline std::string hex(unsigned int value, int width = 0) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setfill('0');
    if (width > 0) oss << std::setw(width);
    oss << value;
    return oss.str();
}

inline std::string reg(unsigned int value) {
    std::ostringstream oss;
    oss << "V" << std::hex << (value & 0xF);
    return oss.str();
}

// String formatting helper - similar to fprintf but returns std::string
template<typename... Args>
inline std::string fmt(const std::string& format, Args... args) {
    std::ostringstream result;
    size_t pos = 0;
    size_t arg_index = 0;
    std::string arg_strings[] = {(std::ostringstream() << args).str()...};
    const size_t num_args = sizeof...(args);
    
    while (pos < format.length()) {
        if (format[pos] == '%' && pos + 1 < format.length() && format[pos + 1] == 's') {
            if (arg_index < num_args) {
                result << arg_strings[arg_index++];
            }
            pos += 2;
        } else {
            result << format[pos++];
        }
    }
    return result.str();
}

#endif