#pragma once 

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <filesystem>

#include <SDL.h>

#include "common/architecture.h"

struct computer_t {
  uint16_t A, D, PC;
  std::vector<uint16_t> Ram;
  std::vector<uint16_t> Screen;
  uint16_t Key;
  const uint32_t ClockSpeed;
  std::vector<uint16_t> BreakPoints;

  computer_t(const std::filesystem::path& romfile);
  ~computer_t();

  uint16_t Peek(uint16_t address);
  void Poke(uint16_t address, uint16_t value);

  void Run(bool debug);

  void SetBreakPoint(uint16_t bp) {
    BreakPoints.push_back(bp);
  }

  private:
  void ProcessInstruction(uint16_t Instruction);
  uint16_t NextPC(uint16_t condition, uint16_t eval);

  void Render(void);

  void loadRom(const std::filesystem::path& romfile);

  void DebugControl(void);

  SDL_Renderer* Renderer;
  SDL_Window* Window;

  std::vector<uint32_t> TexPixels;
  SDL_Texture* TexBuffer;

  bool step;
  std::vector<uint16_t> rom;
  ar::symbol_table_t symbols;
};
