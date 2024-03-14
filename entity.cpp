#include "entity.h"
#include "module.h"
#include "sig.h"
#include "xorstr.h"


Entity* g_pEntity = new Entity();
Entity* g_pPlayer = new Entity();
uintptr_t uipDormant, uipTeamNum, uipHealth, uipLifeState, uipVecOrigin;
// Forward declarations
extern Static* g_pStatic;
extern Module* g_pClient;


Entity::Entity()
{
	m_uipEntPtr = 0;
	m_bDormant = 0;
	m_iTeamNum = 0;
	m_iHealth = 0;
	m_lifeState = 1;
}

Entity::~Entity()
{
}

int Entity::UpdateEntity(uintptr_t pEnt)
{
	if (pEnt)
	{
		Clear();

		m_uipEntPtr = (uintptr_t)pEnt;
		m_bDormant = *(bool*)((uintptr_t)pEnt + uipDormant); // hazedumper::signatures::m_bDormant
		m_iTeamNum = *(int*)((uintptr_t)pEnt + uipTeamNum);
		m_iHealth = *(int*)((uintptr_t)pEnt + uipHealth);
		m_lifeState = *(bool*)((uintptr_t)pEnt + uipLifeState);
		m_vOrigin = *(Vector3*)((uintptr_t)pEnt + uipVecOrigin);
	}

	return 1;
}

int	Entity::InitEntity()
{
	Sig* AccessSig = new Sig();

	// "client_panorama.dll" | "8A 81 ?? ?? ?? ?? C3 32 C0"  extra = 8 (E5+8 = 0xED)
	uipDormant = AccessSig->NetvarsScan(g_pClient->GetImage(), xorstr("\x8A\x81\x00\x00\x00\x00\xC3\x32\xC0").crypt_get(),
		xorstr("xx????xxx").crypt_get(), 2, 4, 0x8);

	// "client_panorama.dll" | "E9 ?? ?? ?? ?? CC CC CC CC CC 8B 81 ?? ?? ?? ?? C3" (0xF4)
	uipTeamNum = AccessSig->NetvarsScan(g_pClient->GetImage(), xorstr("\xE9\x00\x00\x00\x00\xCC\xCC\xCC\xCC\xCC\x8B\x81\x00\x00\x00\x00\xC3").crypt_get(),
		xorstr("x????xxxxxxx????x").crypt_get(), 12, 4);

	// "client_panorama.dll" | "83 B9 ?? ?? ?? ?? 00 7F 2D" (0x100)
	uipHealth = AccessSig->NetvarsScan(g_pClient->GetImage(), xorstr("\x83\xB9\x00\x00\x00\x00\x00\x7F\x2D").crypt_get(),
		xorstr("xx????xxx").crypt_get(), 2, 4);

	// "client_panorama.dll" | "80 BE ?? ?? ?? ?? 01 7C 09" (0x25F)
	uipLifeState = AccessSig->NetvarsScan(g_pClient->GetImage(), xorstr("\x80\xBE\x00\x00\x00\x00\x01\x7C\x09").crypt_get(),
		xorstr("xx????xxx").crypt_get(), 2, 4);

	// "client_panorama.dll" | "F3 0F 10 87 ?? ?? ?? ?? 0F 2E 87 A8" (0x138)
	uipVecOrigin = AccessSig->NetvarsScan(g_pClient->GetImage(), xorstr("\xF3\x0F\x10\x87\x00\x00\x00\x00\x0F\x2E\x87\xA8").crypt_get(),
		xorstr("xxxx????xxxx").crypt_get(), 4, 4);

	delete AccessSig;

	return 1;
}

bool Entity::IsValid()
{
	if (!GetEntPtr()) { return 0; }

	if (!GetTeamNum()) { return 0; }

	if (IsDead()) { return 0; }

	if (IsDormant()) { return 0; }

	if (GetOrigin().IsZero()) { return 0; }

	if (GetHealth() < 1) { return 0; }

	return 1;
}

bool Entity::IsPlayer()
{
	if (!GetEntPtr()) { return 0; }

	//if (GetName().empty()) { return 0; }

	if (!GetTeamNum()) { return 0; }

	return 1;
}

uintptr_t Entity::GetEntPtr()
{
	return m_uipEntPtr;
}

Vector3 Entity::GetOrigin()
{
	return m_vOrigin;
}

Vector3 Entity::GetBonePosition(Ent * pEnt, int boneNb)
{
	if (!pEnt)
		return Vector3(0,0,0);

	Matrix3x4 mBone = pEnt->pBoneMatrix->BoneList[boneNb];
	return Vector3(mBone.m[0][3], mBone.m[1][3], mBone.m[2][3]);
}

int Entity::GetTeamNum()
{
	return m_iTeamNum;
}

int Entity::GetHealth()
{
	return m_iHealth;
}

bool Entity::IsDormant()
{
	return m_bDormant;
}

bool Entity::IsDead()
{
	return m_lifeState;
}

