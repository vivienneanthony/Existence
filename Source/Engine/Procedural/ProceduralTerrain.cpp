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



#include "CoreEvents.h"
#include "Engine.h"
#include "ProcessUtils.h"
#include "Octree.h"
#include "Model.h"
#include "Material.h"
#include "ResourceCache.h"
#include "Graphics.h"

#include "ProceduralTerrain.h"

using namespace Urho3D;

ProceduralTerrain::ProceduralTerrain(Context* context):
    LogicComponent(context)
    , worldtype(0)
    , subworldtype(0)
    , sealevel(0.5f)
    , creationtime(0)
    , width(0)
    , height(0)
    , oct_octaves(0)
    , oct_persistence(0.50)
    , oct_override(false)
    , oct_o1(0.0f)
    , oct_o2(0.0f)
    , oct_o3(0.0f)
    , oct_o4(0.0f)
    , oct_o5(0.0f)
    , oct_o6(0.0f)
    , oct_o7(0.0f)
    , oct_o8(0.0f)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}


ProceduralTerrain::~ProceduralTerrain()
{
    //dtor
}

/// Move all of these to proceduralTerrain
void ProceduralTerrain::RegisterObject(Context* context)
{
    context->RegisterFactory<ProceduralTerrain>();

    /// set defaults
    ATTRIBUTE("World Type", int, worldtype, 0.0f, AM_DEFAULT);
    ATTRIBUTE("Sub World Type", int, subworldtype, 0.0f, AM_DEFAULT);
    ATTRIBUTE("Sea Level", float, sealevel, 0.5f, AM_DEFAULT);

    ATTRIBUTE("Creation", int, creationtime,0,AM_DEFAULT);

    /// required data
    ATTRIBUTE("Width", int, width,0,AM_DEFAULT);
    ATTRIBUTE("Height", int, height,0,AM_DEFAULT);

    /// octave information
    ATTRIBUTE("Override", bool, oct_override,false,AM_DEFAULT);
    ATTRIBUTE("Octaves", int, oct_octaves,0,AM_DEFAULT);
    ATTRIBUTE("Base Persistence",float,oct_persistence,0.50f,AM_DEFAULT);

    ATTRIBUTE("Octave 1", float,oct_o1,0.0f,AM_DEFAULT);
    ATTRIBUTE("Octave 2", float,oct_o2,0.0f,AM_DEFAULT);
    ATTRIBUTE("Octave 3", float,oct_o3,0.0f,AM_DEFAULT);
    ATTRIBUTE("Octave 4", float,oct_o4,0.0f,AM_DEFAULT);
    ATTRIBUTE("Octave 5", float,oct_o5,0.0f,AM_DEFAULT);
    ATTRIBUTE("Octave 6", float,oct_o6,0.0f,AM_DEFAULT);
    ATTRIBUTE("Octave 7", float,oct_o7,0.0f,AM_DEFAULT);
    ATTRIBUTE("Octave 8", float,oct_o8,0.0f,AM_DEFAULT);
}

void ProceduralTerrain::Initialize(void)
{
    return;
}

void ProceduralTerrain::SetWorldType(int worldtypevalue, int subworldtypevalue, float sealevelvalue, unsigned long int creationtimevalue)
{

    /// copy sealevel
    sealevel=sealevelvalue;

    if(sealevelvalue<0.0f)
    {
        sealevel=0.0f;
    }
    if(sealevelvalue>1.0f)
    {
        sealevel=1.0f;
    }

    worldtype=worldtypevalue;
    subworldtype=subworldtypevalue;
    creationtime=creationtimevalue;
}
void ProceduralTerrain::SetDimensions(int x, int y)
{

    /// Set width and height
    width=x;
    height=y;

    return;
}

/// Set Octaves
void ProceduralTerrain::SetOctaves(float override, int octaves, float persistence, float o1, float o2, float o3, float o4, float o5, float o6, float o7, float o8)
{
    /// Copy Octaves
    oct_octaves=octaves;
    if(octaves<=0)
    {
        oct_octaves=1;
    }
    if(octaves>8)
    {
        oct_octaves=8;
    }

    /// Copy Persistence
    oct_persistence=persistence;
    if(persistence<0.0f)
    {
        oct_persistence=0.0f;
    }
    if(persistence>1.0f)
    {
        oct_persistence=1.0f;
    }

    /// Copy ovveride
    oct_override=override;
    if(override<0)
    {
        oct_override=false;
    }


    if(octaves>1)
    {
        oct_override=true;
    }


    /// Copy Octave 1
    oct_o1=o1;
    if(o1<0.0f)
    {
        oct_o1=0.0f;
    }
    if(o1>1.0f)
    {
        oct_o1=1.0f;
    }

    /// Copy Octave 2
    oct_o2=o2;
    if(o2<0.0f)
    {
        oct_o2=0.0f;
    }
    if(o2>1.0f)
    {
        oct_o2=1.0f;
    }

    /// Copy Octave 3
    oct_o3=o3;
    if(o3<0.0f)
    {
        oct_o3=0.0f;
    }
    if(o3>1.0f)
    {
        oct_o3=1.0f;
    }

    /// Copy Octave 3
    oct_o4=o4;
    if(o4<0.0f)
    {
        oct_o4=0.0f;
    }
    if(o4>1.0f)
    {
        oct_o4=1.0f;
    }

    /// Copy Octave 1
    oct_o1=o1;
    if(o5<0.0f)
    {
        oct_o5=0.0f;
    }
    if(o5>1.0f)
    {
        oct_o5=1.0f;
    }

    /// Copy Octave 2
    oct_o2=o2;
    if(o6<0.0f)
    {
        oct_o6=0.0f;
    }
    if(o6>1.0f)
    {
        oct_o6=1.0f;
    }

    /// Copy Octave 3
    oct_o7=o7;
    if(o7<0.0f)
    {
        oct_o7=0.0f;
    }
    if(o7>1.0f)
    {
        oct_o7=1.0f;
    }

    /// Copy Octave 3
    oct_o8=o8;
    if(o8<0.0f)
    {
        oct_o8=0.0f;
    }
    if(o8>1.0f)
    {
        oct_o8=1.0f;
    }


    return;
}

/// Return settings
proceduralterrain ProceduralTerrain::Get(void)
{
    /// create structure to return
    proceduralterrain result;

    /// copy basic
    result.worldtype=worldtype;
    result.subworldtype=subworldtype;
    result.sealevel=sealevel;

    result.creationtime=creationtime;

    /// required data
    result.width=width;
    result.height=height;

    /// octave information
    result.oct_octaves=oct_octaves;
    result.oct_persistence=oct_persistence;
    result.oct_override=oct_override;

    result.oct_o1=oct_o1;
    result.oct_o2=oct_o2;
    result.oct_o3=oct_o3;
    result.oct_o4=oct_o4;
    result.oct_o5=oct_o5;
    result.oct_o6=oct_o6;
    result.oct_o7=oct_o7;
    result.oct_o8=oct_o8;


    return result;
}
