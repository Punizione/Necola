#pragma once

#include "../includes/ehandle.h"
#include "../includes/cutilvector.h"

typedef void* FileNameHandle_t;
typedef short HSOUNDSCRIPTHANDLE;

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() {}

	virtual bool	IsReliable(void) const = 0;
	virtual bool	IsInitMessage(void) const = 0;

	virtual int		GetRecipientCount(void) const = 0;
	virtual int		GetRecipientIndex(int slot) const = 0;
};

struct SndInfo_t
{
	// Sound Guid
	int			m_nGuid;
	FileNameHandle_t m_filenameHandle;		// filesystem filename handle - call IFilesystem to conver this to a string
	int			m_nSoundSource;
	int			m_nChannel;
	// If a sound is being played through a speaker entity (e.g., on a monitor,), this is the
	//  entity upon which to show the lips moving, if the sound has sentence data
	int			m_nSpeakerEntity;
	float		m_flVolume;
	float		m_flLastSpatializedVolume;
	// Radius of this sound effect (spatialization is different within the radius)
	float		m_flRadius;
	int			m_nPitch;
	Vector		*m_pOrigin;
	Vector		*m_pDirection;

	// if true, assume sound source can move and update according to entity
	bool		m_bUpdatePositions;
	// true if playing linked sentence
	bool		m_bIsSentence;
	// if true, bypass all dsp processing for this sound (ie: music)	
	bool		m_bDryMix;
	// true if sound is playing through in-game speaker entity.
	bool		m_bSpeaker;
	// true if sound is playing with special DSP effect
	bool		m_bSpecialDSP;
	// for snd_show, networked sounds get colored differently than local sounds
	bool		m_bFromServer;
};

class EmitSound_t
{
public:

	int							m_nChannel;
	char const					*m_pSoundName;
	float						m_flVolume;
	SoundLevel_t				m_SoundLevel;
	int							m_nFlags;
	int							m_nPitch;
	int							m_nSpecialDSP;
	const Vector				*m_pOrigin;
	float						m_flSoundTime; ///< NOT DURATION, but rather, some absolute time in the future until which this sound should be delayed
	float						*m_pflSoundDuration;
	bool						m_bEmitCloseCaption;
	bool						m_bWarnOnMissingCloseCaption;
	bool						m_bWarnOnDirectWaveReference;
	int							m_nSpeakerEntity;
	mutable CUtlVector< Vector >	m_UtlVecSoundOrigin;  ///< Actual sound origin(s) (can be multiple if sound routed through speaker entity(ies) )
	mutable HSOUNDSCRIPTHANDLE		m_hSoundScriptHandle;
};


class IEngineSound
{
public:
	// Precache a particular sample
	virtual bool PrecacheSound(const char *pSample, bool bPreload = false, bool bIsUISound = false) = 0;
	virtual bool IsSoundPrecached(const char *pSample) = 0;
	virtual void PrefetchSound(const char *pSample) = 0;
	virtual bool IsLoopingSound(const char *pSample) = 0;


