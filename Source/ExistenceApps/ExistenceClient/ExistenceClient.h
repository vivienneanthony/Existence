#ifndef EXISTENCECLIENT_H
#define EXISTENCECLIENT_H

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


#define DISPLAYMESH_MUILTIPLECHARACTER 1
#define DISPLAYMESH_SINGLECHARACTER 2

#define UIBLANK 0
#define UIMAIN  1

#define CAMERAORIENTATIONROTATEYAW 1
#define CAMERAORIENTATIONROTATEPITCH 1

#include "ExistenceApps.h"
#include "Account.h"
#include "factions.h"
#include "Player.h"
#include "../../Engine/Procedural/Rules.h"
#include <time.h>

#include "GameStateHandler.h"


/// This first example, maintaining tradition, prints a "Hello World" message.
/// Furthermore it shows:
///     - Using the Sample / Application classes, which initialize the Urho3D engine and run the main loop
///     - Adding a Text element to the graphical user interface
///     - Subscribing to and handling of update events
class ExistenceClient : public ExistenceApp
{
    OBJECT(ExistenceClient);




public:
    /// Construct.
    ExistenceClient(Context* context);

    /// Setup after engine initialization and before running the main loop.
    virtual void Start();

protected:
    /// Return XML patch instructions for screen joystick layout for a specific sample app, if any.https://github.com/urho3d/Urho3D/tree/master/Source/Samples
    virtual String GetScreenJoystickPatchString() const
    {
        return
            "<patch>"
            "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Hat0']]\">"
            "        <attribute name=\"Is Visible\" value=\"false\" />"
            "    </add>"
            "</patch>";
    }

private:

    /// Subscribe to application-wide logic update events.
    void SubscribeToEvents();
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Events Keyboard
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);


    // Diaplay login screen

    void SetupScreenViewport(void);
    void LoginUI(bool exist);

    void LoginScreenUILoginHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void LoginScreenUINewAccountHandleClosePressed(StringHash eventType, VariantMap& eventData);

   bool loadHUDFile(const char * filename, const int positionx, const int positiony);

    void ProgressScreenUI(void);
    void ProgressScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData);

    void CreatePlayerScreenUI(void);
    void CreatePlayerUIHandleClosePressed(StringHash eventType, VariantMap& eventData);

    void CreateAccountScreenUI(void);
    void CreateAccountUIHandleClosePressed(StringHash eventType, VariantMap& eventData);

    void CreatePlayerUIHandleControlClicked(StringHash eventType, VariantMap& eventData);

    int loadplayerMesh(Node * playermeshNode, int alienrace, int gender,int mode);

    int mainScreenUI(void);
    void MainScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void HandlerCameraOrientation(StringHash eventType, VariantMap& eventData);

    // file access
    void LoadAccount(void);
    void SaveAccount(accountinformation account);
    void SavePlayer(int writemode);

    void InitializeConsole(void);
    void HandleConsoleCommand(StringHash eventType, VariantMap& eventData);

    void HandleInput(const String& input);
    void eraseScene(void);

    void loadDummyScene(void);
    void loadScene(const int mode, const char * lineinput);
    void AddLogoViewport(void);
    void loadSceneUI(void);

    void MoveCamera(float timeStep);
    void CreateCharacter(void);
    void Print(const String& output);

    void CharacterSelectionHandler(VariantMap& eventData);
    int LoadCharacterMesh(String nodename, unsigned int alienrace, unsigned int gender);

    void HandlePostUpdates(StringHash eventType, VariantMap& eventData);
    void HandleMouseReleased(StringHash eventType, VariantMap& eventData);

    void HandlePersonalitySelectionItemClick(StringHash eventType, VariantMap& eventData);

    void HandleCharacterSelectedReleased(StringHash eventType, VariantMap& eventData);

    void CameraOrientationRotateMove (float degrees, int movement);
    void loadSceneCreationCreation(const char * lineinput);

    void UpdatePlayerInfoBar(void);

    int LoadAccountPlayers(void);

    int CreateCursor(void);

    int ConsoleActionEnvironment(const char * lineinput);
    int ConsoleActionCamera(const char * lineinput);
    int ConsoleActionDebug(const char * lineinput);
    int ConsoleActionCharacter(const char * lineinput);
    int ConsoleActionRenderer(const char * lineinput);
    int ConsoleActionBuild(const char * lineinput);

    void GenerateScene(terrain_rule terrainrule);
    int GenerateSceneBuildWorld(terrain_rule terrainrule);
    int GenerateSceneUpdateEnvironment(terrain_rule terrainrule);

    /// The Window.
    SharedPtr<Window> window_;
    SharedPtr<Window> window2_;

    /// The UI's root UIElement.
    SharedPtr<UIElement> uiRoot_;
    SharedPtr<Viewport> viewport;
    SharedPtr<RenderPath> effectRenderPath;

    /// Remembered drag begin position.
    IntVector2 dragBeginPosition_;

    /// Shared pointer for input
    SharedPtr<Input> input_;

    /// This is temoporarily the necessary code
    bool accountexist;

    /// Shared pointer for a single character
    WeakPtr<Character> character_;

    /// Game state handler
    Gamestatehandler  ExistenceGameState;

    /// Class and variable declation for character/player related information
    Player  TemporaryPlayer;
    Player  * TemporaryAccountPlayerList;
    unsigned int TemporaryAccountPlayerSelected;
    unsigned int TemporaryAccountPlayerListLimit;

    /// Class and variable declaration for alien race alliance information
    vector<string> aliensarray;
    vector<string> tempaliensarray;
};

#endif
