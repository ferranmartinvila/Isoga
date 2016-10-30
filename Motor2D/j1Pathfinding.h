#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Queue.h"
#include "p2Point.h"
#include "j1Module.h"


enum TERRAIN {

	GRASS = 27,
	WATER,
	MUD,
	DEEP_WATER

};

// ----------------------------------------------------
class j1Pathfinding : public j1Module
{
public:

	//Constructor
	j1Pathfinding();

	// Destructor
	virtual ~j1Pathfinding();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before game loop
	bool Start();

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	//Path functions
	bool SetPathStart(iPoint coordenate);
	bool SetPathGoal(iPoint coordenate);

	//BFS functions
	void PropagateBFS();

	//Dijkstra functions
	void PropagateDijkstra();

	//A* functions


	void ResetPath();

private:

	bool correct_path = false;

	p2Queue<iPoint>		open;
	p2List<iPoint>		close;

	iPoint start;
	iPoint goal;

	//Textures
	SDL_Texture* tex_goal;

	//Fx
	uint goal_find;
};

#endif // __j1PATHFINDING_H__