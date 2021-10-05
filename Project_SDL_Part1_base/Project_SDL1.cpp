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

SDL_Surface* load_surface_for(const char* path,
                              SDL_Surface* window_surface_ptr) {
  // Helper function to load a png for a specific surface
  // See SDL_ConvertSurface
  SDL_Surface* img = IMG_Load(path);
  if (img == NULL) {
    std::cout << "Bug IMG LOAD" << std::endl;
    return NULL;
  }

  SDL_Surface* ret = SDL_ConvertSurface(img, window_surface_ptr->format, 0);
  if (ret == NULL) {
    std::cout << "BUG CONVERT SURFACE" << std::endl;
    return NULL;
  }

  SDL_FreeSurface(img);
  return ret;
}
} // namespace
/*
sheep::sheep(SDL_SURFACE *window_surface_ptr) : animal::animal("./media/sheep.png", window_surface_ptr)
{
}
wolf::wolf(SDL_SURFACE *window_surface_ptr) : animal::animal("./media/wolf.png", *window_surface_ptr)
{
}
*/

/*
Animal
*/

animal::animal(const std::string& file_path, SDL_Surface* window_surface_ptr) {
  this->window_surface_ptr_ = window_surface_ptr;
  //this->image_ptr_ = load_surface_for(file_path, this->window_surface_ptr);
  
  
  this->file_path = file_path;
  char * S = new char[file_path.length() + 1];
  for (size_t i = 0; i < file_path.length(); i++)
  {
    S[i] = file_path[i];
  }
  S[file_path.length()] = 0;
  this->image_ptr_ = load_surface_for(S, window_surface_ptr);

  // 0 + boundary < x < frame_width - boundary
  // 0 + boundary < y < frame_height - boundary
  this->pos = Vector2{frame_width / 2, frame_height / 2};
  this->spd = Vector2{((float) rand()/RAND_MAX )* 80 - 40, ((float) rand()/RAND_MAX )* 80 - 40};
}

animal::~animal(){
  // C'est pas a animal de free le pointeur
  SDL_FreeSurface(this->image_ptr_);
}

void animal::draw() {
  SDL_Rect dst_rect = SDL_Rect{int(this->pos.x), int(this->pos.y), int(this->pos.x + this->image_ptr_->w), int(this->pos.y + this->image_ptr_->h)};
  SDL_BlitSurface(this->image_ptr_, NULL, this->window_surface_ptr_, &dst_rect);
}












/*
Wolf
*/

void wolf::move()
{
  //srand(time(NULL));
  //float newmovex = ((float) rand()/RAND_MAX )* 2 - 1;
  //float newmovey = ((float) rand()/RAND_MAX )* 2 - 1;
  
  if (this->pos.x + this->spd.x < frame_boundary || this->pos.x + this->spd.x > frame_width - this->image_ptr_->w - frame_boundary)
    this->spd.x = -this->spd.x;
  if (this->pos.y + this->spd.y < frame_boundary || this->pos.y + this->spd.y > frame_height - this->image_ptr_->h - frame_boundary)
    this->spd.y = -this->spd.y;
  this->pos.x += this->spd.x;
  this->pos.y  += this->spd.y;
}










/*
 Sheep 
 */
void sheep::move()
{
  //srand(time(NULL));
  
  /*float newmovex = ((float) rand()/RAND_MAX )* 2 - 1;
  float newmovey = ((float) rand()/RAND_MAX )* 2 - 1;
  this->spd.x += newmovex;
  this->spd.y  += newmovey;*/
  /*if (this->pos.x + this->spd.x < 0 ||this->pos.x + this->spd.x > frame_width - 67)
    this->spd.x = -this->spd.x;
  if (this->pos.y + this->spd.y < 0 ||this->pos.y + this->spd.y > frame_height - 71)
    this->spd.y = -this->spd.y;*/
  if (this->pos.x + this->spd.x < frame_boundary || this->pos.x + this->spd.x > frame_width - this->image_ptr_->w - frame_boundary)
    this->spd.x = -this->spd.x;
  if (this->pos.y + this->spd.y < frame_boundary || this->pos.y + this->spd.y > frame_height - this->image_ptr_->h - frame_boundary)
    this->spd.y = -this->spd.y;
  this->pos.x  += this->spd.x;
  this->pos.y  += this->spd.y;
}
























/*
Ground
*/
ground::ground(SDL_Surface* window_surface_ptr)
{
    this->window_surface_ptr_ = window_surface_ptr;
} // todo: Ctor
ground::~ground()
{
    SDL_FreeSurface(this->window_surface_ptr_);
} // todo: Dtor, again for clean up (if necessary)
void ground::add_animal(animal* a)
{
    this->animals.push_back(a);
}// todo: Add an animal
void ground::update()
{
  //for (auto &animal : this->animals) {
  for (unsigned i = 0; i < this->animals.size(); ++i) {
      animals[i]->move();
      animals[i]->draw();
  }
    
} // todo: "refresh the screen": Move animals and draw them










/*
Application
*/



application::application(unsigned int n_sheep, unsigned int n_wolf) {
    this->window_ptr_ = SDL_CreateWindow("Rust>C++", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, frame_width, frame_height, SDL_WINDOW_SHOWN);
    if (this->window_ptr_ == NULL) {
      std::cout << std::string(SDL_GetError()) << std::endl;
      abort();
    }
    this->window_surface_ptr_ = SDL_GetWindowSurface(this->window_ptr_);
    if (this->window_surface_ptr_ == NULL) {
      std::cout << std::string(SDL_GetError()) << std::endl;
      abort();
    }
    this->my_ground = new ground(this->window_surface_ptr_);

    for (unsigned i = 0 ; i < n_sheep ; ++i)
        this->my_ground->add_animal(new sheep(this->window_surface_ptr_));

    for (unsigned i = 0; i < n_wolf ; ++i)
        this->my_ground->add_animal(new wolf(this->window_surface_ptr_));
}

application::~application() {
    delete(this->my_ground);
    SDL_FreeSurface(this->window_surface_ptr_);
    SDL_DestroyWindow(this->window_ptr_);
}



int application::loop(unsigned int period) {
    bool close = false;


    while (!close) {
        while (SDL_PollEvent(&this->window_event_)) {
            switch (this->window_event_.type) {
                case SDL_QUIT:
                    close = true;
                    break;
            }
        }
        
        SDL_FillRect(this->window_surface_ptr_, NULL, SDL_MapRGB(this->window_surface_ptr_->format, 0, 255, 0));
        
        this->my_ground->update();
        
        SDL_UpdateWindowSurface(this->window_ptr_);
        SDL_Delay(frame_time * 1000);
  }

    return 0;
}
