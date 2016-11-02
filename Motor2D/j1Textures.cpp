#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"

#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )

j1Textures::j1Textures() : j1Module()
{
	name.create("textures");
}

// Destructor
j1Textures::~j1Textures()
{}

// Called before render is available
bool j1Textures::Awake(pugi::xml_node& config)
{
	LOG("Init Image library");
	bool ret = true;
	// load support for the PNG image format
	int flags = IMG_INIT_PNG;
	int init = IMG_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool j1Textures::Start()
{
	LOG("start textures");
	bool ret = true;
	return ret;
}

// Called before quitting
bool j1Textures::CleanUp()
{
	LOG("Freeing textures and Image library");
	p2List_item<SDL_Texture*>* item;

	for(item = textures.start; item != NULL; item = item->next)
	{
		SDL_DestroyTexture(item->data);
	}

	textures.clear();
	IMG_Quit();
	return true;
}

// Load new texture from file path
SDL_Texture* const j1Textures::Load(const char* path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load_RW(App->fs->Load(path), 1);

	if(surface == NULL)
	{
		LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

// Unload texture
bool j1Textures::UnLoad(SDL_Texture* texture)
{
	p2List_item<SDL_Texture*>* item;

	for(item = textures.start; item != NULL; item = item->next)
	{
		if(texture == item->data)
		{
			SDL_DestroyTexture(item->data);
			textures.del(item);
			return true;
		}
	}

	return false;
}

// Translate a surface into a texture
SDL_Texture* const j1Textures::LoadSurface(SDL_Surface* surface)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(App->render->renderer, surface);

	if(texture == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		textures.add(texture);
	}

	return texture;
}

// Retrieve size of a texture
void j1Textures::GetSize(const SDL_Texture* texture, uint& width, uint& height) const
{
	SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, (int*) &width, (int*) &height);
}


//FONTS----------------------------------------------------
int j1Textures::LoadFont(const char* texture_path, const char* characters, uint rows) {
	int id = -1;
	//Bad data enterd
	if (texture_path == nullptr || characters == nullptr || rows == 0) {
		LOG("Could not load fonts");
		return id;
	}
	SDL_Texture* texture = App->tex->Load(texture_path);

	//Charaters over the capacity
	if (texture == nullptr || strlen(characters) >= MAX_FONT_CHARS) {

		LOG("Can't load font at %s with characters '%s'", texture_path, characters);
		return id;
	}

	id = 0;
	for (; id < MAX_FONTS; id++) {
		if (fonts[id].graphic == nullptr)break;
	}
	//Characters array if full
	if (id == MAX_FONTS) {
		LOG("Cannot load font %s. Array full (max %d)", texture_path, MAX_FONTS);
		return id;
	}

	//Here starts the bitmap font creation
	Font* f = &fonts[id];
	f->graphic = texture;
	strcpy_s(fonts[id].table, MAX_FONT_CHARS, characters);
	f->rows = rows;
	f->lenght = strlen(characters);
	f->row_chars = fonts[id].row_chars / rows;

	uint w, h;
	App->tex->GetSize(texture, w, h);
	f->row_chars = fonts[id].lenght / rows;
	f->char_w = w / fonts[id].row_chars;
	f->char_h = h / rows;

	LOG("BMP font loaded from %s", texture_path);

	return id;
}

void j1Textures::UnLoadFont(int font_id) {
	if (font_id >= 0 && font_id < MAX_FONTS && fonts[font_id].graphic != nullptr)
	{
		App->tex->UnLoad(fonts[font_id].graphic);
		fonts[font_id].graphic = nullptr;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}

void j1Textures::BlitFont(int x, int y, int font_id, const char* text, BLIT_POINT blit_point) const
{
	if (text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts[font_id].graphic == nullptr)
	{
		LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = &fonts[font_id];
	SDL_Rect rect;
	uint len = strlen(text);

	rect.w = font->char_w;
	rect.h = font->char_h;

	int digits = 0;
	for (uint i = 0; i < len; ++i)
	{
		for (uint j = 0; j < font->lenght; ++j)
		{
			if (font->table[j] == text[i]) {
				digits++;
			}
		}
	}

	if (blit_point == CENTER)x -= rect.w*digits / 2;
	else if (blit_point == RIGHT) x -= rect.w*digits;
	else if (blit_point == LEFT) x += rect.w*digits;

	for (uint i = 0; i < len; ++i)
	{
		// we could use strchr instead ?
		for (uint j = 0; j < font->lenght; ++j)
		{
			if (font->table[j] == text[i])
			{
				rect.x = rect.w * (j % font->row_chars);
				rect.y = rect.h * (j / font->row_chars);
				App->render->Blit(font->graphic, x, y, &rect, 0.0f);
				x += rect.w;
				break;
			}
		}
	}
}