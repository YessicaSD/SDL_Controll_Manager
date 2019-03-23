#ifndef __J1ENTITY_H_
#define __J1ENTITY_H_

#include "j1Module.h"
#include "Animation.h"
#include "p2List.h"
#include "p2Point.h"
#include "SDL/include/SDL_render.h"

struct SDL_Texture;
struct Collider;

enum entities_types
{
	ALLIED_INFANT,
	MAX_ENTITIES,
};
class j1Entity 
{
protected: 
	bool showPath= false;
	
public:
	SDL_Texture * texture;
	bool toDelete = false;

	fPoint position = {0.0F,0.0F};
	iPoint TilePos = {0,0};

	Collider* collider = nullptr;
	entities_types type;
	bool death = false, selected = false;

public:
	j1Entity(fPoint position, SDL_Texture * tex, entities_types type);
	virtual ~j1Entity();
	

	virtual bool PreUpdate(float dt) { return true; };
	virtual void Move(float dt) {};
	virtual void Draw(float dt) {};
	virtual void DebugDraw() {};
	virtual void OnCollision(Collider* collider) {};
	
};

#endif // __j1ENTITY_H__