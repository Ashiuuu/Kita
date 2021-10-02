#include "Project_SDL1.h"

/*
class ground {
private:
  // Attention, NON-OWNING ptr, again to the screen
  SDL_Surface* window_surface_ptr_;
  vector<animal> animals;
  // Some attribute to store all the wolves and sheep
  // here

public:
  ground(SDL_Surface* window_surface_ptr); // todo: Ctor
  ~ground(){}; // todo: Dtor, again for clean up (if necessary)
  void add_animal(some argument here); // todo: Add an animal
  void update(); // todo: "refresh the screen": Move animals and draw them
  // Possibly other methods, depends on your implementation
};
*/


ground::ground(SDL_Surface* window_surface_ptr)
{
    this.window_surface_ptr_ = window_surface_ptr;
    this.animals = new vector<animal>();
} // todo: Ctor
ground::~ground()
{
    delete(this.animals)
}; // todo: Dtor, again for clean up (if necessary)
ground::add_animal(animal* a)
{
    this.animals.push(a)
}// todo: Add an animal
ground::update()
{
    anim(auto &animal : this->animals) {
        animal->move();
        animal->draw();
    }
    
} // todo: "refresh the screen": Move animals and draw them
