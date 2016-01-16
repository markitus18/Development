#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module()
{
	name.create("pathFinding");
}

//Destructor
j1PathFinding::~j1PathFinding()
{

}

// Called before render is available
bool j1PathFinding::Awake()
{
	return true;
}

bool j1PathFinding::Start()
{
	layer = new MapLayer;
	mapData = new map;
	startTile = endTile = iPoint{ -1, -1 };
	LoadMapData();
	return true;
}
// Called each loop iteration
bool j1PathFinding::Update(float dt)
{
	if (App->input->GetInputState() == false)
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
		{
			App->pathFinding->automaticIteration = true;
			App->pathFinding->FindPath();
		}
		if (App->input->GetKey(SDL_SCANCODE_0) == KEY_UP)
		{
			if (!App->pathFinding->pathStarted)
			{
				App->pathFinding->automaticIteration = false;
				App->pathFinding->FindPath();
			}

			App->pathFinding->StepUp();
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			App->pathFinding->allowDiagonals = !App->pathFinding->allowDiagonals;
			if (App->pathFinding->allowDiagonals)
			{
				LOG("-- Pathfinding: Diagonals enabled --");
			}
			else
				LOG("-- Pathfinding: Diagonals disabled --");
		}

		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_UP)
		{
			App->pathFinding->allowCorners = !App->pathFinding->allowCorners;
			if (App->pathFinding->allowCorners)
			{
				LOG("-- Pathfinding: Corners enabled --");
			}
			else
				LOG("-- Pathfinding: Corners disabled --");
		}
	}

	return true;
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	delete layer;
	delete mapData;
	return true;
}

void j1PathFinding::FindPath()
{
	StartPathFinding();
	if (automaticIteration)
	{
		AutomaticPath();
	}
}

void j1PathFinding::AutomaticPath()
{
	while (!pathFinished)
	{
		StepUp();
	}
}
bool j1PathFinding::StepUp()
{
	bool ret = true;
	if (!pathFinished)
	{
		if (openList.count() > 0)
		{
			if (!newLowest)
			{
				lowestFNode = GetLowestF();
				lowestF = lowestFNode->data->f;
			}
			newLowest = false;
			if (AddChilds(lowestFNode, endTile))
			{
				FinishPathFinding(path);
			}
		}
		else
		{
			pathFinished = true;
			pathStarted = false;
			LOG("-- Pathfinding: Could not find a path --");
		}
	}
	return ret;
}
bool j1PathFinding::LoadMapData()
{
	bool ret = true;
	LOG("-- Pathfinding: Loading meta data");
	p2List_item<MapLayer*>* item;
	for (item = App->map->data.layers.start; item; item = item->next)
	{
		if (item->data->name == "Meta")
		{

			mapData->height = item->data->height;
			mapData->width = item->data->width;
			mapData->data = new uint[mapData->height*mapData->width];
			for (int i = 0; i < mapData->width * mapData->height; i++)
			{
				if (item->data->data[i] == 25)
					mapData->data[i] = 1;
				else
					mapData->data[i] = 0;
			}
			ret = true;
		}
	}
	if (!ret)
		LOG("-- Pathfinding: Could not load meta tileset --");
	else
		LOG("-- Pathfinding: Meta tilesed loaded correctly --");
	return ret;

}

bool j1PathFinding::IfPathPossible()
{
	bool ret = false;
	if (startTileExists && endTileExists && !(startTile.x == endTile.x && startTile.y == endTile.y))
	{
		if (startTile.x >= 0 && startTile.x < App->map->data.width && startTile.y >= 0 && startTile.y < App->map->data.height)
			if (mapData->isWalkable(startTile.x, startTile.y) && mapData->isWalkable(endTile.x, endTile.y))
				ret = true;
	}
	if (!ret)
		LOG("-- Pathfinding: No possible path to be found --");
	return ret;
}

bool j1PathFinding::StartPathFinding()
{
	bool ret = false;
	if (mapChanged)
	{
		LoadMapData();
		mapChanged = false;
	}

	if (IfPathPossible())
	{
		pathStarted = false;
		pathFinished = false;
		newLowest = false;
		lowestF = App->map->data.height * App->map->data.width;
		if (openList.count() != 0)
			openList.clear();
		if (closedList.count() != 0)
			closedList.clear();
		if (path.Count() != 0)
			path.Clear();
		ret = CreateFirstNode();

		if (ret)
		{
			pathStarted = true;
			LOG("-- Pathfinding: Correct pathfinding start --");
		}
		else
			LOG("-- Pathfinding: Could not start the process--");

	}
	return ret;
}

bool j1PathFinding::CreateFirstNode()
{
	bool ret = false;
	if (startTileExists && endTileExists)
	{
		node* firstNode = new node;
		firstNode->parent = NULL;
		firstNode->tile.x = startTile.x;
		firstNode->tile.y = startTile.y;
		firstNode->g = 0;
		firstNode->h = (abs(endTile.x - firstNode->tile.x) + abs(endTile.y - firstNode->tile.y)) * 10;
		firstNode->f = firstNode->h;
		openList.add(firstNode);
		ret = true;
	}
	if (ret)
		LOG("-- Pathfinding: First node created --");
	else
		LOG("-- Pathfinding: Could not create first node --");
	return ret;
}

