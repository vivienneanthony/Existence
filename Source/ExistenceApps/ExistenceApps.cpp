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

#include "Application.h"
#include "Camera.h"
#include "Console.h"
#include "Cursor.h"
#include "DebugHud.h"
#include "Engine.h"
#include "FileSystem.h"
#include "Graphics.h"
#include "Input.h"
#include "InputEvents.h"
#include "Renderer.h"
#include "ResourceCache.h"
#include "Scene.h"
#include "SceneEvents.h"
#include "Sprite.h"
#include "Texture2D.h"
#include "Timer.h"
#include "UI.h"
#include "XMLFile.h"
#include "ExistenceApps.h"

ExistenceApp::ExistenceApp(Context* context) :
    Application(context),
    yaw_(0.0f),
    pitch_(0.0f),
    touchEnabled_(false),
    screenJoystickIndex_(M_MAX_UNSIGNED),
    screenJoystickSettingsIndex_(M_MAX_UNSIGNED),
    paused_(false)
{
}

void ExistenceApp::Setup()
{
    // Modify engine startup parameters
    engineParameters_["WindowTitle"] = GetTypeName();
    engineParameters_["LogName"]     = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"]  = true;
    engineParameters_["Headless"]    = false;
}

void ExistenceApp::Start()
{

    // Set custom window Title & Icon
    SetWindowTitleAndIcon();

    // Create console and debug HUD
    CreateConsoleAndDebugHud();

    // Subscribe scene update event
    SubscribeToEvent(E_SCENEUPDATE, HANDLER(ExistenceApp, HandleSceneUpdate));
}



void ExistenceApp::SetWindowTitleAndIcon()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();
    Image* icon = cache->GetResource<Image>("Textures/UrhoIcon.png");
    graphics->SetWindowIcon(icon);
    graphics->SetWindowTitle("Urho3D ExistenceApp");
}

void ExistenceApp::CreateConsoleAndDebugHud()
{
    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
}


void ExistenceApp::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    // Move the camera by touch, if the camera node is initialized by descendant ExistenceApp class
    if (touchEnabled_ && cameraNode_)
    {
        Input* input = GetSubsystem<Input>();
        for (unsigned i = 0; i < input->GetNumTouches(); ++i)
        {
            TouchState* state = input->GetTouch(i);
            if (!state->touchedElement_)    // Touch on empty space
            {
                if (state->delta_.x_ ||state->delta_.y_)
                {
                    Camera* camera = cameraNode_->GetComponent<Camera>();
                    if (!camera)
                        return;

                    Graphics* graphics = GetSubsystem<Graphics>();
                    yaw_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.x_;
                    pitch_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.y_;

                    // Construct new orientation for the camera scene node from yaw and pitch; roll is fixed to zero
                    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
                }
                else
                {
                    // Move the cursor to the touch position
                    Cursor* cursor = GetSubsystem<UI>()->GetCursor();
                    if (cursor && cursor->IsVisible())
                        cursor->SetPosition(state->position_);
                }
            }
        }
    }
}
