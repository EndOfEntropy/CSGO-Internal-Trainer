#pragma once
#include "math.h"

class Engine
{
public:
	char		m_pMapName[32];

	Engine();
	~Engine();

	bool		WorldToScreen(const Vector3& vIn, Vector3& vOut);

	const char* GetMapName();

};
