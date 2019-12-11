#include "DebugDrawTriangleComponentSystem.h"

namespace Reality
{
	DebugDrawTriangleComponentSystem::DebugDrawTriangleComponentSystem()
	{
		requireComponent<DebugDrawTriangleComponent>();
	}

	void DebugDrawTriangleComponentSystem::Update(float deltaTime)
	{
		for (auto e : getEntities())
		{
			auto triangle = e.getComponent<DebugDrawTriangleComponent>();
			getWorld().data.renderUtil->DrawTriangle(triangle.Point1, triangle.Point2, triangle.Point3, triangle.color);
		}
	}
}
