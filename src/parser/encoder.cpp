#include "parser/encoder.h"

#include "parser/bencode.h"
#include <string>
#include <stdexcept>
#include <variant>

static std::string encodeInt(long long i) {
    return "i" + std::to_string(i) + "e";
}

static std::string encodeString(const std::string &str) {
    return std::to_string(str.size()) + ":" + str;
}

static std::string encodeList(const BencodeList &list) {
    std::string out = "l";
    for (const auto& item : list)
        out += encode(item);

    out += "e";
    return out;
}

static std::string encodeDict(const BencodeDict& dict) {
    std::string out = "d";
    for (const auto &[key, value] : dict) {
        out += encodeString(key);
        out += encode(value);
    }

    out += "e";
    return out;
}

std::string encode(const BencodeValue& v) {
    if (std::holds_alternative<long long>(v.value))
        return encodeInt(std::get<long long>(v.value));

    if (std::holds_alternative<std::string>(v.value))
        return encodeString(std::get<std::string>(v.value));

    if (std::holds_alternative<BencodeList>(v.value))
        return encodeList(std::get<BencodeList>(v.value));

    if (std::holds_alternative<BencodeDict>(v.value))
        return encodeDict(std::get<BencodeDict>(v.value));

    throw std::runtime_error("Unknown BencodeValue type");
}
