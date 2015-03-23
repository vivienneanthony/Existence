#ifndef MANAGER_H
#define MANAGER_H

#include "Object.h"
#include "Ptr.h"
#include <vector>

#define MANAGESTATICMODEL 0
#define MANAGELIGHT 100

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

class URHO3D_API Manager : public Object
{
    OBJECT(Manager);
public:
    Manager(Context* context);
    static void RegisterNewSubsystem(Context* context);

    virtual ~Manager();

    /// Setscene
    void SetScene(Scene * scene);

    /// Add Objects
    int AddObject(int type, const char * name, float x, float y, float z, const char *filename, bool physics=false);

    /// public
    int AddGeneratedObject(Node * node);

    /// Save nodes
    int SaveManagedNodes(const char *filename);
    int SaveManagedGeneratedNodes(const char *filename);

    int LoadManagedNodes(const char *filename);

protected:

private:

    /// create a vector list of managed nodes
    Vector <Node *> ManagedNodes;
    Vector <Node *> ManagedGeneratedNodes;

     /// Scene.
    Scene * scene_;


};

#endif // MANAGER_H
