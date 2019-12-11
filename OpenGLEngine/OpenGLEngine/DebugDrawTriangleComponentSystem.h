#pragma once
#include "ECSConfig.h"
#include "DebugDrawTriangleComponent.h"

namespace Reality
{
	class DebugDrawTriangleComponentSystem : public ECSSystem
	{
	public:
		DebugDrawTriangleComponentSystem();
		void Update(float deltaTime);
	};
}
