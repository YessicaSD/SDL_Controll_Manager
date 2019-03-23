#ifndef __DYNAMIC_ENTITY_H_
#define __DYNAMIC_ENTITY_H_
#include "j1Entity.h"
#include "Animation.h"
#include "j1Movement.h"
#include "j1Timer.h"
#include "CollidersFuntions.h"
#include <vector>

struct SDL_Color;
struct SingleUnit;

#define MAX_UNIT_PRIORITY 8
enum Unit_Direction
{
	UnitDirection_Up,
	UnitDirection_Down,
	UnitDirection_Left,
	UnitDirection_Right,
	UnitDirection_UpLeft,
	UnitDirection_UpRight,
	UnitDirection_DownLeft,
	UnitDirection_DownRight,
	UnitDirection_Idle,
};

enum Unit_State
{
	idle,
	waiting,
	getPath,
	IncrementWaypoint,
	followPath,
	max_state
};

enum CollisionType {
	CollisionType_NoCollision,
	CollisionType_SameCell,
	CollisionType_TowardsCell,
	CollisionType_ItsCell,
	CollisionType_DiagonalCrossing
};

class Unit: public j1Entity
{
private:
	SingleUnit * singleUnit = nullptr;
	
	Unit_Direction state_diretion;

	fPoint speed;
	fPoint direction;
	
	

public:
	std::vector<iPoint> Path;
	Circle posCollider;
	Unit_State state = idle;
	CollisionType ColliderState = CollisionType_NoCollision;
	Unit* waitingUnit = nullptr;
	Unit(fPoint position, SDL_Texture * tex, entities_types type);
	~Unit()
	{
		if (singleUnit != nullptr)
			delete singleUnit;
	}
	
	void Draw(float dt) override;
	void Move(float dt) override;
	void DebugDraw() override;
	iPoint goal;
	iPoint next_Goal;
	j1Timer waitTime;
	void OnCollision(Collider* c1, Collider* c2);
	void UnitStateMachine(float dt);
	void SetUnitDirection(Unit_Direction unitDirection);
	Unit_Direction GetUnitDirection();
	void SetUnitDirectionByValue(fPoint unitDirection);
	fPoint GetUnitDirectionByValue() const;

	SingleUnit* GetSingleUnit() const;
	bool Unit::MoveOfTheWayOf(Unit* u);

	void SetToWaiting(Unit* waitU);
};

#endif // __DYNAMIC_ENTITY_H_