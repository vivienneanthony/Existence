/// Random Number Generator
/// Based off Chris code with some additional commands
/// Vivienne Anthony 2015
#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

#include <iostream>
#include <ctime>

using namespace std;

class RandomNumberGenerator
{
public:

    void SetRandomSeed(unsigned long long int seed);
    int Rand();
    int RandRange(unsigned long long int range);
    float RandStandardNormal(void);

    RandomNumberGenerator();
    ~RandomNumberGenerator();
private:
    unsigned long long int randomSeed;
};

#endif
