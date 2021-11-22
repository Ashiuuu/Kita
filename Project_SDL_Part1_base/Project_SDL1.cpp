// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <math.h>
#include <numeric>
#include <random>
#include <string>

void init()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
        throw std::runtime_error("init():" + std::string(SDL_GetError()));

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
        throw std::runtime_error("init(): SDL_image could not initialize! "
                                 "SDL_image Error: "
                                 + std::string(IMG_GetError()));
}

namespace
{
    // Defining a namespace without a name -> Anonymous workspace
    // Its purpose is to indicate to the compiler that everything
    // inside of it is UNIQUELY used within this source file.

    SDL_Surface *load_surface_for(const char *path,
                                  SDL_Surface *window_surface_ptr)
    {
        // Helper function to load a png for a specific surface
        // See SDL_ConvertSurface
        SDL_Surface *img = IMG_Load(path);
        if (img == NULL)
        {
            std::cout << "Bug IMG LOAD" << std::endl;
            return NULL;
        }

        SDL_Surface *ret =
            SDL_ConvertSurface(img, window_surface_ptr->format, 0);
        if (ret == NULL)
        {
            std::cout << "BUG CONVERT SURFACE" << std::endl;
            return NULL;
        }

        SDL_FreeSurface(img);
        return ret;
    }
} // namespace

float get_rand_float(int low, int high)
{
    return low + rand() / (RAND_MAX / (high - low + 1) + 1);
}

Vector2 sub_vector(const Vector2 &a, const Vector2 &b)
{
    // return Vector2(b.x - a.x, b.y - a.y);
    return { b.x - a.x, b.y - a.y };
}

Vector2 mul_vector(const Vector2 &a, float b)
{
    // return Vector2(a.x * b, a.y * b);
    return { a.x * b, a.y * b };
}

