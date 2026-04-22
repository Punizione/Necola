#pragma once
#include "../includes/cutilvector.h"



enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD	= SEEK_SET,
	FILESYSTEM_SEEK_CURRENT = SEEK_CUR,
	FILESYSTEM_SEEK_TAIL	= SEEK_END,
};

enum FilesystemMountRetval_t
{
	FILESYSTEM_MOUNT_OK = 0,
	FILESYSTEM_MOUNT_FAILED,
};


enum SearchPathAdd_t
{
	PATH_ADD_TO_HEAD,			// First path searched
	PATH_ADD_TO_TAIL,			// Last path searched
	PATH_ADD_TO_TAIL_ATINDEX,	// First path searched
};

enum PathTypeFilter_t
{
	FILTER_NONE               = 0,	// no filtering, all search path types match
	FILTER_CULLPACK           = 1,	// pack based search paths are culled (maps and zips)
	FILTER_CULLNONPACK        = 2,	// non-pack based search paths are culled
	FILTER_CULLLOCALIZED      = 3,	// Ignore localized paths, assumes CULLNONPACK
	FILTER_CULLLOCALIZED_ANY  = 4,	// Ignore any localized paths
};

typedef void* FileHandle_t;
typedef void* FileNameHandle_t;
typedef void* FSAsyncFile_t;
typedef void* FSAsyncControl_t;
typedef void* FSAllocFunc_t;

typedef int FileFindHandle_t;
typedef int WaitForResourcesHandle_t;

typedef unsigned int PathTypeQuery_t;

#define FILESYSTEM_INVALID_HANDLE	( FileHandle_t )0

class IBaseFileSystem
{
public:
	virtual int				Read( void* pOutput, int size, FileHandle_t file ) = 0;
	virtual int				Write( void const* pInput, int size, FileHandle_t file ) = 0;

	virtual FileHandle_t	Open( const char *pFileName, const char *pOptions, const char *pathID = 0 ) = 0;
	virtual void			Close( FileHandle_t file ) = 0;


	virtual void			Seek( FileHandle_t file, int pos, FileSystemSeek_t seekType ) = 0;
	virtual unsigned int	Tell( FileHandle_t file ) = 0;
	virtual unsigned int	Size( FileHandle_t file ) = 0;
	virtual unsigned int	Size( const char *pFileName, const char *pPathID = 0 ) = 0;

	virtual void			Flush( FileHandle_t file ) = 0;
	virtual bool			Precache( const char *pFileName, const char *pPathID = 0 ) = 0;

	virtual bool			FileExists( const char *pFileName, const char *pPathID = 0 ) = 0;
	virtual bool			IsFileWritable( char const *pFileName, const char *pPathID = 0 ) = 0;
	virtual bool			SetFileWritable( char const *pFileName, bool writable, const char *pPathID = 0 ) = 0;

