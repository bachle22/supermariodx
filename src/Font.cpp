#include "Font.h"

// Get sprite ID from char input
LPSPRITE Font::Map(char c)
{
	// Check if char is in range of known sprite IDs
	if (!(c >= 48 && c <= 57 || c >= 64 && c <= 90)) return NULL;
	return fonts[c];
}

std::vector<LPSPRITE> Font::StringToSprites(std::string str)
{
	std::vector<LPSPRITE> sprites;
	LPSPRITE sprite;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		char c = (char)str[i];
		sprite = Map(c);
		if (sprite != NULL) sprites.push_back(sprite);
	}
	return sprites;
}


Font::Font()
{
	Sprites* sprites = Sprites::GetInstance();
	fonts.insert({ '0', sprites->Get(FONT_0) });
	fonts.insert({ '1', sprites->Get(FONT_1) });
	fonts.insert({ '2', sprites->Get(FONT_2) });
	fonts.insert({ '3', sprites->Get(FONT_3) });
	fonts.insert({ '4', sprites->Get(FONT_4) });
	fonts.insert({ '5', sprites->Get(FONT_5) });
	fonts.insert({ '6', sprites->Get(FONT_6) });
	fonts.insert({ '7', sprites->Get(FONT_7) });
	fonts.insert({ '8', sprites->Get(FONT_8) });
	fonts.insert({ '9', sprites->Get(FONT_9) });
	fonts.insert({ 'A', sprites->Get(FONT_A) });
	fonts.insert({ 'B', sprites->Get(FONT_B) });
	fonts.insert({ 'C', sprites->Get(FONT_C) });
	fonts.insert({ 'D', sprites->Get(FONT_D) });
	fonts.insert({ 'E', sprites->Get(FONT_E) });
	fonts.insert({ 'F', sprites->Get(FONT_F) });
	fonts.insert({ 'G', sprites->Get(FONT_G) });
	fonts.insert({ 'H', sprites->Get(FONT_H) });
	fonts.insert({ 'I', sprites->Get(FONT_I) });
	fonts.insert({ 'J', sprites->Get(FONT_J) });
	fonts.insert({ 'K', sprites->Get(FONT_K) });
	fonts.insert({ 'L', sprites->Get(FONT_L) });
	fonts.insert({ 'M', sprites->Get(FONT_M) });
	fonts.insert({ 'N', sprites->Get(FONT_N) });
	fonts.insert({ 'O', sprites->Get(FONT_O) });
	fonts.insert({ 'P', sprites->Get(FONT_P) });
	fonts.insert({ 'Q', sprites->Get(FONT_Q) });
	fonts.insert({ 'R', sprites->Get(FONT_R) });
	fonts.insert({ 'S', sprites->Get(FONT_S) });
	fonts.insert({ 'T', sprites->Get(FONT_T) });
	fonts.insert({ 'U', sprites->Get(FONT_U) });
	fonts.insert({ 'V', sprites->Get(FONT_V) });
	fonts.insert({ 'W', sprites->Get(FONT_W) });
	fonts.insert({ 'X', sprites->Get(FONT_X) });
	fonts.insert({ 'Y', sprites->Get(FONT_Y) });
	fonts.insert({ 'Z', sprites->Get(FONT_Z) });
}
