#ifndef EnvironmentBuild_H
#define EnvironmentBuild_H

#include "Image.h"
#include <vector>
#include "../../Engine/Procedural/Procedural.h"
#include "../../Engine/Procedural/Rules.h"
#include "../../Engine/Procedural/RandomNumberGenerator.h"

namespace Urho3D
{
class Geometry;
class Drawable;
class Light;
class Material;
class Pass;
class Technique;
class Octree;
class Graphics;
class RenderPath;
class RenderSurface;
class ResourceCache;
class Skeleton;
class OcclusionBuffer;
class Texture2D;
class TextureCube;
class View;
class Zone;
class Scene;
}

using namespace Urho3D;
using namespace std;


/// Define objects
#define WorldOjectCollisionMapLimit  1000

/// Define building blocks
#define EnvironmentBuild_ROCKS            0
#define EnvironmentBuild_TREES            50
#define EnvironmentBuild_ICESPIKES        100

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

class EnvironmentBuild : public LogicComponent
{
    /// Define subclass
    OBJECT(EnvironmentBuild)

public:

    /// Construct.
    EnvironmentBuild(Context* context);
    virtual ~EnvironmentBuild();

    /// Register object factory and attributes.
    static void RegisterNewSubsystem(Context* context);
    void SetScene(Scene *scene);

    int Initialize(void);

    /// public will remove
    int CreateRockObjectAlongPath(float x, float z, float numberofobjects, float length);
    int CreateTreeObjectAlongPath(float worldsize_x, float worldsize_y, float x, float z, float numberofobjects, float length, Image * terrainHeightMap);
    int CreateObjectsAlongPath(int objecttypes, float worldsize_x, float worldsize_y, float x, float z, float numberofobjects, float length, Image * terrainHeightMap);
    int GenerateWorldObjects(const time_t &timeseed,  terrain_rule terrainrule);
    int GenerateGrass2( float worldsize_x, float worldsize_y, Image * terrainHeightMap);

protected:

private:
    float ComputeDistance(int x1, int y1, int x2, int y2);

    /// Saved Collision Objects to 0
    int SaveCollisionObjects;

    /// Set world limit of objects to test for collision
    vector <WorldOjectCollisionMap> CollisionBounds;;
    terrain_rule terrainrules;
    RandomNumberGenerator RandomRule;

    /// Scene.
    Scene * scene_;

};

#endif // EnvironmentBuild_H
