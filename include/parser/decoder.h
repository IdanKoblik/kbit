#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include <string>
#include "bencode.h"

BencodeValue decode(const std::string& str, size_t& pos);

void print_bencode(const BencodeValue& value, int indent);

#endif // DECODER_H
