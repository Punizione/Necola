#pragma once

#include "../../utils/Vector.h"
#include "ehandle.h"

class C_BaseEntity;
typedef CHandle<C_BaseEntity> EHANDLE;
// CTakeDamageInfo structure based on Source SDK 2013
class CTakeDamageInfo
{

	Vector			m_vecDamageForce;
	Vector			m_vecDamagePosition;
	Vector			m_vecReportedPosition;	// Position players are told damage is coming from
	EHANDLE			m_hInflictor;
	EHANDLE			m_hAttacker;
	EHANDLE			m_hWeapon;
	float			m_flDamage;
	float			m_flMaxDamage;
	float			m_flBaseDamage;			// The damage amount before skill leve adjustments are made. Used to get uniform damage forces.
	int				m_bitsDamageType;
	int				m_iDamageCustom;
	int				m_iDamageStats;
	int				m_iAmmoType;			// AmmoType of the weapon used to cause this damage, if any
	float			m_flRadius;
};
