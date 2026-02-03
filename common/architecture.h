#pragma once 

#include <cstdint>
#include <string>
#include <unordered_map>
#include <exception>
#include <sstream>

namespace ar {

  typedef std::unordered_map<std::string, uint16_t> symbol_table_t;

  class MachineError : public std::exception {
    public:
    MachineError(const std::string& instruction) {
      std::stringstream s;

      s << "Invalid instruction: " << instruction;
      message = s.str();
    }

    const char* what() const noexcept {
      return message.c_str();
    }

    private:
    std::string message;
  };

  class RomError : public std::exception {
    public:
    RomError() {
    }

    const char* what() const noexcept {
      return "Invalid rom!";
    }
  };

  uint16_t CToMachineCode(const std::string& c);
  uint16_t OperationToMachineCode(const std::string& code);
  uint16_t OutputToMachineCode(const std::string& output);
  uint16_t JumpToMachineCode(const std::string& jump);

  std::string DisassembleInstruction(uint16_t code);

} // namespace ar
