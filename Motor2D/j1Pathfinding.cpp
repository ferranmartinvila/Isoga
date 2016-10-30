#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include <math.h>

j1Pathfinding::j1Pathfinding() : j1Module()
{
	name.create("pathfinding");
}

// Destructor
j1Pathfinding::~j1Pathfinding()
{}

// Called before render is available
bool j1Pathfinding::Awake(pugi::xml_node& config)
{
	LOG("Loading Pathfinding data");
	bool ret = true;
	goal.x = goal.y = -1;
	return ret;
}

bool j1Pathfinding::Start() {
	bool ret = true;

	tex_goal = App->tex->Load("textures/goal_texture.png");
	goal_find = App->audio->LoadFx("audio/fx/goal_find.wav");

	return ret;
}

bool j1Pathfinding::SetPathStart(iPoint coordenate) {
	
	if (App->map->IsWalkable(coordenate.x, coordenate.y) && close.find(coordenate) == -1) {
		start = coordenate;
		open.Push(start);
		close.add(start);
		correct_path = true;
	}
	else correct_path = false;

	return correct_path;
}

bool j1Pathfinding::SetPathGoal(iPoint coordenate) {

	if (App->map->IsWalkable(coordenate.x, coordenate.y)) {
		goal = coordenate;
		correct_path = true;
	}
	else correct_path = false;

	return correct_path;
}

void j1Pathfinding::ResetPath()
{
	open.Clear();
	close.clear();
}

void j1Pathfinding::PropagateBFS()
{

	if (close.find(goal) != -1) {
		App->audio->PlayFx(goal_find);
		return;
	}

	iPoint point;
		
	if (open.start != NULL && close.find(goal) == -1) {

		open.Pop(point);

		if (open.find(point) == -1)close.add(point);

		iPoint neighbor[4];

		neighbor[0] = { point.x - 1,point.y };
		neighbor[1] = { point.x + 1,point.y };
		neighbor[2] = { point.x,point.y - 1 };
		neighbor[3] = { point.x,point.y + 1 };

		for (uint j = 0; j < 4; j++) {

			if (close.find(neighbor[j]) == -1 && App->map->IsWalkable(neighbor[j].x, neighbor[j].y)) {

				open.Push(neighbor[j]);
				close.add(neighbor[j]);

			}

		}
	}
}

void j1Pathfinding::DrawBFS()
{
	iPoint point;

	// Draw visited
	p2List_item<iPoint>* item = close.start;

	while (item)
	{
		point = item->data;
		TileSet* tileset = App->map->GetTilesetFromTileId(50);

		SDL_Rect r = tileset->GetTileRect(50);
		iPoint pos = App->map->MapToWorld(point.x, point.y);

		App->render->Blit(tileset->texture, pos.x, pos.y, &r);

		item = item->next;
	}

	// Draw frontier
	for (uint i = 0; i < open.Count(); ++i)
	{
		point = *(open.Peek(i));
		TileSet* tileset = App->map->GetTilesetFromTileId(49);

		SDL_Rect r = tileset->GetTileRect(49);
		iPoint pos = App->map->MapToWorld(point.x, point.y);

		App->render->Blit(tileset->texture, pos.x, pos.y, &r);
	}

	//Draw goal
	if (App->map->IsWalkable(goal.x,goal.y)) {
		
		iPoint pos = App->map->MapToWorld(goal.x, goal.y);

		App->render->Blit(tex_goal, pos.x, pos.y);
	}

}

void j1Pathfinding::PropagateDijkstra() {

	if (close.find(goal) != -1) {
		App->audio->PlayFx(goal_find); 
		return;
	}

	iPoint point;

	if (open.start != NULL) {

		open.Pop(point);

		if (open.find(point) == -1)close.add(point);

		iPoint neighbor[4];

		neighbor[0] = { point.x - 1, point.y };
		neighbor[1] = { point.x + 1, point.y };
		neighbor[2] = { point.x, point.y - 1 };
		neighbor[3] = { point.x, point.y + 1 };

		for (uint k = 0; k < 4; k++) {

			if (close.find(neighbor[k]) == -1 && App->map->IsWalkable(neighbor[k].x, neighbor[k].y)) {


				close.add(neighbor[k]);
				open.Push(neighbor[k]);


			}

		}

	}
}



void j1Pathfinding::Draw()
{
	
	DrawBFS();

}

// Called before quitting
bool j1Pathfinding::CleanUp()
{
	LOG("Unloading pathfinding");

	open.Clear();
	close.clear();

	return true;
}