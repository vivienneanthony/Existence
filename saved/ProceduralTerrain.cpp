#include "ProceduralTerrain.h"

using namespace Urho3D;

ProceduralTerrain::ProceduralTerrain(Context* context)
LogicComponent(context),
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}


ProceduralTerrain::~ProceduralTerrain()
{
    //dtor
}

void ProceduralTerrain::RegisterObject(Context* context)
{
    context->RegisterFactory<Character>();

}


