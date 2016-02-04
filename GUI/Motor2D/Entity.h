#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"

enum Entity_Directions
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT,
};

class Entity
{
	//Public methods
public:

	Entity::Entity();
	Entity::Entity(Entity* entity);
	Entity::~Entity();

	virtual bool Start();
	virtual bool Update(float dt);
	virtual bool Destroy();

	//Set methods
	void SetPosition(float, float);
	void SetPosition(fPoint);
	void SetHP(int);

	//Get methods
	fPoint GetPosition();
	int GetHP();

	//Private methods
private:


	//Public properties
public:


	//Private properties
protected:

	fPoint position;
	int HP;
};
#endif //__ENTITY_H