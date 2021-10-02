#include "Project_SDL1.h"

/*
class application {
private:
  // The following are OWNING ptrs
  SDL_Window* window_ptr_;
  SDL_Surface* window_surface_ptr_;
  SDL_Event window_event_;

  // Other attributes here, for example an instance of ground

public:
  application(unsigned n_sheep, unsigned n_wolf); // Ctor
  ~application();                                 // dtor

  int loop(unsigned period); // main loop of the application.
                             // this ensures that the screen is updated
                             // at the correct rate.
                             // See SDL_GetTicks() and SDL_Delay() to enforce a
                             // duration the application should terminate after
                             // 'period' seconds
};
*/

application::application(unsigned n_sheep, unsigned n_wolf) {
    this->window_ptr_ = SDL_CreateWindow("Rust>C++", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (this->window_ptr_ == NULL)
        return;
    this->window_surface_ptr_ = SDL_GetWindowSurface(this->window_ptr_);
    if (this->window_surface_ptr_)
        return;

    this->my_ground = new ground(this->window_surface_ptr_);

    for (unsigned i ; i < n_sheep ; ++i)
        this->ground->add_animal(new sheep("./media/sheep.png"), this->window_surface_ptr_);
    for (unsigned i ; i < n_wolf ; ++i)
        this->ground->add_animal(new sheep("./media/wolf.png"), this->window_surface_ptr_);
}

application::~application() {
    delete(this->my_ground);
    SDL_FreeSurface(this->window_surface_ptr_);
    SDL_DestroyWindow(this->window_ptr_);
}

int loop(unsigned period) {
    bool close = false;

    while (!close) {
        while (SDL_PollEvent(&this->window_event_)) {
            switch (this->window_event_.type) {
                case SDL_QUIT:
                    close = true;
                    break;
            }
        }

        SDL_FillRect(this->window_surface_ptr_, NULL, 0);
        this->ground->update();
        SDL_UpdateWindowSurface(this->window_ptr_);
    }
}