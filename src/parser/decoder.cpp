#include "parser/decoder.h"

#include <stdexcept>
#include <iostream>

static long long decodeInt(const std::string& str, size_t& pos) {
    if (str[pos] != 'i')
        throw std::runtime_error("Invalid integer start");

    pos++; // skip 'i'

    size_t start = pos;
    while (str[pos] != 'e') {
        if (pos >= str.size())
            throw std::runtime_error("Unterminated integer");

        pos++;
    }

    long long value = std::stoll(str.substr(start, pos - start));
    pos++; // skip 'e'
    return value;
}

static std::string decodeString(const std::string& str, size_t& pos) {
    size_t start = pos;
    
    while (std::isdigit(str[pos])) {
        pos++;
        if (pos >= str.size())
            throw std::runtime_error("Invalid string length");
    }

    if (str[pos] != ':')
        throw std::runtime_error("Invalid string format");

    size_t len = std::stoull(str.substr(start, pos - start));
    pos++; // skip ';'

    if (pos + len > str.size())
        throw std::runtime_error("String out of bounds");

    std::string result = str.substr(pos, len);
    pos += len;

    return result;
}

static BencodeList decodeList(const std::string& str, size_t& pos) {
    if (str[pos] != 'l')
        throw std::runtime_error("Invalid list start");

    pos++; // skip 'l'
    std::vector<BencodeValue> list;

    while (str[pos] != 'e') {
        if (pos >= str.size())
            throw std::runtime_error("Unterminated list");

        list.push_back(decode(str, pos));
    }

    pos++; // skip 'e'
    return list;
}

static BencodeDict decodeDict(const std::string& str, size_t& pos) {
    if (str[pos] != 'd')
        throw std::runtime_error("Invalid map start");

    pos++; // skip 'd'
    std::unordered_map<std::string, BencodeValue> dict;


    while (str[pos] != 'e') {
        if (pos >= str.size())
            throw std::runtime_error("Unterminated dict");

        std::string key = decodeString(str, pos);
        BencodeValue value = decode(str, pos);
        dict.emplace(std::move(key), std::move(value));
    }

    pos++; // skip 'e'
    return dict;
}

BencodeValue decode(const std::string& str, size_t& pos) {
    if (pos >= str.size())
        throw std::runtime_error("Unexpected end of input");

    char c = str[pos];

    if (c == 'i')
        return BencodeValue(decodeInt(str, pos));

    if (c == 'l')
        return BencodeValue(decodeList(str, pos));

    if (c == 'd')
        return BencodeValue(decodeDict(str, pos));

    if (std::isdigit(c))
        return BencodeValue(decodeString(str, pos));

    throw std::runtime_error("Unknown bencode type");
}

void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) 
        std::cout << "  ";
}

void print_bencode(const BencodeValue& value, int indent) {
    std::visit([indent](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, long long>) {
            std::cout << v;
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << '"' << v << '"';
        } else if constexpr (std::is_same_v<T, BencodeList>) {
            std::cout << "[\n";
            for (const auto& item : v) {
                print_indent(indent + 1);
                print_bencode(item, indent + 1);
                std::cout << ",\n";
            }
            print_indent(indent);
            std::cout << "]";
        } else if constexpr (std::is_same_v<T, BencodeDict>) {
            std::cout << "{\n";
            for (const auto& [key, val] : v) {
                print_indent(indent + 1);
                std::cout << '"' << key << "\": ";
                print_bencode(val, indent + 1);
                std::cout << ",\n";
            }
            print_indent(indent);
            std::cout << "}";
        }
    }, value.value);
}
