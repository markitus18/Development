#include "Entity.h"

Entity::Entity()
{

}

Entity::Entity(Entity* entity)
{

}

Entity::~Entity(){}

bool Entity::Start()
{

	return true;
}

bool Entity::Update(float dt)
{
	return true;
}

bool Entity::Destroy()
{

	return true;
}

//Setters
void Entity::setPosition(float x, float y)
{
	position.x = x;
	position.y = y;
}
void Entity::setPosition(fPoint _position)
{
	position = _position;
}
void Entity::setHP(int _HP)
{

}

fPoint Entity::getPosition()
{
	return position;
}

int Entity::getHP()
{
	return HP;
}

