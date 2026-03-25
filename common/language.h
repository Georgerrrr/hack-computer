#pragma once 

#include <filesystem>
#include <string>
#include <vector>
#include "common/architecture.h"

struct rom_t {
  std::vector<uint16_t> code;
  ar::symbol_table_t symbols;

  void write(const std::filesystem::path& file, bool exportSymbols);
};

void loadFile(const std::filesystem::path& file, std::vector<std::string>& lines);