	virtual float GetSoundDuration(const char *pSample) = 0;

	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
		float flVolume, SoundLevel_t iSoundlevel, int iFlags = 0, int iPitch = SNDLVL_NORM,
		const Vector *pOrigin = 0, const Vector *pDirection = 0, CUtlVector< Vector >* pUtlVecOrigins = 0, bool bUpdatePositions= true, float soundtime = 0.0f, int speakerentity = -1) = 0;

	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
		float flVolume, float flAttenuation, int iFlags = 0, int iPitch = SNDLVL_NORM,
		const Vector *pOrigin = 0, const Vector *pDirection = 0, CUtlVector< Vector >* pUtlVecOrigins = 0, bool bUpdatePositions= true, float soundtime = 0.0f, int speakerentity = -1) = 0;

	virtual void EmitSentenceByIndex(IRecipientFilter& filter, int iEntIndex, int iChannel, int iSentenceIndex,
		float flVolume, SoundLevel_t iSoundlevel, int iFlags = 0, int iPitch = SNDLVL_NORM,
		const Vector *pOrigin = 0, const Vector *pDirection = 0, CUtlVector< Vector >* pUtlVecOrigins = 0 , bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) = 0;

	virtual void StopSound(int iEntIndex, int iChannel, const char *pSample) = 0;


	virtual void StopAllSounds(bool bClearBuffers) = 0;


	virtual void SetRoomType(IRecipientFilter& filter, int roomType) = 0;


	virtual void SetPlayerDSP(IRecipientFilter& filter, int dspType, bool fastReset) = 0;


	virtual void EmitAmbientSound(const char *pSample, float flVolume, int iPitch = SNDLVL_NORM, int flags = 0, float soundtime = 0.0f) = 0;



	virtual float GetDistGainFromSoundLevel(SoundLevel_t soundlevel, float dist) = 0;


	virtual int		GetGuidForLastSoundEmitted() = 0;
	virtual bool	IsSoundStillPlaying(int guid) = 0;
	virtual void	StopSoundByGuid(int guid) = 0;

	virtual void	SetVolumeByGuid(int guid, float fvol) = 0;

	virtual void	GetActiveSounds(CUtlVector< SndInfo_t >& sndlist) = 0;
	virtual void	PrecacheSentenceGroup(const char *pGroupName) = 0;
	virtual void	NotifyBeginMoviePlayback() = 0;
	virtual void	NotifyEndMoviePlayback() = 0;

	virtual bool	GetSoundChannelVolume(const char* sound, float &flVolumeLeft, float &flVolumeRight) = 0;
	virtual float	GetElapsedTimeByGuid(int guid) = 0;
};

/*
class CEngineSoundClient : public IEngineSound
{
public:
	
	virtual bool PrecacheSound( const char *pSample, bool bPreload, bool bIsUISound );
	virtual bool IsSoundPrecached( const char *pSample );
	virtual void PrefetchSound( const char *pSample );

	virtual float GetSoundDuration( const char *pSample );  

	virtual void EmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample, 
		float flVolume, float flAttenuation, int iFlags, int iPitch, int iSpecialDSP, 
		const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime = 0.0f, int speakerentity = -1 );

	virtual void EmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample, 
		float flVolume, soundlevel_t iSoundLevel, int iFlags, int iPitch, int iSpecialDSP, 
		const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime = 0.0f, int speakerentity = -1 );

	virtual void EmitSentenceByIndex( IRecipientFilter& filter, int iEntIndex, int iChannel, int iSentenceIndex, 
		float flVolume, soundlevel_t iSoundLevel, int iFlags, int iPitch, int iSpecialDSP, 
		const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime = 0.0f, int speakerentity = -1 );

	virtual void StopSound( int iEntIndex, int iChannel, const char *pSample );

	virtual void StopAllSounds(bool bClearBuffers);

	virtual void SetRoomType( IRecipientFilter& filter, int roomType );
	virtual void SetPlayerDSP( IRecipientFilter& filter, int dspType, bool fastReset );

	virtual void EmitAmbientSound( const char *pSample, float flVolume, 
		int iPitch, int flags, float soundtime = 0.0f );

	virtual float GetDistGainFromSoundLevel( soundlevel_t soundlevel, float dist );

	// Client .dll only functions
	virtual int		GetGuidForLastSoundEmitted();
	virtual bool	IsSoundStillPlaying( int guid );
	virtual void	StopSoundByGuid( int guid );
	// Set's master volume (0.0->1.0)
	virtual void	SetVolumeByGuid( int guid, float fvol );

	// Retrieves list of all active sounds
	virtual void	GetActiveSounds( CUtlVector< SndInfo_t >& sndlist );

	virtual void	PrecacheSentenceGroup( const char *pGroupName );
	virtual void	NotifyBeginMoviePlayback();
	virtual void	NotifyEndMoviePlayback();


};
*/
namespace I { inline IEngineSound* EngineSound = nullptr; }