	virtual long			GetFileTime( const char *pFileName, const char *pPathID = 0 ) = 0;

	
	virtual bool			ReadFile( const char *pFileName, const char *pPath, void* buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;
	virtual bool			WriteFile( const char *pFileName, const char *pPath, void* buf ) = 0;
	virtual bool			UnzipFile( const char *pFileName, const char *pPath, const char *pDestination ) = 0;
};


class IFileSystem : public IAppSystem, public IBaseFileSystem
{
public:
	virtual bool			IsSteam() const = 0;
	virtual	FilesystemMountRetval_t MountSteamContent( int nExtraAppId = -1 ) = 0;
	virtual void			AddSearchPath( const char *pPath, const char *pathID, SearchPathAdd_t addType = PATH_ADD_TO_TAIL ) = 0;
	virtual bool			RemoveSearchPath( const char *pPath, const char *pathID = 0 ) = 0;
	virtual void			RemoveAllSearchPaths( void ) = 0;
	virtual void			RemoveSearchPaths( const char *szPathID ) = 0;
	virtual void			MarkPathIDByRequestOnly( const char *pPathID, bool bRequestOnly ) = 0;
	virtual const char		*RelativePathToFullPath( const char *pFileName, const char *pPathID, char *pLocalPath, int localPathBufferSize, PathTypeFilter_t pathFilter = FILTER_NONE, PathTypeQuery_t *pPathType = NULL ) = 0;
	virtual int				GetSearchPath( const char *pathID, bool bGetPackFiles, char *pPath, int nMaxLen ) = 0;
	virtual bool			AddPackFile( const char *fullpath, const char *pathID ) = 0;
	virtual bool			IsLocalizedPath ( const char * ) = 0;
	virtual void			RemoveFile( char const* pRelativePath, const char *pathID = 0 ) = 0;
	virtual bool			RenameFile( char const *pOldPath, char const *pNewPath, const char *pathID = 0 ) = 0;
	virtual void			CreateDirHierarchy( const char *path, const char *pathID = 0 ) = 0;
	virtual bool			IsDirectory( const char *pFileName, const char *pathID = 0 ) = 0;
	virtual void			FileTimeToString( char* pStrip, int maxCharsIncludingTerminator, long fileTime ) = 0;
	virtual void			SetBufferSize( FileHandle_t file, unsigned nBytes ) = 0;
	virtual bool			IsOk( FileHandle_t file ) = 0;
	virtual bool			EndOfFile( FileHandle_t file ) = 0;
	virtual char			*ReadLine( char *pOutput, int maxChars, FileHandle_t file ) = 0;
	virtual int				FPrintf( FileHandle_t file, const char *pFormat, ... ) = 0;
	virtual void*			LoadModule( const char *pFileName, const char *pPathID = 0, bool bValidatedDllOnly = true ) = 0;
	virtual void			UnloadModule( void* pModule ) = 0;
	virtual const char		*FindFirst( const char *pWildCard, FileFindHandle_t *pHandle ) = 0;
	virtual const char		*FindNext( FileFindHandle_t handle ) = 0;
	virtual bool			FindIsDirectory( FileFindHandle_t handle ) = 0;
	virtual void			FindClose( FileFindHandle_t handle ) = 0;
	virtual const char		*FindFirstEx( const char *pWildCard, const char *pPathID,FileFindHandle_t *pHandle) = 0;
	// virtual void			FindFileAbsoluteList( CUtlVector< CUtlString > &outAbsolutePathNames, const char *pWildCard, const char *pPathID ) = 0;
	// virtual const char		*GetLocalPath( const char *pFileName, char *pLocalPath, int localPathBufferSize ) = 0;
	// virtual bool			FullPathToRelativePath( const char *pFullpath, char *pRelative, int maxlen ) = 0;
	// virtual bool			GetCurrentDirectory( char* pDirectory, int maxlen ) = 0;
	// virtual FileNameHandle_t	FindOrAddFileName( char const *pFileName ) = 0;
	// virtual bool				String( const FileNameHandle_t& handle, char *buf, int buflen ) = 0;
	// 		// FSAsyncStatus_t	AsyncRead( const FileAsyncRequest_t &request, FSAsyncControl_t *phControl = NULL )	{ return AsyncReadMultiple( &request, 1, phControl ); 	}
	// virtual FSAsyncStatus_t	AsyncReadMultiple( const FileAsyncRequest_t *pRequests, int nRequests,  FSAsyncControl_t *phControls = NULL ) = 0;
	// virtual FSAsyncStatus_t	AsyncAppend(const char *pFileName, const void *pSrc, int nSrcBytes, bool bFreeMemory, FSAsyncControl_t *pControl = NULL ) = 0;
	// virtual FSAsyncStatus_t	AsyncAppendFile(const char *pAppendToFileName, const char *pAppendFromFileName, FSAsyncControl_t *pControl = NULL ) = 0;
	// virtual void			AsyncFinishAll( int iToPriority = 0 ) = 0;
	// virtual void			AsyncFinishAllWrites() = 0;
	// virtual FSAsyncStatus_t	AsyncFlush() = 0;
	// virtual bool			AsyncSuspend() = 0;
	// virtual bool			AsyncResume() = 0;
	// virtual FSAsyncStatus_t	AsyncBeginRead( const char *pszFile, FSAsyncFile_t *phFile ) = 0;
	// virtual FSAsyncStatus_t	AsyncEndRead( FSAsyncFile_t hFile ) = 0;
	// virtual FSAsyncStatus_t	AsyncFinish( FSAsyncControl_t hControl, bool wait = true ) = 0;
	// virtual FSAsyncStatus_t	AsyncGetResult( FSAsyncControl_t hControl, void **ppData, int *pSize ) = 0;
	// virtual FSAsyncStatus_t	AsyncAbort( FSAsyncControl_t hControl ) = 0;
	// virtual FSAsyncStatus_t	AsyncStatus( FSAsyncControl_t hControl ) = 0;
	// virtual FSAsyncStatus_t	AsyncSetPriority(FSAsyncControl_t hControl, int newPriority) = 0;
	// virtual void			AsyncAddRef( FSAsyncControl_t hControl ) = 0;
	// virtual void			AsyncRelease( FSAsyncControl_t hControl ) = 0;
	// virtual WaitForResourcesHandle_t WaitForResources( const char *resourcelist ) = 0;
	// virtual bool			GetWaitForResourcesProgress( WaitForResourcesHandle_t handle, float *progress /* out */ , bool *complete /* out */ ) = 0;
	// virtual void			CancelWaitForResources( WaitForResourcesHandle_t handle ) = 0;
	// virtual int				HintResourceNeed( const char *hintlist, int forgetEverything ) = 0;
	// virtual bool			IsFileImmediatelyAvailable(const char *pFileName) = 0;
	// virtual void			GetLocalCopy( const char *pFileName ) = 0;
	// virtual void			PrintOpenedFiles( void ) = 0;
	// virtual void			PrintSearchPaths( void ) = 0;

};

namespace I { inline IFileSystem* FileSystem = nullptr; }
