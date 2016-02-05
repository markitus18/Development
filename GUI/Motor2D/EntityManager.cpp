#include "j1App.h"
#include "j1Textures.h"
#include "Unit.h"
#include "j1Render.h"
#include "j1Input.h"
#include "EntityManager.h"

EntityManager::EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

EntityManager::~EntityManager()
{

}

bool EntityManager::Start()
{
	entity_tex = App->tex->Load("textures/unit.png");
	unit_base = App->tex->Load("textures/unit_base.png");
	path_tex = App->tex->Load("textures/path.png");

	for (uint i = 0; i < unitList.count(); i++)
	{
		unitList[i]->Start();
	}

	return true;
}

bool EntityManager::Update(float dt)
{
	ManageInput();

	p2List_item<Unit*>* item = NULL;
	item = unitList.start;

	while (item)
	{
		item->data->Update(dt);
		item = item->next;
	}
	
	/*
	p2List_item<Building*>* item = NULL;
	item = buildingList.start;

	while (item)
	{
	item->data->Update(dt);
	App->render->Blit(entity_tex, item->data->getPosition().x, item->data->getPosition().y, new SDL_Rect{ 0, 0, 20, 20 }, 1.0f, item->data->GetDirection());

	item = item->next;
	}
	*/

	ManageInput();
	return true;
}

bool EntityManager::PostUpdate(float dt)
{
	if (selectionRect.w != 0 || selectionRect.h != 0)
		App->render->DrawQuad(selectionRect, 255, 255, 255, 255, false, false);
	return true;
}
bool EntityManager::CleanUp()
{
	p2List_item<Unit*>* item = NULL;
	item = unitList.start;
	while (item)
	{
		delete item->data;
		item = item->next;
	}
	return true;
}

void EntityManager::ManageInput()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_UP)
	{
		if (unitList[0]->GetLevel() < 3)
			unitList[0]->SetLevel(unitList[0]->GetLevel() + 1);
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		App->input->GetMousePosition(selectionRect.x, selectionRect.y);
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		selectionRect.w = x - selectionRect.x;
		selectionRect.h = y - selectionRect.y;
	}
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		selectionRect.w = selectionRect.h = 0;
	}
}
bool EntityManager::addUnit(Unit& _unit)
{
	unitList.add(&_unit);
	return true;
}
