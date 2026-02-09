#ifndef DECODER_H
#define DECODER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <stdint.h>

struct BencodeValue;

using BencodeList = std::vector<BencodeValue>;
using BencodeDict = std::unordered_map<std::string, BencodeValue>;

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

BencodeValue decode(const std::string& str, size_t& pos);

void print_bencode(const BencodeValue& value, int indent);

#endif // DECODER_H
