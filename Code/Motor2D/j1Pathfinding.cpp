#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Map.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name = "pathfinding";
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

bool j1PathFinding::PostUpdate(float dt)
{
	
	// https://www.youtube.com/watch?v=f7mtWD9GdJ4 
	//Explanation to Local Statics

	static iPoint origin;
	static bool origin_selected = false;
	static bool createdDebugPath = false;

	iPoint mousePos;
	App->input->GetMousePosition(mousePos.x, mousePos.y);
	iPoint p = App->render->ScreenToWorld(mousePos.x, mousePos.y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonState(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{

			origin_selected = false;

			if (CreatePath(origin, p) != -1)
			{
				createdDebugPath = true;
			}

		}
		else
		{
			origin = p;
			origin_selected = true;
			createdDebugPath = false;
			debugPath.clear();

		}
	}

	if (createdDebugPath)
	{
		uint debugPathSize = debugPath.size();
		if (debugPathSize == 0)
		{
		
			const std::vector<iPoint>* path = App->pathfinding->GetLastPath();
			uint sizeArray = path->size();
			for (uint i = 0; i < sizeArray; ++i)
			{
				debugPath.push_back(path->at(i));
			}
		}
		else
		{
			for (uint i = 0; i < debugPathSize; ++i)
			{
				iPoint pos = App->map->MapToWorld(debugPath.at(i).x, debugPath.at(i).y);
				App->render->Blit(debug_tex, pos.x, pos.y);
			}
		}

	}
	return true;
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}



// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const std::vector<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point)
{
	p2List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : numSteps(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : numSteps(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : numSteps(node.numSteps), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();
		

	// north
	cell.create(pos.x, pos.y + 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return numSteps + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	numSteps = parent->numSteps + 1;
	h = pos.DistanceTo(destination);

	return numSteps + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	if ( origin== destination || !IsWalkable(origin) || !IsWalkable(destination) )
		return -1;

	PathList openList, closeList;

	openList.list.add({ 0, origin.DistanceTo(destination), origin, nullptr });

	bool findDestination = false;

	while (openList.list.count()>0 && !findDestination)
	{
		p2List_item<PathNode>* lowerNode = openList.GetNodeLowestScore();
		p2List_item<PathNode>* currNode = closeList.list.add(lowerNode->data);
		openList.list.del(lowerNode);

		if (currNode->data.pos == destination)
		{
			findDestination = true;
			break;
		}

		PathList neighbours;
		currNode->data.FindWalkableAdjacents(neighbours);


		for (p2List_item<PathNode>* nodeNeigh = neighbours.list.start; nodeNeigh; nodeNeigh = nodeNeigh->next)
		{
			p2List_item<PathNode>* closeItem = (p2List_item<PathNode>*) closeList.Find(nodeNeigh->data.pos);
			p2List_item<PathNode>* openItem = (p2List_item<PathNode>*) openList.Find(nodeNeigh->data.pos);

			if (closeItem != NULL)
				continue;

			nodeNeigh->data.CalculateF(destination);
			if (openItem == NULL)
				openList.list.add(nodeNeigh->data);

			else if (nodeNeigh->data.numSteps < openItem->data.numSteps)
			{
				openItem->data.parent = &currNode->data;

			}


		}


	}
	if (findDestination)
	{
		last_path.clear();
		const p2List_item<PathNode>* pathNode = closeList.list.end;
		while (pathNode != NULL)
		{
			last_path.push_back(pathNode->data.pos);
			if (pathNode->data.parent != nullptr)
				pathNode = closeList.Find(pathNode->data.parent->pos);
			else
				pathNode = NULL;
		}
		//Here we flip last_path order -------------------------------------
		std::vector<iPoint> aux;
		for (int i = last_path.size()-1; i >= 0; --i)
		{
			aux.push_back(last_path.at(i));
		}
		last_path.swap(aux);
		return 1;
	}


	return -1;
}

