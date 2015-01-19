#ifndef WORLDBUILD_H
#define WORLDBUILD_H

#include <vector>

using namespace Urho3D;
using namespace std;

#define WorldOjectCollisionMapLimit  1000

/// Temporary structure
struct WorldOjectCollisionMap
{
    float size_x;
    float size_y;
    float size_z;
    float origin_x;
    float origin_y;
    float origin_z;
    int lod;
};

class WorldBuild : public LogicComponent
{
    /// Define subclass
    OBJECT(WorldBuild)

public:

    /// Construct.
    WorldBuild(Context* context);
    virtual ~WorldBuild();

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    int Init(void);

    /// public
    int CreateRockObjectAlongPath(float x, float z, float numberofobjects, float length);
    int CreateTreeObjectAlongPath(float x, float z, float numberofobjects, float length);

protected:

private:
    float ComputeDistance(int x1, int y1, int x2, int y2);

    /// Saved Collision Objects to 0
    int SaveCollisionObjects;

    /// Set world limit of objects to test for collision
    //WorldOjectCollisionMap  * CollisionBounds;
    vector <WorldOjectCollisionMap> CollisionBounds;;
};

#endif // WORLDBUILD_H
