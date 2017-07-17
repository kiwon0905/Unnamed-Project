#pragma once

#include "Packer.h"
constexpr std::size_t pow2(std::size_t n);
void encode(const void * data, std::size_t size, Packer & pakcer);
void decode(const void * data, std::size_t size, Packer & packer);