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
#include <typeinfo>

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

Vector2::Vector2()
{
    this->x = 0;
    this->y = 0;
}

Vector2::Vector2(float a, float b)
{
    this->x = a;
    this->y = b;
}

bool Vector2::is_nan()
{
    return this->x != this->x || this->y != this->y;
}

void Vector2::operator+=(const Vector2 &a)
{
    this->x += a.x;
    this->y += a.y;
}

Vector2 operator-(const Vector2 &a, const Vector2 &o)
{
    return Vector2(a.x - o.x, a.y - o.y);
}

Vector2 operator*(const Vector2 &a, float b)
{
    return Vector2(b * a.x, b * a.y);
}

std::ostream &operator<<(std::ostream &o, const Vector2 &a)
{
    return o << "(" << a.x << "," << a.y << ")";
}

float Vector2::get_norm() const
{
    float res = sqrt(this->x * this->x + this->y * this->y);
    if (res != res)
    {
        std::cout << "NaN in get_norm" << std::endl;
        abort();
    }
    return res;
}

Vector2 normalize(const Vector2 &o)
{
    float norm = o.get_norm();
    if (norm == 0)
    {
        std::cout << "Norm = 0 in normalize" << std::endl;
    }
    Vector2 res = Vector2(o.x / norm, o.y / norm);
    if (res.is_nan())
    {
        std::cout << "NaN in normalize" << std::endl;
        abort();
    }
    return res;
}

float vector_distance(const Vector2 &a, const Vector2 &b)
{
    return (a - b).get_norm();
}

Vector2 random_vector(float norm)
{
    float x = 0;
    while (x == 0)
        x = get_rand_float(-10, 10);
    float y = 0;
    while (y == 0)
        y = get_rand_float(-10, 10);

    Vector2 res = normalize(Vector2(x, y)) * norm;
    if (res.is_nan())
    {
        std::cout << "NaN in random vector" << std::endl;
        abort();
    }
    return res;
}

bool interacting_object::has_attribute(const std::string &att)
{
    return this->attribute == att;
    // return att.compare(this->attribute);
}

std::string interacting_object::get_attribute()
{
    return this->attribute;
}

/*
Shepard
*/

shepherd::shepherd(SDL_Surface *window_surface_ptr)
    : animal::animal("media/shepherd.png", window_surface_ptr)
{
    this->attribute = "shepherd";
}

void shepherd::interract(animal &other)
{
    return;
}

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

interacting_object::interacting_object(SDL_Surface *window_surface_ptr,
                                       const std::string &path)
{
    char *S = new char[path.length() + 1];
    for (size_t i = 0; i < path.length(); i++)
    {
        S[i] = path[i];
    }
    S[path.length()] = 0;

    this->window_surface_ptr_ = window_surface_ptr;
    this->image_ptr_ = load_surface_for(S, window_surface_ptr);

    this->pos = {
        get_rand_float(100, window_surface_ptr->w - 100 - this->image_ptr_->w),
        get_rand_float(100, window_surface_ptr->h - 100 - this->image_ptr_->h)
    };
}

interacting_object::~interacting_object()
{
    SDL_FreeSurface(this->image_ptr_);
}

/*
Animal
*/

animal::animal(const std::string &file_path, SDL_Surface *window_surface_ptr)
    : interacting_object::interacting_object(window_surface_ptr, file_path)
{
    this->hp = 600;
    this->spd = random_vector(this->speed_norm);
    /*this->window_surface_ptr_ = window_surface_ptr;
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
*/
    // 0 + boundary < x < frame_width - boundary
    // 0 + boundary < y < frame_height - boundary
    // this->pos = Vector2{frame_width / 2, frame_height / 2};

    // this->spd = Vector2{((float) rand()/RAND_MAX )* 80 - 40, ((float)
    // rand()/RAND_MAX )* 80 - 40};
    // this->spd = Vector2{ 5, 5 };
}

/*animal::~animal()
{
    // C'est pas a animal de free le pointeur
    SDL_FreeSurface(this->image_ptr_);
}*/

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
wolf::wolf(SDL_Surface *window_surface_ptr)
    : animal::animal("media/wolf.png", window_surface_ptr)
{
    this->attribute = "wolf";
}

