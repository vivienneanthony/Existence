#ifndef PROCEDURALTERRAIN_H
#define PROCEDURALTERRAIN_H

//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

//#include "Player.h"
#include "Controls.h"
#include "LogicComponent.h"
#include "Context.h"

using namespace Urho3D;


struct proceduralterrain
{

    int worldtype; /// set worldtype
    int subworldtype;
    float sealevel; /// sealevel
    long int timeseed;  /// timeseed

    unsigned long long int creationtime;

    /// required data
    int width;
    int height;

    /// octave information
    int oct_octaves;
    float oct_persistence;
    bool oct_override;

    float oct_o1;
    float oct_o2;
    float oct_o3;
    float oct_o4;
    float oct_o5;
    float oct_o6;
    float oct_o7;
    float oct_o8;

    int offsetx;
    int offsety;
};

class ProceduralTerrain : public LogicComponent
{
    OBJECT(ProceduralTerrain)
public:
    ProceduralTerrain(Context *content);
    ProceduralTerrain();
    virtual ~ProceduralTerrain();

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    void SetOctaves(float override, int octaves, float persistence,  float o1, float o2, float o3, float o4, float o5, float o6, float o7, float o8);
    void SetDimensions(int x, int y);
    void SetWorldType(int worldtypevalue, int subworldtypevalue, float sealevelvalue, unsigned long int creationtimevalue);
    void Initialize(void);
    proceduralterrain Get(void);

protected:
private:
    int worldtype; /// set worldtype
    int subworldtype;
    float sealevel; /// sealevel
    long int timeseed;  /// timeseed

    unsigned long long int creationtime;

    /// required data
    int width;
    int height;

    /// octave information
    int oct_octaves;
    float oct_persistence;
    bool oct_override;

    float oct_o1;
    float oct_o2;
    float oct_o3;
    float oct_o4;
    float oct_o5;
    float oct_o6;
    float oct_o7;
    float oct_o8;

};

#endif // PROCEDURALTERRAIN_H
