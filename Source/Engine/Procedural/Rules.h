#ifndef PROCEDURAL_RULES_H
#define PROCEDURAL_RULES_H

/// Define world types
#define WORLD_DESERT       0
#define WORLD_TERRAIN       50
#define WORLD_WATER         75
#define WORLD_ICE           100

struct terrain_rule
{
    int worldtype;
    float moutainrange; // value 0.0f to 1.0f
    float cratersdeep; // deep craters
    time_t timeseed;
};

struct planet_rule
{
    int worldtype;
    time_t timeseed;
};

#endif

