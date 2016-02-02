#include "p2Defs.h"
#include "p2Log.h"
#include "p2Vec2.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1SceneUnit.h"
#include "j1PathFinding.h"
#include "EntityManager.h"

j1SceneUnit::j1SceneUnit(bool start_enabled) : j1Module(start_enabled)
{
	name.create("scene_unit");
}

// Destructor
j1SceneUnit::~j1SceneUnit()
{}

// Called before render is available
bool j1SceneUnit::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1SceneUnit::Start()
{
	App->map->Load("iso.tmx");
	if (App->map->data.type == MAPTYPE_ORTHOGONAL)
		debug_tex = App->tex->Load("textures/current_tile.png");
	else
		debug_tex = App->tex->Load("textures/path.png");

	instructions = App->tex->Load("textures/instructions.png");
	instructions_title = App->tex->Load("textures/instructions_title.png");
	grid_tex = App->tex->Load("textures/grid.png");

	unit = new Unit;
	iPoint unitPos = App->map->MapToWorld(3, 5);
	unit->SetPosition(unitPos.x, unitPos.y);
	unit->SetHP(0);
	unit->SetBehaviour(RUN);
	unit->SetType(RED);
	App->entityManager->addUnit(*unit);

	/*
	unit = new Unit(0, 0);

	targetList.PushBack(iPoint{ App->map->MapToWorld(12, 4).x - 16, App->map->MapToWorld(12, 4).y - 16 });
	targetList.PushBack(iPoint{ App->map->MapToWorld(12, 10).x - 16, App->map->MapToWorld(12, 10).y - 16 });
	targetList.PushBack(iPoint{ App->map->MapToWorld(20, 10).x - 16, App->map->MapToWorld(20, 10).y - 16 });
	targetList.PushBack(iPoint{ App->map->MapToWorld(20, 4).x - 16, App->map->MapToWorld(20, 4).y - 16 });

	unit->SetTarget(targetList[0].x, targetList[0].y);
	*/
	return true;
}

