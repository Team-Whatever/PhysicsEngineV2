#pragma once
#include "ECSConfig.h"

namespace Reality
{
	struct DebugDrawTriangleComponent
	{
		DebugDrawTriangleComponent(glm::vec3 _Point1 = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 _Point2 = glm::vec3(2.0f, 1.0f, 2.0f),
			glm::vec3 _Point3 = glm::vec3(5.0f, 5.0f, 5.0f), Color _color = Color())
			: Point1(_Point1), Point2(_Point2), Point3(_Point3), color(_color) {}
		glm::vec3 Point1;
		glm::vec3 Point2;
		glm::vec3 Point3;
		Color color;
	};
}
