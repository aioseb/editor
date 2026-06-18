#pragma once
#include <cstdint>
#include <random>
#include <time.h>
#include "config.h"

struct Color {
	uint8_t r = 255;
	uint8_t b = 255;
	uint8_t g = 255;
	uint8_t a = 255;
};

constexpr Color WHITE = { 255, 255, 255, 255 };
constexpr Color BLACK = { 0, 0, 0, 255 };
constexpr Color RED = { 255, 0, 0, 255 };
constexpr Color GREEN = { 0, 255, 0, 255 };
constexpr Color BLUE = { 0, 0, 255, 255 };

Color randomColor();
float linearizeDepth(float depth);
void printInstructions();