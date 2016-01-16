#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "j1Module.h"
#include "j1Console.h"

// ----------------------------------------------------

struct Properties
{
	p2List<p2SString>	names;
	p2DynArray<int>		values;

	int GetProperty(char* p)
	{
		uint i = names.find(p);
		if (i <= values.Count())
			return values[i];
		return 0;
	}
	// TODO 5: Create a generic structure to hold properties
	// TODO 7: Our custom properties should have one method
	// to ask for the value of a custom property
};

// ----------------------------------------------------
struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
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

	inline void Set(int x, int y, int newId)
	{
		if (y*width + x >= 0 && y*width + x <= width*height)
			data[y*width + x] = newId;
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
	int					tileCount;
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

	// Called before first frame
	bool Start();

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	void ChangeTile(int x, int y, int id);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;

#pragma region Commands
	struct C_Map_Render : public Command
	{
		C_Map_Render() : Command("map_render", "Enable / Disable map render", 1, "map_r"){}
		void function(const p2DynArray<p2SString>* arg)
		{
			p2SString str = arg->At(1)->GetString();
			if (str == "enable")
			{
				App->map->data.layers.start->data->properties.values[0] = 1;
			}
			else if (str == "disable")
			{
				App->map->data.layers.start->data->properties.values[0] = 0;
			}
			else
				LOG("map_render: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());

		}
	};
	C_Map_Render c_Map_Render;

	struct C_Map_Debug : public Command
	{
		C_Map_Debug() : Command("map_debug", "Enable / Disable map debug", 1, "map_d"){}
		void function(const p2DynArray<p2SString>* arg)
		{
			p2SString str = arg->At(1)->GetString();
			if (str == "enable")
			{
				App->map->data.layers.start->next->data->properties.values[0] = 1;
			}
			else if (str == "disable")
			{
				App->map->data.layers.start->next->data->properties.values[0] = 0;
			}
			else
				LOG("map_debug: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());

		}
	};
	C_Map_Debug c_Map_Debug;
};

#endif // __j1MAP_H__