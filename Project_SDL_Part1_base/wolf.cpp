#include "Project_SDL1.h"

/*
class wolf : public animal {
  wolf();
  ~wolf();
  // implement functions that are purely virtual in base class
};
*/

wolf::move()
{
  srand(time(NULL));
  float newmovex = ((float) rand()/RAND_MAX )* 2 - 1;
  float newmovey = ((float) rand()/RAND_MAX )* 2 - 1;
  this.spd[0] += newmovex;
  this.spd[1]  += newmovey;
  if (pos[0] + spd[0] < 0 ||pos[0] + spd[0] > 720 - 62)
    spd[0] = -spd[0];
  if (pos[1] + spd[1] < 0 ||pos[1] + spd[1] > 480 - 42)
    spd[1] = -spd[1];
  this.pos[0]  += this.spd[0];
  this.pos[1]  += this.spd[1];
}