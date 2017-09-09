#pragma once

#include "Packer.h"
void encode(const void * data, std::size_t size, Packer & pakcer);
void decode(const void * data, std::size_t size, Packer & packer);