float vector_norm(const Vector2 &a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

float vector_distance(const Vector2 &a, const Vector2 &b)
{
    return vector_norm(sub_vector(a, b));
}

Vector2 normalize_vector(const Vector2 &a)
{
    float norm = vector_norm(a);
    // return Vector2(a.x / norm, a.y / norm);
    return { a.x / norm, a.y / norm };
}

Vector2 random_vector(float norm)
{
    float x = get_rand_float(-10, 10);
    float y = get_rand_float(-10, 10);

    Vector2 unit_vector = normalize_vector({ x, y });
    return mul_vector(unit_vector, norm);
}

/*
Shepard
*/

shepherd::shepherd(SDL_Surface *window_surface_ptr)
    : animal::animal("media/shepherd.png", window_surface_ptr){};

void shepherd::move()
{
    SDL_Event event;
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
    SDL_PollEvent(&event);

    if (keyboard_state_array[SDL_SCANCODE_W]
        || keyboard_state_array[SDL_SCANCODE_UP])
        this->spd.y -= 5;
    if (keyboard_state_array[SDL_SCANCODE_S]
        || keyboard_state_array[SDL_SCANCODE_DOWN])
        this->spd.y += 5;
    if (keyboard_state_array[SDL_SCANCODE_A]
        || keyboard_state_array[SDL_SCANCODE_LEFT])
        this->spd.x -= 5;
    if (keyboard_state_array[SDL_SCANCODE_D]
        || keyboard_state_array[SDL_SCANCODE_RIGHT])
        this->spd.x += 5;

    if (this->pos.x + this->spd.x < frame_boundary
        || this->pos.x + this->spd.x
            > frame_width - this->image_ptr_->w - frame_boundary)
        this->spd.x = -this->spd.x;
    if (this->pos.y + this->spd.y < frame_boundary
        || this->pos.y + this->spd.y
            > frame_height - this->image_ptr_->h - frame_boundary)
        this->spd.y = -this->spd.y;
    this->pos.x += this->spd.x;
    this->pos.y += this->spd.y;
    this->spd.y = 0;
    this->spd.x = 0;
}

/*
Animal
*/

animal::animal(const std::string &file_path, SDL_Surface *window_surface_ptr)
{
    this->window_surface_ptr_ = window_surface_ptr;
    // this->image_ptr_ = load_surface_for(file_path,
    // this->window_surface_ptr);

    this->file_path = file_path;
    char *S = new char[file_path.length() + 1];
    for (size_t i = 0; i < file_path.length(); i++)
    {
        S[i] = file_path[i];
    }
    S[file_path.length()] = 0;
    this->image_ptr_ = load_surface_for(S, window_surface_ptr);

    // 0 + boundary < x < frame_width - boundary
    // 0 + boundary < y < frame_height - boundary
    // this->pos = Vector2{frame_width / 2, frame_height / 2};
    this->pos = Vector2{
        get_rand_float(frame_boundary,
                       frame_width - frame_boundary - this->image_ptr_->w),
        get_rand_float(frame_boundary,
                       frame_height - frame_boundary - this->image_ptr_->h)
    };
    // this->spd = Vector2{((float) rand()/RAND_MAX )* 80 - 40, ((float)
    // rand()/RAND_MAX )* 80 - 40};
    this->spd = Vector2{ 5, 5 };
}

animal::~animal()
{
    // C'est pas a animal de free le pointeur
    SDL_FreeSurface(this->image_ptr_);
}

void animal::draw()
{
    SDL_Rect dst_rect = SDL_Rect{ int(this->pos.x), int(this->pos.y),
                                  int(this->pos.x + this->image_ptr_->w),
                                  int(this->pos.y + this->image_ptr_->h) };
    SDL_BlitSurface(this->image_ptr_, NULL, this->window_surface_ptr_,
                    &dst_rect);
}

/*
Wolf
*/

void wolf::detect(std::vector<sheep *> sheeplist)
{
    sheep *closest;
    Vector2 closest_vector;
    float dist = 50000;
    for (unsigned int i = 0; i < sheeplist.size(); i++)
    {
        float temp = vector_distance(this->pos, sheeplist[i]->pos);
        if (temp < dist)
        {
            dist = temp;
            closest = sheeplist[i];
            closest_vector = sub_vector(sheeplist[i]->pos, this->pos);
        }
    }
    this->spd =
        mul_vector(normalize_vector(closest_vector), (-1) * this->speed_norm);
}

void wolf::move()
{
    // srand(time(NULL));
    // float newmovex = ((float) rand()/RAND_MAX )* 2 - 1;
    // float newmovey = ((float) rand()/RAND_MAX )* 2 - 1;

    if (this->pos.x + this->spd.x < frame_boundary
        || this->pos.x + this->spd.x
            > frame_width - this->image_ptr_->w - frame_boundary)
        this->spd.x = -this->spd.x;
    if (this->pos.y + this->spd.y < frame_boundary
        || this->pos.y + this->spd.y
            > frame_height - this->image_ptr_->h - frame_boundary)
        this->spd.y = -this->spd.y;
    this->pos.x += this->spd.x;
    this->pos.y += this->spd.y;
}

/*
 Sheep
 */

sheep::sheep(SDL_Surface *window_surface_ptr)
    : animal::animal("media/sheep.png", window_surface_ptr)
{
    if (get_rand_float(0, 1) > 0)
        male = true;
    else
    {
        cooldown = 0;
    }
    std::cout << "sex is :" << male << std::endl;

    this->spd = random_vector(this->speed_norm);
}

void sheep::detect(std::vector<wolf *> wolfslist)
{
    wolf *closest;
    Vector2 closest_vector;
    float dist = 50000;
    for (unsigned int i = 0; i < wolfslist.size(); i++)
    {
        float temp = vector_distance(this->pos, wolfslist[i]->pos);
        if (temp < dist)
        {
            dist = temp;
            closest = wolfslist[i];
            closest_vector = sub_vector(wolfslist[i]->pos, this->pos);
        }
    }
    if (dist < 300)
    {
        this->spd = mul_vector(normalize_vector(closest_vector),
                               this->speed_norm * 1.5);
        // this->spd.x = closest->spd.x;
        // this->spd.y = closest->spd.y;
    }
}

void sheep::move()
{
    if (this->cooldown > 0)
        this->cooldown--;

    if (this->pos.x + this->spd.x < frame_boundary
        || this->pos.x + this->spd.x
            > frame_width - this->image_ptr_->w - frame_boundary)
        this->spd.x = -this->spd.x;
    if (this->pos.y + this->spd.y < frame_boundary
        || this->pos.y + this->spd.y
            > frame_height - this->image_ptr_->h - frame_boundary)
        this->spd.y = -this->spd.y;
    this->pos.x += this->spd.x;
    this->pos.y += this->spd.y;
}

/*
dog
*/

void dog::move(const Vector2 &player_pos)
{
    Vector2 pos = player_pos;

    Vector2 vector_to_player = sub_vector(this->pos, player_pos);
    Vector2 unit_vector = normalize_vector(vector_to_player);
    float distance_to_player = vector_norm(vector_to_player);

    if (distance_to_player > 10.5)
    {
        this->spd = mul_vector(unit_vector, this->speed_norm);
    }
    else if (distance_to_player < 9.5)
    {
        this->spd = mul_vector(unit_vector, (-1) * this->speed_norm);
    }
    else
    {
        if (unit_vector.y < 0)
        {
            unit_vector.x = unit_vector.x * (-1);
        }
        else
        {
            unit_vector.y = unit_vector.y * (-1);
        }
    }

    this->spd = mul_vector(unit_vector, this->speed_norm);

    this->pos.x += this->spd.x;
    this->pos.y += this->spd.y;
}

/*
Ground
*/
ground::ground(SDL_Surface *window_surface_ptr)
{
    this->window_surface_ptr_ = window_surface_ptr;
    this->player = new shepherd(window_surface_ptr);
    this->doggo = new dog(window_surface_ptr);
} // todo: Ctor

ground::~ground()
{
    SDL_FreeSurface(this->window_surface_ptr_);
    delete (this->player);
} // todo: Dtor, again for clean up (if necessary)
void ground::add_animal(animal *a, bool issheep)
{
    if (!issheep)
        this->wolfs.push_back((wolf *)a);
    else
        this->sheeps.push_back((sheep *)a);
} // todo: Add an animal
void ground::update()
{
    player->move();
    player->draw();

    doggo->move(this->player->pos);
    doggo->draw();
    // for (auto &animal : this->animals) {
    for (unsigned i = 0; i < this->sheeps.size(); ++i)
    {
        sheeps[i]->detect(this->wolfs);
        sheeps[i]->move();
        sheeps[i]->draw();
    }
    for (unsigned i = 0; i < this->wolfs.size(); ++i)
    {
        wolfs[i]->detect(this->sheeps);
        wolfs[i]->move();
        wolfs[i]->draw();
    }
    for (unsigned int i = 0; i < sheeps.size(); i++) // D’ailleurs, i++ ou ++i ?
    {
        if (!sheeps[i]->male && sheeps[i]->cooldown == 0)
        {
            for (unsigned int k = 0; k < this->sheeps.size(); k++)
            {
                if (sheeps[i] == sheeps[k])
                    continue;
                if (sheeps[k]->male
                    && vector_distance(sheeps[i]->pos, sheeps[k]->pos) < 80)
                {
                    sheep *baby = new sheep(this->window_surface_ptr_);
                    this->sheeps.push_back(baby);

                    std::cout << "added a baby: " << k << std::endl;
                    // this->add_animal(baby, true);
                    baby->pos.x = sheeps[i]->pos.x;
                    baby->pos.y = sheeps[i]->pos.y;
                    sheeps[i]->cooldown = 3600;
                    baby->cooldown = 3600;
                    break;
                }
            }
        }
    }
    for (unsigned int i = 0; i < wolfs.size(); ++i)
    {
        for (auto j = this->sheeps.begin(); j != this->sheeps.end(); ++j)
        {
            if (vector_distance(wolfs[i]->pos, (*j)->pos) < 80)
            {
                this->sheeps.erase(j);
                break;
            }
        }
    }
} // todo: "refresh the screen": Move animals and draw them

/*
Application
*/

application::application(unsigned int n_sheep, unsigned int n_wolf)
{
    this->window_ptr_ = SDL_CreateWindow("Rust>C++", SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED, frame_width,
                                         frame_height, SDL_WINDOW_SHOWN);
    if (this->window_ptr_ == NULL)
    {
        std::cout << std::string(SDL_GetError()) << std::endl;
        abort();
    }
    this->window_surface_ptr_ = SDL_GetWindowSurface(this->window_ptr_);
    if (this->window_surface_ptr_ == NULL)
    {
        std::cout << std::string(SDL_GetError()) << std::endl;
        abort();
    }
    this->my_ground = new ground(this->window_surface_ptr_);

    for (unsigned i = 0; i < n_sheep; ++i)
        this->my_ground->sheeps.push_back(new sheep(this->window_surface_ptr_));

    for (unsigned i = 0; i < n_wolf; ++i)
        this->my_ground->wolfs.push_back(new wolf(this->window_surface_ptr_));
}

application::~application()
{
    delete (this->my_ground);
    SDL_FreeSurface(this->window_surface_ptr_);
    SDL_DestroyWindow(this->window_ptr_);
}

int application::loop(unsigned int period)
{
    bool close = false;

    unsigned int elapsed_time = 0;

    while (!close && elapsed_time < period * 1000)
    {
        unsigned int loop_begin = SDL_GetTicks();
        while (SDL_PollEvent(&this->window_event_))
        {
            switch (this->window_event_.type)
            {
            case SDL_QUIT:
                close = true;
                break;
            }
        }

        SDL_FillRect(this->window_surface_ptr_, NULL,
                     SDL_MapRGB(this->window_surface_ptr_->format, 0, 255, 0));

        this->my_ground->update();

        SDL_UpdateWindowSurface(this->window_ptr_);
        unsigned int loop_end = SDL_GetTicks();
        unsigned int delta = loop_end - loop_begin;
        if (delta < frame_time * 1000)
        {
            SDL_Delay(frame_time * 1000 - delta);
            elapsed_time += frame_time * 1000;
        }
        else
        {
            elapsed_time += delta;
        }
        // SDL_Delay(frame_time * 1000);
    }

    return 0;
}
