#include "parser/bencode.h"

#include <stdexcept>

BencodeDict asDict(const BencodeValue& v) {
    if (!std::holds_alternative<BencodeDict>(v.value))
        throw std::runtime_error("BencodeValue is not a dictionary!");

    return std::get<BencodeDict>(v.value);
}

BencodeList asList(const BencodeValue& v) {
    if (!std::holds_alternative<BencodeList>(v.value))
        throw std::runtime_error("BencodeValue is not a list!");

    return std::get<BencodeList>(v.value);
}

const std::string& asString(const BencodeValue& v) {
    return std::get<std::string>(v.value);
}

long long asInt(const BencodeValue& v) {
    return std::get<long long>(v.value);
}
