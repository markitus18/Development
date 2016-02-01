#include "entityManager.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Input.h"

EntityManager::EntityManager(bool start_enabled) : j1Module(start_enabled)
{

}

EntityManager::~EntityManager()
{

}

bool EntityManager::Start()
{

	entity_tex = App->tex->Load("textures/entity.png");

	Unit* unit = new Unit;
	unit->setPosition(10, 10);
	unit->setHP(0);
	unit->SetBehaviour(PATROL);
	unit->SetType(RED);
	addUnit(*unit);

	Unit* unit1 = new Unit;
	unit1->setPosition(400, 400);
	unit1->setHP(0);
	unit->SetBehaviour(PATROL);
	unit1->SetType(GREEN);
	addUnit(*unit1);

	Unit* unit2 = new Unit;
	unit2->setPosition(1000, 500);
	unit2->setHP(0);
	unit->SetBehaviour(PATROL);
	unit2->SetType(BLUE);
	addUnit(*unit2);

	Unit* unit3 = new Unit;
	unit3->setPosition(800, 50);
	unit3->setHP(0);
	unit->SetBehaviour(PATROL);
	unit3->SetType(YELLOW);
	addUnit(*unit3);

	return true;
}

bool EntityManager::Update(float dt)
{
	p2List_item<Unit*>* item = NULL;
	item = unitList.start;

	while (item)
	{
		//		item->data->SetTarget(App->map->MapToWorld(App->scene->currentTile_x, App->scene->currentTile_y).x + 16, App->map->MapToWorld(App->scene->currentTile_x, App->scene->currentTile_y).y + 16);
		item->data->Update(dt);
		App->render->Blit(entity_tex, (int)item->data->getPosition().x - 10, (int)item->data->getPosition().y - 10, new SDL_Rect{ 0 + 20 * (item->data->GetLevel() - 1), 0 + 20 * item->data->GetType(), 20, 20 }/*, 1.0f, item->data->GetDirection()*/);

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

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
	{
		if (unitList[1]->GetLevel() < 3)
			unitList[1]->SetLevel(unitList[1]->GetLevel() + 1);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_UP)
	{
		if (unitList[2]->GetLevel() < 3)
			unitList[2]->SetLevel(unitList[2]->GetLevel() + 1);
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_UP)
	{
		if (unitList[3]->GetLevel() < 3)
			unitList[3]->SetLevel(unitList[3]->GetLevel() + 1);
	}
}
bool EntityManager::addUnit(Unit& _unit)
{
	unitList.add(&_unit);
	return true;
}