p2List_item<j1PathFinding::node*>* j1PathFinding::GetLowestF() const
{
	p2List_item<node*>* item = NULL;
	p2List_item<node*>* node = NULL;
	int f = (App->map->data.height * App->map->data.width) * 10;
	for (item = openList.start; item; item = item->next)
	{
		if (item->data->f <= f)
		{
			f = item->data->f;
			node = item;
		}
	}
	return node;
}

bool j1PathFinding::CreateSideNode(node* nParent, int x, int y, iPoint end, int amount, bool isDiagonal)
{
	bool ret = false;
	node* newNode = new node;
	newNode->parent = nParent;
	newNode->tile.x = x;
	newNode->tile.y = y;

	if (isDiagonal && !allowCorners)
	{
		if (!mapData->isWalkable(nParent->tile.x, newNode->tile.y) || !mapData->isWalkable(newNode->tile.x, nParent->tile.y))
		{
			delete newNode;
			return false;
		}
	}

	//Initial cost from the start
	newNode->g = nParent->g + amount;

	//Estimated cost to the end using Manhatan Distance
	newNode->h = (abs(end.x - x) + abs(end.y - y)) * 10;

	//Result of f + g
	newNode->f = newNode->g + newNode->h;

	//Compare if the current node already exists and it is not closed
	if (!IsNodeClosed(newNode))
	{
		if (!CheckIfExists(newNode))
		{
			if (newNode->f <= lowestFNode->data->f)
			{
				newLowest = true;
				lowestF = newNode->f;
				lowestFNode = openList.At(openList.find(newNode));
				int i = 0;
			}
			ret = CheckIfEnd(newNode, end);
		}
		else
			delete newNode;
	}
	else
		delete newNode;

	return ret;
}

bool j1PathFinding::AddChild(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal)
{
	bool ret = false;
	if (x >= 0 && y >= 0)
	{
		if (mapData->isWalkable(x, y))
		{
			ret = CreateSideNode(nParent, x, y, endTile, cost, isDiagonal);
		}
	}
	return ret;
}
bool j1PathFinding::AddChilds(p2List_item<node*>* nParent, iPoint end)
{
	if (AddChild(nParent->data, nParent->data->tile.x + 1, nParent->data->tile.y, endTile, 10, false))
		return true;
	if (AddChild(nParent->data, nParent->data->tile.x, nParent->data->tile.y + 1, endTile, 10, false))
		return true;
	if (AddChild(nParent->data, nParent->data->tile.x - 1, nParent->data->tile.y, endTile, 10, false))
		return true;
	if (AddChild(nParent->data, nParent->data->tile.x, nParent->data->tile.y - 1, endTile, 10, false))
		return true;
	if (allowDiagonals)
	{
		if (AddChild(nParent->data, nParent->data->tile.x + 1, nParent->data->tile.y + 1, endTile, 14, true))
			return true;
		if (AddChild(nParent->data, nParent->data->tile.x + 1, nParent->data->tile.y - 1, endTile, 14, true))
			return true;
		if (AddChild(nParent->data, nParent->data->tile.x - 1, nParent->data->tile.y + 1, endTile, 14, true))
			return true;
		if (AddChild(nParent->data, nParent->data->tile.x - 1, nParent->data->tile.y - 1, endTile, 14, true))
			return true;
	}

	//Move the parent to the closed list
	openList.transfer(nParent, closedList);

	return false;
}


//Check if the node is in the open list and leaves in the list the less cost value node
bool j1PathFinding::CheckIfExists(node* node)
{
	bool nodeExists = false;
	int nodeIndex;
	for (int i = 0; i < openList.count() && !nodeExists; i++)
	{
		if (openList[i]->tile.x == node->tile.x && openList[i]->tile.y == node->tile.y)
		{
			nodeExists = true;
			nodeIndex = i;
		}
	}

	if (nodeExists)
	{
		if (node->f < openList[nodeIndex]->f)
		{
			openList.del(openList.At(nodeIndex));
			openList.add(node);
		}
		else
		{
			return true;
		}

	}
	else
		openList.add(node);
	return false;
}

bool j1PathFinding::IsNodeClosed(node*  node)
{
	bool ret = false;
	for (int i = 0; i < closedList.count() && !ret; i++)
	{
		if (closedList[i]->tile.x == node->tile.x && closedList[i]->tile.y == node->tile.y)
		{
			ret = true;
		}
	}
	return ret;
}

bool j1PathFinding::CheckIfEnd(node* node, iPoint end)
{
	if (node->tile.x == endTile.x && node->tile.y == endTile.y)
	{
		goal = node;
		pathStarted = false;
		pathFinished = true;
		LOG("-- Pathfinding: path finished --");
		return true;
	}

	return false;
}

void j1PathFinding::FinishPathFinding(p2DynArray<iPoint>& pathRef)
{
	node* node;
	int i = 0;
	for (node = goal; node->parent; node = node->parent)
	{
		iPoint tile;
		LOG("-- Pathfinding: node %d: %d x, %d y", i, node->tile.x, node->tile.y);
		pathRef.PushBack(node->tile);
		tile = pathRef[path.Count() - 1];
		i++;
	}
}

bool j1PathFinding::map::isWalkable(int x, int y) const
{
	if (x < App->map->data.height && x >= 0 && y < App->map->data.height && y >= 0)
	{
		if (data[y*width + x] == 1)
			return true;
	}
	return false;
}