// Called each loop iteration
bool j1SceneUnit::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1SceneUnit::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += (int)(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= (int)(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += (int)(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= (int)(200.0f * dt);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		iPoint target = App->map->MapToWorld(currentTile_x, currentTile_y);
		unit->SetTarget(target.x, target.y);
	}
	//change start tile
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_UP)
	{
		if (App->map->data.type == (MAPTYPE_ISOMETRIC))
		{
			if (App->pathFinding->startTile.x != currentTile_x || App->pathFinding->startTile.y != currentTile_y)
			{
				App->pathFinding->startTile.x = currentTile_x;
				App->pathFinding->startTile.y = currentTile_y;
				App->pathFinding->startTileExists = true;
			}
			else
			{
				if (!App->pathFinding->startTileExists)
					App->pathFinding->startTileExists = true;
				else
					App->pathFinding->startTileExists = false;
			}
		}
	}
	//Change end tile
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
	{
		if (App->map->data.type == (MAPTYPE_ISOMETRIC))
		{
			if (App->pathFinding->endTile.x != currentTile_x || App->pathFinding->endTile.y != currentTile_y)
			{
				App->pathFinding->endTile.x = currentTile_x;
				App->pathFinding->endTile.y = currentTile_y;
				App->pathFinding->endTileExists = true;
			}
			else
			{
				if (!App->pathFinding->endTileExists)
					App->pathFinding->endTileExists = true;
				else
					App->pathFinding->endTileExists = false;
			}
		}

	}

	//Enable / Disable instructions
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_UP)
	{
		renderInstructions = !renderInstructions;
	}
	//Enable / Disable grid
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_UP)
	{
		renderGrid = !renderGrid;
	}
	//Enable / Disable forces debug
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_UP)
	{
		renderForces = !renderForces;
	}

	//Change target entity
	/*
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint pos = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	pos = App->map->MapToWorld(pos.x, pos.y);
	unit->SetTarget(pos.x + 16, pos.y + 16);
	}
	*/
	App->map->Draw();
	// --- TO CHANGE: clean drawing path debugging --
	if (App->pathFinding->pathFinished)
	{
		for (size_t i = 0; i < App->pathFinding->path.Count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->path[i].x, App->pathFinding->path[i].y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 0, 64, 64 });
		}
	}

	else if (App->pathFinding->pathStarted)
	{
		for (size_t i = 0; i < App->pathFinding->openList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->openList[i]->tile.x, App->pathFinding->openList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 0, 64, 64 });
		}
		for (size_t i = 0; i < App->pathFinding->closedList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->closedList[i]->tile.x, App->pathFinding->closedList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 64, 64, 64 });
		}
	}


	// --- TO CHANGE: clean drawing start/end tiles and current tile ---
	//Drawing start and end
	iPoint startPosition = App->map->MapToWorld(App->pathFinding->startTile.x, App->pathFinding->startTile.y);
	iPoint endPosition = App->map->MapToWorld(App->pathFinding->endTile.x, App->pathFinding->endTile.y);
	if (App->pathFinding->startTileExists)
		if (App->map->data.tilesets.start->next)
			App->render->Blit(App->map->data.tilesets.start->next->data->texture, startPosition.x, startPosition.y, new SDL_Rect{ 0, 64, 64, 64 });
	if (App->pathFinding->endTileExists)
		if (App->map->data.tilesets.start->next)
			App->render->Blit(App->map->data.tilesets.start->next->data->texture, endPosition.x, endPosition.y, new SDL_Rect{ 64, 64, 64, 64 });
	//Getting current mouse tile
	int x, y;
	App->input->GetMousePosition(x, y);
	int mapX, mapY;
	mapX = x - App->render->camera.x;
	mapY = y - App->render->camera.y;
	iPoint map_coordinates = App->map->WorldToMap(mapX, mapY);

	iPoint currentTile = App->map->MapToWorld(map_coordinates.x, map_coordinates.y);
	currentTile_x = map_coordinates.x;
	currentTile_y = map_coordinates.y;

	//Rendering current tile
	if (App->map->data.type == (MAPTYPE_ISOMETRIC))
		App->render->Blit(debug_tex, currentTile.x, currentTile.y, new SDL_Rect{ 0, 0, 64, 64 });
	else
		App->render->Blit(debug_tex, currentTile.x, currentTile.y, new SDL_Rect{ 0, 0, 32, 32 });

	//Rendering instructions
	if (renderInstructions)
	{
		App->render->Blit(instructions, 1024 - 250 - App->render->camera.x, 768 - 340 - App->render->camera.y, new SDL_Rect{ 0, 0, 250, 340 });
	}
	else
	{
		App->render->Blit(instructions_title, 1024 - 250 - App->render->camera.x, 768 - 89 - App->render->camera.y, new SDL_Rect{ 0, 0, 250, 89 });
	}

	//Rendering grid
	if (renderGrid)
	{
		App->render->Blit(grid_tex, 0, 0, new SDL_Rect{ 0, 0, 1600, 480 });
	}

	//Printing title
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d Draw:%d, %d",
		App->map->data.width, App->map->data.height,
		App->map->data.tile_width, App->map->data.tile_height,
		App->map->data.tilesets.count(),
		map_coordinates.x, map_coordinates.y,
		currentTile.x, currentTile.y);

	//	App->win->SetTitle(title.GetString());
	
	//Drawing line testing
	if (renderForces)
	{
	SDL_SetRenderDrawColor(App->render->renderer, 0, 255, 255, 255);
	p2Vec2<float> line = unit->currentVelocity;
	line.Normalize();
	line *= 3;
	float lineX1 = line.position.x + App->render->camera.x;
	float lineY1 = line.position.y + App->render->camera.y;
	float lineX2 = (line.x * 30 + lineX1);
	float lineY2 = (line.y * 30 + lineY1);
	SDL_RenderDrawLine(App->render->renderer, (int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2);

	SDL_SetRenderDrawColor(App->render->renderer, 0, 255, 0, 255);
	p2Vec2<float> line1 = unit->currentVelocity;
	line1 = unit->desiredVelocity;
	line1.Normalize();
	line1 *= 3;
	lineX1 = line1.position.x + App->render->camera.x;
	lineY1 = line1.position.y + App->render->camera.y;
	lineX2 = (line1.x * 30 + lineX1);
	lineY2 = (line1.y * 30 + lineY1);
	SDL_RenderDrawLine(App->render->renderer, (int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2);
	
	for (size_t i = 0; i < targetList.Count(); i++)
	{
	App->render->Blit(target_tex, (int)targetList[i].x - 13, (int)targetList[i].y - 13, new SDL_Rect{ 0, 0, 26, 26 });
	}
	App->render->DrawCircle((int)unit->target.x + App->render->camera.x, (int)unit->target.y + App->render->camera.y, (int)unit->GetSlowRad(), 255, 255, 255);
	}
	
	//App->render->Blit(entity_tex, (int)unit->GetPosition().x - 10, (int)unit->GetPosition().y - 10, new SDL_Rect{ 0, 0, 20, 20 }, 1.0f, unit->GetDirection());

	//	App->render->Blit(target_tex, (int)entity->target.x - 13, (int)entity->target.y - 13, new SDL_Rect{ 0, 0, 26, 26 });


	//	App->entityManager->unitList[0]->SetTarget(App->map->MapToWorld(currentTile_x, currentTile_y).x + 16, App->map->MapToWorld(currentTile_x, currentTile_y).y + 16);

	//Update entity
	/*
	if (unit->Update(dt))
	{
	if ((entityTargetIndex) < targetList.Count() - 1)
	entityTargetIndex++;
	else
	entityTargetIndex = 0;
	unit->SetTarget(targetList[entityTargetIndex].x, targetList[entityTargetIndex].y);
	}
	*/
	return true;
}

// Called each loop iteration
bool j1SceneUnit::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1SceneUnit::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
