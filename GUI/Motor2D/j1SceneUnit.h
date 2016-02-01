#ifndef __j1SceneUnit_H__
#define __j1SceneUnit_H__

#include "j1Module.h"
#include "Unit.h"

struct SDL_Texture;

class j1SceneUnit : public j1Module
{
public:

	j1SceneUnit(bool);

	// Destructor
	virtual ~j1SceneUnit();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	int currentTile_x;
	int currentTile_y;

	bool startTileExists;
	bool endTileExists;
	iPoint startTile;
	iPoint endTile;

	bool renderInstructions = false;
	bool renderGrid = false;
	bool renderForces = true;

private:
	SDL_Texture* debug_tex;
	SDL_Texture* instructions_title;
	SDL_Texture* instructions;
	SDL_Texture* entity_tex;
	SDL_Texture* target_tex;
	SDL_Texture* grid_tex;

	Unit* unit;
	int entityTargetIndex;
	p2DynArray<iPoint> targetList;
};

#endif // __j1SceneUnit_H__