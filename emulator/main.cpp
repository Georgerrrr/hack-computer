#include <filesystem>

#include "common/architecture.h"
#include "computer.h"

struct Args {
  bool Debug = false;
  std::filesystem::path RomFile;
};

Args ParseArgs(int argc, char** argv) {
  Args a;

  for (auto i = 1 ; i < argc - 1 ; i++) {
    if (std::string(argv[i]) == "-d") a.Debug = true;
  }

  a.RomFile = argv[argc - 1];

  return a;
}

int main(int argc, char** argv) {
  if (argc < 2) return 0;

  auto args = ParseArgs(argc, argv);

  computer_t Computer(args.RomFile);
  Computer.Run(args.Debug);
}
