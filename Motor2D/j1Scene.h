#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"
struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

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

	//Timer data
	j1Timer time_data;

	int initial_time = 0;
	int crone_time = 0;
	int ratio = 1000;
	int seconds = 0;
	int decimes = 0;
	int minutes = 0;

	//Textures data
	SDL_Texture* tex_goal;
	SDL_Texture* tex_path;

	//Fx data 
	uint goal_find;

	//Fonts data
	int debug_font;
	char debug_text[40];

	bool debug_mode;
};

#endif // __j1SCENE_H__