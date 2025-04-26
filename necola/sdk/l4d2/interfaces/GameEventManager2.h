#pragma once
#include "../entities/IGameEventListener2.h"

class bf_write;
class bf_read;


class IGameEventManager2 : public IBaseInterface
{
public:
	virtual	~IGameEventManager2( void ) {};

	virtual int LoadEventsFromFile( const char *filename ) = 0;
	virtual void  Reset() = 0;	
	virtual bool AddListener( IGameEventListener2 *listener, const char *name, bool bServerSide ) = 0;
	virtual bool FindListener( IGameEventListener2 *listener, const char *name ) = 0;
	virtual void RemoveListener( IGameEventListener2 *listener) = 0;
	virtual IGameEvent *CreateEvent( const char *name, bool bForce = false, int *pCookie = NULL ) = 0;
	virtual bool FireEvent( IGameEvent *event, bool bDontBroadcast = false ) = 0;
	virtual bool FireEventClientSide( IGameEvent *event ) = 0;
	virtual IGameEvent *DuplicateEvent( IGameEvent *event ) = 0;
	virtual void FreeEvent( IGameEvent *event ) = 0;
	virtual bool SerializeEvent( IGameEvent *event, bf_write *buf ) = 0;
	virtual IGameEvent *UnserializeEvent( bf_read *buf ) = 0;
};


namespace I { inline IGameEventManager2* GameEventManager = nullptr; }