void wolf::interract(animal &other)
{
    if (other.has_attribute("sheep") == true)
    {
        float curr_distance = vector_distance(this->pos, other.pos);
        if (curr_distance < this->closest_dist)
        {
            this->closest_sheep = other.pos;
            this->closest_dist = curr_distance;
        }
    }
    else if (other.has_attribute("dog") == true)
    {
        Vector2 vector_to_dog = other.pos - this->pos;
        this->dog_position = other.pos;
        float distance_to_dog = vector_to_dog.get_norm();
        if (distance_to_dog < 250)
        {
            this->is_afraid = true;
        }
        else
        {
            this->is_afraid = false;
        }
    }
}

void wolf::move()
{
    if (this->hp != 0)
        this->hp--;

    this->spd = normalize(this->closest_sheep - this->pos) * this->speed_norm;
    this->closest_dist = FLT_MAX;

    if (this->is_afraid == true)
    {
        this->spd =
            normalize(this->pos - this->dog_position) * this->speed_norm;
    }

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
        male = false;
    else
    {
        cooldown = 0;
    }
    // std::cout << "sex is :" << male << std::endl;
    this->attribute = "sheep";
    // this->spd = random_vector(this->speed_norm);
}

void sheep::interract(animal &other)
{
    if (other.has_attribute("wolf"))
    {
        float temp = vector_distance(this->pos, other.pos);
        if (temp < 80)
        {
            this->hp = 0;
            other.hp = 600;
        }
        else if (temp < this->dist_wolf)
        {
            this->dist_wolf = temp;
            this->closest_wolf = other.pos;
        }
    }
    if (other.has_attribute("sheep"))
    {
        if (other.male && !this->male
            && vector_distance(this->pos, other.pos) < 80
            && this->cooldown == 0)
            this->hp = 69;
    }
}

void sheep::move()
{
    if (this->dist_wolf < 150)
    {
        this->spd =
            normalize(this->pos - this->closest_wolf) * this->speed_norm * 1.5;
        // std::cout << this->spd.x << "," << this->spd.y << std::endl;
    }
    else
    {
        this->spd = normalize(this->spd) * this->speed_norm;
    }

    if (this->cooldown != 0)
    {
        this->cooldown--;
    }

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

    this->dist_wolf = FLT_MAX;
}

/*
dog
*/
dog::dog(SDL_Surface *window_surface_ptr)
    : animal::animal("media/doggo.png", window_surface_ptr)
{
    this->attribute = "dog";
}

void dog::interract(animal &other)
{
    if (other.has_attribute("shepherd") == false)
        return;
    this->spd = Vector2(0, 0);
    this->pos =
        Vector2(other.pos.x + this->circle_dist * cos(this->phase / 50),
                other.pos.y + this->circle_dist * sin(this->phase / 50));
}

void dog::move()
{
    if (this->pos.x < frame_boundary)
        this->pos.x = frame_boundary;
    else if (this->pos.x > frame_width - this->image_ptr_->w - frame_boundary)
        this->pos.x = frame_width - this->image_ptr_->w - frame_boundary;
    if (this->pos.y < frame_boundary)
        this->pos.y = frame_boundary;
    else if (this->pos.y > frame_height - this->image_ptr_->h - frame_boundary)
        this->pos.y = frame_height - this->image_ptr_->h - frame_boundary;

    this->phase++;

    /*    if (this->pos.x + this->spd.x < frame_boundary
             || this->pos.x + this->spd.x
                 > frame_width - this->image_ptr_->w - frame_boundary)
             this->spd.x = -this->spd.x;
     if (this->pos.y + this->spd.y < frame_boundary
         || this->pos.y + this->spd.y
             > frame_height - this->image_ptr_->h - frame_boundary)
         this->spd.y = -this->spd.y;
     this->pos.x += this->spd.x;
     this->pos.y += this->spd.y;*/
}

/*
Ground
*/
ground::ground(SDL_Surface *window_surface_ptr)
{
    this->window_surface_ptr_ = window_surface_ptr;
} // todo: Ctor

ground::~ground()
{
    std::cout << "Your score is " << this->score << std::endl;
    SDL_FreeSurface(this->window_surface_ptr_);
    delete (this->player);
} // todo: Dtor, again for clean up (if necessary)

