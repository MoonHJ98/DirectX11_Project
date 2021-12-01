#pragma once

#include "Includes.h"
class Capsule
{
public:
	enum Axis { Axis_X, Axis_Y, Axis_Z };

public:
	static void CreateCapsule(vector<GeometricPrimitive::VertexType>& vertices, vector<uint16_t>& indices, float radius, float height, Axis axis = Axis::Axis_Y, int stackCount = 20, int sliceCount = 20);
private:
	static Vector3* CreateVertex(float radius, float height, Axis axis, int stackCount, int sliceCount, int& lineCount);
};

