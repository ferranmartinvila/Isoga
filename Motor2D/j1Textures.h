#ifndef __j1TEXTURES_H__
#define __j1TEXTURES_H__

#include "j1Module.h"
#include "p2List.h"

struct SDL_Texture;
struct SDL_Surface;

#define MAX_FONTS 100
#define MAX_FONT_CHARS 256

struct Font {
	char table[MAX_FONT_CHARS];
	SDL_Texture* graphic = nullptr;
	//Lines, row lenght, font weight, font height, chars in a row
	uint rows, lenght, char_w, char_h, row_chars;
};

enum BLIT_POINT {

	CENTER,
	LEFT,
	RIGHT

};

class j1Textures : public j1Module
{
public:

	j1Textures();

	// Destructor
	virtual ~j1Textures();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();


	SDL_Texture* const	Load(const char* path);
	bool				UnLoad(SDL_Texture* texture);
	SDL_Texture* const	LoadSurface(SDL_Surface* surface);
	void				GetSize(const SDL_Texture* texture, uint& width, uint& height) const;

	int LoadFont(const char* texture_path, const char* characters, uint rows = 1);
	//Unload
	void UnLoadFont(int font_id);
	//Create texture from data
	void BlitFont(int x, int y, int bmp_font_id, const char* text, BLIT_POINT blit_point = RIGHT)const;


public:

	p2List<SDL_Texture*>	textures;

private:

	Font fonts[MAX_FONTS];

};


#endif // __j1TEXTURES_H__