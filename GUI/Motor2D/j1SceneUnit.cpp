#include "p2Defs.h"
#include "p2Log.h"
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
#include "j1Gui.h"
#include "UIElements.h"


j1SceneUnit::j1SceneUnit(bool start_enabled) : j1Module(start_enabled)
{
	name.create("scene_unit");
}

// Destructor
j1SceneUnit::~j1SceneUnit()
{}

// Called before render is available
bool j1SceneUnit::Awake(pugi::xml_node& node)
{
	LOG("Loading Scene");
	App->SetCurrentScene(this);
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

	//bar test
	UIRect* rect1 = App->gui->CreateRect("testRect1", { 0, 100, 150, 20 }, 0, 0, 0);
	UIRect* rect2 = App->gui->CreateRect("testRect1", { 5, 105, 140, 10 }, 255, 0, 0);
	testBar = App->gui->CreateBar("testBar", (UIElement*)rect1, (UIElement*)rect2, &testInt, &currTestInt);
	testBar->SetIgnoreCamera();
	return true;
}

// Called each loop iteration
bool j1SceneUnit::PreUpdate()
{
	//Getting current mouse tile
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	currentTile_x = p.x;
	currentTile_y = p.y;
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x - 32, p.y - 16, new SDL_Rect{ 0, 0, 64, 32 });
	return true;
}

// Called each loop iteration
bool j1SceneUnit::Update(float dt)
{
	ManageInput(dt);
	
	App->map->Draw();

	//Drawing point 0, 0
	if (renderForces)
	{
		App->render->DrawCircle(0, 0, 10, 255, 255, 255);
		App->render->DrawLine(0, -20, 0, 20, 255, 0, 0);
		App->render->DrawLine(-20, 0, 20, 0, 255, 0, 0);
		// --- TO CHANGE: clean drawing start/end tiles and current tile ---
		//Drawing start and end
		iPoint startPosition = App->map->MapToWorld(App->pathFinding->startTile.x, App->pathFinding->startTile.y);
		iPoint endPosition = App->map->MapToWorld(App->pathFinding->endTile.x, App->pathFinding->endTile.y);
		if (App->pathFinding->startTileExists)
			App->render->Blit(App->map->data.tilesets.start->next->data->texture, startPosition.x - 32, startPosition.y - 16, new SDL_Rect{ 0, 32, 64, 32 });
		if (App->pathFinding->endTileExists)
			App->render->Blit(App->map->data.tilesets.start->next->data->texture, endPosition.x - 32, endPosition.y - 16, new SDL_Rect{ 64, 32, 64, 32 });
	}

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

void j1SceneUnit::ManageInput(float dt)
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		currTestInt += 5;
		p2DynArray<iPoint> newPath;
		fPoint unitPos = unit->GetPosition();
		iPoint unitTile = App->map->WorldToMap(unitPos.x, unitPos.y);
		iPoint dstTile = { currentTile_x, currentTile_y };
		bool ret = App->pathFinding->GetNewPath(unitTile, dstTile, newPath);
		if (ret)
		{
			unit->SetNewPath(newPath);
		}

		if (entityType < 3)
			entityType++;
		else
			entityType = 0;
		switch (entityType)
		{ 
			case 0:
				unit->SetType(RED);
				break;
			case 1:
				unit->SetType(YELLOW);
				break;
			case 2:
				unit->SetType(GREEN);
				break;
			case 3:
				unit->SetType(BLUE);
				break;
		}

	}

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += (int)(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= (int)(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += (int)(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= (int)(200.0f * dt);


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

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);

		LOG("Mouse Position: %i, %i", p.x, p.y);
		LOG("Current tile: %i, %i", currentTile_x, currentTile_y);
		p = App->map->MapToWorld(currentTile_x, currentTile_y);
		LOG("Draw Pos :%i, %i", p.x, p.y);
	}
}