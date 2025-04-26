#pragma once
#include "iGameEvents.h"

class IGameEventListener2
{
public:
	virtual	~IGameEventListener2( void ) {};
	virtual void FireGameEvent( IGameEvent *event ) = 0;

	virtual int	 GetEventDebugID( void ) = 0;
};