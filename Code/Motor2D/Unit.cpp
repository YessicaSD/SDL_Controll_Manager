#include "Unit.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1EntityManager.h"
#include "j1Input.h"
#include "p2Log.h"
//#include "SDL\include\SDL_rect.h
Unit::Unit(fPoint position, SDL_Texture * tex, entities_types type):j1Entity(position,tex,type)
{
	posCollider.radius = 5;
	posCollider.center = position;
}
void Unit::Draw(float dt)
{
	SDL_Color color;
	if (selected)
	{
		color = { 255, 69, 25, 255 };
	}
	else
	{
		color = { 178, 34, 0, 255 };
	}
	SDL_Rect frame = { position.x-5,position.y-5,10,10 };
	App->render->DrawQuad(frame, color.r, color.g, color.b, color.a, true);
	App->render->DrawCircle(position.x, position.y, posCollider.radius, 255, 255, 255, 255);
	
}

void Unit::Move(float dt)
{
	//TilePos = App->map->WorldToMap((int)position.x, (int)position.y);
	UnitStateMachine(dt);
}

void Unit::DebugDraw()
{
	if (Path.size() > 0)
	{
		for (std::vector<iPoint>::iterator iter = Path.begin(); iter != Path.end(); ++iter)
		{
			SDL_Rect tile = { 0,0,10,10 };
			iPoint pos = App->map->MapToWorld((*iter).x, (*iter).y);
			tile.x = pos.x;
			tile.y = pos.y;

			App->render->DrawQuad(tile, 50, 100, 0, 200);
		}

	}
}

void Unit::OnCollision(Collider * c1, Collider * c2)
{
}
bool Unit::MoveOfTheWayOf(Unit* u)
{
	
	iPoint neightBoar[4];
	neightBoar[UnitDirection_Up] = { TilePos.x,TilePos.y - 1 }; //up
	neightBoar[UnitDirection_Down] = { TilePos.x,TilePos.y + 1 }; //down
	neightBoar[UnitDirection_Left] = { TilePos.x - 1,TilePos.y }; //left
	neightBoar[UnitDirection_Right] = { TilePos.x + 1,TilePos.y }; //right

	for (uint i = 0; i < 4; ++i)
	{
		if (App->pathfinding->IsWalkable(neightBoar[i])!=true)
		{
			neightBoar[i].SetToZero();
		}
	}
	//x axis --------------------------------------
	if (u->state_diretion == UnitDirection_Left || u->state_diretion == UnitDirection_Right)
	{
		if (neightBoar[UnitDirection_Up].IsZero() != true
			&& App->entities->InThisTile_IsUnits(neightBoar[UnitDirection_Up]) == nullptr)
		{
			state = getPath;
			goal = neightBoar[UnitDirection_Up];
			return true;
		}


		else if (neightBoar[UnitDirection_Down].IsZero() != true
			&& App->entities->InThisTile_IsUnits(neightBoar[UnitDirection_Down]) == nullptr)
		{
			state = getPath;
			goal = neightBoar[UnitDirection_Down];
			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		if (neightBoar[UnitDirection_Left].IsZero() != true
			&& App->entities->InThisTile_IsUnits(neightBoar[UnitDirection_Left]) == nullptr)
		{
			state = getPath;
			goal = neightBoar[UnitDirection_Left];
			return true;
		}

		else if (neightBoar[UnitDirection_Right].IsZero() != true
			&& App->entities->InThisTile_IsUnits(neightBoar[UnitDirection_Left]) == nullptr)
		{
			state = getPath;
			goal = neightBoar[UnitDirection_Right];
			return true;
		}

		else
		{
			return false;
		}
	}

	
}

void Unit::SetToWaiting(Unit * waitU)
{
	state = waiting;
	waitingUnit = waitU;
}

void Unit::UnitStateMachine(float dt)
{
	switch (state)
	{
	case idle:
		break;
	case waiting:
	{
		if (next_Goal != waitingUnit->next_Goal && next_Goal != waitingUnit->position.ReturniPoint())
		{
			if (Path.size() > 0)
				state = followPath;
			else
				state = getPath;
		}
	}
		break;
	case getPath:
	{
		int result = App->pathfinding->CreatePath(TilePos, goal);
		if (result != -1)
		{
			Path.clear();
			Path = *App->pathfinding->GetLastPath();
			state = IncrementWaypoint;
			Path.erase(Path.begin());
			
		}
		else
		{
			state = idle;
		}
	}

	break;
	case followPath:
	{
		if (Path.size() > 0)
		{

			iPoint world_next_pos = App->map->MapToWorld(next_Goal.x, next_Goal.y);
			fPoint aux = { (float)world_next_pos.x + App->map->data.tile_width*0.5F,(float)world_next_pos.y + App->map->data.tile_height*0.5F };


			speed = aux - position;
			iPoint int_pos = { (int)position.x,(int)position.y };
			//Check if we are on the center of the tile
			if (posCollider.IsPointIn(aux))
			{
				Path.erase(Path.begin());
				TilePos = next_Goal;
				state = IncrementWaypoint;
			}

		}
		else
		{
			state = idle;
		}
		iPoint aux = { (int)(speed.x*dt * 20),(int)(speed.y*dt * 20) };
		position.x += (float)aux.x;
		position.y += (float)aux.y;
		posCollider.center = position;
	}
	break;

	case IncrementWaypoint:
	{
		
		next_Goal = *Path.begin();

		iPoint aux = next_Goal - TilePos;
		SetUnitDirectionByValue(aux.Return_fPoint());
		state = followPath;

		if (Path.size() == 0)
			state = idle;
		
	}
	break;
	case max_state:
	{

	}
	break;
	default:
		break;
	}

}

void Unit::SetUnitDirection(Unit_Direction unitDirection)
{
	state_diretion = unitDirection;
	switch (unitDirection) {

	case UnitDirection_Idle:

		direction.x = 0.0f;
		direction.y = 0.0f;
		break;

	case UnitDirection_Up:

		direction.x = 0.0f;
		direction.y = -1.0f;
		break;

	case UnitDirection_Down:

		direction.x = 0.0f;
		direction.y = 1.0f;
		break;

	case UnitDirection_Left:

		direction.x = -1.0f;
		direction.y = 0.0f;
		break;

	case UnitDirection_Right:

		direction.x = 1.0f;
		direction.y = 0.0f;
		break;

	case UnitDirection_UpLeft:

		direction.x = -1.0f;
		direction.y = -1.0f;
		break;

	case UnitDirection_UpRight:

		direction.x = 1.0f;
		direction.y = -1.0f;
		break;

	case UnitDirection_DownLeft:

		direction.x = -1.0f;
		direction.y = 1.0f;
		break;

	case UnitDirection_DownRight:

		direction.x = 1.0f;
		direction.y = 1.0f;
		break;
	}
}

Unit_Direction Unit::GetUnitDirection()
{
	return state_diretion;
}

void Unit::SetUnitDirectionByValue(fPoint unitDirection)
{
	direction = unitDirection;
	if (direction.x > 0.0f) {
			state_diretion = UnitDirection_Right;
	}
	else if (direction.x < 0.0f) {

			state_diretion = UnitDirection_Left;
	}
	else {

		if (direction.y > 0.0f)
			state_diretion= UnitDirection_Down;

		else if (direction.y < 0.0f)
			state_diretion= UnitDirection_Up;

		else
			state_diretion= UnitDirection_Idle;
	}

}

fPoint Unit::GetUnitDirectionByValue() const
{
	return direction;
}

SingleUnit * Unit::GetSingleUnit() const
{
	return singleUnit;
}
