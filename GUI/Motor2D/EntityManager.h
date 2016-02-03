#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__


#include "j1Module.h"
#include "p2List.h"

struct SDL_Texture;
class Unit;

class EntityManager : public j1Module
{
public:
	EntityManager(bool);
	~EntityManager();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void ManageInput();

	bool addUnit(Unit& _unit);
	bool deleteUnit();

	//	bool addBuilding(Entity& _entity);
	//	bool deleteBuilding();
	//should be priv
	p2List<Unit*> unitList;

	SDL_Texture* entity_tex;
	SDL_Texture* unit_base;
	SDL_Texture* path_tex;
private:

	//p2List<Building*> buildingList;

};

#endif //_ENTITYMANAGER_H__