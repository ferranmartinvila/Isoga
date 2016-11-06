#ifndef _J1_MODULE_PLAYER_
#define _J1_MODULE_PLAYER_

#include "j1Module.h"
#include "p2Point.h"

struct SDL_Texture;

enum DIRECTION {

	NORTH,
	SOUTH,
	EAST,
	WEST,
	NORTH_EAST,
	NORTH_WEST,
	SOUTH_EAST,
	SOUTH_WEST,
	NO_DIR

};

class j1ModulePlayer: public j1Module {
public:

	j1ModulePlayer();
	~j1ModulePlayer();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	bool Update(float dt);

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Load Player data
	bool Load(pugi::xml_node&);

	//Save Player data
	bool Save(pugi::xml_node&);

	//Draw Player
	void Draw();
private:

	//Time between updates
	uint32 update_rate;
	//Last update time
	uint32 last_update = 0;

	//Check if the player update is avaliable
	bool UpdateTick();

public:

	SDL_Texture*	player_texture;
	
	iPoint			player_coordinates;

	DIRECTION		player_direction;

	uint			path_cell;
};
#endif