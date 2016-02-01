#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"

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
	void setPosition(float, float);
	void setPosition(fPoint);
	void setHP(int);

	//Get methods
	fPoint getPosition();
	int getHP();

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