#include <stdlib.h>

#include "Unit.h"

#include "j1Render.h"
#include "j1App.h"
#include "j1Map.h"
#include "EntityManager.h"
#include "j1SceneUnit.h"

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
	}
	else
	{
		GetNewTarget();
	}
	if (path.Count() > 0)
	{
		for (uint i = 0; i < path.Count(); i++)
		{
			iPoint position = App->map->MapToWorld(path[i].x, path[i].y);
			SDL_Rect rect = { 0, 0, 64, 32 };
			if (i < currentNode)
				rect = { 0, 64, 64, 32 };
			App->render->Blit(App->entityManager->path_tex, position.x - 32, position.y - 16, &rect);
		}
	}
	Draw();
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
		velocity.SetToZero();
		currentVelocity.SetToZero();
		position = target;
		targetChange = false;
		GetNewTarget();
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

void Unit::GetNewTarget()
{
	if (currentNode + 1 < path.Count())
	{
		currentNode++;
		iPoint newPos = App->map->MapToWorld(path[currentNode].x, path[currentNode].y);
		SetTarget(newPos.x, newPos.y);
	}
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

void Unit::SetNewPath(p2DynArray<iPoint>& newPath)
{
	path.Clear();
	path += newPath;
	targetChange = true;
	currentNode = -1;
	GetNewTarget();
}

void Unit::Draw()
{
	App->render->Blit(App->entityManager->unit_base, position.x - 32, position.y - 16);
	App->render->Blit(App->entityManager->entity_tex, position.x - 32, position.y - 55, new SDL_Rect{ 0 + 70 * (level - 1), 0 + 70 * type, 65, 70 });// , 1.0f, GetDirection());

	if (App->sceneUnit->renderForces)
		DrawDebug();
}

void Unit::DrawDebug()
{
	float lineX1, lineX2, lineY1, lineY2;
	/*
	p2Vec2<float> line = currentVelocity;
	line.Normalize();
	line *= 3;
	lineX1 = line.position.x + App->render->camera.x;
	lineY1 = line.position.y + App->render->camera.y;
	lineX2 = (line.x * 30 + lineX1);
	lineY2 = (line.y * 30 + lineY1);
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, 255, 255, 255);
	*/	
	p2Vec2<float> line1 = currentVelocity;
	line1 = desiredVelocity;
	line1.Normalize();
	line1 *= 3;
	lineX1 = line1.position.x + App->render->camera.x;
	lineY1 = line1.position.y + App->render->camera.y;
	lineX2 = (line1.x * 30 + lineX1);
	lineY2 = (line1.y * 30 + lineY1);
	App->render->DrawLine((int)lineX1, (int)lineY1, (int)lineX2, (int)lineY2, 0, 255, 0);

	App->render->DrawCircle((int)target.x, (int)target.y, (int)GetSlowRad(), 255, 255, 255);
	App->render->DrawCircle(position.x, position.y, 10, 255, 255, 255, 255);

}