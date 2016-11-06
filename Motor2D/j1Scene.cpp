#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1ModulePlayer.h"

j1Scene::j1Scene() : j1Module()
{
	debug_mode = false;
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	//Load the map
	if (App->map->Load("iso_walk.tmx")) {

		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetWalkabilityMap(w, h, data);

		RELEASE_ARRAY(data);

		if (App->map->CreateWalkCostMap(w, h, &data))
			App->pathfinding->SetWalkCostMap(w, h, data);

		RELEASE_ARRAY(data);

	}
	//Load and play the music
	App->audio->PlayMusic("audio/music/GOW_Pandora.ogg");
	
	//Load textures
	tex_goal = App->tex->Load("textures/goal_texture.png");
	tex_path = App->tex->Load("textures/path_texture.png");

	//Load Fx
	goal_find = App->audio->LoadFx("audio/fx/goal_find.wav");
	
	//Load fonts
	debug_font = App->tex->LoadFont("textures/debug_font.png", "!*#$%&`()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[[]|`", 1);

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	
	//FILE SYSTEM------------------------------------------
	if(App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	//MAP MOVEMENT-----------------------------------------
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += SDL_ceil(500 * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= SDL_ceil(500 * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += SDL_ceil(500 * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= SDL_ceil(500 * dt);

	//PATHFINDING------------------------------------------
	if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
		int x, y;
		App->input->GetMousePosition(x, y);
		App->pathfinding->SetPathStart(App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y));
	}

	if (App->input->GetMouseButtonDown(3) == KEY_DOWN) {
		int x, y;
		App->input->GetMousePosition(x, y);
		App->pathfinding->SetPathGoal(App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y));
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		App->pathfinding->ResetPath();

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_REPEAT)
		App->pathfinding->PropagateBFS();

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_REPEAT)
		App->pathfinding->PropagateDijkstra();

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) 
		App->pathfinding->CreatePath(App->pathfinding->start, App->pathfinding->goal, false);

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) 
		App->pathfinding->CreatePath(App->pathfinding->start, App->pathfinding->goal, true);

	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
		App->pathfinding->CreatePath(App->pathfinding->start, App->pathfinding->goal, true, true);

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
		App->player->player_coordinates = App->pathfinding->start;

	//DEBUG------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		App->map->CollideLayer();
		debug_mode = !debug_mode;
	}

	App->map->Draw();
	App->pathfinding->Draw();

	//FONTS------------------------------------------------
	if (debug_mode) {
		//Propagate BFS
		sprintf_s(debug_text, 24, "%s", "PRESS.[1].PROPAGATE.BFS");
		App->tex->BlitFont(220, 15, debug_font, debug_text);
		//Propagate Dijkstra
		sprintf_s(debug_text, 29, "%s", "PRESS.[2].PROPAGATE.DIJKSTRA");
		App->tex->BlitFont(264, 30, debug_font, debug_text);
		//A* without diagonals
		sprintf_s(debug_text, 28, "%s", "PRESS.[3].PROPAGATE.A*.PATH");
		App->tex->BlitFont(264, 45, debug_font, debug_text);
		//A* with diagonals
		sprintf_s(debug_text, 43, "%s", "PRESS.[4].PROPAGATE.A*.PATH.WITH.DIAGONALS");
		App->tex->BlitFont(398, 60, debug_font, debug_text);

		//Change distance algorithm
		sprintf_s(debug_text, 53, "%s", "PRESS.[5].PROPAGATE.A*.WALK.COST.PATH.WITH.DIAGONALS");
		App->tex->BlitFont(488, 75, debug_font, debug_text);

	}
	


	//Map coordinates
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);

	p2SString title("MAP DATA[Map: %dx%d Tiles: %dx%d Tilesets: %d Tile: %d,%d]     TIME DATA[Time: %u  AvgFPS: %.2f  FPS: %i Frames: %i]    PLAYER[X: %i  Y: %i]",
		//Map Data
		App->map->data.width, App->map->data.height,
		App->map->data.tile_width, App->map->data.tile_height,
		App->map->data.tilesets.count(),
		map_coordinates.x, map_coordinates.y,
		//Time Data
		App->GetTime(),
		App->GetAvgFPS(),
		App->GetFPS(),
		App->GetFramesCount(),
		App->player->player_coordinates.x,
		App->player->player_coordinates.y);

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
