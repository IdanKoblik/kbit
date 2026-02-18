#ifndef BENCODE_H
#define BENCODE_H

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdint.h>

struct BencodeValue;

using BencodeList = std::vector<BencodeValue>;
using BencodeDict = std::map<std::string, BencodeValue>;

struct BencodeValue {
    using Variant = std::variant<
        long long,
        std::string,
        BencodeList,
        BencodeDict
    >;

    Variant value;

    BencodeValue() = default;

    template<typename T>
    BencodeValue(T&& v) : value(std::forward<T>(v)) {}
};

BencodeDict asDict(const BencodeValue& v);

BencodeList asList(const BencodeValue& v);

const std::string& asString(const BencodeValue& v);

long long asInt(const BencodeValue& v);

#endif // BENCODE_H
