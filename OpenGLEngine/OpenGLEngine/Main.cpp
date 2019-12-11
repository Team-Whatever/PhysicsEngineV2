//#define STB_IMAGE_IMPLEMENTATION
#include "UpdateTransformMatricesSystem.h"
#include "RenderingSystem.h"
#include "RenderingSystemV2.h"
#include "InputEventSystem.h"
#include "RotateSystem.h"
#include "ParticleSystem.h"
#include "ParticleSpawnerSystem.h"
#include "GravityForceGeneratorSystem.h"
#include "FixedSpringForceGeneratorSystem.h"
#include "BuoyancyForceGeneratorSystem.h"
#include "ForceAccumulatorSystem.h"
#include "PairedSpringForceGeneratorSystem.h"
#include "SphereContactGeneratorSystem.h"
#include "ParticleContactResolutionSystem.h"
#include "CableComponentSystem.h"
#include "RodSystem.h"
#include "ForceAndTorqueAccumulatorSystem.h"
#include "RigidBodySystem.h"
#include "RigidbodyGravityForceGeneratorSystem.h"
#include "DebugDrawTriangleComponentSystem.h"
#include "ContactGenerationSystem.h"
#include "ContactResolutionSystem.h"
#include "SphereColliderSystem.h"
#include "BoxColliderSystem.h"
#include "MoveInBoundsSystem.h"
#include "FPSControlSystem.h"
#include "DynamicDirectionalLightSystem.h"
#include "DynamicPointLightSystem.h"
#include "DynamicSpotLightSystem.h"
#include "FlightSimulatorSystem.h"
#include "FollowCameraSystem.h"
#include "InfiniteSpawnSystem.h"
#include "InfiniteSpawnTargetSystem.h"
#include "AeroControlSystem.h"
#include "SetAerodynamicTensorSystem.h"
#include "AeroSystem.h"
#include "CameraLookSystem.h"
#include "LifeTimeSystem.h"
#include <string>
#include <stdlib.h>     
#include <time.h>      

#define DEBUG_LOG_LEVEL 3

using namespace Reality;

void LoadShaders(ECSWorld& world);
void LoadModels(ECSWorld& world);
void MakeABridge(ECSWorld& world);
void MakeFlight(ECSWorld& world);
void TestContacts(ECSWorld& world);
void TestCollision(ECSWorld& world);
void SetupLights(ECSWorld& world);

