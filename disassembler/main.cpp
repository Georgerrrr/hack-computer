#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "common/architecture.h" 

ar::symbol_table_t symbols;

bool validateHeader(const char header[4]) {
  if (header[0] != 'H') return false;
  if (header[1] != 'r') return false;
  if (header[2] != 'o') return false;
  if (header[3] != 'm') return false;
  return true;
}

void read(const std::filesystem::path& file, std::vector<std::string>& instructions)
{
  char header[4];
  uint16_t numSymbols;
  uint16_t romOffset;

  std::ifstream f(file.c_str(), std::ios::binary);

  f.read(header, 4);

  if (!validateHeader(header)) {
    std::cout << "Invalid file type!\n";
    return;
  }

  f.read(reinterpret_cast<char*>(&numSymbols), sizeof(uint16_t));
  f.read(reinterpret_cast<char*>(&romOffset), sizeof(uint16_t));

  for (auto i = 0 ; i < numSymbols ; i++) {
    std::string key;
    uint16_t value;
    std::getline(f, key, '\0');
    f.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    symbols[key] = value;
  }

  f.seekg(romOffset, std::ios::beg);

  uint16_t nextInstruction;
  while (f.read(reinterpret_cast<char*>(&nextInstruction), sizeof(uint16_t))) {
    std::cout << ar::DisassembleInstruction(nextInstruction) << "\n";
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Must pass rom file!\n";
    return 0;
  }

  std::vector<std::string> instructions;

  read(argv[1], instructions);
}
