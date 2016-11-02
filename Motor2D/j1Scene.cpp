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

j1Scene::j1Scene() : j1Module()
{
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

	}
	//Load and play the music
	App->audio->PlayMusic("audio/music/GOW_Pandora.ogg");

	//Load Fx
	
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
	//WATCH------------------------------------------------
	int crone_time = time_data.Read();

	if (crone_time >= (initial_time + ratio))
	{
		seconds++;
		if (seconds > 9)
		{
			seconds = 0;
			++decimes;
		}

		if (decimes > 5)
		{
			decimes = 0;
			++minutes;
		}
		initial_time = crone_time;
	}
	
	//FILE SYSTEM------------------------------------------
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	//MAP MOVEMENT-----------------------------------------
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 5;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 5;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 5;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 5;

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

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		App->pathfinding->CreatePath(App->pathfinding->start, App->pathfinding->goal);
	}


	//DEBUG------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		App->map->CollideLayer();
	}



	App->map->Draw();
	App->pathfinding->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("MAP DATA[Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d]     TIME DATA[Time: %i:%i%i  FPS: %i Frames: %i]",
		App->map->data.width, App->map->data.height,
		App->map->data.tile_width, App->map->data.tile_height,
		App->map->data.tilesets.count(),
		map_coordinates.x, map_coordinates.y, minutes, decimes, seconds, App->render->GetFPS(),App->render->GetAllFrames());

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
