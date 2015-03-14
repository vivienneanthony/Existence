#ifndef MANAGER_H
#define MANAGER_H

#include "Object.h"
#include "Ptr.h"

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

    int SetScene(Scene* scene);
    int AddObject(int type, const char * name, float x, float y, float z, const char *filename);

    virtual ~Manager();
protected:
private:
  /// Scene pointer.
    WeakPtr<Scene> scene_;
};

#endif // MANAGER_H
