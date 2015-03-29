#ifndef MANAGER_H
#define MANAGER_H

#include "Object.h"
#include "Ptr.h"
#include <vector>

#define MANAGESTATICMODEL 0
#define MANAGELIGHT 100


#define COLLISION_BOX       0
#define COLLISION_CONVEX    1
#define COLLISION_TRIANGLE  2


using namespace Urho3D;
using namespace std;

class URHO3D_API Manager : public LogicComponent
{
    OBJECT(Manager);
public:
    Manager(Context* context);
    static void RegisterNewSubsystem(Context* context);

    virtual ~Manager();

    /// Setscene
    void SetScene(Scene * scene);

    /// Add Objects
    int AddObject(int type, const char * name, float x, float y, float z, const char *filename, unsigned int physics=false);

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
