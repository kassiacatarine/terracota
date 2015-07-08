#ifndef MAP2_H
#define MAP2_H

#include <core/level.h>
#include "stage.h"

using std::shared_ptr;
class Map2: public Level
{
public:
	Map2(ObjectID id);
	~Map2();
	void generate_map();
    void run_physics(unsigned long elapsed);
};

#endif
