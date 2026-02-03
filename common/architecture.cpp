#include "architecture.h"

namespace ar {

  static constexpr auto AInstruction  = 0xE000;
  static constexpr auto OutA          = 0x0020;
  static constexpr auto OutD          = 0x0010;
  static constexpr auto OutM          = 0x0008;

  static const std::unordered_map<std::string, uint16_t> operations= {
    { "0",   0b0000101010000000 },
    { "1",   0b0000111111000000 },
    { "-1",  0b0000111010000000 },
    { "D",   0b0000001100000000 },
    { "A",   0b0000110000000000 },
    { "M",   0b0001110000000000 },
    { "!D",  0b0000001101000000 },
    { "!A",  0b0000110001000000 },
    { "!M",  0b0001110001000000 },
    { "-D",  0b0000001111000000 },
    { "-A",  0b0000110011000000 },
    { "-M",  0b0001110011000000 },
    { "D+1", 0b0000011111000000 },
    { "A+1", 0b0000110111000000 },
    { "M+1", 0b0001110111000000 },
    { "D-1", 0b0000001110000000 },
    { "A-1", 0b0000110010000000 },
    { "M-1", 0b0001110010000000 },
    { "D+A", 0b0000000010000000 },
    { "D+M", 0b0001000010000000 },
    { "D-A", 0b0000010011000000 },
    { "D-M", 0b0001010011000000 },
    { "A-D", 0b0000000111000000 },
    { "M-D", 0b0001000111000000 },
    { "D&A", 0b0000000000000000 },
    { "D&M", 0b0001000000000000 },
    { "D|A", 0b0000010101000000 },
    { "D|M", 0b0001010101000000 }
  }; 

  static const std::unordered_map<std::string, uint16_t> jumps = {
    { "JGT", 0x1 },
    { "JEQ", 0x2 },
    { "JGE", 0x3 },
    { "JLT", 0x4 },
    { "JNE", 0x5 },
    { "JLE", 0x6 },
    { "JMP", 0x7 }
  };


  uint16_t CToMachineCode(const std::string& c) {
    return std::stoi(c);
  }

  uint16_t OperationToMachineCode(const std::string& code) {
    if (operations.find(code) == operations.end()) {
      throw MachineError(code);
    }

    return operations.at(code);
  }

  uint16_t OutputToMachineCode(const std::string& output) {
    uint16_t out;

    out = 0;
    if (output.find("A") != std::string::npos)
      out |= OutA;
    if (output.find("D") != std::string::npos)
      out |= OutD;
    if (output.find("M") != std::string::npos)
      out |= OutM;
    return out;
  }

  uint16_t JumpToMachineCode(const std::string& jump) {
    if (jumps.find(jump) == jumps.end()) {
      throw MachineError(jump);
    }


    return jumps.at(jump);
  }

  std::string DisassembleInstruction(uint16_t code) {
    std::string out = "";

    if ((code & AInstruction) != AInstruction) {
      out += '@';
      out.append(std::to_string(code));
      return out;
    }

    if ((0x38 & code) != 0) {
      if ((code & OutA)) out += 'A';
      if ((code & OutD)) out += 'D';
      if ((code & OutM)) out += 'M';
      out += '=';
    }

    for( auto& operation : operations) {
      if ((0x1fc0 & code) == operation.second)
        out.append(operation.first);
    }

    for (auto& jump : jumps) {
      if ((0x7 & code) == jump.second) {
        out += ';';
        out.append(jump.first);
      }
    }

    return out;
  }

} // namespace ar
