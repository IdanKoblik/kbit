#include <gtest/gtest.h>

#include "parser/decoder.h"

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

TEST(DecoderTest, Integer) {
    size_t pos = 0;
    BencodeValue v = decode("i42e", pos);

    EXPECT_EQ(asInt(v), 42);
    EXPECT_EQ(pos, 4);
}

TEST(DecoderTest, String) {
    size_t pos = 0;
    BencodeValue v = decode("4:spam", pos);

    EXPECT_EQ(asString(v), "spam");
    EXPECT_EQ(pos, 6);
}

TEST(DecoderTest, List) {
    size_t pos = 0;
    BencodeValue v = decode("li1e3:abce", pos);

    const auto& list = asList(v);
    ASSERT_EQ(list.size(), 2u);
    EXPECT_EQ(asInt(list[0]), 1);
    EXPECT_EQ(asString(list[1]), "abc");
}

TEST(DecoderTest, Dictionary) {
    size_t pos = 0;
    BencodeValue v = decode("d3:cow3:moo4:spam4:eggse", pos);

    const auto& dict = asDict(v);
    EXPECT_EQ(asString(dict.at("cow")), "moo");
    EXPECT_EQ(asString(dict.at("spam")), "eggs");
}

TEST(DecoderTest, NestedStructures) {
    size_t pos = 0;
    BencodeValue v = decode("d4:listli1ei2ei3ee3:numi99ee", pos);

    const auto& dict = asDict(v);

    const auto& list = asList(dict.at("list"));
    ASSERT_EQ(list.size(), 3u);
    EXPECT_EQ(asInt(list[0]), 1);
    EXPECT_EQ(asInt(list[1]), 2);
    EXPECT_EQ(asInt(list[2]), 3);

    EXPECT_EQ(asInt(dict.at("num")), 99);
}
