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

j1Pathfinding::j1Pathfinding() : j1Module(),walkability_map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)

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
	tex_path = App->tex->Load("textures/path_texture.png");
	goal_find = App->audio->LoadFx("audio/fx/goal_find.wav");

	return ret;
}

bool j1Pathfinding::SetPathStart(iPoint coordenate) {
	
	if (IsWalkable(coordenate) && close.find(coordenate) == -1) {
		start = coordenate;
		open.Push(start,0);
		close.add(start);
		correct_path = true;
	}
	else correct_path = false;

	return correct_path;
}

bool j1Pathfinding::SetPathGoal(iPoint coordenate) {

	if (IsWalkable(coordenate)) {
		goal = coordenate;
		correct_path = true;
	}
	else correct_path = false;

	return correct_path;
}

void j1Pathfinding::SetWalkabilityMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(walkability_map);
	walkability_map = new uchar[width*height];
	memcpy(walkability_map, data, width*height);
}

int j1Pathfinding::CreatePath(const iPoint& origin, const iPoint& goal) {
	
	// TODO 1: if origin or destination are not walkable, return -1
	if (IsWalkable(origin) == false || IsWalkable(goal) == false)return -1;
	

	//Clean the last path
	last_path.Clear();

	// TODO 2: Create two lists: open, close
	PathList open_list;
	PathList close_list;

	//Origin node
	PathNode origin_node(0,origin.DistanceManhattan(goal),origin,nullptr);
	//Working node
	PathNode in_work;

	// Add the origin tile to open
	open_list.list.add(origin_node);
	
	// Iterate while we have tile in the open list
	while (open_list.list.count() > 0) {

		// TODO 3: Move the lowest score cell from open list to the closed list
		  close_list.list.add(open_list.GetNodeLowestScore()->data);
		
		LOG("G:%i H:%i\n", open_list.GetNodeLowestScore()->data.g, open_list.GetNodeLowestScore()->data.h);
		
		open_list.list.del(open_list.GetNodeLowestScore());

		
		// TODO 4: If we just added the destination, we are done!
		if (close_list.list.end->data.pos == goal) {
			// Backtrack to create the final path
			for (p2List_item<PathNode>* node = close_list.list.end; node->data.parent != nullptr; node = close_list.Find(node->data.parent->pos)) {

				last_path.PushBack(node->data.pos);

			}
			// Use the Pathnode::parent and Flip() the path when you are finish
			last_path.Flip();

			return close_list.list.count();
		}

		// TODO 5: Fill a list of all adjancent nodes
		PathList adjacent_nodes;
		
		close_list.list.end->data.FindWalkableAdjacents(adjacent_nodes);

		// TODO 6: Iterate adjancent nodes:
		for (p2List_item<PathNode>* adjacent_node = adjacent_nodes.list.start; adjacent_node != NULL; adjacent_node = adjacent_node->next) {
			
			// ignore nodes in the closed list
			if (close_list.Find(adjacent_node->data.pos) != NULL)
			{
				continue;
			}
			
			// If it is NOT found
			
			// If it is already in the open list 
			p2List_item<PathNode>* node = open_list.Find(adjacent_node->data.pos);
			if (node != NULL) {

				//Check if it is a better path (compare G)
				adjacent_node->data.CalculateF(goal);
				if (adjacent_node->data.g < node->data.g) {

					// If it is a better path, Update the parent
					node->data.parent  = adjacent_node->data.parent;

				}
			}
			//Calculate its F and add it to the open list
			else{

				adjacent_node->data.CalculateF(goal);
				open_list.list.add(adjacent_node->data);

			}
		}

	}

	return -1;

}

const p2DynArray<iPoint>* j1Pathfinding::GetLastPath() const
{
	return &last_path;
}

bool j1Pathfinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

bool j1Pathfinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileWalkability(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

uchar j1Pathfinding::GetTileWalkability(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return walkability_map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
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

			if (close.find(neighbor[j]) == -1 && IsWalkable(neighbor[j])) {

				open.Push(neighbor[j]);
				close.add(neighbor[j]);

			}

		}
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

			if (close.find(neighbor[k]) == -1 && IsWalkable(neighbor[k])) {

					close.add(neighbor[k]);
					open.Push(neighbor[k], GetTileWalkability(neighbor[k]));

			}

		}

	}

}



void j1Pathfinding::Draw()
{
	
	iPoint point;

	// Draw visited
	p2List_item<iPoint>* item = close.start;

	while (item)
	{
		point = item->data;
		TileSet* tileset = App->map->GetTilesetFromTileId(26);

		SDL_Rect r = tileset->GetTileRect(26);
		iPoint pos = App->map->MapToWorld(point.x, point.y);

		App->render->Blit(tileset->texture, pos.x, pos.y, &r);

		item = item->next;
	}

	// Draw frontier
	for (uint i = 0; i < open.Count(); ++i)
	{
		point = *(open.Peek(i));
		TileSet* tileset = App->map->GetTilesetFromTileId(25);

		SDL_Rect r = tileset->GetTileRect(25);
		iPoint pos = App->map->MapToWorld(point.x, point.y);

		App->render->Blit(tileset->texture, pos.x, pos.y, &r);
	}

	//Draw goal

	iPoint pos = App->map->MapToWorld(goal.x, goal.y);

	App->render->Blit(tex_goal, pos.x, pos.y);

	//Draw Path
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(tex_path, pos.x, pos.y);
	}
}

// Called before quitting
bool j1Pathfinding::CleanUp()
{
	LOG("Unloading pathfinding");

	open.Clear();
	close.clear();

	RELEASE_ARRAY(walkability_map);

	return true;
}




// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceManhattan(destination);
		//pos.DistanceTo(destination);

	return g + h;
}