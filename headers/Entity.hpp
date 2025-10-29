#pragma once

#include "Logger.hpp"
#include "Renderer.hpp"
#include "Timer.hpp"


struct Size
{
    float width;
    float height;
};

struct Vec2
{
    float x;
    float y;
};

struct Position
{
    Vec2 current;
    Vec2 previous;
};

class Entity
{
public:

    Entity();
    virtual ~Entity();

    void render(Renderer* renderer, float lerp);
    void start();
    bool started();
    void stop();
    void updateBegin();
    void update(double time);

    int         direction_;
    Position    position_;
    Vec2        movement_;
    Size        size_;
    float       speed_;

    uint64_t    changeTimeStamp;
    uint64_t    changeTimeStampPrev;
};