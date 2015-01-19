// Water example.
// This sample demonstrates:
//     - Creating a large plane to represent a water body for rendering
//     - Setting up a second camera to render reflections on the water surface

#include "Scripts/Utilities/Sample.as"

Scene@ scene_;
Node@ cameraNode;
Node@ terrainNode;
float yaw = 0.0f;
float pitch = 0.0f;

Terrain@ terrain;

bool isSwitchTriplanar = true;

Text@ instructionText;

void Start()
{
    // Execute the common startup for samples
    SampleStart();

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateInstructions();

    // Setup the viewports for displaying the scene and rendering the water reflection
    SetupViewports();

    // Hook up to the frame update and render post-update events
    SubscribeToEvents();
}

void CreateScene()
{
    scene_ = Scene();

    // Create octree, use default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
    scene_.CreateComponent("Octree");
    scene_.CreateComponent("DebugRenderer");

    // Create a Zone component for ambient lighting & fog control
    Node@ zoneNode = scene_.CreateChild("Zone");
    Zone@ zone = zoneNode.CreateComponent("Zone");
    zone.boundingBox = BoundingBox(-1000.0f, 1000.0f);
    zone.ambientColor = Color(0.15f, 0.15f, 0.15f);
    zone.fogColor = Color(1.0f, 1.0f, 1.0f);
    zone.fogStart = 500.0f;
    zone.fogEnd = 750.0f;

    // Create a directional light to the world. Enable cascaded shadows on it
    Node@ lightNode = scene_.CreateChild("DirectionalLight");
    lightNode.direction = Vector3(0.3f, -0.5f, 0.425f);
    Light@ light = lightNode.CreateComponent("Light");
    light.lightType = LIGHT_DIRECTIONAL;
    light.castShadows = true;
    light.shadowBias = BiasParameters(0.00025f, 0.5f);
    light.shadowCascade = CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f);
    light.specularIntensity = 0.5f;
    // Apply slightly overbright lighting to match the skybox
    light.color = Color(1.2f, 1.2f, 1.2f);

    // Create skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
    // illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
    // generate the necessary 3D texture coordinates for cube mapping
    Node@ skyNode = scene_.CreateChild("Sky");
    skyNode.SetScale(500.0); // The scale actually does not matter
    Skybox@ skybox = skyNode.CreateComponent("Skybox");
    skybox.model = cache.GetResource("Model", "Models/Box.mdl");
    skybox.material = cache.GetResource("Material", "Materials/Skybox.xml");

    // Create heightmap terrain
    terrainNode = scene_.CreateChild("Terrain");
    terrainNode.position = Vector3(0.0f, 0.0f, 0.0f);
    terrain = terrainNode.CreateComponent("Terrain");
    terrain.patchSize = 64;
    terrain.spacing = Vector3(2.0f, 2.f, 2.0f); // Spacing between vertices and vertical resolution of the height map
    terrain.smoothing = true;

	terrain.heightMap = cache.GetResource("Image", "Textures/HeightMap.png");
    /*terrain.heightMap = cache.GetResource("Image", "Textures/HeightMapTriPlanar.png");*/
	
    terrain.material = cache.GetResource("Material", "Materials/TerrainTriPlanar.xml");
    // The terrain consists of large triangles, which fits well for occlusion rendering, as a hill can occlude all
    // terrain patches and other objects behind it
    terrain.occluder = true;

    // Create the camera. Set far clip to match the fog. Note: now we actually create the camera node outside
    // the scene, because we want it to be unaffected by scene load / save
    cameraNode = Node();
    Camera@ camera = cameraNode.CreateComponent("Camera");
    camera.farClip = 750.0f;

    // Set an initial position for the camera scene node above the ground
    cameraNode.position = Vector3(0.0f, 150.0f, 400.0f);
}

void CreateInstructions()
{
    // Construct new Text object, set string to display and font to use
    instructionText = ui.root.CreateChild("Text");
    //instructionText.text = "Use WASD keys and mouse to move. SPACE to switch " + (!isSwitchTriplanar ? "Tri-Planar" : "Regular" + " shader");
	ChangeInstructionText();
    instructionText.SetFont(cache.GetResource("Font", "Fonts/Anonymous Pro.ttf"), 15);
    instructionText.textAlignment = HA_CENTER;

    // Position the text relative to the screen center
    instructionText.horizontalAlignment = HA_CENTER;
    instructionText.verticalAlignment = VA_CENTER;
    instructionText.SetPosition(0, ui.root.height / 4);
}

void ChangeInstructionText(){
	instructionText.text = "Use WASD keys and mouse to move. SPACE to switch " + (!isSwitchTriplanar ? "Tri-Planar" : "Regular") + " shader";
}

void SetupViewports()
{
    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    Viewport@ viewport = Viewport(scene_, cameraNode.GetComponent("Camera"));
    renderer.viewports[0] = viewport;

}


void SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent("Update", "HandleUpdate");
    // Subscribe key down event
    SubscribeToEvent("KeyDown", "HandleKeyDownDemo");
}

void MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (ui.focusElement !is null)
        return;

    // Movement speed as world units per second
    const float MOVE_SPEED = 30.0;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    IntVector2 mouseMove = input.mouseMove;
    yaw += MOUSE_SENSITIVITY * mouseMove.x;
    pitch += MOUSE_SENSITIVITY * mouseMove.y;
    pitch = Clamp(pitch, -90.0, 90.0);

    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode.rotation = Quaternion(pitch, yaw, 0.0);

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input.keyDown['W'])
        cameraNode.TranslateRelative(Vector3(0.0f, 0.0f, 1.0f) * MOVE_SPEED * timeStep);
    if (input.keyDown['S'])
        cameraNode.TranslateRelative(Vector3(0.0f, 0.0f, -1.0f) * MOVE_SPEED * timeStep);
    if (input.keyDown['A'])
        cameraNode.TranslateRelative(Vector3(-1.0f, 0.0f, 0.0f) * MOVE_SPEED * timeStep);
    if (input.keyDown['D'])
        cameraNode.TranslateRelative(Vector3(1.0f, 0.0f, 0.0f) * MOVE_SPEED * timeStep);

	

}

void HandleKeyDownDemo(StringHash eventType, VariantMap& eventData)
{
    int key = eventData["Key"].GetInt();
    bool repeat = eventData["Repeat"].GetBool();


    if (key == KEY_SPACE && !repeat) // Switch terrain shader
    {
        isSwitchTriplanar = !isSwitchTriplanar;
        if(isSwitchTriplanar)
            terrain.material = cache.GetResource("Material", "Materials/TerrainTriPlanar.xml");
        else
            terrain.material = cache.GetResource("Material", "Materials/Terrain.xml");
		ChangeInstructionText();
	}


	if (key == KEY_UP)
        terrainNode.TranslateRelative(Vector3(0.0f, 0.0f, 1.0f));
	if (key == KEY_DOWN)
        terrainNode.TranslateRelative(Vector3(0.0f, 0.0f, -1.0f));
	if (key == KEY_LEFT)
        terrainNode.TranslateRelative(Vector3(-1.0f, 0.0f, 0.0f));
	if (key == KEY_RIGHT)
        terrainNode.TranslateRelative(Vector3(1.0f, 0.0f, 0.0f));
	
    HandleKeyDown(eventType, eventData);
}

void HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    // Take the frame time step, which is stored as a float
    float timeStep = eventData["TimeStep"].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);
}

// Create XML patch instructions for screen joystick layout specific to this sample app
String patchInstructions = "";