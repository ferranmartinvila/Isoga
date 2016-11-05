#include "j1ModulePlayer.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Input.h"
#include "p2Log.h"

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
	player_texture = App->tex->Load("textures/purple_robot.png");
	if (player_texture != NULL)return true;
	
	LOG("Cannot load Player Data!");
	return false;
}

bool j1ModulePlayer::PreUpdate()
{
	//Update the player direction if get user input
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)player_direction = WEST;
	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)player_direction = NORTH_WEST;
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)player_direction = NORTH;
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)player_direction = NORTH_EAST;
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)player_direction = EAST;
	else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)player_direction = SOUTH_EAST;
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)player_direction = SOUTH;
	else if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)player_direction = SOUTH_WEST;

	return true;
}

bool j1ModulePlayer::Update(float dt)
{
	if (UpdateTick()) {

		if (player_direction == WEST) { player_coordinates.x--; player_coordinates.y++; player_direction = NO_DIR; }
		if (player_direction == NORTH_WEST) { player_coordinates.x--; player_direction = NO_DIR; }
		if (player_direction == NORTH) {  player_coordinates.y--; player_coordinates.x--; player_direction = NO_DIR; }
		if (player_direction == NORTH_EAST) { player_coordinates.y--; player_direction = NO_DIR; }
		if (player_direction == EAST) { player_coordinates.x++; player_coordinates.y--; player_direction = NO_DIR; }
		if (player_direction == SOUTH_EAST) { player_coordinates.x++; player_direction = NO_DIR; }
		if (player_direction == SOUTH) { player_coordinates.x++; player_coordinates.y++; player_direction = NO_DIR; }
		if (player_direction == SOUTH_WEST) { player_coordinates.y++; player_direction = NO_DIR; }

	}

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
		//LOG it
		LOG("Player Update Time: %u\n", current_time);

		return true;

	}
	return false;
}
