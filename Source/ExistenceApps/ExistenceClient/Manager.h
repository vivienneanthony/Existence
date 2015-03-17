#ifndef MANAGER_H
#define MANAGER_H

#include "Object.h"
#include "Ptr.h"
#include <vector>

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

    void SetScene(Scene * scene);

    int AddObject(int type, const char * name, float x, float y, float z, const char *filename);

    int SaveScene(int mode);


protected:

private:
    /// Node related functions
    int SaveSceneNode(Node * node, XMLElement parentelement);
    int SaveSceneNodeComponents(Node *node, XMLElement parentelement);

    /// Weak Pointer children
    Vector<SharedPtr<Node> > children_;

     /// Scene.
    Scene * scene_;
      XMLFile * savesceneexportxml;
      XMLElement configElem;
};

#endif // MANAGER_H
