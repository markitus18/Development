#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2List.h"
#include "j1Console.h"

class j1PathFinding : public j1Module
{

	struct node
	{
		node* parent;
		iPoint tile;
		int f, g, h;
	};

	struct map
	{
		int width;
		int height;
		uint* data;

		bool isWalkable(int x, int y) const;
	};

public:
	j1PathFinding();

	//Destructor
	virtual ~j1PathFinding();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();


	p2List_item<node*>*  GetLowestF() const;

	void AutomaticPath();
	bool StepUp();

	void FindPath();
	bool LoadMapData();
	bool IfPathPossible();
	bool StartPathFinding();
	bool CreateFirstNode();
	//Adding new nodes methods
	bool CreateSideNode(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal);
	bool AddChild(node* nParent, int x, int y, iPoint end, int cost, bool isDiagonal);
	bool AddChilds(p2List_item<node*>* nParent, iPoint end);

	bool IsNodeClosed(node* node);
	bool CheckIfExists(node* node);
	bool CheckIfEnd(node* node, iPoint end);
	void FinishPathFinding(p2DynArray<iPoint>& pathRef);

private:

	int				lowestF;
	bool			newLowest = false;
	node*			goal;
	node*			lastParent;
	p2List_item<node*>* lowestFNode;
	MapLayer*		layer;

public:
	p2List<node*>	openList;
	p2List<node*>	closedList;
	map*		mapData;
	bool		startTileExists = false;
	bool		endTileExists = false;
	iPoint		startTile;
	iPoint		endTile;
	bool		allowDiagonals = false;
	bool		allowCorners = false;
	bool		mapChanged = false;
	bool		automaticIteration = true;
	bool		pathStarted = false;
	bool		pathFinished = false;

	bool		editMode = false;

	p2DynArray<iPoint> path;

#pragma region Commands
	struct C_Path_Corners: public Command
	{
		C_Path_Corners() : Command("pathfinding_corner", "Enable / Disable pathfinding corners", 1, "pf_c"){}
		void function(const p2DynArray<p2SString>* arg)
		{
			p2SString str = arg->At(1)->GetString();
			if (str == "enable")
			{
				App->pathFinding->allowCorners = true;
				LOG("-- Pathfinding: Corners enabled --");
			}
			else if (str == "disable")
			{
				App->pathFinding->allowCorners = false;
				LOG("-- Pathfinding: Corners disabled --");
			}
			else
				LOG("pathfinding_corner: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());

		}
	};
	C_Path_Corners c_Path_Corners;

	struct C_Path_Diag : public Command
	{
		C_Path_Diag() : Command("pathfinding_diag", "Enable / Disable pathfinding diagonals", 1, "pf_d"){}
		void function(const p2DynArray<p2SString>* arg)
		{
			p2SString str = arg->At(1)->GetString();
			if (str == "enable")
			{
				App->pathFinding->allowDiagonals = true;
				LOG("-- Pathfinding: Diagonals enabled --");
			}
			else if (str == "disable")
			{
				App->pathFinding->allowDiagonals = false;
				LOG("-- Pathfinding: Diagonals disabled --");
			}
			else
				LOG("pathfinding_diag: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());

		}
	};
	C_Path_Diag c_Path_Diag;

	struct C_Path_EditMode : public Command
	{
		C_Path_EditMode() : Command("map_edit", "Enable / Disable  map edit", 1, "pf_e"){}
		void function(const p2DynArray<p2SString>* arg)
		{
			p2SString str = arg->At(1)->GetString();
			if (str == "enable")
			{
				App->pathFinding->editMode = true;
				LOG("-- Map: Map edition enabled  --");
			}
			else if (str == "disable")
			{
				App->pathFinding->editMode = false;
				LOG("-- Map: Map edition disabled --");
			}
			else
				LOG("map_edit: unexpected command '%s', expecting enable / disable", arg->At(1)->GetString());

		}
	};
	C_Path_EditMode c_Path_EditMode;
#pragma endregion
};
#endif // __j1PATHFINDING_H__