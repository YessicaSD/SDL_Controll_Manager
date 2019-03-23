#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
private:
	SDL_Rect cube;
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt) override;

	// Called each loop iteration
	bool Update(float dt) override;

	// Called before all Updates
	bool PostUpdate(float dt)override;

	// Called before quitting
	bool CleanUp();
	SDL_Texture* debug_tex;
private:
	
};

#endif // __j1SCENE_H__