#include "j1ModulePlayer.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1Audio.h"

j1ModulePlayer::j1ModulePlayer()
{
	name.create("player");
	player_texture = NULL;
}

j1ModulePlayer::~j1ModulePlayer()
{
}

bool j1ModulePlayer::Awake(pugi::xml_node &config)
{
	//Load player coordinates
	player_coordinates.x = config.child("coordinates").attribute("x_tile").as_int();
	player_coordinates.y = config.child("coordinates").attribute("y_tile").as_int();
	LOG("Player coordenate Loaded");

	return true;
}

bool j1ModulePlayer::Start()
{
	//Player Texture
	player_texture = App->tex->Load("textures/purple_robot.png");
	
	//Player Update rate
	update_rate = 350;
	
	//Position in the path when running it
	path_cell = 0;

	//Steps Fx
	steps_fx = App->audio->LoadFx("audio/fx/steps_fx.wav");
	//Player tp Fx
	player_tp_fx = App->audio->LoadFx("audio/fx/player_tp_fx.wav");
	//Path complete fx
	path_complete_fx = App->audio->LoadFx("audio/fx/path_complete_fx.wav");


	if (player_texture != NULL)return true;
	
	LOG("Cannot load Player Data!");
	return false;
}

bool j1ModulePlayer::PreUpdate()
{
	//Update the player direction if get user input
	if		(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)player_direction = WEST;
	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)player_direction = NORTH_WEST;
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)player_direction = NORTH;
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)player_direction = NORTH_EAST;
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)player_direction = EAST;
	else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT)player_direction = SOUTH_EAST;
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)player_direction = SOUTH;
	else if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT)player_direction = SOUTH_WEST;

	return true;
}

bool j1ModulePlayer::Update(float dt)
{

	//TickUpdate
	if (UpdateTick() == true) {

		//Player Input Move
		if (player_direction == WEST) { player_coordinates.x--; player_coordinates.y++; player_direction = NO_DIR; }
		if (player_direction == NORTH_WEST) { player_coordinates.x--; player_direction = NO_DIR; }
		if (player_direction == NORTH) {  player_coordinates.y--; player_coordinates.x--; player_direction = NO_DIR; }
		if (player_direction == NORTH_EAST) { player_coordinates.y--; player_direction = NO_DIR; }
		if (player_direction == EAST) { player_coordinates.x++; player_coordinates.y--; player_direction = NO_DIR; }
		if (player_direction == SOUTH_EAST) { player_coordinates.x++; player_direction = NO_DIR; }
		if (player_direction == SOUTH) { player_coordinates.x++; player_coordinates.y++; player_direction = NO_DIR; }
		if (player_direction == SOUTH_WEST) { player_coordinates.y++; player_direction = NO_DIR; }

		//Player run the path
		uint path_size = App->pathfinding->last_path.Count();
		
		if (path_size > 0) {
		
			if (path_cell <= path_size -1 && player_coordinates == *App->pathfinding->last_path.At(path_cell)) {
				
				if (path_cell < path_size - 1)path_cell++;
				
				else {
					//Path complete fx
					App->audio->PlayFx(path_complete_fx);
					//Reset player path cell
					path_cell = 0;
				}

				
				player_coordinates = *App->pathfinding->last_path.At(path_cell);
				//Play steps fx
				App->audio->PlayFx(steps_fx);
				if (App->pathfinding->Is_Portal(player_coordinates.x, player_coordinates.y))App->audio->PlayFx(player_tp_fx);

			}
			else path_cell = 0;
		}

		else {
			//Player Independent Teleport
			iPoint best_portal = App->pathfinding->GetBestPortal(App->pathfinding->goal);

			if (player_coordinates != best_portal && App->pathfinding->Is_Portal(player_coordinates.x, player_coordinates.y)) {

				//Tp player
				player_coordinates = best_portal;
				//Play portal fx
				App->audio->PlayFx(player_tp_fx);
			}
		}
	}

	//Update
	App->player->Draw();

	return true;
}

bool j1ModulePlayer::PostUpdate()
{
	return true;
}

bool j1ModulePlayer::CleanUp()
{
	return true;
}

bool j1ModulePlayer::Load(pugi::xml_node &load_xml)
{
	return true;
}

bool j1ModulePlayer::Save(pugi::xml_node &save_xml)
{
	return true;
}

void j1ModulePlayer::Draw()
{
	
	//Calculate the position
	iPoint pos = App->map->MapToWorld(player_coordinates.x,player_coordinates.y);
	
	//Render the player texture
	App->render->Blit(player_texture, pos.x , pos.y );

}

bool j1ModulePlayer::UpdateTick()
{
	//Update current time
	uint64  current_time = App->GetMSTime();

	if (last_update + update_rate < current_time) {
		
		//Update last update time
		last_update = current_time;

		return true;

	}
	return false;
}
