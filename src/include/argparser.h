#ifndef ARGPARSER_H
#define ARGPARSER_H
#pragma once
#include "Arguments.h"
#include <string>
#include <unordered_set>
#include <vector>

using StrVec = std::vector<std::string>;
using ArgSet = std::unordered_set<Arguments>;

/// <summary>
/// Wrapper function.
/// </summary>
/// <param name="args">String vector of flags passed to program.</param>
/// <returns>Logical Argument set.</returns>
auto parse_args(const StrVec&) -> ArgSet;
/// <summary>
/// Helper function to convert input to logic.
/// </summary>
/// <param name="flag">Character to match to argument.</param>
/// <returns>Relevant Argument.</returns>
auto check_arg(char) -> Arguments;

#endif