#include <cstdio>
#include <vector>
#include <sstream>
#include <string>
#include <functional>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <exception>

#include "common/architecture.h"
#include "common/language.h"

static constexpr auto AInstruction  = 0xE000;
static constexpr auto OutA          = 0x0020;
static constexpr auto OutD          = 0x0010;
static constexpr auto OutM          = 0x0008;

class ParseError : public std::exception {
  public:
  ParseError(const char* m, size_t l)
  {
    std::stringstream s;
    s << "(" << l << ") " << m;
    message = s.str();
  }

  const char* what() const noexcept {
    return message.c_str();
  }

  private:
  std::string message;
};

typedef std::vector<std::pair<std::string, uint16_t>> unresolved_t;

void assembleCInstruction(size_t line, const std::string& token, std::vector<uint16_t>& output, const ar::symbol_table_t& symbols, unresolved_t& unresolved) {
  auto cValue = token.substr(1);

  if (cValue.length() == 0) {
    throw ParseError("@ symbol must be followed by a numeric, or a valid label!", line);
  }

  if (cValue.find_first_not_of("1234567890") == std::string::npos) {
    output.push_back(ar::CToMachineCode(cValue));
    return;
  }
  if (auto key = symbols.find(cValue) ; key != symbols.end()) {
    output.push_back(key->second);
    return;
  }

  unresolved.push_back({cValue, output.size()});
  output.push_back(0); // push some dummy value
}

uint16_t assembleAInstruction(size_t line, const std::string& token) {

  uint16_t out = AInstruction; /* set top bits */

  auto output = token.find_first_of('=');
  auto jump = token.find_first_of(';');

  if (output != std::string::npos) { /* if line contains equals */
    auto store = token.substr(0, output);

    out |= ar::OutputToMachineCode(store);
  }

  if (jump != std::string::npos) {
    auto condition = token.substr(jump+1);
    out |= ar::JumpToMachineCode(condition);
  }

  auto compute = token.substr(output+1, jump - 1 - output);

  out |= ar::OperationToMachineCode(compute);
  return out;
}

rom_t assembleLines(const std::vector<std::string>& lines, bool variables) {
  enum {
    notComment,
    lineComment,
    multiLineComment
  } inComment;
  inComment = notComment;

  const std::string whitespace = " \t";

  rom_t out;

  out.symbols = {
    {"SCREEN", 16384},
    {"KBD", 24576}
  };

  unresolved_t unresolved;

  auto lineNumber = 0;

  for (auto& line : lines) {
    lineNumber++;

    auto lineStart = line.find_first_not_of(whitespace);
    auto lineEnd = 0;

    if (lineStart == std::string::npos) continue;

    while ((lineStart = line.find_first_not_of(whitespace, lineEnd)) != std::string::npos) {
      lineEnd = line.find_first_of(whitespace, lineStart);

      if (inComment == lineComment) continue;

      auto token = line.substr(lineStart, lineEnd - lineStart);

      if (token == "*/") {
        if (inComment == multiLineComment) {
          inComment = notComment;
          continue;
        }

        throw ParseError("Invalid symbol \"*/\"!", lineNumber);
      }
      else if (inComment == multiLineComment) continue;

      if (token == "//") {
        inComment = lineComment;
        continue;
      }
      else if (token == "/*") {
        inComment = multiLineComment;
        continue;
      }

      if (*token.begin() == '@') {
        assembleCInstruction(lineNumber, token, out.code, out.symbols, unresolved);
      }
      else if (*token.begin() == '(') {
        auto start = token.find_first_of('(');
        auto end   = token.find_first_of(')');
        if (end == std::string::npos)
          throw ParseError("Expected )!", lineNumber);

        auto symbol = token.substr(start+1, end-1);
        if (symbol.size() == 0) 
          throw ParseError("Invalid label!", lineNumber);

        if (out.symbols.find(symbol) != out.symbols.end()) 
          throw ParseError("Duplicate labels!", lineNumber);

        out.symbols[symbol] = out.code.size();
      }
      else {
        out.code.push_back(assembleAInstruction(lineNumber, token));
      }
    }

    if (inComment == lineComment)
      inComment = notComment;
  }

  size_t count = 16;

  const std::vector<std::function<void(const std::pair<std::string, uint16_t>&)>> resolveNames = {
    [&](const std::pair<std::string, uint16_t>& ur) { 
      throw ParseError("Unresolved symbol!", ur.second);
    },
    [&](const std::pair<std::string, uint16_t>& ur) {
      out.code[ur.second] = count;
      count++;
    }
  };


  for (auto& ur : unresolved) {
    if (out.symbols.find(ur.first) == out.symbols.end())
      resolveNames[variables](ur);

    else
      out.code[ur.second] = out.symbols[ur.first];
  }

  return out;
}

struct Args {
  bool debug = false;
  bool setoutname = false;
  bool variables = false;
  bool run = false;
  std::filesystem::path outname;
  std::vector<std::filesystem::path> sources;

  void Process(int argc, char** argv) {
    run = false;

    if (argc == 1) {
      printf("%s args sources...\nargs:\n\t-d : debug\n\t-o : output filename\n\t-v : enable variables\n", argv[0]);
      return;
    }

    for (auto i = 1 ; i < argc ; i++) {
      if (std::string(argv[i]) == "-d") 
      {
        debug = true;
      }
      else if (std::string(argv[i]) == "-v")
      {
        variables = true;
      }
      else if (std::string(argv[i]) == "-o")
      {
        i++;
        if (i >= argc) {
          printf("args not in format -o <outname>\n");
          return;
        }
        setoutname = true;
        outname = std::filesystem::path(argv[i]);
      }
      else
      {
        sources.push_back(argv[i]);
      }
    }

    if (!setoutname) {
      outname = sources[0];
    }

    run = true;
  }
};

int main(int argc, char** argv) {
  Args args;

  std::vector<std::string> lines;

  args.Process(argc, argv);

  if (!args.run) return 0;

  if (args.sources.size() == 0) {
    printf("Must provide at least one source file!\n");
    return 0;
  }

  loadFile(args.sources[0], lines);
  auto rom = assembleLines(lines, args.variables);

  rom.write(args.outname.replace_extension(".rom"), args.debug);
}
