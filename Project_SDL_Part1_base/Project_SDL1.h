﻿// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>
// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 1400; // Width of window in pixel
constexpr unsigned frame_height = 900; // Height of window in pixel
// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

// Helper function to initialize SDL
void init();

struct Vector2
{
    float x;
    float y;
};

Vector2 sub_vector(const Vector2 &a, const Vector2 &b);
Vector2 mul_vector(const Vector2 &a, float b);
float vector_norm(const Vector2 &a);
float vector_distance(const Vector2 &a, const Vector2 &b);
Vector2 normalize_vector(const Vector2 &a);

class animal
{
private:
    SDL_Surface *window_surface_ptr_; // ptr to the surface on which we want the
                                      // animal to be drawn, also non-owning
    // todo: Attribute(s) to define its position
    std::string file_path;

protected:
    SDL_Surface *image_ptr_; // The texture of the sheep (the loaded image), use
                             // load_surface_for

    float speed_norm = 5;

public:
    Vector2 pos;
    Vector2 spd;

public:
    explicit animal(const std::string &file_path,
                    SDL_Surface *window_surface_ptr);
    // todo: The constructor has to load the sdl_surface that corresponds to the
    // texture
    ~animal(); // todo: Use the destructor to release memory and "clean up
               // behind you"

    void draw(); // todo: Draw the animal on the screen <-> window_surface_ptr.
                 // Note that this function is not virtual, it does not depend
                 // on the static type of the instance

    virtual void move(){}; // todo: Animals move around, but in a different
                           // fashion depending on which type of animal
};

class shepherd : public animal
{
public:
    shepherd(SDL_Surface *window_surface_ptr);
    void move() final;
};

// Insert here:
class sheep;

class wolf : public animal
{
public:
    wolf(SDL_Surface *window_surface_ptr)
        : animal::animal("media/wolf.png", window_surface_ptr)
    {}
    void move() final;
    void detect(std::vector<sheep *> sheeplist);
    // implement functions that are purely virtual in base class
};
// class sheep, derived from animal
class sheep : public animal
{
public:
    bool male = false;
    long cooldown;

public:
    sheep(SDL_Surface *window_surface_ptr);
    void move() final;
    void detect(std::vector<wolf *> wolfslist);
    // implement functions that are purely virtual in base class
};

class dog : public animal
{
public:
    dog(SDL_Surface *window_surface_ptr)
        : animal::animal("media/doggo.png", window_surface_ptr)
    {}
    void move(const Vector2 &player);
};

// The "ground" on which all the animals live (like the std::vector
// in the zoo example).
class ground
{
private:
    // Attention, NON-OWNING ptr, again to the screen
    SDL_Surface *window_surface_ptr_;

    // Some attribute to store all the wolves and sheep
    // here

public:
    std::vector<wolf *> wolfs;
    std::vector<sheep *> sheeps;
    shepherd *player;
    ground(SDL_Surface *window_surface_ptr); // todo: Ctor
    ~ground(); // todo: Dtor, again for clean up (if necessary)
    void add_animal(animal *a, bool issheep); // todo: Add an animal
    void update(); // todo: "refresh the screen": Move animals and draw them
    // Possibly other methods, depends on your implementation
};

// The application class, which is in charge of generating the window
class application
{
private:
    // The following are OWNING ptrs
    SDL_Window *window_ptr_;
    SDL_Surface *window_surface_ptr_;
    SDL_Event window_event_;

    // Other attributes here, for example an instance of ground
    ground *my_ground;

public:
    application(unsigned int n_sheep, unsigned int n_wolf); // Ctor
    ~application(); // dtor

    int loop(unsigned int period); // main loop of the application.
                                   // this ensures that the screen is updated
                                   // at the correct rate.
                                   // See SDL_GetTicks() and SDL_Delay() to
                                   // enforce a duration the application should
                                   // terminate after 'period' seconds
};