std::string Entity::ParseModelName(std::string modelName)
{
	size_t start = modelName.rfind("/") + 1;
	std::string str = modelName.substr(start);

	if (count(str.begin(), str.end(), '_') > 1)
	{
		size_t end = str.rfind("_");
		return (str.substr(0, end));
	}
	else
	{
		size_t end = str.rfind(".");
		return (str.substr(0, end));
	}
}

std::string Entity::GetModelName(Ent * pEnt)
{
	if (!pEnt)
		return NULL;

	std::string szName = pEnt->pModel_t->szName;
	return (ParseModelName(szName));
}

char * Entity::GetName(int entNb)	// Because we have to increment 0x34 (ext.mode), the ent# is required
{
	Items* pItems = (Items*)((uintptr_t)(g_pStatic->uClientState->dwClientState->dwUserInfoTable->dwUser->dwItems) + (0x34 * entNb));

	return (pItems->dwPlayerInfo->szName);
}

int Entity::GetDistanceInMeters(Ent * pEnt)
{
	Vector3 vec;
	// uintptr_t pLocalPlayer = *(uintptr_t*)(g_pClient->GetImage() + offset of localplayer);

	return (vec.Distance(GetBonePosition((Ent *)g_pStatic->pLocalPlayer, 0), GetBonePosition(pEnt, 0)) * 0.0254f);	// game units to meters ( * 0.0254f)
}

void Entity::Clear()
{
	m_uipEntPtr = 0;
	m_bDormant = 0;
	m_iTeamNum = 0;
	m_iHealth = 0;
	m_lifeState = 0;
	m_vOrigin = Vector3(0, 0, 0);
}

std::vector<std::vector<int>> Entity::GetBoneStruct(std::string model)
{
	if (model == (std::string)xorstr("tm_leet").crypt_get())
	{
		return {
			{7, 39, 40, 41}, {7, 11, 12, 13},		// arms
			{3, 74, 75}, {3, 67, 68},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("tm_separatist").crypt_get())
	{
		return {
			{7, 38, 39, 40}, {7, 10, 11, 12},		// arms
			{3, 66, 67}, {3, 73, 74},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("tm_phoenix").crypt_get())
	{
		return {
			{7, 38, 39, 40}, {7, 10, 11, 12},		// Arms
			{3, 66, 67}, {3, 73, 74},				// Legs
			{3, 6, 7, 8}							// Body
		};
	}

	if (model == (std::string)xorstr("tm_professional").crypt_get())
	{
		return {
			{7, 39, 40, 41}, {7, 11, 12, 13},		// Arms
			{3, 71, 72}, {3, 78, 79},				// Legs
			{3, 6, 7, 8}							// Body
		};
	}

	if (model == (std::string)xorstr("tm_anarchist").crypt_get())
	{
		return {
			{7, 40, 41, 42}, {7, 12, 13, 14},		// arms
			{3, 68, 69}, {3, 75, 76},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("tm_balkan").crypt_get())
	{
		return {
			{7, 38, 39, 40}, {7, 10, 11, 12},		// arms
			{3, 66, 67}, {3, 73, 74},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("ctm_idf").crypt_get())
	{
		return {
			{7, 39, 40, 41}, {7, 11, 12, 13},		// arms
			{3, 78, 79}, {3, 71, 72},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("ctm_swat").crypt_get())	//TBU = To Be Updated | not in the game anymore
	{
		return {
			{7, 39, 40, 41}, {7, 11, 12, 13},		// arms
			{3, 78, 79}, {3, 71, 72},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("ctm_sas").crypt_get())
	{
		return {
			{7, 40, 41, 42}, {7, 11, 12, 13},		// Arms
			{3, 73, 74}, {3, 82, 83},				// Legs
			{3, 6, 7, 8}							// Body
		};
	}

	if (model == (std::string)xorstr("ctm_st6").crypt_get())
	{
		return {
			{7, 38, 39, 40}, {7, 10, 11, 12},		// arms
			{3, 66, 67}, {3, 73, 74},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("ctm_fbi").crypt_get())
	{
		return {
	 		{7, 39, 40, 41}, {7, 11, 12, 13},		// Arms
			{3, 72, 73}, {3, 81, 82},				// Legs
			{3, 6, 7, 8}							// Body
		};
	}

	if (model == (std::string)xorstr("ctm_gsg9").crypt_get())
	{
		return {
			{7, 38, 39, 40}, {7, 10, 11, 12},		// arms
			{3, 66, 67}, {3, 73, 74},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	if (model == (std::string)xorstr("ctm_gign").crypt_get())
	{
		return {
			{7, 40, 41, 42}, {7, 11, 12, 13},		// arms
			{3, 77, 78}, {3, 70, 71},				// Legs
			{3, 4, 5, 6, 7, 8}						// Body
		};
	}

	// body only as default
	return {
		{3, 4, 5, 6, 7, 8}							// Body
	};
}