#include <fstream> 

#include "language.h"

void rom_t::write(const std::filesystem::path &file, bool exportSymbols) {
  std::ofstream f(file.c_str(), std::ios_base::binary);

  uint16_t numSymbols = exportSymbols * symbols.size();
  uint16_t romOffset = 8;

  const char header[4] = {'H', 'r', 'o', 'm'};

  f.write(reinterpret_cast<const char*>(header), 4);
  f.write(reinterpret_cast<char*>(&numSymbols), sizeof(uint16_t));
  f.write(reinterpret_cast<char*>(&romOffset), sizeof(uint16_t));

  if (exportSymbols) {
    for (auto& symbol : symbols) {
      f.write(reinterpret_cast<const char*>(symbol.first.data()), symbol.first.size() + 1);

      romOffset += sizeof(std::string::value_type) * (symbol.first.size() + 1);
      f.write(reinterpret_cast<const char*>(&symbol.second), sizeof(uint16_t));
      romOffset += sizeof(uint16_t);
    }

    if (romOffset & 1) {
      uint8_t buffer = 0;
      f.write(reinterpret_cast<char*>(&buffer), sizeof(uint8_t));
      romOffset++;
    }

    f.seekp(6, std::ios::beg);
    f.write(reinterpret_cast<char*>(&romOffset), sizeof(uint16_t));
    f.seekp(0, std::ios::end);
  }

  f.write(reinterpret_cast<char*>(code.data()), code.size() * sizeof(uint16_t));
}

void loadFile(const std::filesystem::path& file, std::vector<std::string>& lines) {
  std::ifstream in(file.c_str(), std::ios_base::binary | std::ios_base::ate);
  if (!in.is_open()) {
    printf("Error opening file!\n");
    return;
  }
  std::string buffer;
  buffer.resize(in.tellg());

  in.seekg(0, std::ios_base::beg);
  in.read(&buffer[0], buffer.size());

  std::string current = "";
  for (const char c : buffer) {
    if (c == '\n') {
      lines.push_back(current);
      current = "";
      continue;
    }
    current += c;
  }

  lines.push_back(current);
}

