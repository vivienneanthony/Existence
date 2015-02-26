/*
 * Original Author @author Tat
 * Mod code Vivienne Anthony
 * c++ rewrite vivienne (WIP)
 * verion 1.0 Functional Rewrite
 *
 * Changes
 *  Java to C++ conversion
 *  Memory pointer usage
 */

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include "OlsenNoise2D.h"

using namespace std;

OlsenNoise2D::OlsenNoise2D()
{
    //ctor
}

OlsenNoise2D::~OlsenNoise2D()
{
    //dtor
}

int * OlsenNoise2D::olsennoise(int iterations, int x, int y, int width, int height)
{
    int maxiterations = iterations;
    int cx=0, cy=0;
    int cxh=0, cyh=0;
    int cwidth, cheight=0;
    int xoff=0, yoff=0;
    int nwidth=0, nheight=0;
    int nx=0, ny=0;
    int nxh=0, nyh=0;
    int m=0;
    int n=0;
    int fieldwidth=0;
    int fieldheight=0;

    int * field = NULL;

    /// add buffer

    x+=2;
    y+=2;
    height+=2;
    width+=2;

    for (int iteration = 0; iteration < maxiterations; iteration++)
    {
        nx = x;
        ny = y;

        nxh = x + width;
        nyh = y + height;

        for (int i = 1,n = maxiterations - iteration; i < n; i++)
        {
            nx = (nx / 2) - 1;
            ny = (ny / 2) - 1;
            nxh = 1 -(-nxh/2);
            nyh = 1 -(-nyh/2);
        }

        xoff = -2*((nx/2)) + nx + 1;
        yoff = -2*((ny/2)) + ny + 1;

        cx = (nx / 2) - 1;
        cy = (ny / 2) - 1;
        cxh = 1 -(-nxh/2);
        cyh = 1 -(-nyh/2);

        nwidth = nxh - nx;
        nheight = nyh - ny;

        cwidth = cxh - cx;
        cheight = cyh - cy;

        /// rest
        fieldwidth=cwidth;
        fieldheight=cheight;

        /// Only happens once
        if (field==NULL)
        {
            /// allocate memory
            field = new int[cheight * cwidth];

            /// blank value
            for (int x = 0; x < cwidth; x++)
            {
                for (int y = 0; y < cheight; y++)
                {
                    field[x+(y*cwidth)]=0;
                }
            }
        }

        /// First loop
        for (int j = 0, m=cwidth; j < m; j++)
        {
            for (int k = 0, n=cheight; k < n; k++)
            {

                long long int elements[3];

                elements[0]=(long long int )cx + j;
                elements[1]=(long long int )cy + k;
                elements[2]=(long long int )iteration;

                field[j+(k*m)] += (hashrandom (elements) & (1 << (7 - iteration)));
            }
        }

        /// Up sampled
        int * upsampled = new int[(fieldwidth*2)*(fieldheight*2)];
        long int upsampledsize=(fieldwidth*2)*(fieldheight*2);

        /// Clear up sampled
        for (int j = 0, m=fieldwidth*2; j < m; j++)
        {
            for (int k = 0,n=fieldheight*2; k < n; k++)
            {
                upsampled[j+(k*m)] = 0;
            }
        }


        for (int j = 0, m=fieldwidth*2; j < m; j++)
        {
            for (int k = 0,n=fieldheight*2; k < n; k++)
            {
                upsampled[j+(k*m)] = field[(j / 2)+((k / 2)*fieldwidth)];
            }
        }

        /// Allocate upsampled to field
        delete [] field;
        field = upsampled;

        /// Copy new fieldwieght and fieldheight
        fieldwidth=fieldwidth*2;
        fieldheight=fieldheight*2;





        /// Blur field
        int * blurfield =new int[(fieldwidth-2)*(fieldheight-2)];
        long int blurfieldsize = (fieldwidth-2)*(fieldheight-2);


/// Clear blur field
        for (int j = 0,m=fieldwidth-2; j < m; j++)
        {
            for (int k = 0, n=fieldheight-2;  k < n; k++)
            {
                blurfield[j+(k*m)] =0;
            }
        }


        for (int j = 0,m=fieldwidth-2; j < m; j++)
        {
            for (int k = 0, n=fieldheight-2;  k < n; k++)
            {
                for (int h = 0; h < 9; h++)
                {
                    blurfield[j+(k*m)] += field[(j + (h % 3))+((k+(h/ 3))*fieldwidth)];
                }
                blurfield[j+(k*m)] /= 9;
            }
        }

        /// Allocate blurfield to field
        delete [] field;
        field = blurfield;

        /// Copy new fieldwieght and fieldheight
        fieldwidth=fieldwidth-2;
        fieldheight=fieldheight-2;

        /// Trim field
        int * trimfield = new int[nwidth*nheight];
        long int trimfieldsize = nwidth*nheight;

/// Clear trim field
        for (int j = 0, m=nwidth; j < m; j++)
        {
            for (int k = 0, n=nheight; k < n; k++)
            {
                trimfield[j+(k*m)] = 0;
            }
        }


        for (int j = 0, m=nwidth; j < m; j++)
        {
            for (int k = 0, n=nheight; k < n; k++)
            {
                trimfield[j+(k*m)] = field[(j + xoff)+((k + yoff)*fieldwidth)];
            }
        }

        /// Allocate trimfield to field
        delete [] field;
        field = trimfield;
    }


    /// Trim field

    width-=2;
    height-=2;

    int * newtrimfield = new int[width*height];
    long int newtrimfieldsize = width*height;


    for (int j = 1, m=width+1; j < m; j++)
    {
        for (int k = 1, n=height+1; k < n; k++)
        {
            newtrimfield[(j-1)+((k-1)*width)] =field[j+(k*(width+2))];
        }
    }

    /// Allocate newtrimfield to field
    delete [] field;
    field = newtrimfield;


    return field;
}

int OlsenNoise2D::hashrandom(long long int elements[3])
{
    long long hashcalc = 0;


    for (int i = 0; i < 3; i++)
    {
        hashcalc ^= elements[i];
        hashcalc = hash(hashcalc);
    }
    return (int) hashcalc;
};

long long OlsenNoise2D::hash(long long v)
{
    long long hash = v;
    long long h = hash;

    switch ((int) hash & 3)
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
    return hash;
};


