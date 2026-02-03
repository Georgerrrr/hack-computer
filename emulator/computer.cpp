#include <cstdio>
#include <iostream>
#include <algorithm>
#include <functional>
#include <fstream>

#include "computer.h"

static constexpr auto RamSize = 16384;
static constexpr auto ScreenSize = 24576;

static constexpr auto AInstruction  = 0xE000;
static constexpr auto AddressingBit = 0x1000;
static constexpr auto ZABit         = 0x0800;
static constexpr auto NABit         = 0x0400;
static constexpr auto ZBBit         = 0x0200;
static constexpr auto NBBit         = 0x0100;
static constexpr auto ADDBit        = 0x0080;
static constexpr auto NOBit         = 0x0040;
static constexpr auto OutA          = 0x0020;
static constexpr auto OutD          = 0x0010;
static constexpr auto OutM          = 0x0008;

static constexpr bool IsBitSet(uint16_t d, uint16_t bit) {
  return ((d & bit) == bit);
}

computer_t::computer_t(const std::filesystem::path& romfile) :
    Ram(RamSize),
    Screen(ScreenSize - RamSize),
    TexPixels(512 * 256),
    A(0),
    D(0),
    PC(0),
    ClockSpeed(3000),
    Renderer(NULL),
    Window(NULL),
    TexBuffer(NULL),
    step(false)
{
  Ram.resize(RamSize, 0);
  Screen.resize(ScreenSize - RamSize, 0);
  TexPixels.resize(512 * 256, 0);

  loadRom(romfile);

  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    printf("Error initialising sdl!\n");
    return;
  }

  Window = SDL_CreateWindow("Hack Computer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 512, SDL_WINDOW_SHOWN);

  if (!Window) {
    printf("Error creating sdl window!\n");
    return;
  }

  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!Renderer) {
    printf("Error creating sdl renderer!\n");
    return;
  }

  TexBuffer = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_XRGB32, SDL_TEXTUREACCESS_STREAMING, 512, 256);
  if (!TexBuffer) {
    printf("Error creating texture!\n");
    return;
  }

  SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
  SDL_RenderClear(Renderer);
}

computer_t::~computer_t() {
  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);
  SDL_Quit();
}

uint16_t computer_t::Peek(uint16_t address) {
  if (address < RamSize) return Ram[address];
  if (address < ScreenSize) return Screen[address - RamSize];
  if (address < ScreenSize + 1) return Key;

  printf("Invalid memory operation!\n");
  return 0;
}

void computer_t::Poke(uint16_t address, uint16_t value) {
  if (address < RamSize) Ram[address] = value;
  else if (address < ScreenSize) Screen[address - RamSize] = value;
  else if (address < ScreenSize + 1) Key = value;
  else printf("Invalid memory operation!\n");
}

void computer_t::Run(bool debug) {
  SDL_Event e;

  const auto InstructionsPerFrame = ClockSpeed / 60;

  if (debug) {
    step = true;
  }

  while (PC < rom.size()) {
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_QUIT: return;
        case SDL_TEXTINPUT: {
          Key = e.text.text[0];
        }; break;
        case SDL_KEYUP: {
          Key = 0;
        }; break;
      }
    }

    if (step) {
      DebugControl();
      ProcessInstruction(rom[PC]);
      if (PC >= rom.size()) return;
    }
    else {
      for (auto i = 0 ; i < InstructionsPerFrame ; i++) {
        if (std::find(BreakPoints.begin(), BreakPoints.end(), PC) != BreakPoints.end()) {
          DebugControl();
        }
        ProcessInstruction(rom[PC]);
        if (PC >= rom.size()) return;
      }
    }

    Render();

    SDL_UpdateWindowSurface(Window);
  }
}

