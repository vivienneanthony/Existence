#ifndef PROCEDURAL_RULES_H
#define PROCEDURAL_RULES_H

struct terrain_rule
{
    float moutainrange; // value 0.0f to 1.0f
    float cratersdeep; // deep craters
    time_t timeseed;
};

struct planet_rule
{
    time_t timeseed;
};

#endif

