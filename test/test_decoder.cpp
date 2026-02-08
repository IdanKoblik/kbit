#include "../decoder.h"
#include <cassert>
#include <iostream>

static long long asInt(const BencodeValue& v) {
    return std::get<long long>(v.value);
}

static const std::string& asString(const BencodeValue& v) {
    return std::get<std::string>(v.value);
}

static const BencodeList& asList(const BencodeValue& v) {
    return std::get<BencodeList>(v.value);
}

static const BencodeDict& asDict(const BencodeValue& v) {
    return std::get<BencodeDict>(v.value);
}

void test_int() {
    size_t pos = 0;
    BencodeValue v = decode("i42e", pos);

    assert(asInt(v) == 42);
    assert(pos == 4);
}

void test_string() {
    size_t pos = 0;
    BencodeValue v = decode("4:spam", pos);

    assert(asString(v) == "spam");
    assert(pos == 6);
}

void test_list() {
    size_t pos = 0;
    BencodeValue v = decode("li1e3:abce", pos);

    const auto& list = asList(v);
    assert(list.size() == 2);
    assert(asInt(list[0]) == 1);
    assert(asString(list[1]) == "abc");
}

void test_dict() {
    size_t pos = 0;
    BencodeValue v = decode("d3:cow3:moo4:spam4:eggse", pos);

    const auto& dict = asDict(v);
    assert(asString(dict.at("cow")) == "moo");
    assert(asString(dict.at("spam")) == "eggs");
}

void test_nested() {
    size_t pos = 0;
    BencodeValue v = decode("d4:listli1ei2ei3ee3:numi99ee", pos);

    const auto& dict = asDict(v);

    const auto& list = asList(dict.at("list"));
    assert(asInt(list[0]) == 1);
    assert(asInt(list[1]) == 2);
    assert(asInt(list[2]) == 3);

    assert(asInt(dict.at("num")) == 99);
}

int main() {
    test_int();
    test_string();
    test_list();
    test_dict();
    test_nested();

    std::cout << "[OK] All decoder tests passed\n";
    return 0;
}
