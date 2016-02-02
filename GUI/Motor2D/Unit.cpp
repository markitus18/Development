#include "Unit.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1SceneUnit.h"

//Include map / pathfinding / render????

Unit::Unit()
{}
Unit::Unit(int x, int y)
{
	position.x = (float)x;
	position.y = (float)y;
}
Unit::~Unit()
{}

bool Unit::Start()
{

	return true;
}
bool Unit::Update(float dt)
{
	if (targetChange)
	{
		UpdateVelocity(dt);
		position.x += currentVelocity.x;
		position.y += currentVelocity.y;
		return false;
	}
	else
	{
		if (behaviour == PATROL)
		{
			SetTarget(rand() % 64, rand() % 64);
		}
	}
	if (path.Count() > 0)
	{
		for (size_t i = 0; i < path.Count(); i++)
		{
		//	iPoint position = App->map->MapToWorld(path[i].x, path[i].y);
		//	App->render->Blit(App->sceneUnit->debug_tex, position.x, position.y, new SDL_Rect{ 0, 0, 64, 64 });
		}
	}
	return true;
}

void Unit::UpdateVelocity(float dt)
{
	desiredVelocity = GetDesiredVelocity();
	//	steeringVelocity = GetSteeringVelocity();
	currentVelocity = GetcurrentVelocity(dt);
}

//Get the desired velocity: target position - entity position
p2Vec2<float> Unit::GetDesiredVelocity()
{
	p2Vec2<float> velocity;
	velocity.position = position;
	velocity.x = target.x - position.x;
	velocity.y = target.y - position.y;

	float distance = velocity.GetModule();

	if (distance < slowingRadius)
	{
		//targetChange = false;
		velocity.Normalize();
		velocity *= maxSpeed* (distance / (slowingRadius));

		if (abs(velocity.x) + abs(velocity.y) < 0.05f)
		{
			velocity.SetToZero();
			currentVelocity.SetToZero();
			position = target;
			targetChange = false;
		}

	}
	else
	{
		velocity.Normalize();
		velocity *= maxSpeed;
	}

	return velocity;
}

//Get the steering velocity: current velocity - desired velocity
/*
p2Vec2<float> Entity::GetSteeringVelocity()
{
p2Vec2<float> velocity;
velocity = desiredVelocity - currentVelocity;
velocity /= maxForce;
if (behaviour == RUN)
{
velocity.Negate();
}
return velocity;
}
*/
p2Vec2<float> Unit::GetcurrentVelocity(float dt)
{
	p2Vec2<float> velocity;
	velocity = desiredVelocity;//currentVelocity + steeringVelocity;
	velocity.position = position;
	velocity *= 300.0f * dt;
	/*
	if (velocity.IsOpposite(desiredVelocity))
	{
	velocity.x += 0.1f;
	velocity.y += 0.1f;
	}
	*/
	return velocity;
}

void Unit::SetTarget(int x, int y)
{
	target.x = (float)x;
	target.y = (float)y;
	targetChange = true;
}

void Unit::SetBehaviour(Behaviour _behaviour)
{
	behaviour = _behaviour;
}

void Unit::SetType(UnitType _type)
{
	type = _type;
}

void Unit::SetLevel(int _level)
{
	level = _level;
}

void Unit::SetMaxSpeed(int speed)
{
	maxSpeed = speed;
}

float Unit::GetSlowRad()
{
	return slowingRadius;
}

float Unit::GetDirection()
{
	return currentVelocity.GetAngle();
}

UnitType Unit::GetType()
{
	return type;
}

int Unit::GetLevel()
{
	return level;
}

void Unit::GetNewPath(int tileX, int tileY)
{
	//iPoint tilePos = App->map->WorldToMap(position.x, position.y);
	//App->pathFinding->GetNewPath(iPoint(0, 0), iPoint(tileX, tileY), path);
}