int main()
{
	ECSWorld world;

	// Init and Load
	world.data.InitRendering();
	//LoadAssets(world);
	
	world.data.renderUtil->camera.Position = Vector3(0, 15.0f, 100.0f);
	world.data.renderUtil->SetFOV(60);
	// Create entities

	// Make a player controller
	auto e = world.createEntity();
	e.addComponent<FPSControlComponent>();

	SetupLights(world);

	MakeFlight(world);
	//MakeABridge(world);
	//TestContacts(world);
	//TestCollision(world);

	// Create Systems
	world.getSystemManager().addSystem<UpdateTransformMatricesSystem>();
	world.getSystemManager().addSystem<RenderingSystem>();
	world.getSystemManager().addSystem<RenderingSystemV2>();
	world.getSystemManager().addSystem<InputEventSystem>();
	world.getSystemManager().addSystem<RotateSystem>();
	world.getSystemManager().addSystem<ParticleSystem>();
	world.getSystemManager().addSystem<ParticleSpawnerSystem>();
	world.getSystemManager().addSystem<GravityForceGeneratorSystem>();
	world.getSystemManager().addSystem<FixedSpringForceGeneratorSystem>();
	world.getSystemManager().addSystem<PairedSpringForceGeneratorSystem>();
	world.getSystemManager().addSystem<BuoyancyForceGeneratorSystem>();
	world.getSystemManager().addSystem<SphereContactGeneratorSystem>();
	world.getSystemManager().addSystem<CableComponentSystem>();
	world.getSystemManager().addSystem<RodSystem>();
	world.getSystemManager().addSystem<ParticleContactResolutionSystem>();
	world.getSystemManager().addSystem<ForceAccumulatorSystem>();
	world.getSystemManager().addSystem<ForceAndTorqueAccumulatorSystem>();
	world.getSystemManager().addSystem<FlightSimulatorSystem>();
	world.getSystemManager().addSystem<FPSControlSystem>();
	world.getSystemManager().addSystem<FollowCameraSystem>();
	world.getSystemManager().addSystem<CameraLookSystem>();
	world.getSystemManager().addSystem<InfiniteSpawnSystem>();
	world.getSystemManager().addSystem<InfiniteSpawnTargetSystem>();
	world.getSystemManager().addSystem<AeroControlSystem>();
	world.getSystemManager().addSystem<SetAerodynamicTensorSystem>();
	world.getSystemManager().addSystem<AeroSystem>();
	world.getSystemManager().addSystem<LifeTimeSystem>();
	world.getSystemManager().addSystem<MoveInBoundsSystem>();
	world.getSystemManager().addSystem<DynamicDirectionalLightSystem>();
	world.getSystemManager().addSystem<DynamicPointLightSystem>();
	world.getSystemManager().addSystem<DynamicSpotLightSystem>();
	world.getSystemManager().addSystem<DebugDrawTriangleComponentSystem>();

	// Rigidbody Physics
	rp3d::CollisionWorld rp3dWorld;
	world.getSystemManager().addSystem<RigidBodySystem>(rp3dWorld);
	world.getSystemManager().addSystem<ContactGenerationSystem>(rp3dWorld);
	world.getSystemManager().addSystem<ContactResolutionSystem>(rp3dWorld);
	world.getSystemManager().addSystem<SphereColliderSystem>(rp3dWorld);
	world.getSystemManager().addSystem<BoxColliderSystem>(rp3dWorld);
	world.getSystemManager().addSystem<RigidbodyGravityForceGeneratorSystem>();



	float time = glfwGetTime();
	float stepTime = glfwGetTime();
	float deltaTime = 0;
	float elapsedDeltaTime = 0;
	float logicDelta = 0;
	float debugDelta = 0;
	bool spawn = false;
	auto player = world.createEntity();

	LoadShaders(world);
	bool shadersLoaded = false;
	bool modelsLoadStarted = false;
	// game loop
	// -----------
	while (!glfwWindowShouldClose(world.data.renderUtil->window->glfwWindow))
	{
		float current = glfwGetTime();
		deltaTime = current - time;
		deltaTime = 1 / 60.0f;
		time = glfwGetTime();

		world.update();

		// Poll OpenGl events
		glfwPollEvents();

		world.data.renderUtil->ClearDisplay(world.data.renderUtil->window->glfwWindow);

		// Load
		if (!shadersLoaded)
		{
			shadersLoaded = world.data.assetLoader->ShadersLoaded();
		}
		if(shadersLoaded && !modelsLoadStarted)
		{
			LoadModels(world);
			modelsLoadStarted = true;
		}
		// Update View
		world.data.renderUtil->UpdateViewMatrix();
		// Process Input
		world.getSystemManager().getSystem<InputEventSystem>().Update(deltaTime);

		// Game Logic Update
		world.getSystemManager().getSystem<FPSControlSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RotateSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<ParticleSpawnerSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<MoveInBoundsSystem>().Update(deltaTime);

		//Flight Sim
		world.getSystemManager().getSystem<FlightSimulatorSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<FollowCameraSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<CameraLookSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<InfiniteSpawnTargetSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<InfiniteSpawnSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<AeroControlSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<SetAerodynamicTensorSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<LifeTimeSystem>().Update(deltaTime);

		// Update Transform
		world.getSystemManager().getSystem<UpdateTransformMatricesSystem>().Update(deltaTime);
		// Physics
		//float fixedDeltaTime = glfwGetKey(world.data.renderUtil->window->glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS ? 1 / 60.0f : 0;		
		float fixedDeltaTime = 1 / 60.0f;
		//world.getSystemManager().getSystem<AeroSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<AeroSystem>().Update(fixedDeltaTime);
		// Particle Force Generators
		world.getSystemManager().getSystem<GravityForceGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<FixedSpringForceGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<PairedSpringForceGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<ForceAccumulatorSystem>().Update(fixedDeltaTime);
		// -----------------------------
		world.getSystemManager().getSystem<BuoyancyForceGeneratorSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<ParticleSystem>().Update(fixedDeltaTime);
		// Rigiidbody Force Generators and collisions
		world.getSystemManager().getSystem<RigidbodyGravityForceGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<ForceAndTorqueAccumulatorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<RigidBodySystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<SphereColliderSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<BoxColliderSystem>().Update(fixedDeltaTime);
		// Physics Solvers
		world.getSystemManager().getSystem<SphereContactGeneratorSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<CableComponentSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<RodSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<ParticleContactResolutionSystem>().Update(fixedDeltaTime);

		world.getSystemManager().getSystem<ContactGenerationSystem>().Update(fixedDeltaTime);
		world.getSystemManager().getSystem<ContactResolutionSystem>().Update(fixedDeltaTime);

		// Rendering Update
		world.getSystemManager().getSystem<DebugDrawTriangleComponentSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<DynamicDirectionalLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<DynamicPointLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<DynamicSpotLightSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RenderingSystem>().Update(deltaTime);
		world.getSystemManager().getSystem<RenderingSystemV2>().Update(deltaTime);

		elapsedDeltaTime = glfwGetTime() - time;
		logicDelta = elapsedDeltaTime - world.data.renderUtil->GetRenderDelta();
		stepTime = glfwGetTime();

		// Debug
		if (DEBUG_LOG_LEVEL > 0)
		{
			world.data.renderUtil->RenderText("FPS : " + std::to_string((int)round(1.0f / deltaTime)), 1810.0f, 1060.0f, 0.5f, Color(0, 1, 1, 1));
		}
		if (DEBUG_LOG_LEVEL > 1)
		{
			int logic = (int)round(logicDelta * 100.0f / deltaTime);
			std::string logicString = logic < 10 ? " " + std::to_string(logic) : std::to_string(logic);
			int render = (int)round(world.data.renderUtil->GetRenderDelta() * 100.0f / deltaTime);
			std::string renderString = logic < 10 ? " " + std::to_string(render) : std::to_string(render);
			int debug = (int)round(debugDelta * 100.0f / deltaTime);
			std::string debugString = logic < 10 ? " " + std::to_string(debug) : std::to_string(debug);
			
			world.data.renderUtil->RenderText("Logic : " + logicString + "%" +
				//+ " | Physics : " + std::to_string((int)round(physicsDelta * 100.0f / deltaTime)) + "%" +
				+ " | Rendering : " + renderString + "%" +
				+ " | Debug : " + debugString + "%"
				, 1680.0f, 1040.0f, 0.25f, Color(0, 1, 1, 1));
		}
		if (DEBUG_LOG_LEVEL > 2)
		{
			world.data.renderUtil->RenderText("Draw Calls : " + std::to_string(world.data.renderUtil->GetDrawCalls())
				+ " | Verts : " + std::to_string(world.data.renderUtil->GetVerts())
				+ " | Tris : " + std::to_string(world.data.renderUtil->GetTris())
				+ " | Lines : " + std::to_string(world.data.renderUtil->GetLines())
				, 1610.0f, 1020.0f, 0.25f, Color(0, 1, 1, 1));
		}

		// Update debug delta
		debugDelta = glfwGetTime() - stepTime;
		stepTime = glfwGetTime();

		world.data.renderUtil->SwapBuffers(world.data.renderUtil->window->glfwWindow);

		// Show FPS in console
		//std::cout << "FPS : " << 1.0f / deltaTime << std::endl;


		//TODO Uncomment for bridge
		if (glfwGetKey(world.data.renderUtil->window->glfwWindow, GLFW_KEY_E) == GLFW_PRESS && !spawn)
		{
			spawn = true;
			player.kill();
			player = world.createEntity();
			player.addComponent<TransformComponent>(Vector3(RANDOM_FLOAT(-20.0F, 20.0F), 40, 5));
			player.addComponent<SphereComponent>(1);
			player.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 1);


			std::cout << "ball supposed to drop..." << std::endl;

		}
		else if (glfwGetKey(world.data.renderUtil->window->glfwWindow, GLFW_KEY_E) == GLFW_RELEASE) {
			spawn = false;
		}
		world.data.renderUtil->DrawSphere(glm::vec3(10, 40, 0), 20.0f, Color::Red);
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void LoadShaders(ECSWorld& world)
{
	world.data.assetLoader->StartShaderLoading({ {"Shaders/Lighting_Maps.vs", "Shaders/Lighting_Maps.fs"} });
}
void LoadModels(ECSWorld& world)
{
	world.data.assetLoader->StartModelLoading({
		//ModelData("Resources/Models/snowy-mountain-terrain/SnowyMountainMesh.obj"),
		//ModelData("Resources/Models/Sponza-master/sponza.obj"),
		//ModelData("Resources/Models/nanosuit/nanosuit.obj"),*/
		ModelData("Resources/Models/supermarine-spitfire/spitfire.fbx",
			{{"spitfire_d.png"}})
		});
}

void MakeFlight(ECSWorld& world)
{
	auto e = world.createEntity();
	glm::vec3 rotationInRads = glm::vec3(glm::radians(-90.0f),
		glm::radians(180.0f), glm::radians(0.0f));
	Quaternion orientation = glm::quat(rotationInRads);
	e.addComponent<TransformComponentV2>(Vector3(0, 30.0f, 0), Vector3(0.10f, 0.1f, 0.1f));
	// Add mesh
	e.addComponent<ModelComponent>("Resources/Models/supermarine-spitfire/spitfire.fbx", Vector3(0, -50, 20), Vector3(-90, 0, 0));
	e.addComponent<RigidBodyComponent>(10.0f ,0.3f, 0.5f);
	e.addComponent<FlighSimulatorComponent>();
	e.addComponent<FollowCameraComponent>(Vector3(0.0f, 15.0f, 40.0f));
	e.addComponent<CameraLookComponent>();
	e.addComponent<InfiniteSpawnTargetComponent>();
	
	Vector3 scale = Vector3(30, 30, 30);
	auto b = world.createEntity();
	b.addComponent<TransformComponentV2>(Vector3(0, 350.0f, 0), Vector3(0.10f, 0.1f, 0.1f));
	b.addComponent<BuoyancyComponent>(scale.y * 0.5f, 10, scale.y, 100, e);

	//std::vector<int> p1 { GLFW_KEY_E };
	//std::vector<int> n1 { GLFW_KEY_Q };
	////Right Wing
	//auto RW = world.createEntity();
	//RW.addComponent<AeroControlComponent>(p1, n1);
	//RW.addComponent<AeroMinMaxComponent>(Mat3(0, 0, 0, 0, 0.000f, 0, 0, -0.0005f, 0),
	//	Mat3(0, 0, 0, 0, 0, 0, 0, 0, 0),
	//	Mat3(0, 0, 0, 0, -0.000f, 0, 0, 0.0005f, 0));
	//RW.addComponent<AeroComponent>(e, Mat3(1.0f), Vector3(100.0f, 0, 50.0f));

	////Left Wing
	//auto LW = world.createEntity();
	//LW.addComponent<AeroControlComponent>(n1, p1);
	//LW.addComponent<AeroMinMaxComponent>(Mat3(0, 0, 0, 0, 0.000f, 0, 0, -0.0005f, 0),
	//	Mat3(0, 0, 0, 0, 0, 0, 0, 0, 0),
	//	Mat3(0, 0, 0, 0, -0.000f, 0, 0, 0.0005f, 0));
	//LW.addComponent<AeroComponent>(e, Mat3(1.0f), Vector3(-100.0f, 0, 50.0f));

	//Rudder 
	std::vector<int> pR = { GLFW_KEY_A };
	std::vector<int> nR = { GLFW_KEY_D };
	auto R = world.createEntity();
	R.addComponent<AeroControlComponent>(pR, nR, 0.001f);
	R.addComponent<AeroMinMaxComponent>(Mat3(0, 0, 0, 0, 0, 0, 0.002f, 0, 0),
		Mat3(0, 0, 0, 0, 0, 0, 0.00f, 0, 0),
		Mat3(0, 0, 0, 0, 0, 0, -0.002f, 0, 0));
	R.addComponent<AeroComponent>(e, Mat3(1.0f), Vector3(0, 0, -200.0f));

	////Back Wing
	//std::vector<int> p2{ GLFW_KEY_W };
	//std::vector<int> n2{ GLFW_KEY_S };
	//auto RW2 = world.createEntity();
	//RW2.addComponent<AeroControlComponent>(p2, n2);
	//RW2.addComponent<AeroMinMaxComponent>(Mat3(0, 0, 0, 0, 0, 0, 0, -0.0015f, 0),
	//	Mat3(0, 0, 0, 0, 0, 0, 0, 0, 0),
	//	Mat3(0, 0, 0, 0, 0, 0, 0, 0.0015f, 0));
	//RW2.addComponent<AeroComponent>(e, Mat3(1.0f), Vector3(0.0f, 0, -200.0f));

	for (int i = -40; i <= 40; i++)
	{
		auto buildingR = world.createEntity();
		buildingR.addComponent<TransformComponentV2>(Vector3(100.0f, 0.0f, 50.0f * i));
		buildingR.addComponent<InfiniteSpawnComponent>(RANDOM_FLOAT(100.0f, 500.0f));

		auto buildingL = world.createEntity();
		buildingL.addComponent<TransformComponentV2>(Vector3(-100.0f, 0.0f, 50.0f * i));
		buildingL.addComponent<InfiniteSpawnComponent>(RANDOM_FLOAT(100.0f, 500.0f));
	}
}

void MakeABridge(ECSWorld & world)
{
	// ------------------------------------------------------------------------
	auto eFixed2 = world.createEntity();
	eFixed2.addComponent<TransformComponent>(Vector3(20, 40, 0));
	eFixed2.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed2.addComponent<SphereComponent>(1);

	auto eLowerPoint2 = world.createEntity();
	eLowerPoint2.addComponent<TransformComponent>(Vector3(20, 30, 0));
	eLowerPoint2.addComponent<SphereComponent>(1);
	eLowerPoint2.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable2 = world.createEntity();
	eCable2.addComponent<CableComponent>(eFixed2, eLowerPoint2, 10);


	// ------------------------------------------------------------------------
	auto eFixed3 = world.createEntity();
	eFixed3.addComponent<TransformComponent>(Vector3(10, 40, 0));
	eFixed3.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed3.addComponent<SphereComponent>(1);

	auto eLowerPoint3 = world.createEntity();
	eLowerPoint3.addComponent<TransformComponent>(Vector3(10, 30, 0));
	eLowerPoint3.addComponent<SphereComponent>(1);
	eLowerPoint3.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable3 = world.createEntity();
	eCable3.addComponent<CableComponent>(eFixed3, eLowerPoint3, 10);


	// ------------------------------------------------------------------------
	auto eFixed4 = world.createEntity();
	eFixed4.addComponent<TransformComponent>(Vector3(0, 40, 0));
	eFixed4.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed4.addComponent<SphereComponent>(1);

	auto eLowerPoint4 = world.createEntity();
	eLowerPoint4.addComponent<TransformComponent>(Vector3(0, 30, 0));
	eLowerPoint4.addComponent<SphereComponent>(1);
	eLowerPoint4.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable4 = world.createEntity();
	eCable4.addComponent<CableComponent>(eFixed4, eLowerPoint4, 10);


	// ------------------------------------------------------------------------
	auto eFixed5 = world.createEntity();
	eFixed5.addComponent<TransformComponent>(Vector3(-10, 40, 0));
	eFixed5.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed5.addComponent<SphereComponent>(1);

	auto eLowerPoint5 = world.createEntity();
	eLowerPoint5.addComponent<TransformComponent>(Vector3(-10, 30, 0));
	eLowerPoint5.addComponent<SphereComponent>(1);
	eLowerPoint5.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable5 = world.createEntity();
	eCable5.addComponent<CableComponent>(eFixed5, eLowerPoint5, 10);


	// ------------------------------------------------------------------------
	auto eFixed6 = world.createEntity();
	eFixed6.addComponent<TransformComponent>(Vector3(-20, 40, 0));
	eFixed6.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed6.addComponent<SphereComponent>(1);

	auto eLowerPoint6 = world.createEntity();
	eLowerPoint6.addComponent<TransformComponent>(Vector3(-20, 30, 0));
	eLowerPoint6.addComponent<SphereComponent>(1);
	eLowerPoint6.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable6 = world.createEntity();
	eCable6.addComponent<CableComponent>(eFixed6, eLowerPoint6, 10);

	auto eRod2 = world.createEntity();
	eRod2.addComponent<RodComponent>(eFixed2, eFixed3, 10);
	auto eRod3 = world.createEntity();
	eRod3.addComponent<RodComponent>(eFixed3, eFixed4, 10);
	auto eRod4 = world.createEntity();
	eRod4.addComponent<RodComponent>(eFixed4, eFixed5, 10);
	auto eRod5 = world.createEntity();
	eRod5.addComponent<RodComponent>(eFixed5, eFixed6, 10);

	// ------------------------------------------------------------------------
	auto eFixed12 = world.createEntity();
	eFixed12.addComponent<TransformComponent>(Vector3(20, 40, 10));
	eFixed12.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed12.addComponent<SphereComponent>(1);

	auto eLowerPoint12 = world.createEntity();
	eLowerPoint12.addComponent<TransformComponent>(Vector3(20, 30, 10));
	eLowerPoint12.addComponent<SphereComponent>(1);
	eLowerPoint12.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable12 = world.createEntity();
	eCable12.addComponent<CableComponent>(eFixed12, eLowerPoint12, 10);

	// ------------------------------------------------------------------------
	auto eFixed13 = world.createEntity();
	eFixed13.addComponent<TransformComponent>(Vector3(10, 40, 10));
	eFixed13.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed13.addComponent<SphereComponent>(1);

	auto eLowerPoint13 = world.createEntity();
	eLowerPoint13.addComponent<TransformComponent>(Vector3(10, 30, 10));
	eLowerPoint13.addComponent<SphereComponent>(1);
	eLowerPoint13.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable13 = world.createEntity();
	eCable13.addComponent<CableComponent>(eFixed13, eLowerPoint13, 10);

	// ------------------------------------------------------------------------
	auto eFixed14 = world.createEntity();
	eFixed14.addComponent<TransformComponent>(Vector3(0, 40, 10));
	eFixed14.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed14.addComponent<SphereComponent>(1);

	auto eLowerPoint14 = world.createEntity();
	eLowerPoint14.addComponent<TransformComponent>(Vector3(0, 30, 10));
	eLowerPoint14.addComponent<SphereComponent>(1);
	eLowerPoint14.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable14 = world.createEntity();
	eCable14.addComponent<CableComponent>(eFixed14, eLowerPoint14, 10);

	// ------------------------------------------------------------------------
	auto eFixed15 = world.createEntity();
	eFixed15.addComponent<TransformComponent>(Vector3(-10, 40, 10));
	eFixed15.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed15.addComponent<SphereComponent>(1);

	auto eLowerPoint15 = world.createEntity();
	eLowerPoint15.addComponent<TransformComponent>(Vector3(-10, 30, 10));
	eLowerPoint15.addComponent<SphereComponent>(1);
	eLowerPoint15.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable15 = world.createEntity();
	eCable15.addComponent<CableComponent>(eFixed15, eLowerPoint15, 10);

	// ------------------------------------------------------------------------
	auto eFixed16 = world.createEntity();
	eFixed16.addComponent<TransformComponent>(Vector3(-20, 40, 10));
	eFixed16.addComponent<ParticleComponent>(10000000, Vector3(0, 0, 0), 0);
	eFixed16.addComponent<SphereComponent>(1);

	auto eLowerPoint16 = world.createEntity();
	eLowerPoint16.addComponent<TransformComponent>(Vector3(-20, 30, 10));
	eLowerPoint16.addComponent<SphereComponent>(1);
	eLowerPoint16.addComponent<ParticleComponent>(5, Vector3(0, 0, 0), 0);

	auto eCable16 = world.createEntity();
	eCable16.addComponent<CableComponent>(eFixed16, eLowerPoint16, 10);


	auto eRod12 = world.createEntity();
	eRod12.addComponent<RodComponent>(eFixed12, eFixed13, 10);
	auto eRod13 = world.createEntity();
	eRod13.addComponent<RodComponent>(eFixed13, eFixed14, 10);
	auto eRod14 = world.createEntity();
	eRod14.addComponent<RodComponent>(eFixed14, eFixed15, 10);
	auto eRod15 = world.createEntity();
	eRod15.addComponent<RodComponent>(eFixed15, eFixed16, 10);


	const glm::vec3 point1 = glm::vec3(eLowerPoint2.getComponent<TransformComponent>().position.x, eLowerPoint2.getComponent<TransformComponent>().position.y, eLowerPoint2.getComponent<TransformComponent>().position.z);
	const glm::vec3 point2 = glm::vec3(eLowerPoint3.getComponent<TransformComponent>().position.x, eLowerPoint3.getComponent<TransformComponent>().position.y, eLowerPoint3.getComponent<TransformComponent>().position.z);
	const glm::vec3 point3 = glm::vec3(eLowerPoint4.getComponent<TransformComponent>().position.x, eLowerPoint4.getComponent<TransformComponent>().position.y, eLowerPoint4.getComponent<TransformComponent>().position.z);
	const glm::vec3 point4 = glm::vec3(eLowerPoint5.getComponent<TransformComponent>().position.x, eLowerPoint5.getComponent<TransformComponent>().position.y, eLowerPoint5.getComponent<TransformComponent>().position.z);
	const glm::vec3 point5 = glm::vec3(eLowerPoint6.getComponent<TransformComponent>().position.x, eLowerPoint6.getComponent<TransformComponent>().position.y, eLowerPoint6.getComponent<TransformComponent>().position.z);


	const glm::vec3 point12 = glm::vec3(eLowerPoint12.getComponent<TransformComponent>().position.x, eLowerPoint12.getComponent<TransformComponent>().position.y, eLowerPoint12.getComponent<TransformComponent>().position.z);
	const glm::vec3 point13 = glm::vec3(eLowerPoint13.getComponent<TransformComponent>().position.x, eLowerPoint13.getComponent<TransformComponent>().position.y, eLowerPoint13.getComponent<TransformComponent>().position.z);
	const glm::vec3 point14 = glm::vec3(eLowerPoint14.getComponent<TransformComponent>().position.x, eLowerPoint14.getComponent<TransformComponent>().position.y, eLowerPoint14.getComponent<TransformComponent>().position.z);
	const glm::vec3 point15 = glm::vec3(eLowerPoint15.getComponent<TransformComponent>().position.x, eLowerPoint15.getComponent<TransformComponent>().position.y, eLowerPoint15.getComponent<TransformComponent>().position.z);
	const glm::vec3 point16 = glm::vec3(eLowerPoint16.getComponent<TransformComponent>().position.x, eLowerPoint16.getComponent<TransformComponent>().position.y, eLowerPoint16.getComponent<TransformComponent>().position.z);

	Color bridgeColour = Color::Cyan;

	auto triangle1 = world.createEntity();
	triangle1.addComponent<DebugDrawTriangleComponent>(point1, point2, point12, bridgeColour);
	auto triangle2 = world.createEntity();
	triangle2.addComponent<DebugDrawTriangleComponent>(point2, point13, point12, bridgeColour);
	auto triangle3 = world.createEntity();
	triangle3.addComponent<DebugDrawTriangleComponent>(point2, point3, point14, bridgeColour);
	auto triangle4 = world.createEntity();
	triangle4.addComponent<DebugDrawTriangleComponent>(point2, point13, point14, bridgeColour);
	auto triangle5 = world.createEntity();
	triangle5.addComponent<DebugDrawTriangleComponent>(point4, point3, point14, bridgeColour);
	auto triangle6 = world.createEntity();
	triangle6.addComponent<DebugDrawTriangleComponent>(point4, point15, point14, bridgeColour);
	auto triangle7 = world.createEntity();
	triangle7.addComponent<DebugDrawTriangleComponent>(point4, point5, point15, bridgeColour);
	auto triangle8 = world.createEntity();
	triangle8.addComponent<DebugDrawTriangleComponent>(point5, point15, point16, bridgeColour);

}


void TestContacts(ECSWorld& world)
{
	for (int i = 0; i < 30; i++)
	{
		auto e = world.createEntity();
		e.addComponent<TransformComponentV2>(Vector3(RANDOM_FLOAT(-200.0f, 200.0f), RANDOM_FLOAT(-200.0f, 200.0f), RANDOM_FLOAT(-200.0f, 200.0f)),
			Vector3(1, 1, 1),
			Vector3(RANDOM_FLOAT(-180.0f, 180.0f), RANDOM_FLOAT(-180.0f, 180.0f), RANDOM_FLOAT(-180.0f, 180.0f)));
		e.addComponent<RigidBodyComponent>();
		e.addComponent<MoveInBoundsComponent>(Vector3(RANDOM_FLOAT(-10.0f, 10.0f), RANDOM_FLOAT(-10.0f, 10.0f), RANDOM_FLOAT(-10.0f, 10.0f)),
			Vector3(200, 200, 200));
		auto col = world.createEntity();
		if ((RANDOM_FLOAT(0.0f, 1.0f) >= 0.5f))
		{
			col.addComponent<SphereColliderComponent>(e, RANDOM_FLOAT(10.0f, 50.0f));
		}
		else
		{
			col.addComponent<BoxColliderComponent>(e, Vector3(RANDOM_FLOAT(30.0f, 70.0f), RANDOM_FLOAT(30.0f, 70.0f), RANDOM_FLOAT(30.0f, 70.0f)));
		}
	}
	/*for (int i = 0; i < 2; i++)
	{
		auto e = world.createEntity();
		e.addComponent<TransformComponentV2>(Vector3(50 * ( i % 2 == 0 ? -1 : 1), 0, 0));
		e.addComponent<RigidBodyComponent>();
		e.addComponent<MoveInBoundsComponent>(Vector3(10 * (i % 2 == 0 ? 1 : -1), 0, 0), Vector3(100, 100, 100));
		auto col = world.createEntity();
		col.addComponent<SphereColliderComponent>(e, 30);
	}*/
}

void TestCollision(ECSWorld& world)
{
	// Floor 1
	auto floor1 = world.createEntity();
	floor1.addComponent<TransformComponentV2>(Vector3(0, -50, 0), Vector3(1, 1, 1), Vector3(0, 0, 0));
	floor1.addComponent<RigidBodyComponent>(100000.0f, 0.4f, 0.3f, Vector3(0, 0, 0), Vector3(0, 0, 0), 0);
	auto floorCol1 = world.createEntity();
	floorCol1.addComponent<BoxColliderComponent>(floor1, Vector3(1000, 10, 1000));

	// Floor 2
	/*auto floor2 = world.createEntity();
	floor2.addComponent<TransformComponentV2>(Vector3(80, -50, 0), Vector3(1, 1, 1), Vector3(0, 0, 30));
	floor2.addComponent<RigidBodyComponent>(10000.0f, 0.0f, 0.0f, Vector3(0, 0, 0), Vector3(0, 0, 0), 0);
	auto floorCol2 = world.createEntity();
	floorCol2.addComponent<BoxColliderComponent>(floor2, Vector3(300, 10, 300));*/

	//// Object 1
	//auto object1 = world.createEntity();
	//object1.addComponent<TransformComponentV2>(Vector3(-30, 50, 0));
	//object1.addComponent<RigidBodyComponent>();
	//auto objectCol1 = world.createEntity();
	//objectCol1.addComponent<SphereColliderComponent>(object1, 10);

	// Object 2
	for (int i = 0; i < 40; i++)
	{
		auto object2 = world.createEntity();
		object2.addComponent<TransformComponentV2>(Vector3(RANDOM_FLOAT(-50.0f, 50.0f), 50, RANDOM_FLOAT(-50.0f, 50.0f)), Vector3(1, 1, 1), Vector3(RANDOM_FLOAT(0, 180), RANDOM_FLOAT(0, 180), RANDOM_FLOAT(0, 180)));
		object2.addComponent<RigidBodyComponent>(10.0f, 0.1f, 0.1f, Vector3(0, 0, 0), Vector3(0, 0, 0), 5);
		auto objectCol2 = world.createEntity();
		objectCol2.addComponent<BoxColliderComponent>(object2, Vector3(10, 10, 10));
	}
}

void SetupLights(ECSWorld& world)
{
	auto l = world.createEntity();
	l.addComponent<TransformComponent>(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(90, 0, 0));
	l.addComponent<DynamicDirectionalLightComponent>(Color(0.00, 0.0, 0), Color::White, Color::Orange);

	// Lanterns
	auto pl1 = world.createEntity();
	pl1.addComponent<TransformComponent>(Vector3(22, 14, 48.5f));
	pl1.addComponent<DynamicPointLightComponent>(100.0f, Color(0.1, 0, 0), Color(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f));
	pl1.addComponent<ParticleComponent>();
	auto hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(23, 15, 48.0f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl1);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22, 13.5f, 50.5f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl1);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(21, 12.5f, 47.5f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl1);

	auto pl2 = world.createEntity();
	pl2.addComponent<TransformComponent>(Vector3(-14.5f, 14, 49.0f));
	pl2.addComponent<DynamicPointLightComponent>(100.0f, Color(0, 0, 0.1f), Color(0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f));
	pl2.addComponent<ParticleComponent>();
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f + 1, 14 - 1, 49.0f - 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl2);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 0.5f, 14 + 1, 49.0f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl2);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f, 14 - 1, 49.0f + 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl2);
	
	auto pl3 = world.createEntity();
	pl3.addComponent<TransformComponent>(Vector3(22, 14, -62.0f));
	pl3.addComponent<DynamicPointLightComponent>(100.0f, Color(0, 0.1f, 0), Color(0.0f, 1.0f, 0.0f), Color(0.0f, 1.0f, 0.0f));
	pl3.addComponent<ParticleComponent>();
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22 - 1, 14 - 1, -62.0f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl3);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22, 14 + 0.5f, -62.0f - 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl3);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(22 + 1, 14, -62.0f + 0.5f));
	hook.addComponent<FixedSpringComponent>(5, 1, pl3);

	auto pl4 = world.createEntity();
	pl4.addComponent<TransformComponent>(Vector3(-14.5f, 14, -61.5f));
	pl4.addComponent<DynamicPointLightComponent>(100.0f, Color(0.1, 0.05, 0), Color(1.0f, 0.55f, 0.0f), Color(1.0f, 0.55f, 0.0f));
	pl4.addComponent<ParticleComponent>();
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 1, 14, -61.5f -1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl4);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f - 0.25f, 14 - 0.5f, -61.5f + 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl4);
	hook = world.createEntity();
	hook.addComponent<TransformComponent>(Vector3(-14.5f + 0.5f, 14+ 1, -61.5f + 1));
	hook.addComponent<FixedSpringComponent>(5, 1, pl4);

	// Spears
	std::vector<Color> cols = { Color(1,0,0), Color(0,1,0), Color(0,0,1), Color(0.7f,0.55f,0) };
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pl1 = world.createEntity();
			pl1.addComponent<TransformComponent>(Vector3((i % 2 == 0 ? 8 : -1), 85, 49.5f - 37 * j), Vector3(1, 1, 1), Vector3(180, 0, 0));
			pl1.addComponent<DynamicSpotLightComponent>(10.0f, 100, Color(0, 0, 0), cols[3 - j], cols[3 - j], 5);
			pl1.addComponent<RotateComponent>((i % 2 == 0 ? 1 : -1) * 100,100,100);
		}
	}
}