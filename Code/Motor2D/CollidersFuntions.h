#ifndef _COLLIDERS_FUNTIONS_H_
#define _COLLIDERS_FUNTIONS_H_
#include "p2Point.h"
struct Circle
{
	fPoint center;
	float radius;
	bool IsPointIn(fPoint point)
	{
		if (point.x >= (center.x - radius)
			&& point.x<= (center.x + radius)
			&& point.y >=(center.y - radius)
			&& point.y <= (center.y + radius))
			return true;
		else
			return false;
	}
};


#endif // !_COLLIDERS_FUNTIONS_H_

