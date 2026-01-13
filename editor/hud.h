#pragma once

#include "pch.h"
#include "scene.h"

struct HUDText {
	sf::Text text;
	char* textValue;
	int charSize;
	sf::Color color;

	int posX = 0;
	int posY = 0;
};

struct TextList {
	sf::Text textArray[32];
	int textCount;
};
extern TextList textList;

void initializeHUD();
void renderHUD();