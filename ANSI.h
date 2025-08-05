/*
  ANSI.h
  ======

  Description:           Defines ANSI escape codes.

*/

#ifndef __ANSI_H__
#define __ANSI_H__

namespace ANSI {
// Font modifiers
// Bold font
const char* Bold = "\e[22m";

// Reset font/colour/text effects
const char* Reset = "\e[0m";

// Colour codes (16 colours only)
// This makes text invisible on kitty
const char* BlackFG = "\e[30m";

const char* RedFG = "\e[31m";
const char* GreenFG = "\e[32m";
const char* YellowFG = "\e[33m";
const char* BlueFG = "\e[34m";
const char* MagentaFG = "\e[35m";
const char* CyanFG = "\e[36m";
const char* WhiteFG = "\e[37m";

// Reset colour only
const char* DefaultFG = "\e[38m";

};  // namespace ANSI

#endif /* __ANSI_H__ */
