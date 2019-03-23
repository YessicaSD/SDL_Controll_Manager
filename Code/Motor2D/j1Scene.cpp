#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"

#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Scene.h"
#include "j1EntityManager.h"
#include "p2Point.h"

j1Scene::j1Scene() : j1Module()
{
	name = "scene";
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load("Map_Ortogonal.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	cube = { 100, 100, 10, 10 };
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	std::string title = "Map:" + std::to_string(App->map->data.width) + "x" + std::to_string(App->map->data.height) +
		" Tiles:" + std::to_string(App->map->data.tile_width) + "x" + std::to_string(App->map->data.tile_height) +
		"Num_Tiles:" + std::to_string(App->map->data.tilesets.size()) +
		" Tile:" + std::to_string(map_coordinates.x) + "x" + std::to_string(map_coordinates.y);
				

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	bool ret = true;
	App->render->DrawQuad(cube, 255, 0, 0, 255);
	/*if((*App->input->controllers.begin())->Get_Axis(SDL_CONTROLLER_AXIS_LEFTX) < -10000)
		cube.x -= 1;
	if ((*App->input->controllers.begin())->Get_Axis(SDL_CONTROLLER_AXIS_LEFTX) > 10000)
		cube.x += 1;

	if ((*App->input->controllers.begin())->Get_Axis(SDL_CONTROLLER_AXIS_LEFTY) < -10000)
		cube.y -= 1;
	if ((*App->input->controllers.begin())->Get_Axis(SDL_CONTROLLER_AXIS_LEFTY) > 10000)
		cube.y += 1;
	if ((*App->input->controllers.begin())->Get_Axis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 10000)
	{
		(*App->input->controllers.begin())->test_haptic();
	
	}*/

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
