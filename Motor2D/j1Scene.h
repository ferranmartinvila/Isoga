#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2DynArray.h"
#include "p2SString.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	//Map Folders
	p2DynArray<p2SString> map_folder;

	uint current_map;

	//Change the current map
	void Change_Map();

	//Load the choosed Map
	bool Load_Current_Map();

	//Textures data
	SDL_Texture* tex_goal;
	SDL_Texture* tex_path;

	//Fx data 
	uint goal_find;

	//Fonts data
	int debug_font;
	char debug_text[60];

	bool debug_mode;
};

#endif // __j1SCENE_H__