void computer_t::DebugControl(void) {
  bool c = false;
  std::string buffer;
  std::string value;

  step = true;

  while (!c) {
    std::cout << "> ";
    std::getline(std::cin, buffer);

    size_t index;
    if ((index = buffer.find_first_of(' ')) != std::string::npos) {
      value = buffer.substr(index+1);
      buffer = buffer.substr(0, index);
    }
    
    if (buffer == "continue") {
      c = true;
      step = false;
    }
    else if (buffer == "step") {
      for (auto& symbol : symbols) {
        if (PC == symbol.second) {
          std::cout << "(" << symbol.first << ")\n";
          break;
        }
      }
      std::cout << ar::DisassembleInstruction(rom[PC]) << std::endl;
      c = true;
    }
    else if (buffer == "exit") {
      std::exit(0);
    }
    else if (buffer == "break") {
      if (symbols.find(value) == symbols.end()) {
        std::cout << "Invalid break point!" << std::endl;
        continue;
      }

      BreakPoints.push_back(symbols.at(value));
    }
    else {
      std::cout << "?" << std::endl;
    }
  }
}

void computer_t::ProcessInstruction(uint16_t Instruction) {
  uint16_t x, y, out, condition;

  if (!IsBitSet(Instruction, AInstruction)) { /* top three bits not set -> is an A instruction. */
    A = Instruction;
    PC++;
    return;
  }

  x = D;
  y = A;

  if (IsBitSet(Instruction, AddressingBit)) {
    y = Peek(A);
  }

  if (IsBitSet(Instruction, ZABit)) {
    x = 0;
  }

  if (IsBitSet(Instruction, NABit)) {
    x = ~x;
  }

  if (IsBitSet(Instruction, ZBBit)) {
    y = 0;
  }

  if (IsBitSet(Instruction, NBBit)) {
    y = ~y;
  }

  out = x & y;

  if (IsBitSet(Instruction, ADDBit)) {
    out = x + y;
  }

  if (IsBitSet(Instruction, NOBit)) {
    out = ~out;
  }

  condition = Instruction & 0x7;
  PC = NextPC(condition, out);
  
  if (IsBitSet(Instruction, OutM)) {
    Poke(A, out);
  }

  if (IsBitSet(Instruction, OutA)) {
    A = out;
  }

  if (IsBitSet(Instruction, OutD)) {
    D = out;
  }
}

uint16_t computer_t::NextPC(uint16_t condition, uint16_t eval) {
  const std::vector<std::function<bool(uint16_t)>> conditions = {
    [](uint16_t eval) { return false; },
    [](uint16_t eval) { return eval > 0; },
    [](uint16_t eval) { return eval == 0; },
    [](uint16_t eval) { return eval >= 0; },
    [](uint16_t eval) { return eval < 0; },
    [](uint16_t eval) { return eval != 0; },
    [](uint16_t eval) { return eval <= 0; },
    [](uint16_t eval) { return true; }
  };

  if (conditions[condition](eval)) {
    return A;
  }
  return PC + 1;
}

void computer_t::Render(void) {
  SDL_Rect src  = {0, 0, 512, 256};
  SDL_Rect dest = {0, 0, 1024, 512};

  for (auto i = 0 ; i < Screen.size() ; i++) {
    for (auto j = 0 ; j < 16 ; j++) {
      if ((Screen[i] & 1 << j) == 0) TexPixels[(i << 4) + j] = 0xffffffff;
      else TexPixels[(i << 4) + j] = 0x0;
    }
  }

  SDL_UpdateTexture(TexBuffer, &src, TexPixels.data(), sizeof(uint32_t) * 512);

  SDL_RenderCopy(Renderer, TexBuffer, &src, &dest);
  SDL_RenderPresent(Renderer);
}

void computer_t::loadRom(const std::filesystem::path& romfile) {
  char header[4]; 
  uint16_t numSymbols;
  uint16_t romOffset;

  std::ifstream f(romfile, std::ios::binary);
  f.seekg(0, std::ios::end);
  size_t romSize = f.tellg();
  f.seekg(0, std::ios::beg);

  f.read(reinterpret_cast<char*>(header), 4);

  if ((header[0] != 'H') ||
      (header[1] != 'r') ||
      (header[2] != 'o') ||
      (header[3] != 'm'))
    throw ar::RomError();

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
  romSize = (romSize - romOffset) / sizeof(uint16_t);
  rom.resize(romSize);

  f.read(reinterpret_cast<char*>(rom.data()), romSize * sizeof(uint16_t)); 
}

