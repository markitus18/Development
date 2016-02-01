#ifndef __UNIT_H__
#define __UNIT_H__

#include "p2Point.h"
#include "p2Vec2.h"
#include "p2DynArray.h"

#include "Entity.h"

enum Behaviour
{
	ATTACK = 0,
	RUN,
	PATROL,
};

enum UnitType
{
	RED = 0,
	GREEN,
	BLUE,
	YELLOW,
};

class Unit : public Entity
{



public:
	Unit();
	Unit(int x, int y);
	~Unit();

	bool Start();
	bool Update(float dt);

	void UpdateVelocity(float dt);
	p2Vec2<float> GetDesiredVelocity();
	p2Vec2<float> GetSteeringVelocity();
	p2Vec2<float> GetcurrentVelocity(float dt);

	void SetTarget(int x, int y);
	void SetBehaviour(Behaviour _behaviour);
	void SetType(UnitType _type);
	void SetLevel(int _level);

	fPoint GetPosition();
	float GetDirection();
	float GetSlowRad();
	UnitType GetType();
	int GetLevel();

private:
	Behaviour behaviour = PATROL;
	UnitType type = RED;
	int level = 1;

	float maxSpeed = 0.5f;
	float maxForce = 50.0f;
	float slowingRadius = 5.0f;

public:
	fPoint target;
	bool targetChange;
	p2Vec2<float> currentVelocity;
	p2Vec2<float> steeringVelocity;
	p2Vec2<float> desiredVelocity;



};

#endif //__UNIT_H__