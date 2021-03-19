#ifndef INCLUDE_ARGPARSER_H
#define INCLUDE_ARGPARSER_H
#pragma once
#include "Flag.h"
#include <string>
#include <unordered_set>
#include <vector>

using StrVec = std::vector<std::string>;
using FlagSet = std::unordered_set<Flag>;

auto parse_flags(const StrVec&) -> FlagSet;
auto check_flag(char) -> Flag;

#endif