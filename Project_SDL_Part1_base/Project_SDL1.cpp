// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>

void init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("init():" + std::string(SDL_GetError()));

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    throw std::runtime_error("init(): SDL_image could not initialize! "
                             "SDL_image Error: " +
                             std::string(IMG_GetError()));
}

namespace {
// Defining a namespace without a name -> Anonymous workspace
// Its purpose is to indicate to the compiler that everything
// inside of it is UNIQUELY used within this source file.

SDL_Surface* load_surface_for(const std::string& path,
                              SDL_Surface* window_surface_ptr) {
  // Helper function to load a png for a specific surface
  // See SDL_ConvertSurface
  SDL_Surface* img = IMG_Load(path);
  if (img == NULL)
    return NULL;

  SDL_Surface* ret = SDL_ConvertSurface(img, window_surface_ptr->format, 0);
  if (ret == NULL)
    return NULL;

  SDL_FreeSurface(img);
  return ret;
}
} // namespace
sheep::sheep()
{
  load_surface_for("/media/sheep.png",&this.image_ptr_);
}
wolf::wolf()
{
  load_surface_for("/media/wolf.png",&this.image_ptr_);
}

animal::animal(const std::string& file_path, SDL_Surface* window_surface_ptr) {
  this->window_surface_ptr_ = window_surface_ptr;
  this->image_ptr_ = nullptr; //load_surface_for(file_path, this->window_surface_ptr);
  
  srand(time(NULL));

  pos = Vector2{((float) rand()/RAND_MAX )* 720,((float) rand()/RAND_MAX )* 480};
  spd = Vector2{((float) rand()/RAND_MAX )* 20 - 10, ((float) rand()/RAND_MAX )* 20 - 10};
}

animal::~animal() {
    SDL_FreeSurface(this->image_ptr_);
    delete(pos);
    delete(spd);
}