bool ground::update()
{
    std::vector<std::vector<std::unique_ptr<animal>>::iterator> dead;
    std::vector<Vector2> babes;
    std::vector<std::unique_ptr<animal>> babe;
    for (auto i = this->animals.begin(); i != this->animals.end(); i++)
    {
        for (auto j = this->animals.begin(); j != this->animals.end(); j++)
        {
            if (i == j)
                continue;
            i->get()->interract(*(j->get()));
        }
        if (i->get()->has_attribute("sheep") && i->get()->hp == 69)
        {
            sheep *a = (sheep *)(i->get());
            // auto baby = new sheep(this->window_surface_ptr_);
            babes.push_back(i->get()->pos);
            // this->animals.push_back(std::unique_ptr<animal>(baby));
            a->cooldown = 600;
            a->hp = 70;
        }
        if (i->get()->hp == 0)
            dead.push_back(i);
        // std::cout << i->get()->pos << std::endl;
        if (i->get()->pos.is_nan())
        {
            std::cout << i->get()->get_attribute() << std::endl;
        }
        i->get()->move();
        i->get()->draw();
    }
    for (auto &i : dead)
    {
        this->animals.erase(i);
    }

    for (auto &i : babes)
    {
        sheep *new_sheep = new sheep(this->window_surface_ptr_);
        new_sheep->pos = i;
        new_sheep->cooldown = 3600;
        this->animals.push_back(std::unique_ptr<animal>(new_sheep));
    }
    babes.clear();
    dead.clear();
    int nbsheep = 0;
    int wolf = 0;
    for (auto i = this->animals.begin(); i != this->animals.end(); i++)
    {
        if (i->get()->has_attribute("sheep"))
            nbsheep++;
        else if (i->get()->has_attribute("wolf"))
        {
            wolf++;
        }
    }
    if (nbsheep == 0 || wolf == 0)
        return true;
    if (this->frame == 60)
    {
        this->score = this->score + double(nbsheep);
        this->score = this->score / 2.0;
        this->frame = 0;
    }
    else
        this->frame++;
    return false;
}
/*
void ground::update()
{
    player->move();
    player->draw();

    doggo->move(this->player->pos);
    doggo->draw();
    // for (auto &animal : this->animals) {
    for (unsigned i = 0; i < this->animals.size(); ++i)
    {
        if (animals[i]->attribute.("sheep"))
            (sheep *)animals[i]->detect(this->animals);
        else if ()
    }
    (wolf *)animals[i]->detect(this->animals);
    this->animals[i]->move();
    this->animals[i]->draw();
    for (unsigned int i = 0; i < animals.size(); i++) // D’ailleurs, i++ ou
++i
?
{
    if (!animals[i]->has_attribute("sheep"))
        continue;
    sheep *a = animals[i];
    if (!a->male && a->cooldown == 0)
    {
        for (unsigned int k = 0; k < this->animals.size(); k++)
        {

        }
    }
}

for (unsigned int i = 0; i < animals.size(); ++i)
{
    if (!animals[i]->has_attribute("wolf"))
        continue;
    for (auto j = this->animals.begin(); j != this->animals.end(); ++j)
    {
        if (!animals[j]->has_attribute("sheep"))
            continue;
        if (vector_distance(animals[i]->pos, (*j)->pos) < 80)
        {
            this->animals.erase(j);
            break;
        }
    }
}
} // todo: "refresh the screen": Move animals and draw them
*/
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
    this->my_ground->animals.push_back(
        std::unique_ptr<animal>(new dog(this->window_surface_ptr_)));

    this->my_ground->animals.push_back(
        std::unique_ptr<animal>(new shepherd(this->window_surface_ptr_)));

    /*this->my_ground->animals.push_back(
        new sheep(this->window_surface_ptr_));*/

    for (unsigned i = 0; i < n_wolf; ++i)
        this->my_ground->animals.push_back(
            std::unique_ptr<animal>(new wolf(this->window_surface_ptr_)));
    // this->my_ground->animals.push_back(new
    // wolf(this->window_surface_ptr_));

    for (unsigned i = 0; i < n_sheep; ++i)
        this->my_ground->animals.push_back(
            std::unique_ptr<animal>(new sheep(this->window_surface_ptr_)));
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

        bool ending = this->my_ground->update();
        if (ending)
        {
            period = 0;
        }
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
