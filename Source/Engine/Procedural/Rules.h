#ifndef PROCEDURAL_RULES_H
#define PROCEDURAL_RULES_H

/// Define world types
#define WORLD_DESERT       0
#define WORLD_TERRAIN       50
#define WORLD_WATER         75
#define WORLD_ICE           100

#define SUBWORLD_TERRAIN       0
#define SUBWORLD_ALIEN         1

struct terrain_rule
{
    int worldtype;      // world type
    int subworldtype;
    float sealevel;
    unsigned long long int creationtime;
};

struct planet_rule
{
    int worldtype;
    unsigned long long int creationtime;
};

#endif

