#ifndef OLSENNOISE2D_H
#define OLSENNOISE2D_H

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

class OlsenNoise2D
{

public:
    OlsenNoise2D();
    virtual ~OlsenNoise2D();

    int * olsennoise(int iterations, int x, int y, int width, int height);

private:
    int hashrandom(long long int elements[3]);
    long long hash(long long v);

};

#endif // OLSENNOISE2D_H
