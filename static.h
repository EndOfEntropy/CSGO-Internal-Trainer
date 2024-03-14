#pragma once
#include <Windows.h>
#include <cstdint>
#include "math.h"

class clientInfo
{
public:
	class Ent *entptr; //0x0000
	int32_t N00000262; //0x0004
	class clientInfo *blink; //0x0008		// linked list
	class clientInfo *flink; //0x000C		// linked list
}; //Size: 0x0010
//static_assert(sizeof(clientInfo) == 0x10);

class CBaseEntityList
{
public:
	char pad_0000[16]; //0x0000
	class clientInfo entList[64]; //0x0010
}; //Size: 0x0410
//static_assert(sizeof(CBaseEntityList) == 0x410);

class Ent
{
public:
	char pad_0000[108]; //0x0000
	class Model_t *pModel_t; //0x006C
	char pad_0070[9782]; //0x0070
	class BoneMatrix *pBoneMatrix; //0x26A8 (but only works if dec 9894 = 0x26A6, mystery)

}; //Size: 0x26AC
//static_assert(sizeof(ent) == 0x26AC);

class BoneMatrix
{
public:
	Matrix3x4 BoneList[128]; //0x0000
}; //Size: 0x1800
//static_assert(sizeof(BoneMatrix) == 0x1800);

class Model_t
{
public:
	int32_t fnHandle; //0x0000
	char szName[260]; //0x0004
	int32_t nLoadFlags; //0x0108
	int32_t nServerCount; //0x010C
	int32_t type; //0x0110
	int32_t flags; //0x0114
	Vector3 vecMins; //0x0118
	Vector3 vecMaxs; //0x0124
	float radius; //0x0130
	char pad_0134[32]; //0x0134
}; //Size: 0x0154
//static_assert(sizeof(model_t) == 0x154);

class pClientState
{
public:
	class ClientState *dwClientState; //0x0000
}; //Size: 0x0004
//static_assert(sizeof(pClientState) == 0x4);

class ClientState
{
public:
	char pad_0000[652]; //0x0000
	char Map[10]; //0x028C
	char pad_0296[20512]; //0x0296
	int16_t N00001510; //0x52B6
	class userInfoTable *dwUserInfoTable; //0x52B8
}; //Size: 0x52BC
//static_assert(sizeof(ClientState) == 0x52BC);

class userInfoTable
{
public:
	char pad_0000[64]; //0x0000
	class User *dwUser; //0x0040
}; //Size: 0x0044
//static_assert(sizeof(userInfoTable) == 0x44);

class User
{
public:
	char pad_0000[12]; //0x0000
	class Items *dwItems; //0x000C
}; //Size: 0x0010
//static_assert(sizeof(User) == 0x10);

class Items
{
public:
	char pad_0000[40]; //0x0000
	class PlayerInfo *dwPlayerInfo; //0x0028
}; //Size: 0x002C
//static_assert(sizeof(Items) == 0x2C);

class PlayerInfo
{
public:
	int64_t unknown; //0x0000
	int64_t steamID64; //0x0008
	char szName[128]; //0x0010
	int32_t userId; //0x0090
	char szSteamID[20]; //0x0094
	char pad_00A8[16]; //0x00A8
	int32_t iSteamID; //0x00B8
	char szFriendsName[128]; //0x00BC
	bool fakeplayer; //0x013C
	bool ishltv; //0x013D
	int32_t customfiles[4]; //0x013E
	char filesdownloaded[1]; //0x014E
}; //Size: 0x014F
//static_assert(sizeof(PlayerInfo) == 0x14F);

class Static
{
public:
	CBaseEntityList*	pCBaseEntList;
	pClientState*		uClientState;
	uintptr_t			pWorldToScreenMatrix;
	uintptr_t			pGlobal;
	uintptr_t			pLocalPlayer;
	// uintptr_t			dwInput;
	// uintptr_t			userCMD;


	Static();
	~Static();

	int	InitStatic();
};

/* Functionning Old Ent
class Ent
{
public:
	char pad_0000[9894]; //0x0000
	class BoneMatrix *pBoneMatrix; //0x26A8 (only works if dec 9894 = 0x26A6, mystery)
}; //Size: 0x26AC
//static_assert(sizeof(ent) == 0x26AC);

	char pad_0000[108]; //0x0000
	class Model_t *pModel_t; //0x006C
	char pad_0070[9782]; //0x0070
	class BoneMatrix *pBoneMatrix; //0x26A8 (but only works if dec 9894 = 0x26A6, mystery)
*/