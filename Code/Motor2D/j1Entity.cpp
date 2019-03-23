#include "j1Entity.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "p2Point.h"
#include "j1App.h"
//#include "j1Collision.h"
#include "j1Render.h"
#include "Brofiler/Brofiler.h"


j1Entity::j1Entity(fPoint position, SDL_Texture* tex, entities_types type):position(position),texture(tex),type(type)
{

}

j1Entity::~j1Entity()
{
}




