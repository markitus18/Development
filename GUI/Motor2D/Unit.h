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
	PURPLE,
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
	bool Unit::GetDesiredVelocity(p2Vec2<float>& newDesiredVelocity);
	p2Vec2<float> GetSteeringVelocity();
	p2Vec2<float> GetcurrentVelocity(float dt);

	bool GetNewTarget();

	void SetTarget(int x, int y);
	void SetNewPath(p2DynArray<iPoint>& newPath);
	void SetBehaviour(Behaviour _behaviour);
	void SetType(UnitType _type);
	void SetLevel(int _level);
	void SetMaxSpeed(int speed);

	Entity_Directions GetDirection();
	float GetSlowRad();
	UnitType GetType();
	int GetLevel();

	void Draw();
	void DrawDebug();
private:
	Behaviour behaviour = PATROL;
	UnitType type = RED;
	int level = 1;


	float maxSpeed = 0.5f;
	float maxForce = 50.0f;
	float slowingRadius = 5.0f;

	p2DynArray<iPoint> path;
	fPoint target;
	bool targetChange = false;
	int currentNode = 0;
	p2Vec2<float> currentVelocity = { 0, 0 };
	p2Vec2<float> steeringVelocity = { 0, 0 };
	p2Vec2<float> desiredVelocity = { 0, 0 };
public:

	//Move some to private (or all)

	 


};

#endif //__UNIT_H__