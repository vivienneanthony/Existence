/// Random Number Generator
/// Based off Chris code with some additional commands
/// Vivienne Anthony 2015

#include <iostream>
#include <ctime>

#include "RandomNumberGenerator.h"


using namespace std;

////
/// Code and Functions
///
RandomNumberGenerator::RandomNumberGenerator()
{
    /// blank
}

RandomNumberGenerator::~RandomNumberGenerator()
{
    /// blank
}

void RandomNumberGenerator::SetRandomSeed(unsigned long long int seed)
{
    randomSeed = seed;

    return;
}

int RandomNumberGenerator:: Rand()
{
    randomSeed = randomSeed * 214013 + 2531011;


    return (randomSeed >> 16) & 32767;
}

int RandomNumberGenerator:: RandRange(unsigned long long int range)
{
    randomSeed = randomSeed * 214013 + 2531011;


    long long int value=(randomSeed >> 16) & 32767;

    return (((value-0)*range)/32767)+0;

}

float RandomNumberGenerator:: RandStandardNormal(void)
{
    float val = 0.0f;
    for (int i = 0; i < 12; i++)
        val += Rand() / 32768.0f;
    val -= 6.0f;

    /// Now val is approximatly standard normal distributed
    return val;
}

