#include "engine.h"
#include "static.h"
#include "d3d.h"


Engine* g_pEngine = new Engine();
// Forward declarations
extern Static* g_pStatic;
extern D3D* g_pD3D;

Engine::Engine()
{
}

Engine::~Engine()
{
}

bool Engine::WorldToScreen(const Vector3& vIn, Vector3& vOut)
{
	Matrix4x4 vMatrix = *(Matrix4x4*)(g_pStatic->pWorldToScreenMatrix);

	vOut[0] = vMatrix.m[0][0] * vIn[0] + vMatrix.m[0][1] * vIn[1] + vMatrix.m[0][2] * vIn[2] + vMatrix.m[0][3];
	vOut[1] = vMatrix.m[1][0] * vIn[0] + vMatrix.m[1][1] * vIn[1] + vMatrix.m[1][2] * vIn[2] + vMatrix.m[1][3];
	// from 3D to 2D coordinates, so z is useless
	float w = vMatrix.m[3][0] * vIn[0] + vMatrix.m[3][1] * vIn[1] + vMatrix.m[3][2] * vIn[2] + vMatrix.m[3][3];

	if (w < 0.1f)
		return 0;
	// Divide by w or multiply by 1/w
	float invw = 1.0f / w;

	vOut[0] *= invw;
	vOut[1] *= invw;

	int width, height;

	g_pD3D->GetScreenSize(&width, &height);
	// Divide by 2 or multiply by 0.5f
	vOut[0] *= 0.5f;
	vOut[1] *= 0.5f;
	// In device space, pixel centers lie at 0.5 offsets, whatever...
	vOut[0] += 0.5f;
	vOut[1] += 0.5f;
	// Scale vector by the screen dimensions
	vOut[0] *= width;
	vOut[1] = height - (vOut[1] * height); // vOut[1] *= height is only the difference, funky...

	return 1;
}

const char* Engine::GetMapName()
{
	if (!g_pStatic->uClientState)
	{
		return nullptr;
	}
	else
	{
		size_t len_out = strlen(g_pStatic->uClientState->dwClientState->Map);
		if (!len_out)
		{
			return nullptr;
		}

		memcpy_s(m_pMapName, sizeof(m_pMapName), (g_pStatic->uClientState->dwClientState->Map), len_out);

		return m_pMapName;
	}
}