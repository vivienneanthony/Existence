///Headers
#include <vector>
#include <cstdio>
#include <cmath>
#include <iostream>

/// Terraub Functions
double * getFieldSquareTerrain(int x0,int y0, int x1, int y1, unsigned int iterations);
double displace(unsigned int iterations,int x, int y);
double getMaxDeviation(unsigned int iterations);
double PRH(unsigned iterations, int x, int y);
double * getFieldDiamondSquaredMap(int x, int y, int width, int height, unsigned int iterations);


/// Main function
int main()
{
    int width=1024;
    int height=1024;
    int iterations=512;
    double * map=new double[width*height];

    map=getFieldDiamondSquaredMap(0,0,width,height,iterations);

    return 1;
}

double * getFieldDiamondSquaredMap(int x, int y, int width, int height, unsigned int iterations)
{
    /// Allocate memory for final mapdouble
    double * map = new double[width*height];

    map = getFieldSquareTerrain(x, y, x+width, y+height, iterations);

    double maxdeviation = getMaxDeviation(iterations);

    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int  y = 0; y < height; y++)
        {
            map[x*y] = map[x*y] / maxdeviation;
        }
    }
    return map;
}

double * getFieldSquareTerrain(int x0, int y0, int x1, int y1, unsigned int iterations)
{
    /// Define final height and width
    int finalwidth  = x1 - x0;
    int finalheight = y1 - y0;

    /// Allocate memory for final map
    double * finalmap = new double[finalwidth*finalheight];

    /// Test iteration
    if (iterations == 0)
    {
        for (unsigned int x = 0; x < finalwidth; x++)
        {
            for (unsigned int y = 0; y < finalheight; y++)
            {
                finalmap[x*y] =  displace(iterations,x0+x,y0+y) ;
            }
            return finalmap;
        }
    }

    /// Define upper height and width for upper maps
    int upper_x0=floor(x0/2) - 1;
    int upper_y0=floor(y0/2) - 1;
    int upper_x1=ceil(x1/2)+1;
    int upper_y1=ceil(y1/2)+1;

    int upper_xwidth= upper_x1-upper_x0;
    int upper_yheight= upper_y1-upper_y0;

    /// Allocate memory for upper map width and height
    double * uppermap = new double[upper_xwidth*upper_yheight];

    /// Pass another iteration
    uppermap = getFieldSquareTerrain(upper_x0, upper_y0, upper_x1, upper_y1, iterations-1);

    /// Define counter height and width
    int counter_x0= upper_x0 * 2;
    int counter_y0= upper_y0 * 2;

    int counter_width = upper_xwidth*2-1;
    int counter_height = upper_yheight*2-1;

    int xoff = x0 - counter_x0;
    int yoff = y0 - counter_y0;

    /// Allocate memory for currentmap using counter height and width
    double * currentmap = new double[counter_width*counter_height];

    /// Copy information to double map
    for (unsigned int x = 0; x < upper_xwidth; x++)
    {
        for (unsigned int y = 0; y< upper_yheight; y++)
        {
            currentmap[(x*2)*(y*2)] = uppermap[x*y];
        }
    }

    for (unsigned int x = 1; x < counter_width-1; x += 2)
    {
        for (unsigned int y = 1; y < counter_height-1; y += 2)
        {
            currentmap[x*y] = ((currentmap[(x - 1)*(y - 1)] + currentmap[(x - 1)*(y + 1)] + currentmap[(x + 1)*(y - 1)] + currentmap[(x + 1)*(y + 1)]) / 4) + displace(iterations,counter_x0+x,counter_y0+y);
        }
    }
    for (unsigned int x = 1; x < counter_width-1; x += 2)
    {
        for (unsigned int y = 2; y < counter_height-1; y += 2)
        {
            currentmap[x*y] = ((currentmap[(x - 1)*y]     + currentmap[(x + 1)*y]     + currentmap[x*(y - 1)]     + currentmap[x*(y + 1)]) / 4) + displace(iterations,counter_x0+x,counter_y0+y);
        }
    }
    for (unsigned int x = 2; x < counter_width-1; x += 2)
    {
        for (unsigned int y = 1; y < counter_height-1; y += 2)
        {
            currentmap[x*y] = ((currentmap[(x - 1)*y]     + currentmap[(x + 1)*y]     + currentmap[x*(y - 1)]     + currentmap[x*(y + 1)]) / 4) + displace(iterations,counter_x0+x,counter_y0+y);
        }
    }

    /// Copy actual information to returned map
    for (unsigned int x = 0; x < finalwidth; x++)
    {
        for (unsigned int y = 0; y < finalheight; y++)
        {
            finalmap[x*y] = currentmap[(x+xoff)*(y+yoff)];

        }
    }

    return finalmap;
}

/// Random function to offset
double displace(unsigned int  iterations, int x, int y)
{
    return (((PRH(iterations,x,y) - 0.5)*2)) / (iterations+1);
}

/// Get maximum deviations
double getMaxDeviation(unsigned int iterations)
{
    double dev = 0.5 / (iterations+1);
    if (iterations <= 0) return dev;
    return getMaxDeviation(iterations-1) + dev;
}

///This function returns the same result for given values but should be somewhat random.
double PRH(unsigned iterations, int x, int y)
{
    unsigned long long hash;
    x &= 0xFFF;
    y &= 0xFFF;
    iterations &= 0xFF;
    hash = (iterations << 24);
    hash |= (y << 12);
    hash |= x;
    unsigned long long rem = hash & 3;
    unsigned long long h = hash;

    switch (rem)
    {
    case 3:
        hash += h;
        hash ^= hash << 32;
        hash ^= h << 36;
        hash += hash >> 22;
        break;
    case 2:
        hash += h;
        hash ^= hash << 22;
        hash += hash >> 34;
        break;
    case 1:
        hash += h;
        hash ^= hash << 20;
        hash += hash >> 2;
    }
    hash ^= hash << 6;
    hash += hash >> 10;
    hash ^= hash << 8;
    hash += hash >> 34;
    hash ^= hash << 50;
    hash += hash >> 12;

    return (hash & 0xFFFF) / static_cast<double>(0xFFFF);
}

