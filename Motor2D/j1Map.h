#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Queue.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Module.h"

// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		p2SString name;
		bool value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while(item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, bool default_value = false) const;

	p2List<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
	p2SString	encoding;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

enum TERRAIN {

	GRASS = 27,
	WATER,
	MUD,
	DEEP_WATER,
	PORTAL

};

// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	layers;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

public:

	MapData data;

	bool CreateWalkabilityMap(int& width, int & height, uchar** buffer)const;
	
	bool CreateWalkCostMap(int& width, int& height, uchar** buffer)const;
	
	int MovementCost(int x, int y) const;

	TileSet* GetTilesetFromTileId(int id) const;
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	void CollideLayer();

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;

	bool collide_layer = false;

	p2List<iPoint> portals;

public:

	bool Is_Portal(int& x, int& y)const;
	iPoint GetBestPortal(iPoint& goal )const;

};

#endif // __j1MAP_H__