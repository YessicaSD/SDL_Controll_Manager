#ifndef __j1ModuleEnemies_H__
#define __j1ModuleEnemies_H__

#include "j1Module.h"
#include "p2Point.h"
#include "j1Textures.h"
#include "p2Defs.h"
#include "Animation.h"
#include "j1Entity.h"
#include "Unit.h"
#include <vector> 
struct Allied_Infa_Info
{
	std::string path;
	Animation AnimInfo;
};
class j1EntityManager : public j1Module
{
private:
	Allied_Infa_Info Allied_Info;
public:
	SDL_Texture * Entities_Textures[entities_types::MAX_ENTITIES];
	std::vector<j1Entity*> list_Entities;
	std::vector<j1Entity*> selected_units;
	pugi::xml_document	enemiesFile;
	pugi::xml_node entitiesNodeDoc;

	j1EntityManager();
	~j1EntityManager();

	j1Entity* AddEntity(entities_types type,fPoint pos);
	bool DestroyEntity(std::vector<j1Entity*>::iterator entity);
	void DestroyAllEntities();
	j1Entity* InThisTile_IsUnits(iPoint tile);

	fPoint middlePoint_Group;
	// TODO 1: Create a funtion that calculates the middle point------------------------------------------------------------------------ -
	fPoint Calculate_middle_Point();

	//Module funtions-----------------
	bool Awake(pugi::xml_node&)override;
	bool Start()override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	void PredictPossibleCollitions();
	

};
#endif // __ModuleEnemies_H__

