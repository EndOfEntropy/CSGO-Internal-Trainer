#pragma once
#include "math.h" // otherwise creates errors
#include "static.h"
#include <string>
#include <vector>


class Entity
{
private:
	uintptr_t	m_uipEntPtr;

	Vector3		m_vOrigin;
	
	int			m_iTeamNum;

	bool		m_bDormant;
	bool		m_lifeState;


public:
	Entity();
	~Entity();

	uintptr_t		GetEntPtr();


	Vector3			GetOrigin();
	Vector3			GetBonePosition(Ent * pEnt, int boneNb);


	int				GetTeamNum();
	int				GetHealth();


	int				m_iHealth;

	bool			IsDormant();
	bool			IsDead();
	bool			IsValid();
	bool			IsPlayer();

	std::string			ParseModelName(std::string modelName);
	std::string			GetModelName(Ent * pEnt);

	char *			GetName(int entNb);

	std::vector<std::vector<int>> GetBoneStruct(std::string model);

	int				GetDistanceInMeters(Ent * pEnt);
	int				UpdateEntity(uintptr_t pEnt);
	int				InitEntity();

	void			Clear();

}; //Size: 0x
//static_assert(sizeof(ent) == 0x10026);