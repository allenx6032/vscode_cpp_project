// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ファイルアクセスプログラム
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxFile.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxChar.h"
#include "DxMemory.h"
#include "DxUseCLib.h"
#include "DxASyncLoad.h"
#include "DxSystem.h"
#include "DxLog.h"

#ifndef DX_NON_DXA
#include "DxArchive_.h"
#endif

#ifndef DX_NON_PRINTF_DX
#include "DxFont.h"
#endif

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// ファイルアクセスハンドル有効性チェック
#define FILEHCHK( HAND, FPOINT )		HANDLECHK(       DX_HANDLETYPE_FILE, HAND, *( ( HANDLEINFO ** )&FPOINT ) )
#define FILEHCHK_ASYNC( HAND, FPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_FILE, HAND, *( ( HANDLEINFO ** )&FPOINT ) )

// 構造体定義 --------------------------------------------------------------------

// メモリに置かれたデータをファイルとして扱うためのデータ構造体
typedef struct tagMEMSTREAMDATA
{
	unsigned char			*DataBuffer ;
	size_t					DataSize ;
	size_t					DataPoint ;
	int						EOFFlag ;
} MEMSTREAMDATA, *LPMEMSTREAMDATA ;

// ファイルアクセス処理が使用するグローバルデータの構造体
struct FILEMANAGEDATA
{
	int						InitializeFlag ;					// 初期化フラグ
} ;

// ファイル情報リスト構造体
struct FILEINFOLIST
{
	int Num;		// ファイルの数
					// ここに FILEINFO が Num の数だけ存在する
} ;


// 内部大域変数宣言 --------------------------------------------------------------

// ファイルアクセス処理が使用するグローバルデータの構造体
FILEMANAGEDATA GFileData ;

// アーカイブファイルを読み込む機能を使用しないかどうかのフラグ
int NotUseDXArchive ;


// 関数プロトタイプ宣言 ----------------------------------------------------------

// ファイルハンドル関数
static	int			InitializeFileHandle( HANDLEINFO *HandleInfo ) ;								// ファイルアクセスハンドルを初期化する
static	int			TerminateFileHandle( HANDLEINFO *HandleInfo ) ;									// ファイルアクセスハンドルの後始末を行う

// メモリに置かれたデータをファイルのデータに例えてつかうための関数
static	LONGLONG	MemStreamTell( void *StreamDataPoint ) ;
static	int			MemStreamSeek( void *StreamDataPoint, LONGLONG SeekPoint, int SeekType ) ;
static	size_t		MemStreamRead( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint ) ;
static	size_t		MemStreamWrite( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint ) ;
static	int			MemStreamEof( void *StreamDataPoint ) ;
static	int			MemStreamIdleCheck( void *StreamDataPoint ) ;

// TCHAR版ストリームデータアクセスエミュレート用関数
static	DWORD_PTR	StreamTCHAR_Open( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag  ) ;
static	int			StreamTCHAR_Close( DWORD_PTR Handle ) ;
static	LONGLONG	StreamTCHAR_Tell( DWORD_PTR Handle ) ;
static	int			StreamTCHAR_Seek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType ) ;
static	size_t		StreamTCHAR_Read( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle ) ;
static	int			StreamTCHAR_Eof( DWORD_PTR Handle ) ;
static	int			StreamTCHAR_IdleCheck( DWORD_PTR Handle ) ;
static	int			StreamTCHAR_ChDir( const wchar_t *Path ) ;
static	int			StreamTCHAR_GetDir( wchar_t *Buffer ) ;
static	DWORD_PTR	StreamTCHAR_FindFirst( const wchar_t *FilePath, FILEINFOW *Buffer ) ;				// 戻り値: -1=エラー  -1以外=FindHandle
static	int			StreamTCHAR_FindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer ) ;					// 戻り値: -1=エラー  0=成功
static	int			StreamTCHAR_FindClose( DWORD_PTR FindHandle ) ;									// 戻り値: -1=エラー  0=成功


// データ宣言 --------------------------------------------------------------------

// ストリームデータアクセス用関数
STREAMDATASHREDTYPE2W StreamFunctionW =
{
#ifndef DX_NON_DXA
	DXA_DIR_Open,
	DXA_DIR_Close,
	DXA_DIR_Tell,
	DXA_DIR_Seek,
	DXA_DIR_Read,
	DXA_DIR_Eof,
	DXA_DIR_IdleCheck,
	DXA_DIR_ChDir,
	DXA_DIR_GetDir,
	DXA_DIR_FindFirst,
	DXA_DIR_FindNext,
	DXA_DIR_FindClose,
#else
	ReadOnlyFileAccessOpen,
	ReadOnlyFileAccessClose,
	ReadOnlyFileAccessTell,
	ReadOnlyFileAccessSeek,
	ReadOnlyFileAccessRead,
	ReadOnlyFileAccessEof,
	ReadOnlyFileAccessIdleCheck,
	ReadOnlyFileAccessChDir,
	ReadOnlyFileAccessGetDir,
	ReadOnlyFileAccessFindFirst,
	ReadOnlyFileAccessFindNext,
	ReadOnlyFileAccessFindClose,
#endif
/*
	_FileOpen,
	_FileClose,
	_FileTell,
	_FileSeek,
	_FileRead,
	_FileEof,
	_FileIdleCheck,
	_FileChDir,
	_FileGetDir,
*/
} ;

STREAMDATASHREDTYPE2 StreamFunction ;

// ストリームデータ制御用関数ポインタのデフォルト配列

// ファイルのストリームデータ制御用ポインタ構造体
STREAMDATASHRED StreamFileFunction =
{
	( LONGLONG ( * )( void *StreamDataPoint ) )StreamTell,
	( int ( * )( void *StreamDataPoint, LONGLONG SeekPoint, int SeekType ) )StreamSeek,
	( size_t ( * )( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint ) )StreamRead,
	( int ( * )( void *StreamDataPoint ) )StreamEof,
	( int ( * )( void *StreamDataPoint ) )StreamIdleCheck,
	( int ( * )( void *StreamDataPoint ) )StreamClose,
} ;

// メモリのストリームデータ制御用ポインタ構造体
STREAMDATASHRED StreamMemFunction =
{
	MemStreamTell,
	MemStreamSeek,
	MemStreamRead,
	MemStreamEof,
	MemStreamIdleCheck,
	MemStreamClose,
} ;

// プログラム --------------------------------------------------------------------

// ファイルアクセス処理の初期化・終了関数

// ファイルアクセス処理の初期化
extern int InitializeFile( void )
{
	// 既に初期化されていたら何もしない
	if( GFileData.InitializeFlag == TRUE )
		return -1 ;

	// ファイル読み込みハンドル管理情報を初期化する
	InitializeHandleManage( DX_HANDLETYPE_FILE, sizeof( FILEACCESSINFO ), MAX_FILE_NUM, InitializeFileHandle, TerminateFileHandle, L"File" ) ;

	// ストリームデータ読みこみ制御用ポインタ配列のデフォルト値をセット
	{
		StreamFileFunction.Read      = ( size_t   ( * )( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint ) )StreamRead ;
		StreamFileFunction.Eof       = ( int      ( * )( void *StreamDataPoint ) )StreamEof ;
		StreamFileFunction.IdleCheck = ( int      ( * )( void *StreamDataPoint ) )StreamIdleCheck ;
		StreamFileFunction.Seek      = ( int      ( * )( void *StreamDataPoint, LONGLONG SeekPoint, int SeekType ) )StreamSeek ;
		StreamFileFunction.Tell      = ( LONGLONG ( * )( void *StreamDataPoint ) )StreamTell ;
		StreamFileFunction.Close     = ( int      ( * )( void *StreamDataPoint ) )StreamClose ;
/*
		StreamFileFunction.Read  = ( size_t ( * )( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint ) )fread ;
//		StreamFileFunction.Write = ( size_t ( * )( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint ) )fwrite ;
		StreamFileFunction.Eof   = ( int ( * )( void *StreamDataPoint ) )feof ;
		StreamFileFunction.Seek  = ( int ( * )( void *StreamDataPoint, long SeekPoint, int SeekType ) )fseek ;
		StreamFileFunction.Tell  = ( long ( * )( void *StreamDataPoint ) )ftell ;
		StreamFileFunction.Close = ( int ( * )( void *StreamDataPoint ) )fclose ;
*/

		StreamMemFunction.Read      = MemStreamRead ;
//		StreamMemFunction.Write     = MemStreamWrite;
		StreamMemFunction.Eof       = MemStreamEof ;
		StreamMemFunction.IdleCheck = MemStreamIdleCheck ;
		StreamMemFunction.Seek      = MemStreamSeek ;
		StreamMemFunction.Tell      = MemStreamTell ;
		StreamMemFunction.Close     = MemStreamClose ;
	}

	// 初期化フラグを立てる
	GFileData.InitializeFlag = TRUE ;

	// 正常終了
	return 0 ;
}

// ファイルアクセス処理の後始末
extern int TerminateFile( void )
{
	// 既に後始末されていたら何もしない
	if( GFileData.InitializeFlag == FALSE )
		return -1 ;

	// ファイルハンドル管理情報の後始末
	TerminateHandleManage( DX_HANDLETYPE_FILE ) ;

	// 初期化フラグを倒す
	GFileData.InitializeFlag = FALSE ;

	// 正常終了
	return 0 ;
}

// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する
extern int NS_ChangeStreamFunction( const STREAMDATASHREDTYPE2 *StreamThread )
{
	// 関数ポインタを更新する
	if( StreamThread == NULL )
	{
		return NS_ChangeStreamFunctionW( NULL ) ;
	}
	else
	{
		StreamFunction = *StreamThread ;
		StreamFunctionW.Open		= StreamTCHAR_Open ;
		StreamFunctionW.Close		= StreamTCHAR_Close ;
		StreamFunctionW.Tell		= StreamTCHAR_Tell ;
		StreamFunctionW.Seek		= StreamTCHAR_Seek ;
		StreamFunctionW.Read		= StreamTCHAR_Read ;
		StreamFunctionW.Eof 		= StreamTCHAR_Eof ;
		StreamFunctionW.IdleCheck	= StreamTCHAR_IdleCheck ;
		StreamFunctionW.ChDir		= StreamTCHAR_ChDir ;
		StreamFunctionW.GetDir		= StreamTCHAR_GetDir ;
		StreamFunctionW.FindFirst	= StreamTCHAR_FindFirst ;
		StreamFunctionW.FindNext	= StreamTCHAR_FindNext ;
		StreamFunctionW.FindClose	= StreamTCHAR_FindClose ;
	}
	
	// 終了
	return 0 ;
}

// ＤＸライブラリでストリームデータアクセスに使用する関数を変更する( wchar_t 使用版 )
extern int NS_ChangeStreamFunctionW( const STREAMDATASHREDTYPE2W *StreamThreadW )
{
	// 関数ポインタを更新する
	if( StreamThreadW == NULL )
	{
		// アーカイブファイルを読み込む機能を使用するかどうかでセットする関数ポインタを変更
#ifndef DX_NON_DXA
		if( NotUseDXArchive == TRUE )
#endif
		{
			// 使用しない場合
			StreamFunctionW.Open		= ReadOnlyFileAccessOpen ;
			StreamFunctionW.Close		= ReadOnlyFileAccessClose ;
			StreamFunctionW.Tell		= ReadOnlyFileAccessTell ;
			StreamFunctionW.Seek		= ReadOnlyFileAccessSeek ;
			StreamFunctionW.Read		= ReadOnlyFileAccessRead ;
			StreamFunctionW.Eof 		= ReadOnlyFileAccessEof ;
			StreamFunctionW.IdleCheck	= ReadOnlyFileAccessIdleCheck ;
			StreamFunctionW.ChDir		= ReadOnlyFileAccessChDir ;
			StreamFunctionW.GetDir		= ReadOnlyFileAccessGetDir ;
			StreamFunctionW.FindFirst	= ReadOnlyFileAccessFindFirst ;
			StreamFunctionW.FindNext	= ReadOnlyFileAccessFindNext ;
			StreamFunctionW.FindClose	= ReadOnlyFileAccessFindClose ;
		}
#ifndef DX_NON_DXA
		else
		{
			// 使用する場合
			StreamFunctionW.Open		= DXA_DIR_Open ;
			StreamFunctionW.Close		= DXA_DIR_Close ;
			StreamFunctionW.Tell		= DXA_DIR_Tell ;
			StreamFunctionW.Seek		= DXA_DIR_Seek ;
			StreamFunctionW.Read		= DXA_DIR_Read ;
			StreamFunctionW.Eof 		= DXA_DIR_Eof ;
			StreamFunctionW.IdleCheck	= DXA_DIR_IdleCheck ;
			StreamFunctionW.ChDir		= DXA_DIR_ChDir ;
			StreamFunctionW.GetDir		= DXA_DIR_GetDir ;
			StreamFunctionW.FindFirst	= DXA_DIR_FindFirst ;
			StreamFunctionW.FindNext	= DXA_DIR_FindNext ;
			StreamFunctionW.FindClose	= DXA_DIR_FindClose ;
		}
#endif
	}
	else
	{
		StreamFunctionW = *StreamThreadW ;
	}
	
	// 終了
	return 0 ;
}

// ＤＸライブラリでストリームデータアクセスに使用する関数がデフォルトのものか調べる( TRUE:デフォルト  FALSE:デフォルトではない )
extern int NS_GetStreamFunctionDefault( void )
{
	// アーカイブファイルを読み込む機能を使用するかどうかで比較する関数を変更
#ifndef DX_NON_DXA
	if( NotUseDXArchive == TRUE )
#endif
	{
		// 使用しない場合
		if( StreamFunctionW.Open		!= ReadOnlyFileAccessOpen       ) return FALSE ;
		if( StreamFunctionW.Close		!= ReadOnlyFileAccessClose      ) return FALSE ;
		if( StreamFunctionW.Tell		!= ReadOnlyFileAccessTell       ) return FALSE ;
		if( StreamFunctionW.Seek		!= ReadOnlyFileAccessSeek       ) return FALSE ;
		if( StreamFunctionW.Read		!= ReadOnlyFileAccessRead       ) return FALSE ;
		if( StreamFunctionW.Eof 		!= ReadOnlyFileAccessEof        ) return FALSE ;
		if( StreamFunctionW.ChDir		!= ReadOnlyFileAccessChDir      ) return FALSE ;
		if( StreamFunctionW.GetDir		!= ReadOnlyFileAccessGetDir     ) return FALSE ;
		if( StreamFunctionW.FindFirst 	!= ReadOnlyFileAccessFindFirst  ) return FALSE ;
		if( StreamFunctionW.FindNext 	!= ReadOnlyFileAccessFindNext   ) return FALSE ;
		if( StreamFunctionW.FindClose 	!= ReadOnlyFileAccessFindClose  ) return FALSE ;
	}
#ifndef DX_NON_DXA
	else
	{
		// 使用する場合
		if( StreamFunctionW.Open		!= DXA_DIR_Open      ) return FALSE ;
		if( StreamFunctionW.Close		!= DXA_DIR_Close     ) return FALSE ;
		if( StreamFunctionW.Tell		!= DXA_DIR_Tell      ) return FALSE ;
		if( StreamFunctionW.Seek		!= DXA_DIR_Seek      ) return FALSE ;
		if( StreamFunctionW.Read		!= DXA_DIR_Read      ) return FALSE ;
		if( StreamFunctionW.Eof 		!= DXA_DIR_Eof       ) return FALSE ;
		if( StreamFunctionW.ChDir		!= DXA_DIR_ChDir     ) return FALSE ;
		if( StreamFunctionW.GetDir 		!= DXA_DIR_GetDir    ) return FALSE ;
		if( StreamFunctionW.FindFirst 	!= DXA_DIR_FindFirst ) return FALSE ;
		if( StreamFunctionW.FindNext 	!= DXA_DIR_FindNext  ) return FALSE ;
		if( StreamFunctionW.FindClose 	!= DXA_DIR_FindClose ) return FALSE ;
	}
#endif
	
	return TRUE ;
}











// 補助関係関数

// フルパスではないパス文字列をフルパスに変換する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
extern int NS_ConvertFullPath( const TCHAR *Src, TCHAR *Dest, const TCHAR *CurrentDir )
{
	return ConvertFullPathT_( Src, Dest, CurrentDir ) ;
}



















// ストリームデータアクセス関数
extern DWORD_PTR StreamOpen( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag )
{
	return StreamFunctionW.Open( Path, UseCacheFlag, BlockReadFlag, UseASyncReadFlag ) ;
}

extern DWORD_PTR StreamOpenT( const TCHAR *Path, int UseCacheFlag, int BlockFlag, int UseASyncReadFlag )
{
#ifdef UNICODE
	return StreamOpen( Path, UseCacheFlag, BlockFlag, UseASyncReadFlag ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, _TCODEPAGE, ( char * )PathBuffer, WCHAR_T_CODEPAGE ) ;
	return StreamOpen( PathBuffer, UseCacheFlag, BlockFlag, UseASyncReadFlag ) ;
#endif
}

extern int StreamClose( DWORD_PTR Handle )
{
	return StreamFunctionW.Close( Handle ) ;
}

extern LONGLONG StreamTell( DWORD_PTR Handle )
{
	return StreamFunctionW.Tell( Handle ) ;
}

extern int StreamSeek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	return StreamFunctionW.Seek( Handle, SeekPoint, SeekType ) ;
}

extern size_t StreamRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle )
{
	return StreamFunctionW.Read( Buffer, BlockSize, DataNum, Handle ) ;
}

extern int StreamEof( DWORD_PTR Handle )
{
	return StreamFunctionW.Eof( Handle ) ;
}

extern int StreamIdleCheck( DWORD_PTR Handle )
{
	return StreamFunctionW.IdleCheck( Handle ) ;
}

extern int StreamChDir( const wchar_t *Path )
{
	return StreamFunctionW.ChDir( Path ) ;
}

extern int StreamChDirT( const TCHAR *Path )
{
#ifdef UNICODE
	return StreamChDir( Path ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, _TCODEPAGE, ( char * )PathBuffer, WCHAR_T_CODEPAGE ) ;
	return StreamChDir( PathBuffer ) ;
#endif
}

extern int StreamGetDir( wchar_t *Buffer )
{
	return StreamFunctionW.GetDir( Buffer ) ;
}

extern int StreamGetDirT( TCHAR *Buffer )
{
#ifdef UNICODE
	return StreamGetDir( Buffer ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	int Result ;

	Result = StreamGetDir( PathBuffer ) ;
	ConvString( ( const char * )PathBuffer, WCHAR_T_CODEPAGE, ( char * )Buffer, _TCODEPAGE ) ;
	return Result ;
#endif
}

extern DWORD_PTR StreamFindFirst( const wchar_t *FilePath, FILEINFOW *Buffer )
{
	return StreamFunctionW.FindFirst( FilePath, Buffer ) ;
}

extern DWORD_PTR StreamFindFirstT( const TCHAR *FilePath, FILEINFO *Buffer )
{
#ifdef UNICODE
	FILEINFOW BufferW ;
	DWORD_PTR Result ;

	Result = StreamFindFirst( FilePath, &BufferW ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoWToFileInfo( &BufferW, Buffer ) ;
	}

	return Result ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;
	FILEINFOW BufferW ;
	DWORD_PTR Result ;

	ConvString( ( const char * )FilePath, _TCODEPAGE, ( char * )PathBuffer, WCHAR_T_CODEPAGE ) ;

	Result = StreamFindFirst( PathBuffer, &BufferW ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoWToFileInfo( &BufferW, Buffer ) ;
	}

	return Result ;
#endif
}

extern int StreamFindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer )
{
	return StreamFunctionW.FindNext( FindHandle, Buffer ) ;
}

extern int StreamFindNextT( DWORD_PTR FindHandle, FILEINFO *Buffer )
{
	FILEINFOW BufferW ;
	int Result ;

	Result = StreamFindNext( FindHandle, &BufferW ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoWToFileInfo( &BufferW, Buffer ) ;
	}

	return Result ;
}

extern int StreamFindClose( DWORD_PTR FindHandle )
{
	return StreamFunctionW.FindClose( FindHandle ) ;
}

extern	const STREAMDATASHREDTYPE2W *StreamGetStruct( void )
{
	return &StreamFunctionW ;
}













// ファイル情報の wchar_t 版と TCHAR 版のデータ変換関数
extern int ConvFileIntoToFileInfoW( FILEINFO  *Src, FILEINFOW *Dest )
{
#ifdef UNICODE
	*( ( FILEINFO * )Dest ) = *Src ;
#else
	ConvString( ( const char * )Src->Name, _TCODEPAGE, ( char * )Dest->Name, WCHAR_T_CODEPAGE ) ;
	Dest->DirFlag       = Src->DirFlag ;
	Dest->Size          = Src->Size ;
	Dest->CreationTime  = Src->CreationTime ;
	Dest->LastWriteTime = Src->LastWriteTime ;
#endif

	return 0 ;
}

extern int ConvFileIntoWToFileInfo( FILEINFOW *Src, FILEINFO  *Dest )
{
#ifdef UNICODE
	*( ( FILEINFOW * )Dest ) = *Src ;
#else
	ConvString( ( const char * )Src->Name, WCHAR_T_CODEPAGE, ( char * )Dest->Name, _TCODEPAGE ) ;
	Dest->DirFlag       = Src->DirFlag ;
	Dest->Size          = Src->Size ;
	Dest->CreationTime  = Src->CreationTime ;
	Dest->LastWriteTime = Src->LastWriteTime ;
#endif

	return 0 ;
}














// ストリームデータアクセス用関数構造体関係

// ストリームデータ読みこみ制御用関数ポインタ構造体のファイル用構造体を得る
extern STREAMDATASHRED *GetFileStreamDataShredStruct( void )
{
	return &StreamFileFunction ;
}

// ストリームデータ読みこみ制御用関数ポインタ構造体のﾒﾓﾘ用構造体を得る
extern STREAMDATASHRED *GetMemStreamDataShredStruct( void )
{
	return &StreamMemFunction ;
}













// ファイルハンドル関係

// ファイルアクセスハンドルを初期化する
static int InitializeFileHandle( HANDLEINFO * )
{
	// 特に何もしない
	return 0 ;
}

// ファイルアクセスハンドルの後始末を行う
static int TerminateFileHandle( HANDLEINFO *HandleInfo )
{
	FILEACCESSINFO *FileInfo = ( FILEACCESSINFO * )HandleInfo ;

	// ハンドルのタイプによって処理を分岐
	switch( FileInfo->HandleType )
	{
	case FILEHANDLETYPE_NORMAL :
		// ファイルを閉じる
		if( FileInfo->FilePointer )
		{
			FCLOSE( FileInfo->FilePointer ) ;
			FileInfo->FilePointer = 0 ;
		}
		break ;

	case FILEHANDLETYPE_FULLYLOAD :
		// ファイルの内容を格納していたメモリを解放
		if( FileInfo->FileImage != NULL )
		{
			DXFREE( FileInfo->FileImage ) ;
			FileInfo->FileImage = NULL ;
		}
		break ;
	}

	// 終了
	return 0 ;
}

// FileRead_open の実処理関数
static int FileRead_open_Static(
	int FileHandle,
	const wchar_t *FilePath,
	int ASync,
	int ASyncThread
)
{
	FILEACCESSINFO *FileInfo ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}
	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

	// ファイルを開く
	if( ASync )
	{
		FileInfo->FilePointer = FOPEN_ASYNC( FilePath ) ;
	}
	else
	{
		FileInfo->FilePointer = FOPEN( FilePath ) ;
	}

	// 開けなかったらエラー
	if( FileInfo->FilePointer == 0 )
		return -1 ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_open の非同期読み込みスレッドから呼ばれる関数
static void FileRead_open_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int FileHandle ;
	const wchar_t *FilePath ;
	int ASync ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	FilePath   = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	ASync      = GetASyncLoadParamInt(    AParam->Data, &Addr ) ;

	Result = FileRead_open_Static( FileHandle, FilePath, ASync, TRUE ) ;

	DecASyncLoadCount( FileHandle ) ;
	if( Result < 0 )
	{
		SubHandle( FileHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// ファイルを開く
extern int FileRead_open_UseGParam(
	const wchar_t *FilePath,
	int ASync,
	int ASyncLoadFlag
)
{
	int FileHandle ;
	FILEACCESSINFO *FileInfo ;

	// 初期化されていなかったら初期化する
	if( GFileData.InitializeFlag == FALSE )
	{
		InitializeFile() ;
	}

	// ハンドルの作成
	FileHandle = AddHandle( DX_HANDLETYPE_FILE, FALSE, -1 ) ;
	if( FileHandle == -1 )
	{
		return -1 ;
	}

	if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		return -1 ;
	}
	FileInfo->HandleType = FILEHANDLETYPE_NORMAL ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FilePath, FullPath ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt(    NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;
		AddASyncLoadParamInt(    NULL, &Addr, ASync ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_open_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt(    AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;
		AddASyncLoadParamInt(    AParam->Data, &Addr, ASync ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( FileRead_open_Static( FileHandle, FilePath, ASync, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return FileHandle ;

ERR :
	SubHandle( FileHandle ) ;

	return 0 ;
}

// ファイルアクセス関数
extern int NS_FileRead_open( const TCHAR *FilePath , int ASync )
{
#ifdef UNICODE
	return FileRead_open_UseGParam( FilePath, ASync, GetASyncLoadFlag() ) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, return -1 )

	Result = FileRead_open_UseGParam( UseFilePathBuffer, ASync, GetASyncLoadFlag() ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FilePath )

	return Result ;
#endif
}

extern LONGLONG NS_FileRead_size( const TCHAR *FilePath )
{
	LONGLONG Result ;
	DWORD_PTR fp ;

#ifdef UNICODE
	fp = FOPEN( FilePath ) ;
#else
	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FilePath, return -1 )

	fp = FOPEN( UseFilePathBuffer ) ;

	TCHAR_TO_WCHAR_T_STRING_END( FilePath )
#endif

	if( fp == 0 )
	{
		return -1 ;
	}

	FSEEK( fp, 0L, SEEK_END ) ;
	Result = FTELL( fp ) ;
	FCLOSE( fp ) ;
	return Result ;
}

extern int NS_FileRead_close( int FileHandle )
{
	return SubHandle( FileHandle ) ;
}

extern LONGLONG NS_FileRead_tell( int FileHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

	return FTELL( FileInfo->FilePointer ) ;
}

// FileRead_seek の実処理関数
static int FileRead_seek_Static( int FileHandle, LONGLONG Offset, int Origin, int ASyncThread )
{
	FILEACCESSINFO *FileInfo ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

	return FSEEK( FileInfo->FilePointer, Offset, Origin ) ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_seek の非同期読み込みスレッドから呼ばれる関数
static void FileRead_seek_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	FILEACCESSINFO *FileInfo ;
	int FileHandle ;
	LONGLONG Offset ;
	int Origin ;
	int Addr ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Offset = GetASyncLoadParamLONGLONG( AParam->Data, &Addr ) ;
	Origin = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	if( !FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		FileInfo->HandleInfo.ASyncLoadResult = FileRead_seek_Static( FileHandle, Offset, Origin, TRUE ) ;
	}

	DecASyncLoadCount( FileHandle ) ;
}

#endif // DX_NON_ASYNCLOAD

// ファイルポインタの位置を変更する
extern int FileRead_seek_UseGParam( int FileHandle, LONGLONG Offset, int Origin, int ASyncLoadFlag )
{
	FILEACCESSINFO *FileInfo ;
	int Result = -1 ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamLONGLONG( NULL, &Addr, Offset ) ;
		AddASyncLoadParamInt( NULL, &Addr, Origin ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_seek_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamLONGLONG( AParam->Data, &Addr, Offset ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Origin ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = FileRead_seek_Static( FileHandle, Offset, Origin, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// 終了
	return Result ;
}

extern int NS_FileRead_seek( int FileHandle, LONGLONG Offset, int Origin )
{
	return FileRead_seek_UseGParam( FileHandle, Offset, Origin, GetASyncLoadFlag() ) ;
}

// FileRead_read の実処理関数
static int FileRead_read_Static(
	int FileHandle,
	void *Buffer,
	int ReadSize,
	int ASyncThread
)
{
	FILEACCESSINFO *FileInfo ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

	return ( int )FREAD( Buffer, 1, ReadSize, FileInfo->FilePointer ) ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_read の非同期読み込みスレッドから呼ばれる関数
static void FileRead_read_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int FileHandle ;
	void *Buffer ;
	int ReadSize ;
	int Addr ;
	FILEACCESSINFO *FileInfo ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Buffer = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ReadSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	if( !FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		FileInfo->HandleInfo.ASyncLoadResult = FileRead_read_Static( FileHandle, Buffer, ReadSize, TRUE ) ;
	}

	DecASyncLoadCount( FileHandle ) ;
}

#endif // DX_NON_ASYNCLOAD


// ファイルからデータを読み込む
extern int FileRead_read_UseGParam(
	int FileHandle,
	void *Buffer,
	int ReadSize,
	int ASyncLoadFlag
)
{
	int Result = -1 ;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, Buffer ) ;
		AddASyncLoadParamInt( NULL, &Addr, ReadSize ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto END ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_read_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, Buffer ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, ReadSize ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto END ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;

		Result = 0 ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		Result = FileRead_read_Static( FileHandle, Buffer, ReadSize, FALSE ) ;
	}

#ifndef DX_NON_ASYNCLOAD
END :
#endif

	// 終了
	return Result ;
}

extern int NS_FileRead_read( void *Buffer, int ReadSize, int FileHandle )
{
	return FileRead_read_UseGParam( FileHandle, Buffer, ReadSize, GetASyncLoadFlag() ) ;
}

extern int NS_FileRead_idle_chk( int FileHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( FileInfo->HandleInfo.ASyncLoadCount != 0 )
		return FALSE ;
#endif // DX_NON_ASYNCLOAD

	return FIDLECHK( FileInfo->FilePointer ) ;
}

// ファイルの終端かどうかを得る
extern int NS_FileRead_eof( int FileHandle )
{
	LONGLONG FileSize, NowPoint ;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

	NowPoint = FTELL( FileInfo->FilePointer                     ) ;
	           FSEEK( FileInfo->FilePointer,        0, SEEK_END ) ;
	FileSize = FTELL( FileInfo->FilePointer                     ) ;
	           FSEEK( FileInfo->FilePointer, NowPoint, SEEK_SET ) ;

	return NowPoint == FileSize ;
}

extern int NS_FileRead_gets( TCHAR *Buffer, int BufferSize, int FileHandle )
{
	LONGLONG i, ReadSize ;
	LONGLONG NowPos ;
	FILEACCESSINFO *FileInfo ;
	int UnitSize ;
	DWORD CharCode1 ;
	int   CharBytes1 ;
	DWORD CharCode2 ;
	int   CharBytes2 ;

	if( FILEHCHK( FileHandle, FileInfo ) )
	{
		return -1 ;
	}

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
	{
		return -1 ;
	}

	if( FEOF( FileInfo->FilePointer ) != 0 )
	{
		return -1 ;
	}

	UnitSize = _TCHARSIZE ;

	NowPos   = FTELL( FileInfo->FilePointer ) ;
	ReadSize = ( LONGLONG )FREAD( Buffer, UnitSize, BufferSize - 1, FileInfo->FilePointer ) ;
	FSYNC( FileInfo->FilePointer )
	if( ReadSize == 0 )
	{
		return -1 ;
	}
	ReadSize *= UnitSize ;

#ifdef UNICODE
	if( *( ( WORD * )&Buffer[ 0 ] ) == 0xfeff )
	{
		FSEEK( FileInfo->FilePointer, 2, SEEK_SET ) ;
		NowPos   = FTELL( FileInfo->FilePointer ) ;
		ReadSize = ( LONGLONG )FREAD( Buffer, UnitSize, ( BufferSize - 1 ), FileInfo->FilePointer ) ;
		FSYNC( FileInfo->FilePointer )
		if( ReadSize == 0 )
		{
			return -1 ;
		}
		ReadSize *= UnitSize ;
	}
#endif

	for( i = 0 ; i < ReadSize ; )
	{
		CharCode1 = GetCharCode( ( const char * )&( ( BYTE * )Buffer )[ i              ], _TCODEPAGE, &CharBytes1 ) ;
		if( CharCode1 == '\0' )
		{
			break ;
		}

		CharCode2 = GetCharCode( ( const char * )&( ( BYTE * )Buffer )[ i + CharBytes1 ], _TCODEPAGE, &CharBytes2 ) ;

		if( CharCode1 == '\r' && CharCode2 == '\n' )
		{
			FSEEK( FileInfo->FilePointer, NowPos + i + CharBytes1 + CharBytes2, SEEK_SET ) ;
			break ;
		}

		i += CharBytes1 ;
	}

	PutCharCode( '\0', _TCODEPAGE, ( char * )&( ( BYTE * )Buffer )[ i ] ) ;

	return ( int )i ;
}

// ファイルから一文字読み出す
extern TCHAR NS_FileRead_getc( int FileHandle )
{
	size_t ReadSize ;
	TCHAR c ;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return _T( '\0' ) ;

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return ( TCHAR )-1 ;

	if( FEOF( FileInfo->FilePointer ) != 0 )
	{
		return ( TCHAR )EOF ;
	}

	c = 0 ;
	ReadSize = FREAD( &c, sizeof( TCHAR ), 1, FileInfo->FilePointer ) ;
	FSYNC( FileInfo->FilePointer )
	if( ReadSize == 0 )
	{
		return ( TCHAR )EOF ;
	}

	return c ;
}

// ファイルから書式化されたデータを読み出す
extern int NS_FileRead_scanf( int FileHandle, const TCHAR *Format, ... )
{
	va_list param;
	int Result;

	va_start( param, Format );

	Result = FileRead_scanf_base( FileHandle, Format, param ) ;

	va_end( param );

	return Result;
}

// ファイルの情報を取得する(戻り値  -1:エラー  -1以外:ファイル情報ハンドル  )
extern DWORD_PTR NS_FileRead_createInfo( const TCHAR *ObjectPath )
{
	int           Num, i ;
	DWORD_PTR     FindHandle ;
	FILEINFO     *FileInfo ;
	FILEINFOLIST *FileInfoList ;

	FindHandle = StreamFindFirstT( ObjectPath, NULL ) ;
	if( FindHandle == ( DWORD_PTR )-1 )
	{
		return ( DWORD_PTR )-1 ;
	}

	Num = 0 ;
	do
	{
		Num ++ ;
	}while( StreamFindNextT( FindHandle, NULL ) == 0 ) ;

	FFINDCLOSE( FindHandle );

	FileInfoList = (FILEINFOLIST *)DXALLOC( sizeof( FILEINFOLIST ) + sizeof( FILEINFO ) * Num ) ;
	if( FileInfoList == NULL )
	{
		return ( DWORD_PTR )-1 ;
	}

	FileInfoList->Num	= Num;
	FileInfo			= ( FILEINFO * )( FileInfoList + 1 ) ;
	FindHandle			= StreamFindFirstT( ObjectPath, FileInfo ) ;
	FileInfo ++ ;
	for( i = 1 ; i < Num; i ++, FileInfo ++ )
	{
		StreamFindNextT( FindHandle, FileInfo ) ;
	}

	FFINDCLOSE( FindHandle ) ;

	return ( DWORD_PTR )FileInfoList ;
}

// ファイル情報ハンドル中のファイルの数を取得する
extern int NS_FileRead_getInfoNum( DWORD_PTR FileInfoHandle )
{
	FILEINFOLIST *FileInfoList ;

	FileInfoList = ( FILEINFOLIST * )FileInfoHandle;

	return FileInfoList->Num;
}

// ファイル情報ハンドル中のファイルの情報を取得する
extern int NS_FileRead_getInfo( int Index, FILEINFO *Buffer, DWORD_PTR FileInfoHandle )
{
	FILEINFOLIST *FileInfoList ;

	FileInfoList = ( FILEINFOLIST * )FileInfoHandle ;
	if( Index < 0 || FileInfoList->Num <= Index )
	{
		return -1 ;
	}

	*Buffer = ( ( FILEINFO * )( FileInfoList + 1 ) )[ Index ] ;

	return 0;
}

// ファイル情報ハンドルを削除する
extern int NS_FileRead_deleteInfo( DWORD_PTR FileInfoHandle )
{
	DXFREE( ( FILEINFOLIST * )FileInfoHandle ) ;

	return 0;
}

// 指定のファイル又はフォルダの情報を取得し、ファイル検索ハンドルも作成する( 戻り値: -1=エラー  -1以外=ファイル検索ハンドル )
extern DWORD_PTR NS_FileRead_findFirst( const TCHAR *FilePath, FILEINFO *Buffer )
{
	return StreamFindFirstT( FilePath, Buffer );
}

// 条件の合致する次のファイルの情報を取得する( 戻り値: -1=エラー  0=成功 )
extern int NS_FileRead_findNext( DWORD_PTR FindHandle, FILEINFO *Buffer )
{
	return StreamFindNextT( FindHandle, Buffer );
}

// ファイル検索ハンドルを閉じる( 戻り値: -1=エラー  0=成功 )
extern int NS_FileRead_findClose( DWORD_PTR FindHandle )
{
	return StreamFindClose( FindHandle ) ;
}








// FileRead_fullyLoad の実処理関数
static int FileRead_fullyLoad_Static(
	int FileHandle,
	const wchar_t *FilePath,
	int ASyncThread
)
{
	FILEACCESSINFO *FileInfo ;
	DWORD_PTR FilePointer ;

	if( ASyncThread )
	{
		if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
			return -1 ;
	}
	else
	{
		if( FILEHCHK( FileHandle, FileInfo ) )
			return -1 ;
	}

	if( FileInfo->HandleType != FILEHANDLETYPE_FULLYLOAD )
	{
		return -1 ;
	}

	// ファイルを開く
	FilePointer = FOPEN( FilePath ) ;

	// 開けなかったらエラー
	if( FilePointer == 0 )
	{
		return -1 ;
	}

	// ファイルのサイズを丸ごと読み込む
	FSEEK( FilePointer, 0, SEEK_END ) ;
	FileInfo->FileSize = FTELL( FilePointer ) ;
	FSEEK( FilePointer, 0, SEEK_SET ) ;
	FileInfo->FileImage = DXALLOC( ( size_t )FileInfo->FileSize ) ;
	if( FileInfo->FileImage == NULL )
	{
		FCLOSE( FilePointer ) ;
		FilePointer = 0 ;
		return -1 ;
	}
	FREAD( FileInfo->FileImage, FileInfo->FileSize, 1, FilePointer ) ;

	// ファイルを閉じる
	FCLOSE( FilePointer ) ;
	FilePointer = 0 ;

	// 正常終了
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD

// FileRead_fullyLoad の非同期読み込みスレッドから呼ばれる関数
static void FileRead_fullyLoad_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int FileHandle ;
	const wchar_t *FilePath ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	FileHandle = GetASyncLoadParamInt(    AParam->Data, &Addr ) ;
	FilePath   = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = FileRead_fullyLoad_Static( FileHandle, FilePath, TRUE ) ;

	DecASyncLoadCount( FileHandle ) ;
	if( Result < 0 )
	{
		SubHandle( FileHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// 指定のファイルの内容を全てメモリに読み込み、その情報のアクセスに必要なハンドルを返す( 戻り値  -1:エラー  -1以外:ハンドル )、使い終わったらハンドルは FileRead_fullyLoad_delete で削除する必要があります
extern	int FileRead_fullyLoad_UseGParam( const wchar_t *FilePath, int ASyncLoadFlag )
{
	int FileHandle ;
	FILEACCESSINFO *FileInfo ;

	// ハンドルの作成
	FileHandle = AddHandle( DX_HANDLETYPE_FILE, FALSE, -1 ) ;
	if( FileHandle == -1 )
	{
		return -1 ;
	}

	if( FILEHCHK_ASYNC( FileHandle, FileInfo ) )
	{
		return -1 ;
	}
	FileInfo->HandleType = FILEHANDLETYPE_FULLYLOAD ;

	// その他データを初期化
	FileInfo->FileImage = NULL ;
	FileInfo->FileSize = -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FilePath, FullPath ) ;

		// パラメータに必要なメモリのサイズを算出
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, FileHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// メモリの確保
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// 処理に必要な情報をセット
		AParam->ProcessFunction = FileRead_fullyLoad_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// データを追加
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// 非同期読み込みカウントをインクリメント
		IncASyncLoadCount( FileHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( FileRead_fullyLoad_Static( FileHandle, FilePath, FALSE ) < 0 )
			goto ERR ;
	}

	// 終了
	return FileHandle ;

ERR :
	SubHandle( FileHandle ) ;

	return -1 ;
}

extern	int NS_FileRead_fullyLoad( const TCHAR *FilePath )
{
#ifdef UNICODE
	return FileRead_fullyLoad_UseGParam( FilePath, GetASyncLoadFlag() ) ;
#else
	wchar_t PathBuffer[ FILEPATH_MAX ] ;

	ConvString( ( const char * )FilePath, _TCODEPAGE, ( char * )PathBuffer, WCHAR_T_CODEPAGE ) ;
	return FileRead_fullyLoad_UseGParam( PathBuffer, GetASyncLoadFlag() ) ;
#endif
}

// FileRead_fullyLoad で読み込んだファイルをメモリから削除する
extern	int NS_FileRead_fullyLoad_delete( int FLoadHandle )
{
	return SubHandle( FLoadHandle ) ;
}

// FileRead_fullyLoad で読み込んだファイルの内容を格納したメモリアドレスを取得する
extern	const void* NS_FileRead_fullyLoad_getImage( int FLoadHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FLoadHandle, FileInfo ) )
		return NULL ;

	if( FileInfo->HandleType != FILEHANDLETYPE_FULLYLOAD )
		return NULL ;

	// ファイルを格納しているメモリアドレスを返す
	return FileInfo->FileImage ;
}

// FileRead_fullyLoad で読み込んだファイルのサイズを取得する
extern	LONGLONG NS_FileRead_fullyLoad_getSize(	int FLoadHandle )
{
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FLoadHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_FULLYLOAD )
		return -1 ;

	// ファイルのサイズを返す
	return FileInfo->FileSize ;
}






// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_base( int FileHandle, const void  *Format, va_list Param )
{
	int Result;
	FILEACCESSINFO *FileInfo ;

	if( FILEHCHK( FileHandle, FileInfo ) )
		return -1 ;

	if( FileInfo->HandleType != FILEHANDLETYPE_NORMAL )
		return -1 ;

#ifdef UNICODE
	Result = FileRead_scanf_baseUTF16LE( ( DWORD )FileInfo->FilePointer, ( const WORD * )Format, Param ) ;
#else
	Result = FileRead_scanf_baseCHAR(    ( DWORD )FileInfo->FilePointer, ( const char * )Format, Param ) ;
#endif

	return Result;
}


// FileRead_scanf_base で１文字ファイルから読み込む処理を行う関数
static int FileRead_scanf_base_help_getchar( DWORD *CharCode, int UnitSize, int *CharBytesP, DWORD_PTR FileHandle )
{
	int ReadSize = 0 ;
	int CharBytes ;
	size_t Res = 0 ;
	BYTE TempBuffer[ 64 ] ;

	Res = FREAD( &TempBuffer[ ReadSize ], UnitSize, 1, FileHandle ) ;
	FSYNC( FileHandle ) ;
	if( Res == 0 )
	{
		return 0 ;
	}
	ReadSize += UnitSize ;

	CharBytes = GetCharBytes( ( const char * )TempBuffer, CHAR_CODEPAGE ) ;
	if( CharBytes > ReadSize )
	{
		Res = FREAD( &TempBuffer[ ReadSize ], CharBytes - ReadSize, 1, FileHandle ) ;
		FSYNC( FileHandle ) ;
		if( Res == 0 )
		{
			return 0 ;
		}
		ReadSize = CharBytes ;
	}

	*CharCode = GetCharCode( ( const char * )TempBuffer, _TCODEPAGE, &CharBytes ) ;

	if( CharBytesP != NULL )
	{
		*CharBytesP = CharBytes ;
	}

	return 1 ;
}

static void FileRead_SkipSpace( DWORD_PTR FileHandle, int *Eof )
{
	DWORD c ;
	size_t res ;
	int UnitSize ;
	int CharBytes ;

	UnitSize = GetCodePageUnitSize( CHAR_CODEPAGE ) ;

	if( Eof )
	{
		*Eof = FALSE ;
	}

	for(;;)
	{
		res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
		if( res == 0 )
		{
			if( Eof )
			{
				*Eof = TRUE ;
			}
			return ;
		}

		if( c != ' ' && c != '\n' && c != '\r' && c != '\t' )
		{
			break ;
		}
	}
	FSEEK( FileHandle, -CharBytes, SEEK_CUR ) ;
}

// FileRead_scanf_base などで使用する文字列から数値を取得するための関数
static DWORD FileRead_scanf_base_help_getnumber( const DWORD *CharCode, int *UseCharNum )
{
	DWORD Result ;
	DWORD AddNum ;
	int i ;
	DWORD NumberNum ;
	DWORD NumberStr[ 256 ] ;

	for( NumberNum = 0 ; CharCode[ NumberNum ] >= '0' && CharCode[ NumberNum ] <= '9' ; NumberNum ++ )
	{
		NumberStr[ NumberNum ] = CharCode[ NumberNum ] ;
	}

	Result = 0 ;
	AddNum = 1 ;
	for( i = ( int )( NumberNum - 1 ) ; i >= 0 ; i -- )
	{
		Result += AddNum * ( NumberStr[ i ] - '0' ) ;
		AddNum *= 10 ;
	}

	if( UseCharNum != NULL )
	{
		*UseCharNum = ( int )NumberNum ;
	}

	return Result ;
}

// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_baseCHAR( DWORD_PTR FileHandle, const char *Format, va_list Param )
{
	DWORD c ;
	DWORD str[256] ;
	DWORD Number[256] ;
	DWORD VStr[1024];
	int ReadNum ;
	int Width ;
	int i, j, k ;
	int num ;
	int num2 ;
	int SkipFlag, VStrRFlag, I64Flag, lFlag, hFlag, Eof, MinusFlag, UIntFlag;
	size_t res;
	LONGLONG int64Num ;
	LONGLONG int64Count ;
	int UseCharNum ;
	int UnitSize ;
	int CharBytes ;
	int PutCharBytes ;
	BYTE *pStr ;
	wchar_t *pStrW = NULL ;
	BYTE TempBuffer[ 64 ] ;

	DWORD FormatStringBaseBuffer[ 1024 ] ;
	DWORD *FormatStringTempBuffer = NULL ;
	int FormatStringSize ;
	DWORD *FCode ;

	if( FEOF( FileHandle ) )
	{
		return EOF ;
	}

	UnitSize = GetCodePageUnitSize( CHAR_CODEPAGE ) ;

	FormatStringSize = StringToCharCodeString( Format, CHAR_CODEPAGE, NULL ) ;
	if( FormatStringSize + sizeof( DWORD ) * 16 > sizeof( FormatStringBaseBuffer ) )
	{
		FormatStringTempBuffer = ( DWORD * )malloc( FormatStringSize + sizeof( DWORD ) * 16 ) ;
		if( FormatStringTempBuffer == NULL )
		{
			return -1 ;
		}

		FCode = FormatStringTempBuffer ;
	}
	else
	{
		FCode = FormatStringBaseBuffer ;
	}
	_MEMSET( FCode, 0, FormatStringSize + sizeof( DWORD ) * 16 ) ;
	StringToCharCodeString( Format, CHAR_CODEPAGE, FCode ) ;

	ReadNum = 0;
	Eof = FALSE;
	while( *FCode != '\0' && FEOF( FileHandle ) == 0 )
	{
		// % かどうかで処理を分岐
		if( FCode[ 0 ] == '%' && FCode[ 1 ] != '%' )
		{
			Width		= -1;
			I64Flag		= FALSE;
			lFlag		= FALSE;
			hFlag		= FALSE;
			UIntFlag	= FALSE;
			SkipFlag	= FALSE;

			FCode ++ ;
			if( *FCode == '\0' )
			{
				break;
			}

			if( *FCode == '*' )
			{
				SkipFlag = TRUE ;
				FCode ++ ;
				if( *FCode == '\0' )
				{
					break ;
				}
			}

			if( *FCode >= '0' && *FCode <= '9' )
			{
				Width = ( int )FileRead_scanf_base_help_getnumber( FCode, &UseCharNum ) ;
				FCode += UseCharNum ;
				if( *FCode == '\0' )
				{
					break;
				}
				if( Width == 0 )
				{
					break;
				}
			}

			switch( *FCode )
			{
			case 'l':
			case 'L':
				lFlag = TRUE ;
				FCode ++ ;
				break ;

			case 'h':
			case 'H':
				hFlag = TRUE ;
				FCode ++ ;
				break ;

			case 'I':
				if( FCode[1] == '6' && FCode[2] == '4' )
				{
					I64Flag = TRUE;
					FCode += 3;
				}
				break;
			}
			if( *FCode == '\0' )
			{
				break;
			}

			if( *FCode == '[' )
			{
				if( lFlag || hFlag || I64Flag )
				{
					break;
				}

				FCode ++ ;
				VStrRFlag = FALSE;
				if( *FCode == '^' )
				{
					VStrRFlag = TRUE;
					FCode++;
				}
				j = 0;
				_MEMSET( VStr, 0, sizeof( VStr ) );

				c = '[';
				while( *FCode != '\0' && *FCode != ']' )
				{
					if( *FCode == '-' && c != '[' && FCode[1] != '\0' && FCode[1] != ']' )
					{
						num  = ( int )( BYTE )c ;
						num2 = ( int )( BYTE )FCode[ 1 ] ;
						if( num2 < num )
						{
							k    = num2 ;
							num2 = num ;
							num  = k ;
						}
						for( k = num; k <= num2; k++ )
						{
							if( c != k )
							{
								*( ( BYTE * )&VStr[ j ] ) = ( BYTE )k ;
								j++;
							}
						}
						FCode += 2;
						c = '[';
					}
					else
					{
						VStr[ j ] = *FCode ;
						c = *FCode ;
						j ++ ;
						FCode ++ ;
					}
				}
				if( *FCode == '\0' )
				{
					break ;
				}

				FCode ++ ;
				pStr = NULL ;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, BYTE * );
				}

				FileRead_SkipSpace( FileHandle, &Eof ) ;
				if( Eof == TRUE )
				{
					break ;
				}

				i = 0 ;
				for(;;)
				{
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

					j = 0;
					while( VStr[ j ] != '\0' )
					{
						if( VStr[ j ] == c )
						{
							break ;
						}
						j ++ ;
					}

					if( ( VStrRFlag == TRUE  && VStr[j] != '\0' ) ||
						( VStrRFlag == FALSE && VStr[j] == '\0' ) )
					{
						break ;
					}

					if( pStr )
					{
						pStr += PutCharCode( c, CHAR_CODEPAGE, ( char * )pStr ) ;
					}
					i ++ ;
					if( Width != 0 && Width == i ) 
					{
						break;
					}
				}

				if( pStr )
				{
					pStr += PutCharCode( '\0', CHAR_CODEPAGE, ( char * )pStr ) ;
				}

				if( Eof == FALSE && Width != i )
				{
					FSEEK( FileHandle, -CharBytes, SEEK_CUR );
				}
			}
			else if( *FCode == 'd' || *FCode == 'D' || *FCode == 'u' || *FCode == 'U' )
			{
				FileRead_SkipSpace( FileHandle, &Eof );
				if( Eof == TRUE )
				{
					break ;
				}

				FCode ++ ;
				UIntFlag  = *FCode == 'u' || *FCode == 'U' ;
				MinusFlag = FALSE ;
				res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == '-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_10INT:
				for( i = 0; i < 255 && ( Width == -1 || Width != i ) && c >= '0' && c <= '9'; i ++ )
				{
					Number[ i ] = c - '0';
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -CharBytes, SEEK_CUR );
				}
				num = i;
				if( num == 0 )
				{
					break;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1; i >= 0; i --, int64Count *= 10 )
				{
					if( UIntFlag )
					{
						int64Num = (LONGLONG)( (ULONGLONG)int64Num + (ULONGLONG)Number[i] * (ULONGLONG)int64Count ) ;
					}
					else 
					{
						int64Num += (LONGLONG)Number[i] * int64Count;
					}
				}

				if( MinusFlag )
				{
					int64Num = -int64Num ;
				}
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						if( UIntFlag )
						{
							*va_arg( Param, ULONGLONG * ) = ( ULONGLONG )int64Num ;
						}
						else
						{
							*va_arg( Param, LONGLONG * ) = ( LONGLONG )int64Num ;
						}
					}
					else if( hFlag )
					{
						if( UIntFlag )
						{
							*va_arg( Param, WORD * ) = ( WORD )int64Num;
						}
						else
						{
							*va_arg( Param, short * ) = ( short )int64Num;
						}
					}
					else
					{
						if( UIntFlag )
						{
							*va_arg( Param, DWORD * ) = ( DWORD )int64Num ;
						}
						else
						{
							*va_arg( Param, int * ) = ( int )int64Num ;
						}
					}
				}
			}
			else
			if( *FCode == 'x' || *FCode == 'X' )
			{
				FileRead_SkipSpace( FileHandle, &Eof );
				if( Eof == TRUE )
				{
					break ;
				}

				FCode ++ ;
				MinusFlag = FALSE ;

				res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}

				if( c == '-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}

					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
				}
STR_16INT:
				i = 0;
				for(;;)
				{
					if( i >= 255 )
					{
						break ;
					}

					if( Width != -1 && Width == i )
					{
						break ;
					}
					else
					if( c >= '0' && c <= '9' )
					{
						Number[ i ] = c - '0' ;
					}
					else
					if( c >= 'a' && c <= 'f' )
					{
						Number[ i ] = c - 'a' + 10 ;
					}
					else
					if( c >= 'A' && c <= 'F' )
					{
						Number[ i ] = c - 'A' + 10 ;
					}
					else
					{
						break ;
					}

					i ++ ;
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -CharBytes, SEEK_CUR );
				}
				num = i ;
				if( num == 0 )
				{
					break ;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1 ; i >= 0 ; i --, int64Count *= 16 )
				{
					int64Num = ( LONGLONG )( ( ULONGLONG )int64Num + ( ULONGLONG )Number[ i ] * ( ULONGLONG )int64Count ) ;
				}
				if( MinusFlag )
				{
					int64Num = -int64Num ;
				}
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						*va_arg( Param, ULONGLONG * ) = ( ULONGLONG )int64Num ;
					}
					else
					if( hFlag )
					{
						*va_arg( Param, WORD      * ) = ( WORD      )int64Num ;
					}
					else
					{
						*va_arg( Param, DWORD     * ) = ( DWORD     )int64Num ;
					}
				}
			}
			else
			if( *FCode == 'o' || *FCode == 'O' )
			{
				FileRead_SkipSpace( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FCode ++ ;
				MinusFlag = FALSE ;
				res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == '-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_8INT:
				for( i = 0; i < 255 && ( Width == -1 || Width != i ) && c >= '0' && c <= '7'; i ++ )
				{
					Number[ i ] = c - '0' ;
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -CharBytes, SEEK_CUR );
				}
				num = i ;
				if( num == 0 )
				{
					break ;
				}
				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num - 1; i >= 0; i --, int64Count *= 8 )
				{
					int64Num = ( LONGLONG )( ( ULONGLONG )int64Num + ( ULONGLONG )Number[ i ] * ( ULONGLONG )int64Count ) ;
				}
				if( MinusFlag )
				{
					int64Num = -int64Num ;
				}
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						*va_arg( Param, ULONGLONG * ) = ( ULONGLONG )int64Num ;
					}
					else
					if( hFlag )
					{
						*va_arg( Param, WORD *      ) =      ( WORD )int64Num ;
					}
					else
					{
						*va_arg( Param, DWORD *     ) =     ( DWORD )int64Num ;
					}
				}
			}
			else
			if( *FCode == 'i' || *FCode == 'I' )
			{
				FileRead_SkipSpace( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FCode ++ ;
				MinusFlag = FALSE;
				res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == '-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
				}
				if( c == '0' )
				{
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}
					if( c == 'x' )
					{
						if( Width != -1 )
						{
							Width -- ;
							if( Width == 0 )
							{
								break ;
							}
						}
						res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
						if( res == 0 )
						{
							Eof = TRUE ;
							break ;
						}
						goto STR_16INT;
					}
					else
					{
						goto STR_8INT;
					}
				}
				else
				{
					goto STR_10INT;
				}
			}
			else 
			if( *FCode == 'c' || *FCode == 'C' )
			{
				FCode ++ ;
				if( Width == -1 )
				{
					Width = 1 ;
				}
				pStr = NULL;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, BYTE * ) ;
				}
				for( i = 0; i < Width; i += PutCharBytes / UnitSize )
				{
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE ;
						break ;
					}

					if( pStr != NULL )
					{
						if( FCode[ -1 ] == 'c' )
						{
							PutCharBytes = PutCharCode( c, CHAR_CODEPAGE, ( char * )pStr ) ;
						}
						else
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, CHAR_CODEPAGE, WCHAR_T_CODEPAGE ), WCHAR_T_CODEPAGE, ( char * )pStr ) ;
						}
						pStr += PutCharBytes ;
					}
					else
					{
						if( FCode[ -1 ] == 'c' )
						{
							PutCharBytes = PutCharCode( c, CHAR_CODEPAGE, ( char * )TempBuffer ) ;
						}
						else
						{
							PutCharBytes = PutCharCode( ConvCharCode( c, CHAR_CODEPAGE, WCHAR_T_CODEPAGE ), WCHAR_T_CODEPAGE, ( char * )TempBuffer ) ;
						}
					}
				}
			}
			else
			if( *FCode == 's' || *FCode == 'S' )
			{
				FCode ++ ;
				FileRead_SkipSpace( FileHandle, &Eof );
				if( Eof == TRUE )
				{
					break ;
				}

				pStr = NULL ;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, BYTE * ) ;
				}

				i = 0;
				for(;;)
				{
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

					if( c == ' ' || c == '\n' || c == '\r' || c == '\t' )
					{
						FSEEK( FileHandle, -CharBytes, SEEK_CUR ) ;
						break ;
					}

					if( pStr != NULL )
					{
						if( FCode[ -1 ] == 'c' )
						{
							PutCharBytes = PutCharCode( c, CHAR_CODEPAGE, ( char * )pStr ) ;
						}
						else
						{
							PutCharBytes += PutCharCode( ConvCharCode( c, CHAR_CODEPAGE, WCHAR_T_CODEPAGE ), WCHAR_T_CODEPAGE, ( char * )pStr ) ;
						}
						pStr += PutCharBytes ;
					}
					else
					{
						if( FCode[ -1 ] == 'c' )
						{
							PutCharBytes = PutCharCode( c, CHAR_CODEPAGE, ( char * )TempBuffer ) ;
						}
						else
						{
							PutCharBytes += PutCharCode( ConvCharCode( c, CHAR_CODEPAGE, WCHAR_T_CODEPAGE ), WCHAR_T_CODEPAGE, ( char * )TempBuffer ) ;
						}
					}

					i += PutCharBytes / UnitSize ;
					if( Width != -1 && i >= Width )
					{
						break ;
					}
				}

				if( pStr != NULL )
				{
					if( FCode[ -1 ] == 'c' )
					{
						pStr += PutCharCode( '\0', CHAR_CODEPAGE, ( char * )pStr ) ;
					}
					else
					{
						pStr += PutCharCode( L'\0', WCHAR_T_CODEPAGE, ( char * )pStr ) ;
					}
				}
			}
			else
			if( *FCode == 'f' || *FCode == 'F' || *FCode == 'g' || *FCode == 'G' || *FCode == 'e' || *FCode == 'E' )
			{
				int num3 ;
				DWORD Number2[256] ;
				DWORD Number3[256] ;
				double doubleNum ;
				double doubleNum2 ;
				double doubleNum3 ;
				double doubleCount ;
				int DotFlag ;
				int IndexFlag ;
				int MinusFlag2 ;

				FileRead_SkipSpace( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FCode ++ ;
				MinusFlag  = FALSE ;
				DotFlag    = FALSE ;
				IndexFlag  = FALSE ;
				MinusFlag2 = FALSE ;
				res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == '-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}

				i = 0 ;	//自然数
				j = 0 ;	//小数
				k = 0 ;	//指数
				for(;;)
				{
					if( c == '.' )
					{
						if( DotFlag )
						{
							break ;
						}
						DotFlag = TRUE ;
					}
					else
					if( c == 'e' || c == 'E' )
					{
						if( IndexFlag )
						{
							break ;
						}
						IndexFlag = TRUE ;
					}
					else
					if( c == '-' || c == '+' )
					{
						if( IndexFlag == FALSE || k != 0 )
						{
							break ;
						}
						if( c == '-' )
						{
							MinusFlag2 = TRUE ;
						}
					}
					else
					if( c >= '0' && c <= '9' )
					{
						if( IndexFlag )
						{
							if( k >= 255 )
							{
								break ;
							}
							Number3[ k ] = c - '0' ;
							k ++ ;
						}
						else
						if( DotFlag )
						{
							if( j >= 255 )
							{
								break ;
							}
							Number2[ j ] = c - '0' ;
							j ++ ;
						}
						else
						{
							if( i >= 255 )
							{
								break ;
							}
							Number[ i ] = c - '0' ;
							i ++ ;
						}
					}
					else
					{
						break ;
					}

					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 )
						{
							break ;
						}
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -CharBytes, SEEK_CUR );
				}
				if( i == 0 && j == 0 )
				{
					break ;
				}

				num  = i ;
				num2 = j ;
				num3 = k ;
				if( num == 0 && num2 == 0 )
				{
					break ;
				}

				doubleCount = 1.0 ;
				doubleNum   = 0 ;
				for( i = num - 1; i >= 0; i --, doubleCount *= 10.0 )
				{
					if( Number[ i ] != 0 )
					{
						doubleNum += Number[ i ] * doubleCount;
					}
				}
				if( MinusFlag )
				{
					doubleNum = -doubleNum ;
				}

				doubleCount = 0.1 ;
				doubleNum2  = 0 ;
				for( i = 0; i < num2; i ++, doubleCount /= 10.0 )
				{
					if( Number2[ i ] != 0 )
					{
						doubleNum2 += Number2[ i ] * doubleCount;
					}
				}
				if( MinusFlag )
				{
					doubleNum2 = -doubleNum2 ;
				}

				int64Count = 1 ;
				int64Num   = 0 ;
				for( i = num3 - 1; i >= 0; i --, int64Count *= 10 )
				{
					int64Num += Number3[ i ] * int64Count ;
				}
				if( MinusFlag2 )
				{
					int64Num = -int64Num ;
				}

				doubleNum3 = 1.0;
				if( int64Num != 0 )
				{
					if( int64Num < 0 )
					{
						int64Num = -int64Num;
						for( i = 0; i < int64Num; i++ )
						{
							doubleNum3 /= 10.0 ;
						}
					}
					else
					{
						for( i = 0; i < int64Num; i++ )
						{
							doubleNum3 *= 10.0 ;
						}
					}
				}

				doubleNum = ( doubleNum + doubleNum2 ) * doubleNum3;

				if( SkipFlag == FALSE )
				{
					if( lFlag )
					{
						*va_arg( Param, double * ) =          doubleNum ;
					}
					else
					{
						*va_arg( Param, float *  ) = ( float )doubleNum ;
					}
				}
			}
			if( SkipFlag == FALSE )
			{
				ReadNum ++ ;
			}
		}
		else
		{
			if( *FCode == ' ' || *FCode == '\n' || *FCode == '\r' || *FCode == '\t' )
			{
				while( *FCode != '\0' && ( *FCode == ' ' || *FCode == '\n' || *FCode == '\r' || *FCode == '\t' ) )
				{
					FCode ++ ;
				}
				FileRead_SkipSpace( FileHandle, &Eof ) ;
				if( Eof == TRUE ) break;
			}
			else
			{
				for( num = 0; *FCode != '\0' && *FCode != ' ' && *FCode != '\n' && *FCode != '\r' && *FCode != '\t' && *FCode != '%'; num++, FCode ++ )
				{
					str[ num ] = *FCode;
				}
				str[ num ] = '\0';

				for( i = 0; i < num; i ++ )
				{
					res = FileRead_scanf_base_help_getchar( &c, UnitSize, &CharBytes, FileHandle ) ;
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}

					if( str[ i ] != c )
					{
						break ;
					}
				}
			}
		}
	}

	if( FormatStringTempBuffer != NULL )
	{
		DXFREE( FormatStringTempBuffer ) ;
		FormatStringTempBuffer = NULL ;
	}

	if( FEOF( FileHandle ) && ReadNum == 0 )
	{
		return EOF;
	}

	return ReadNum;
}
















static void FileRead_SkipSpaceUTF16LE( DWORD_PTR FileHandle, int *Eof )
{
	WORD c ;
	size_t res ;

	if( Eof ) *Eof = FALSE ;
	for(;;)
	{
		res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
		FSYNC( FileHandle )
		if( res == 0 )
		{
			if( Eof ) *Eof = TRUE;
			return;
		}
		if( c != L' ' && c != L'\n' && c != L'\r' ) break;
	}
	FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
}

static void FileRead_strcpy_UTF16LE( WORD *Dest, const WORD *Src )
{
	int i ;
	for( i = 0 ; Src[i] != L'\0' ; i ++ ) Dest[i] = Src[i] ;
	Dest[i] = L'\0' ;
}

// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_baseUTF16LE( DWORD_PTR FileHandle, const WORD *FormatUTF16LE, va_list Param )
{
	WORD c, c2[2], tstr[512], str[256], Number[128], Number2[128], Number3[128], VStr[1024];
	int ReadNum, Width, i, j, k, num, num2, num3;
	int SkipFlag, VStrRFlag, I64Flag, lFlag, hFlag, Eof, MinusFlag, UIntFlag;
	int DotFlag, IndexFlag, MinusFlag2 ;
	size_t res;
	LONGLONG int64Num, int64Count;
	double doubleNum, doubleNum2, doubleNum3, doubleCount;
	WORD *pStr;
	char *pStrA ;

	if( FEOF( FileHandle ) ) return EOF;

	if( FTELL( FileHandle ) == 0 )
	{
		FREAD( &c, sizeof( WORD ), 1, FileHandle ) ;
		FSYNC( FileHandle )
		if( *( ( WORD * )&c ) != 0xfeff )
		{
			FSEEK( FileHandle, 0, SEEK_SET ) ;
		}
	}

	ReadNum = 0;
	Eof = FALSE;
	while( *FormatUTF16LE != L'\0' && FEOF( FileHandle ) == 0 )
	{
		if( FormatUTF16LE[0] == L'%' && FormatUTF16LE[1] != L'%' )
		{
			Width = -1;
			I64Flag = FALSE;
			lFlag = FALSE;
			hFlag = FALSE;
			UIntFlag = FALSE;
			SkipFlag = FALSE;
			FormatUTF16LE ++ ;
			if( *FormatUTF16LE == L'\0' ) break;
			if( *FormatUTF16LE == L'*' )
			{
				SkipFlag = TRUE;
				FormatUTF16LE ++ ;
			}
			if( *FormatUTF16LE == L'\0' ) break;
			if( *FormatUTF16LE >= L'0' && *FormatUTF16LE <= L'9' )
			{
				for( i = 0; FormatUTF16LE[i] >= L'0' && FormatUTF16LE[i] <= L'9'; i++ )
					str[i] = FormatUTF16LE[i];
				str[i] = L'\0';
				FormatUTF16LE += i;
				Width = CL_atoi( DX_CODEPAGE_UTF16LE, ( const char * )str ) ;
				if( Width == 0 ) break;
			}
			if( *FormatUTF16LE == L'\0' ) break;
			switch( *FormatUTF16LE )
			{
			case L'l': case L'H': lFlag = TRUE; FormatUTF16LE++ ; break;
			case L'h': case L'L': hFlag = TRUE; FormatUTF16LE++ ; break;
			case L'I':
				if( FormatUTF16LE[1] == L'6' && FormatUTF16LE[2] == L'4' )
				{
					I64Flag = TRUE;
					FormatUTF16LE += 3;
				}
				break;
			}
			if( *FormatUTF16LE == L'\0' ) break;

			if( *FormatUTF16LE == L'[' )
			{
				if( lFlag || hFlag || I64Flag ) break;

				FormatUTF16LE ++ ;
				VStrRFlag = FALSE;
				if( *FormatUTF16LE == L'^' )
				{
					VStrRFlag = TRUE;
					FormatUTF16LE++;
				}
				j = 0;
				_MEMSET( VStr, 0, sizeof( VStr ) );
				c = L'[';
				while( *FormatUTF16LE != L'\0' && *FormatUTF16LE != L']' )
				{
					if( CHECK_CPU_LE_UTF16LE_4BYTE( *FormatUTF16LE ) == TRUE )
					{
						if( FormatUTF16LE[1] == L'\0' )
						{
							FormatUTF16LE++;
							break;
						}
						if( FormatUTF16LE[1] == L']' ) break;
						VStr[j]   = FormatUTF16LE[0];
						VStr[j+1] = FormatUTF16LE[1];
						j += 2 ;
						FormatUTF16LE += 2 ;
						c = L'[';
					}
					else
					{
						if( *FormatUTF16LE == L'-' && c != L'[' && FormatUTF16LE[1] != L'\0' && FormatUTF16LE[1] != L']' )
						{
							num  = (int)(WORD)c;
							num2 = (int)(WORD)FormatUTF16LE[1];
							if( num2 < num )
							{
								k = num2; num2 = num; num = k;
							}
							for( k = num; k <= num2; k++ )
							{
								if( c != k )
								{
									*((WORD *)&VStr[j]) = (WORD)k;
									j++;
								}
							}
							FormatUTF16LE += 2;
							c = L'[';
						}
						else
						{
							VStr[j] = *FormatUTF16LE;
							c = *FormatUTF16LE;
							j ++ ;
							FormatUTF16LE ++ ;
						}
					}
				}
				if( *FormatUTF16LE == L'\0' ) break;
				FormatUTF16LE ++ ;
				pStr = NULL;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, WORD * );
				}
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;
				i = 0;
				for(;;)
				{
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( CHECK_CPU_LE_UTF16LE_4BYTE( c ) == TRUE )
					{
						c2[0] = c;
						res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
						FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						c2[1] = c;

						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( CHECK_CPU_LE_UTF16LE_4BYTE( VStr[j] ) == TRUE )
							{
								if( VStr[j] == c2[0] && VStr[j+1] == c2[1] ) break;
								j += 2;
							}
							else
							{
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) )
						{
							FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
							break;
						}

						if( Width - i == 1 )
						{
							FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr ++ ;
							}
							i ++ ;
						}
						else
						{
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr[1] = c2[1];
								pStr += 2 ;
							}
							i += 2 ;
						}
					}
					else
					{
						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( CHECK_CPU_LE_UTF16LE_4BYTE( VStr[j] ) == TRUE )
							{
								j += 2;
							}
							else
							{
								if( VStr[j] == c ) break;
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) ) break;
						if( pStr )
						{
							*pStr = c;
							pStr ++ ;
						}
						i ++ ;
					}
					if( Width != 0 && Width == i ) break;
				}
				if( pStr ) *pStr = L'\0';
				if( Eof == FALSE && Width != i )
				{
					FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
				}
			}
			else if( *FormatUTF16LE == L'd' || *FormatUTF16LE == L'D' || *FormatUTF16LE == L'u' || *FormatUTF16LE == L'U' )
			{
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				UIntFlag = *FormatUTF16LE == L'u' || *FormatUTF16LE == L'U';
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_10INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'9'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 10 )
				{
					if( UIntFlag )
						int64Num = (int)( (ULONGLONG)int64Num + (ULONGLONG)Number[i] * (ULONGLONG)int64Count );
					else 
						int64Num += (LONGLONG)Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						if( UIntFlag ) *va_arg( Param,      ULONGLONG * ) =      (ULONGLONG)int64Num;
						else           *va_arg( Param,       LONGLONG * ) =       (LONGLONG)int64Num;
					}
					else if( hFlag )
					{
						if( UIntFlag ) *va_arg( Param, unsigned short * ) = (unsigned short)int64Num;
						else           *va_arg( Param,          short * ) =          (short)int64Num;
					}
					else
					{
						if( UIntFlag ) *va_arg( Param,   unsigned int * ) =   (unsigned int)int64Num;
						else           *va_arg( Param,            int * ) =            (int)int64Num;
					}
				}
			}
			else if( *FormatUTF16LE == L'x' || *FormatUTF16LE == L'X' )
			{
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_16INT:
				i = 0;
				for(;;)
				{
					if( i >= 127 ) break;
					if( Width != -1 && Width == i ) break;
					else if( c >= L'0' && c <= L'9' ) Number[i] = ( WORD )( c - L'0' );
					else if( c >= L'a' && c <= L'f' ) Number[i] = ( WORD )( c - L'a' + 10 );
					else if( c >= L'A' && c <= L'F' ) Number[i] = ( WORD )( c - L'A' + 10 );
					else break;
					i ++ ;
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 16 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *FormatUTF16LE == L'o' || *FormatUTF16LE == L'O' )
			{
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_8INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'7'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 8 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *FormatUTF16LE == L'i' || *FormatUTF16LE == L'I' )
			{
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( c == L'0' )
				{
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( c == L'x' )
					{
						if( Width != -1 )
						{
							Width -- ;
							if( Width == 0 ) break;
						}
						res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
						FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						goto STR_16INT;
					}
					else
					{
						goto STR_8INT;
					}
				}
				else
				{
					goto STR_10INT;
				}
			}
			else if( *FormatUTF16LE == L'c' )
			{
				FormatUTF16LE ++ ;
				if( Width == -1 ) Width = 1;
				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( FormatUTF16LE[ -1 ] == 'c' )
					{
						pStr = va_arg( Param, WORD * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}
				for( i = 0; i < Width; i ++ )
				{
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					tstr[ i ] = c ;
				}
				tstr[ i ] = L'\0' ;
				if( pStr )
				{
					FileRead_strcpy_UTF16LE( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, DX_CODEPAGE_UTF16LE, pStrA, DX_CODEPAGE_SHIFTJIS ) ;
				}
			}
			else if( *FormatUTF16LE == L's' || *FormatUTF16LE == L'S' )
			{
				FormatUTF16LE ++ ;
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( FormatUTF16LE[ -1 ] == 's' )
					{
						pStr = va_arg( Param, WORD * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}

				i = 0;
				for(;;)
				{
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE; 
						break;
					}
					if( CHECK_CPU_LE_UTF16LE_4BYTE( c ) == FALSE && ( c == L' ' || c == L'\n' || c == L'\r' ) )
					{
						FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
						break;
					}
					tstr[ i ] = c ;
					i ++ ;
					if( Width != -1 && i >= Width ) break;

					if( CHECK_CPU_LE_UTF16LE_4BYTE( c ) == TRUE )
					{
						res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
						FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE; 
							break;
						}
						tstr[ i ] = c ;
						i ++ ;
						if( Width != -1 && i >= Width ) break;
					}
				}
				tstr[ i ] = '\0' ;
				if( pStr )
				{
					FileRead_strcpy_UTF16LE( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, DX_CODEPAGE_UTF16LE, pStrA, DX_CODEPAGE_SHIFTJIS ) ;
				}
			}
			else if( *FormatUTF16LE == L'f' || *FormatUTF16LE == L'F' || *FormatUTF16LE == L'g' || *FormatUTF16LE == L'G' )
			{
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				FormatUTF16LE ++ ;
				MinusFlag = FALSE;
				DotFlag = FALSE;
				IndexFlag = FALSE;
				MinusFlag2 = FALSE;
				res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}

				i = 0;	//自然数
				j = 0;	//小数
				k = 0;	//指数
				for(;;)
				{
					if( c == L'.' )
					{
						if( DotFlag ) break;
						DotFlag = TRUE;
					}
					else if( c == L'e' || c == L'E' )
					{
						if( IndexFlag ) break;
						IndexFlag = TRUE;
					}
					else if( c == L'-' || c == L'+' )
					{
						if( IndexFlag == FALSE || k != 0 ) break;
						if( c == L'-' ) MinusFlag2 = TRUE;
					}
					else if( c >= L'0' && c <= L'9' )
					{
						if( IndexFlag )
						{
							if( k >= 127 ) break ;
							Number3[k] = ( WORD )( c - L'0' );
							k ++ ;
						}
						else if( DotFlag )
						{
							if( j >= 127 ) break ;
							Number2[j] = ( WORD )( c - L'0' );
							j ++ ;
						}
						else
						{
							if( i >= 127 ) break ;
							Number[i] = ( WORD )( c - L'0' );
							i ++ ;
						}
					}
					else break;

					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( WORD ), SEEK_CUR );
				}
				if( i == 0 && j == 0 ) break;
				num = i;
				num2 = j;
				num3 = k;
				if( num == 0 && num2 == 0 ) break;

				doubleCount = 1.0;
				doubleNum = 0;
				for( i = num - 1; i >= 0; i --, doubleCount *= 10.0 )
				{
					if( Number[i] != 0 )
						doubleNum += Number[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum = -doubleNum;

				doubleCount = 0.1;
				doubleNum2 = 0;
				for( i = 0; i < num2; i ++, doubleCount /= 10.0 )
				{
					if( Number2[i] != 0 )
						doubleNum2 += Number2[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum2 = -doubleNum2;

				int64Count = 1;
				int64Num = 0;
				for( i = num3 - 1; i >= 0; i --, int64Count *= 10 )
				{
					int64Num += Number3[i] * int64Count;
				}
				if( MinusFlag2 ) int64Num = -int64Num;

				doubleNum3 = 1.0;
				if( int64Num != 0 )
				{
					if( int64Num < 0 )
					{
						int64Num = -int64Num;
						for( i = 0; i < int64Num; i++ )
							doubleNum3 /= 10.0;
					}
					else
					{
						for( i = 0; i < int64Num; i++ )
							doubleNum3 *= 10.0;
					}
				}

				doubleNum = ( doubleNum + doubleNum2 ) * doubleNum3;

				if( SkipFlag == FALSE )
				{
					if( lFlag ) *va_arg( Param, double * ) = doubleNum;
					else        *va_arg( Param, float *  ) = (float)doubleNum;
				}
			}
			if( SkipFlag == FALSE ) ReadNum ++ ;
		}
		else
		{
			if( *FormatUTF16LE == L' ' || *FormatUTF16LE == L'\n' || *FormatUTF16LE == L'\r' )
			{
				while( *FormatUTF16LE != L'\0' && ( *FormatUTF16LE == L' ' || *FormatUTF16LE == L'\n' || *FormatUTF16LE == L'\r' ) ) FormatUTF16LE ++ ;
				FileRead_SkipSpaceUTF16LE( FileHandle, &Eof );
				if( Eof == TRUE ) break;
			}
			else
			{
				for( i = 0; *FormatUTF16LE != L'\0' && *FormatUTF16LE != L' ' && *FormatUTF16LE != L'\n' && *FormatUTF16LE != L'\r' && *FormatUTF16LE != L'%'; i++, FormatUTF16LE ++ )
					str[i] = *FormatUTF16LE;
				str[i] = L'\0';
				num = i;
				for( i = 0; i < num; i ++ )
				{
					res = FREAD( &c, sizeof( WORD ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( str[i] != c ) break;
				}
			}
		}
	}

	if( FEOF( FileHandle ) && ReadNum == 0 ) return EOF;

	return ReadNum;
}

































#if 0

static void FileRead_SkipSpaceW( DWORD_PTR FileHandle, int *Eof )
{
	wchar_t c;
	size_t res;

	if( Eof ) *Eof = FALSE;
	for(;;)
	{
		res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
		FSYNC( FileHandle )
		if( res == 0 )
		{
			if( Eof ) *Eof = TRUE;
			return;
		}
		if( c != L' ' && c != L'\n' && c != L'\r' ) break;
	}
	FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
}

// サロゲートペアかどうかの判定( TRUE:４バイト文字上位  FALSE:２バイト文字 )
static int _CheckUTF16H( wchar_t c )
{
	return ( DWORD )c >= 0xd800 && ( DWORD )c <= 0xdbff ? TRUE : FALSE ;
}

static void __WCSCPY( wchar_t *Dest, const wchar_t *Src )
{
	int i ;
	for( i = 0 ; Src[i] != L'\0' ; i ++ ) Dest[i] = Src[i] ;
	Dest[i] = L'\0' ;
}

// ファイルから書式化されたデータを読み出す
extern int FileRead_scanf_baseW( DWORD_PTR FileHandle, const wchar_t *Format, va_list Param )
{
	wchar_t c, c2[2], tstr[512], str[256], Number[128], Number2[128], Number3[128], VStr[1024];
	int ReadNum, Width, i, j, k, num, num2, num3;
	int SkipFlag, VStrRFlag, I64Flag, lFlag, hFlag, Eof, MinusFlag, UIntFlag;
	int DotFlag, IndexFlag, MinusFlag2 ;
	size_t res;
	LONGLONG int64Num, int64Count;
	double doubleNum, doubleNum2, doubleNum3, doubleCount;
	wchar_t *pStr;
	char *pStrA ;

	if( FEOF( FileHandle ) ) return EOF;

	if( FTELL( FileHandle ) == 0 )
	{
		FREAD( &c, sizeof( wchar_t ), 1, FileHandle ) ;
		FSYNC( FileHandle )
		if( *( ( WORD * )&c ) != 0xfeff )
		{
			FSEEK( FileHandle, 0, SEEK_SET ) ;
		}
	}

	ReadNum = 0;
	Eof = FALSE;
	while( *Format != L'\0' && FEOF( FileHandle ) == 0 )
	{
		if( Format[0] == L'%' && Format[1] != L'%' )
		{
			Width = -1;
			I64Flag = FALSE;
			lFlag = FALSE;
			hFlag = FALSE;
			UIntFlag = FALSE;
			SkipFlag = FALSE;
			Format ++ ;
			if( *Format == L'\0' ) break;
			if( *Format == L'*' )
			{
				SkipFlag = TRUE;
				Format ++ ;
			}
			if( *Format == L'\0' ) break;
			if( *Format >= L'0' && *Format <= L'9' )
			{
				for( i = 0; Format[i] >= L'0' && Format[i] <= L'9'; i++ )
					str[i] = Format[i];
				str[i] = L'\0';
				Format += i;
				Width = _ATOIW( ( wchar_t * )str );
				if( Width == 0 ) break;
			}
			if( *Format == L'\0' ) break;
			switch( *Format )
			{
			case L'l': case L'H': lFlag = TRUE; Format++ ; break;
			case L'h': case L'L': hFlag = TRUE; Format++ ; break;
			case L'I':
				if( Format[1] == L'6' && Format[2] == L'4' )
				{
					I64Flag = TRUE;
					Format += 3;
				}
				break;
			}
			if( *Format == L'\0' ) break;

			if( *Format == L'[' )
			{
				if( lFlag || hFlag || I64Flag ) break;

				Format ++ ;
				VStrRFlag = FALSE;
				if( *Format == L'^' )
				{
					VStrRFlag = TRUE;
					Format++;
				}
				j = 0;
				_MEMSET( VStr, 0, sizeof( VStr ) );
				c = L'[';
				while( *Format != L'\0' && *Format != L']' )
				{
					if( _CheckUTF16H( *Format ) == TRUE )
					{
						if( Format[1] == L'\0' )
						{
							Format++;
							break;
						}
						if( Format[1] == L']' ) break;
						VStr[j]   = Format[0];
						VStr[j+1] = Format[1];
						j += 2 ;
						Format += 2 ;
						c = L'[';
					}
					else
					{
						if( *Format == L'-' && c != L'[' && Format[1] != L'\0' && Format[1] != L']' )
						{
							num  = (int)(unsigned short)c;
							num2 = (int)(unsigned short)Format[1];
							if( num2 < num )
							{
								k = num2; num2 = num; num = k;
							}
							for( k = num; k <= num2; k++ )
							{
								if( c != k )
								{
									*((unsigned short *)&VStr[j]) = (unsigned short)k;
									j++;
								}
							}
							Format += 2;
							c = L'[';
						}
						else
						{
							VStr[j] = *Format;
							c = *Format;
							j ++ ;
							Format ++ ;
						}
					}
				}
				if( *Format == L'\0' ) break;
				Format ++ ;
				pStr = NULL;
				if( SkipFlag == FALSE )
				{
					pStr = va_arg( Param, wchar_t * );
				}
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;
				i = 0;
				for(;;)
				{
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( _CheckUTF16H( c ) == TRUE )
					{
						c2[0] = c;
						res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
						FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						c2[1] = c;

						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( _CheckUTF16H( VStr[j] ) == TRUE )
							{
								if( VStr[j] == c2[0] && VStr[j+1] == c2[1] ) break;
								j += 2;
							}
							else
							{
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) )
						{
							FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
							break;
						}

						if( Width - i == 1 )
						{
							FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr ++ ;
							}
							i ++ ;
						}
						else
						{
							if( pStr )
							{
								pStr[0] = c2[0];
								pStr[1] = c2[1];
								pStr += 2 ;
							}
							i += 2 ;
						}
					}
					else
					{
						j = 0;
						while( VStr[j] != L'\0' )
						{
							if( _CheckUTF16H( VStr[j] ) == TRUE )
							{
								j += 2;
							}
							else
							{
								if( VStr[j] == c ) break;
								j ++ ;
							}
						}

						if( ( VStrRFlag == TRUE  && VStr[j] != L'\0' ) || ( VStrRFlag == FALSE && VStr[j] == L'\0' ) ) break;
						if( pStr )
						{
							*pStr = c;
							pStr ++ ;
						}
						i ++ ;
					}
					if( Width != 0 && Width == i ) break;
				}
				if( pStr ) *pStr = L'\0';
				if( Eof == FALSE && Width != i )
				{
					FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
			}
			else if( *Format == L'd' || *Format == L'D' || *Format == L'u' || *Format == L'U' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				UIntFlag = *Format == L'u' || *Format == L'U';
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_10INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'9'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 10 )
				{
					if( UIntFlag )
						int64Num = (int)( (ULONGLONG)int64Num + (ULONGLONG)Number[i] * (ULONGLONG)int64Count );
					else 
						int64Num += (LONGLONG)Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )
					{
						if( UIntFlag ) *va_arg( Param,      ULONGLONG * ) =      (ULONGLONG)int64Num;
						else           *va_arg( Param,       LONGLONG * ) =       (LONGLONG)int64Num;
					}
					else if( hFlag )
					{
						if( UIntFlag ) *va_arg( Param, unsigned short * ) = (unsigned short)int64Num;
						else           *va_arg( Param,          short * ) =          (short)int64Num;
					}
					else
					{
						if( UIntFlag ) *va_arg( Param,   unsigned int * ) =   (unsigned int)int64Num;
						else           *va_arg( Param,            int * ) =            (int)int64Num;
					}
				}
			}
			else if( *Format == L'x' || *Format == L'X' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_16INT:
				i = 0;
				for(;;)
				{
					if( i >= 127 ) break;
					if( Width != -1 && Width == i ) break;
					else if( c >= L'0' && c <= L'9' ) Number[i] = ( WORD )( c - L'0' );
					else if( c >= L'a' && c <= L'f' ) Number[i] = ( WORD )( c - L'a' + 10 );
					else if( c >= L'A' && c <= L'F' ) Number[i] = ( WORD )( c - L'A' + 10 );
					else break;
					i ++ ;
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 16 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *Format == L'o' || *Format == L'O' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
STR_8INT:
				for( i = 0; i < 127 && ( Width == -1 || Width != i ) && c >= L'0' && c <= L'7'; i ++ )
				{
					Number[i] = ( WORD )( c - L'0' );
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				num = i;
				if( num == 0 ) break;
				int64Count = 1;
				int64Num = 0;
				for( i = num - 1; i >= 0; i --, int64Count *= 8 )
				{
					int64Num += Number[i] * int64Count;
				}
				if( MinusFlag ) int64Num = -int64Num;
				if( SkipFlag == FALSE )
				{
					if( I64Flag )    *va_arg( Param, LONGLONG * ) = (LONGLONG)int64Num;
					else if( hFlag ) *va_arg( Param, short *    ) =    (short)int64Num;
					else             *va_arg( Param, int *      ) =      (int)int64Num;
				}
			}
			else if( *Format == L'i' || *Format == L'I' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}
				if( c == L'0' )
				{
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( c == L'x' )
					{
						if( Width != -1 )
						{
							Width -- ;
							if( Width == 0 ) break;
						}
						res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
						FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE;
							break;
						}
						goto STR_16INT;
					}
					else
					{
						goto STR_8INT;
					}
				}
				else
				{
					goto STR_10INT;
				}
			}
			else if( *Format == L'c' )
			{
				Format ++ ;
				if( Width == -1 ) Width = 1;
				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( Format[ -1 ] == 'c' )
					{
						pStr = va_arg( Param, wchar_t * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}
				for( i = 0; i < Width; i ++ )
				{
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					tstr[ i ] = c ;
				}
				tstr[ i ] = L'\0' ;
				if( pStr )
				{
					__WCSCPY( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, WCHAR_T_CODEPAGE, pStrA, DX_CODEPAGE_SHIFTJIS ) ;
				}
			}
			else if( *Format == L's' || *Format == L'S' )
			{
				Format ++ ;
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				pStr = NULL;
				pStrA = NULL;
				if( SkipFlag == FALSE )
				{
					if( Format[ -1 ] == 's' )
					{
						pStr = va_arg( Param, wchar_t * );
					}
					else
					{
						pStrA = va_arg( Param, char * );
					}
				}

				i = 0;
				for(;;)
				{
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE; 
						break;
					}
					if( _CheckUTF16H( c ) == FALSE && ( c == L' ' || c == L'\n' || c == L'\r' ) )
					{
						FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
						break;
					}
					tstr[ i ] = c ;
					i ++ ;
					if( Width != -1 && i >= Width ) break;

					if( _CheckUTF16H( c ) == TRUE )
					{
						res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
						FSYNC( FileHandle )
						if( res == 0 )
						{
							Eof = TRUE; 
							break;
						}
						tstr[ i ] = c ;
						i ++ ;
						if( Width != -1 && i >= Width ) break;
					}
				}
				tstr[ i ] = '\0' ;
				if( pStr )
				{
					__WCSCPY( pStr, tstr ) ;
				}
				if( pStrA )
				{
					ConvString( ( const char * )tstr, WCHAR_T_CODEPAGE, pStrA, DX_CODEPAGE_SHIFTJIS ) ;
				}
			}
			else if( *Format == L'f' || *Format == L'F' || *Format == L'g' || *Format == L'G' )
			{
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;

				Format ++ ;
				MinusFlag = FALSE;
				DotFlag = FALSE;
				IndexFlag = FALSE;
				MinusFlag2 = FALSE;
				res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
				FSYNC( FileHandle )
				if( res == 0 )
				{
					Eof = TRUE;
					break;
				}
				if( c == L'-' )
				{
					MinusFlag = TRUE;
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
				}

				i = 0;	//自然数
				j = 0;	//小数
				k = 0;	//指数
				for(;;)
				{
					if( c == L'.' )
					{
						if( DotFlag ) break;
						DotFlag = TRUE;
					}
					else if( c == L'e' || c == L'E' )
					{
						if( IndexFlag ) break;
						IndexFlag = TRUE;
					}
					else if( c == L'-' || c == L'+' )
					{
						if( IndexFlag == FALSE || k != 0 ) break;
						if( c == L'-' ) MinusFlag2 = TRUE;
					}
					else if( c >= L'0' && c <= L'9' )
					{
						if( IndexFlag )
						{
							if( k >= 127 ) break ;
							Number3[k] = ( WORD )( c - L'0' );
							k ++ ;
						}
						else if( DotFlag )
						{
							if( j >= 127 ) break ;
							Number2[j] = ( WORD )( c - L'0' );
							j ++ ;
						}
						else
						{
							if( i >= 127 ) break ;
							Number[i] = ( WORD )( c - L'0' );
							i ++ ;
						}
					}
					else break;

					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( Width != -1 )
					{
						Width -- ;
						if( Width == 0 ) break;
					}
				}
				if( Eof == FALSE )
				{
					FSEEK( FileHandle, -( int )sizeof( wchar_t ), SEEK_CUR );
				}
				if( i == 0 && j == 0 ) break;
				num = i;
				num2 = j;
				num3 = k;
				if( num == 0 && num2 == 0 ) break;

				doubleCount = 1.0;
				doubleNum = 0;
				for( i = num - 1; i >= 0; i --, doubleCount *= 10.0 )
				{
					if( Number[i] != 0 )
						doubleNum += Number[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum = -doubleNum;

				doubleCount = 0.1;
				doubleNum2 = 0;
				for( i = 0; i < num2; i ++, doubleCount /= 10.0 )
				{
					if( Number2[i] != 0 )
						doubleNum2 += Number2[i] * doubleCount;
				}
				if( MinusFlag ) doubleNum2 = -doubleNum2;

				int64Count = 1;
				int64Num = 0;
				for( i = num3 - 1; i >= 0; i --, int64Count *= 10 )
				{
					int64Num += Number3[i] * int64Count;
				}
				if( MinusFlag2 ) int64Num = -int64Num;

				doubleNum3 = 1.0;
				if( int64Num != 0 )
				{
					if( int64Num < 0 )
					{
						int64Num = -int64Num;
						for( i = 0; i < int64Num; i++ )
							doubleNum3 /= 10.0;
					}
					else
					{
						for( i = 0; i < int64Num; i++ )
							doubleNum3 *= 10.0;
					}
				}

				doubleNum = ( doubleNum + doubleNum2 ) * doubleNum3;

				if( SkipFlag == FALSE )
				{
					if( lFlag ) *va_arg( Param, double * ) = doubleNum;
					else        *va_arg( Param, float *  ) = (float)doubleNum;
				}
			}
			if( SkipFlag == FALSE ) ReadNum ++ ;
		}
		else
		{
			if( *Format == L' ' || *Format == L'\n' || *Format == L'\r' )
			{
				while( *Format != L'\0' && ( *Format == L' ' || *Format == L'\n' || *Format == L'\r' ) ) Format ++ ;
				FileRead_SkipSpaceW( FileHandle, &Eof );
				if( Eof == TRUE ) break;
			}
			else
			{
				for( i = 0; *Format != L'\0' && *Format != L' ' && *Format != L'\n' && *Format != L'\r' && *Format != L'%'; i++, Format ++ )
					str[i] = *Format;
				str[i] = L'\0';
				num = i;
				for( i = 0; i < num; i ++ )
				{
					res = FREAD( &c, sizeof( wchar_t ), 1, FileHandle );
					FSYNC( FileHandle )
					if( res == 0 )
					{
						Eof = TRUE;
						break;
					}
					if( str[i] != c ) break;
				}
			}
		}
	}

	if( FEOF( FileHandle ) && ReadNum == 0 ) return EOF;

	return ReadNum;
}

#endif


























// メモリに置かれたデータをファイルのデータに例えてつかうための関数
extern void *MemStreamOpen( const void *DataBuffer, unsigned int DataSize )
{
	MEMSTREAMDATA *NewMemData ;

	// メモリの確保
	if( ( NewMemData = ( MEMSTREAMDATA * )DXALLOC( sizeof( MEMSTREAMDATA ) ) ) == NULL ) return NULL ;
	_MEMSET( NewMemData, 0, sizeof( MEMSTREAMDATA) ) ;

	// データのセット
	NewMemData->DataBuffer = ( unsigned char * )DataBuffer ;
	NewMemData->DataPoint = 0 ;
	NewMemData->DataSize = DataSize ;
	NewMemData->EOFFlag = FALSE ;

	// 終了
	return NewMemData ;
}


extern int MemStreamClose( void *StreamDataPoint )
{
	// メモリの解放
	DXFREE( StreamDataPoint ) ;

	// 終了
	return 0 ;
}

static LONGLONG MemStreamTell( void *StreamDataPoint )
{
	// データポインタを返す
	return ( LONGLONG )( ( MEMSTREAMDATA * )StreamDataPoint )->DataPoint ;
}

static int MemStreamSeek( void *StreamDataPoint, LONGLONG SeekPoint, int SeekType )
{
	MEMSTREAMDATA *MemData = ( MEMSTREAMDATA * )StreamDataPoint ;
	LONGLONG NewPoint = 0 ;

	// シークタイプによって処理を分岐
	switch( SeekType )
	{
	case STREAM_SEEKTYPE_SET :
		NewPoint = SeekPoint ;
		break ;

	case STREAM_SEEKTYPE_END :
		NewPoint = ( LONGLONG )( SeekPoint + MemData->DataSize ) ;
		break ;

	case STREAM_SEEKTYPE_CUR :
		NewPoint = ( LONGLONG )( SeekPoint + MemData->DataPoint ) ;
		break ;
	}

	// 補正
	if( NewPoint > ( LONGLONG )MemData->DataSize )
	{
		MemData->DataPoint = MemData->DataSize ;
	}
	else
	{
		if( NewPoint < 0 )
		{
			MemData->DataPoint = 0 ;
		}
		else
		{
			MemData->DataPoint = ( size_t )NewPoint ;
		}
	}

	MemData->EOFFlag = FALSE ;

	// 終了
	return 0 ;
} ;

static size_t MemStreamRead( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint )
{
	MEMSTREAMDATA *MemData = ( MEMSTREAMDATA * )StreamDataPoint ;

	// EOF の検出
	if( MemData->DataPoint == ( int )MemData->DataSize )
	{
		MemData->EOFFlag = TRUE ;
		return 0 ;
	}
	else
	{
		// 読みこむデータの数の補正
		if( BlockSize * DataNum > ( int )( MemData->DataSize - MemData->DataPoint ) )
		{
			DataNum = ( ( MemData->DataSize - MemData->DataPoint ) / BlockSize ) * BlockSize ;
		}

		if( DataNum != 0 )
		{
			_MEMCPY( Buffer, MemData->DataBuffer + MemData->DataPoint, DataNum * BlockSize ) ;
			MemData->DataPoint += BlockSize * DataNum ;
		}
		else
		{
			size_t MoveSize ;

			MoveSize = MemData->DataSize - MemData->DataPoint ;
			if( MoveSize != 0 )
			{
				_MEMCPY( Buffer, MemData->DataBuffer + MemData->DataPoint, MoveSize ) ;
				MemData->DataPoint += MoveSize ;
				DataNum = 1 ;
			}
		}

		MemData->EOFFlag = FALSE ;
	}

	return DataNum ;
}
	
static size_t MemStreamWrite( void *Buffer, size_t BlockSize, size_t DataNum, void *StreamDataPoint )
{
	MEMSTREAMDATA *MemData = ( MEMSTREAMDATA * )StreamDataPoint ;

	// EOF の検出
	if( MemData->DataPoint == ( int )MemData->DataSize )
	{
		MemData->EOFFlag = TRUE ;
		return 0 ;
	}
	else
	{
		// 書きこむデータの数の補正
		if( BlockSize * DataNum > ( int )( MemData->DataSize - MemData->DataPoint ) )
		{
			DataNum = ( ( MemData->DataSize - MemData->DataPoint ) / BlockSize ) * BlockSize ;
		}

		if( DataNum != 0 )
		{
			_MEMCPY( MemData->DataBuffer + MemData->DataPoint, Buffer, DataNum * BlockSize ) ;
			MemData->DataPoint += BlockSize * DataNum ;
		}

		MemData->EOFFlag = FALSE ;
	}

	return DataNum ;
}

static int MemStreamEof( void *StreamDataPoint )
{
	MEMSTREAMDATA *MemData = ( MEMSTREAMDATA * )StreamDataPoint ;

	// EOF を返す
	return MemData->EOFFlag ;
}

static int MemStreamIdleCheck( void * /*StreamDataPoint*/ )
{
	// 常にアイドリング状態
	return TRUE ;
}

















// TCHAR版ストリームデータアクセスエミュレート用関数
static DWORD_PTR StreamTCHAR_Open( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag )
{
#ifdef UNICODE
	return StreamFunction.Open( Path, UseCacheFlag, BlockReadFlag, UseASyncReadFlag ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, WCHAR_T_CODEPAGE, ( char * )PathBuffer, _TCODEPAGE ) ;
	return StreamFunction.Open( PathBuffer, UseCacheFlag, BlockReadFlag, UseASyncReadFlag ) ;
#endif
}

static int StreamTCHAR_Close( DWORD_PTR Handle )
{
	return StreamFunction.Close( Handle ) ;
}

static LONGLONG StreamTCHAR_Tell( DWORD_PTR Handle )
{
	return StreamFunction.Tell( Handle ) ;
}

static int StreamTCHAR_Seek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	return StreamFunction.Seek( Handle, SeekPoint, SeekType ) ;
}

static size_t StreamTCHAR_Read( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle )
{
	return StreamFunction.Read( Buffer, BlockSize, DataNum, Handle ) ;
}

static int StreamTCHAR_Eof( DWORD_PTR Handle )
{
	return StreamFunction.Eof( Handle ) ;
}

static int StreamTCHAR_IdleCheck( DWORD_PTR Handle )
{
	return StreamFunction.IdleCheck( Handle ) ;
}

static int StreamTCHAR_ChDir( const wchar_t *Path )
{
#ifdef UNICODE
	return StreamFunction.ChDir( Path ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	ConvString( ( const char * )Path, WCHAR_T_CODEPAGE, ( char * )PathBuffer, _TCODEPAGE ) ;
	return StreamFunction.ChDir( PathBuffer ) ;
#endif
}

static int StreamTCHAR_GetDir( wchar_t *Buffer )
{
#ifdef UNICODE
	return StreamFunction.GetDir( Buffer ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	int Result ;

	Result = StreamFunction.GetDir( PathBuffer ) ;
	ConvString( ( const char * )PathBuffer, _TCODEPAGE, ( char * )Buffer, WCHAR_T_CODEPAGE ) ;
	return Result ;
#endif
}

static DWORD_PTR StreamTCHAR_FindFirst( const wchar_t *FilePath, FILEINFOW *Buffer )
{
#ifdef UNICODE
	return StreamFunction.FindFirst( FilePath, ( FILEINFO * )Buffer ) ;
#else
	TCHAR PathBuffer[ FILEPATH_MAX ] ;
	FILEINFO BufferT ;
	DWORD_PTR Result ;

	ConvString( ( const char * )FilePath, WCHAR_T_CODEPAGE, ( char * )PathBuffer, _TCODEPAGE ) ;

	Result = StreamFunction.FindFirst( PathBuffer, &BufferT ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoToFileInfoW( &BufferT, Buffer ) ;
	}

	return Result ;
#endif
}

static int StreamTCHAR_FindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer )
{
#ifdef UNICODE
	return StreamFunction.FindNext( FindHandle, ( FILEINFO * )Buffer ) ;
#else
	FILEINFO BufferT ;
	int Result ;

	Result = StreamFunction.FindNext( FindHandle, &BufferT ) ;
	if( Buffer != NULL )
	{
		ConvFileIntoToFileInfoW( &BufferT, Buffer ) ;
	}

	return Result ;
#endif
}

static int StreamTCHAR_FindClose( DWORD_PTR FindHandle )
{
	return StreamFunction.FindClose( FindHandle ) ;
}























// フルパスではないパス文字列をフルパスに変換する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
//extern	int ConvertFullPath_( const char *Src, char *Dest, const char *CurrentDir )
//{
//	int i, j, k ;
//	char iden[FILEPATH_MAX], cur[FILEPATH_MAX] ;
//
//	if( CurrentDir == NULL )
//	{
//		wchar_t curW[ FILEPATH_MAX ];
//
//		FGETDIR( curW ) ;
//		ConvString( ( const char * )curW, WCHAR_T_CODEPAGE, cur, _TCODEPAGE ) ;
//
//		CurrentDir = cur ;
//	}
//
//	if( Src == NULL )
//	{
//		_STRCPY( Dest, CurrentDir ) ;
//		goto END ;
//	}
//
//	i = 0 ;
//	j = 0 ;
//	k = 0 ;
//	
//	// 最初に『\』又は『/』が２回連続で続いている場合はネットワークを介していると判断
//	if( ( Src[0] == '\\' && Src[1] == '\\' ) ||
//		( Src[0] == '/'  && Src[1] == '/'  ) )
//	{
//		Dest[0] = '\\';
//		Dest[1] = '\0';
//
//		i += 2;
//		j ++ ;
//	}
//	else
//	// 最初が『\』又は『/』の場合はカレントドライブのルートディレクトリまで落ちる
//	if( Src[0] == '\\' || Src[0] == '/' )
//	{
//		Dest[0] = CurrentDir[0] ;
//		Dest[1] = CurrentDir[1] ;
//		Dest[2] = '\0' ;
//
//		i ++ ;
//		j = 2 ;
//	}
//	else
//	// ドライブ名が書かれていたらそのドライブへ
//	if( Src[1] == ':' )
//	{
//		Dest[0] = Src[0] ;
//		Dest[1] = Src[1] ;
//		Dest[2] = '\0' ;
//
//		i = 2 ;
//		j = 2 ;
//
//		if( Src[i] == '\\' || Src[i] == '/' ) i ++ ;
//	}
//	else
//	// それ以外の場合はカレントディレクトリ
//	{
//		_STRCPY( Dest, CurrentDir ) ;
//		j = _STRLEN( Dest ) ;
//		if( Dest[j-1] == '\\' || Dest[j-1] == '/' )
//		{
//			Dest[j-1] = '\0' ;
//			j -- ;
//		}
//	}
//
//	for(;;)
//	{
//		switch( Src[i] )
//		{
//		case '\0' :
//			if( k != 0 )
//			{
//				Dest[j] = '\\' ; j ++ ;
//				_STRCPY( &Dest[j], iden ) ;
//				j += k ;
//				k = 0 ;
//			}
//			goto END ;
//
//		case '\\' :
//		case '/' :
//			// 文字列が無かったらスキップ
//			if( k == 0 )
//			{
//				i ++ ;
//				break ;
//			}
//			if( _STRCMP( iden, "." ) == 0 )
//			{
//				// なにもしない
//			}
//			else
//			if( _STRCMP( iden, ".." ) == 0 )
//			{
//				// 一つ下のディレクトリへ
//				j -- ;
//				for(;;)
//				{
//					if( Dest[j] == '\\' || Dest[j] == '/' || Dest[j] == ':' ) break ;
//					j -= CheckMultiByteString( Dest, j - 1, _GET_CHARSET() ) == 2 ? 2 : 1 ;
//				}
////				while( Dest[j] != '\\' && Dest[j] != '/' && Dest[j] != ':' ) j -- ;
//				if( Dest[j] != ':' ) Dest[j] = '\0' ;
//				else j ++ ;
//			}
//			else
//			{
//				Dest[j] = '\\' ; j ++ ;
//				_STRCPY( &Dest[j], iden ) ;
//				j += k ;
//			}
//
//			k = 0 ;
//			i ++ ;
//			break ;
//		
//		default :
//			if( CheckMultiByteChar( Src[i], _GET_CHARSET() ) == FALSE )
//			{
//				iden[k] = Src[i] ;
//				iden[k+1] = 0 ; 
//				k ++ ;
//				i ++ ;
//			}
//			else
//			{
//				*(( unsigned short * )&iden[k] ) = *(( unsigned short * )&Src[i] ) ;
//				iden[k+2] = '\0' ;
//				k += 2 ;
//				i += 2 ;
//			}
//			break ;
//		}
//	}
//	
//END :
//	// 正常終了
//	return 0 ;
//}

// フルパスではないパス文字列をフルパスに変換する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が NULL の場合は現在のカレントディレクトリを使用する )
extern	int ConvertFullPathW_( const wchar_t *Src, wchar_t *Dest, const wchar_t *CurrentDir )
{
	int i, j, k ;
	wchar_t  iden[ FILEPATH_MAX ] ;
	wchar_t  cur[ FILEPATH_MAX ] ;
	int      CharNum ;
	wchar_t *LastCharAddress ;
	DWORD    LastCharCode ;
	int      LastCharBytes ;

	if( CurrentDir == NULL )
	{
		FGETDIR( cur ) ;
		CurrentDir = cur ;
	}

	if( Src == NULL )
	{
		_WCSCPY( Dest, CurrentDir ) ;
		goto END ;
	}

	i = 0 ;
	j = 0 ;
	k = 0 ;
	
	// 最初に『\』又は『/』が２回連続で続いている場合はネットワークを介していると判断
	if( ( Src[0] == L'\\' && Src[1] == L'\\' ) ||
		( Src[0] == L'/'  && Src[1] == L'/'  ) )
	{
		Dest[0] = L'\\';
		Dest[1] = L'\0';

		i += 2;
		j ++ ;
	}
	else
	// 最初が『\』又は『/』の場合はカレントドライブのルートディレクトリまで落ちる
	if( Src[0] == L'\\' || Src[0] == L'/' )
	{
		Dest[0] = CurrentDir[0] ;
		Dest[1] = CurrentDir[1] ;
		Dest[2] = L'\0' ;

		i ++ ;
		j = 2 ;
	}
	else
	// ドライブ名が書かれていたらそのドライブへ
	if( Src[1] == L':' )
	{
		Dest[0] = Src[0] ;
		Dest[1] = Src[1] ;
		Dest[2] = L'\0' ;

		i = 2 ;
		j = 2 ;

		if( Src[i] == L'\\' || Src[i] == L'/' )
		{
			i ++ ;
		}
	}
	else
	// それ以外の場合はカレントディレクトリ
	{
		_WCSCPY( Dest, CurrentDir ) ;
		j = _WCSLEN( Dest ) ;

		CharNum         = GetStringCharNum( ( const char * )CurrentDir, WCHAR_T_CODEPAGE ) ;
		LastCharAddress = ( wchar_t * )GetStringCharAddress( ( const char * )CurrentDir, WCHAR_T_CODEPAGE, CharNum - 1 ) ;
		LastCharCode    = GetCharCode( ( const char * )LastCharAddress, WCHAR_T_CODEPAGE, &LastCharBytes ) ;
		if( LastCharCode == L'\\' || LastCharCode == L'/' )
		{
			*LastCharAddress = L'\0' ;
			j -- ;
		}
	}

	for(;;)
	{
		switch( Src[i] )
		{
		case L'\0' :
			if( k != 0 )
			{
				Dest[j] = L'\\' ;
				j ++ ;
				_WCSCPY( &Dest[j], iden ) ;
				j += k ;
				k = 0 ;
			}
			goto END ;

		case L'\\' :
		case L'/' :
			// 文字列が無かったらスキップ
			if( k == 0 )
			{
				i ++ ;
				break ;
			}
			if( _WCSCMP( iden, L"." ) == 0 )
			{
				// なにもしない
			}
			else
			if( _WCSCMP( iden, L".." ) == 0 )
			{
				// 一つ浅いディレクトリへ
				Dest[ j ] = L'\0' ;
				CharNum         = GetStringCharNum(  ( const char * )Dest, WCHAR_T_CODEPAGE ) ;
				LastCharAddress = ( wchar_t * )GetStringCharAddress( ( const char * )Dest, WCHAR_T_CODEPAGE, CharNum - 1 ) ;
				LastCharCode    = GetCharCode( ( const char * )LastCharAddress, WCHAR_T_CODEPAGE, &LastCharBytes ) ;
				j -= LastCharBytes / sizeof( wchar_t ) ;
				for(;;)
				{
					if( Dest[j] == L'\\' || Dest[j] == L'/' || Dest[j] == L':' )
					{
						break ;
					}

					Dest[ j ] = L'\0' ;
					CharNum -- ;
					LastCharAddress = ( wchar_t * )GetStringCharAddress( ( const char * )Dest, WCHAR_T_CODEPAGE, CharNum - 1 ) ;
					LastCharCode    = GetCharCode( ( const char * )LastCharAddress, WCHAR_T_CODEPAGE, &LastCharBytes ) ;
					j -= LastCharBytes / sizeof( wchar_t ) ;
				}
				if( Dest[j] != L':' )
				{
					Dest[j] = L'\0' ;
				}
				else
				{
					j ++ ;
				}
			}
			else
			{
				Dest[ j ] = L'\\' ;
				j ++ ;

				_WCSCPY( &Dest[j], iden ) ;
				j += k ;
			}

			k = 0 ;
			i ++ ;
			break ;
		
		default :
			if( CHECK_WCHAR_T_DOUBLE( Src[i] ) == FALSE )
			{
				iden[k] = Src[i] ;
				iden[k+1] = 0 ; 
				k ++ ;
				i ++ ;
			}
			else
			{
				iden[k]   = Src[i] ;
				iden[k+1] = Src[i+1] ;
				iden[k+2] = '\0' ;
				k += 2 ;
				i += 2 ;
			}
			break ;
		}
	}
	
END :
	// 正常終了
	return 0 ;
}

extern int ConvertFullPathT_( const TCHAR *Src, TCHAR *Dest, const TCHAR *CurrentDir )
{
#ifdef UNICODE
	return ConvertFullPathW_( Src, Dest, CurrentDir ) ;
#else
	wchar_t SrcBuffer[ FILEPATH_MAX ] ;
	wchar_t DestBuffer[ FILEPATH_MAX ] ;
	wchar_t CurrentDirBuffer[ FILEPATH_MAX ] ;
	int Result ;

	ConvString( ( const char * )Src, _TCODEPAGE, ( char * )SrcBuffer, WCHAR_T_CODEPAGE ) ;
	if( CurrentDir != NULL )
	{
		ConvString( ( const char * )CurrentDir, _TCODEPAGE, ( char * )DestBuffer, WCHAR_T_CODEPAGE ) ;
	}

	Result = ConvertFullPathW_( SrcBuffer, DestBuffer, CurrentDir != NULL ? CurrentDirBuffer : NULL ) ;

	ConvString( ( const char * )DestBuffer, WCHAR_T_CODEPAGE, ( char * )Dest, _TCODEPAGE ) ;

	return Result ;
#endif
}

// 指定のファイルパスを指定のフォルダパスから相対アクセスするための相対パスを作成する
// ( FilePath や StartFolderPath がフルパスではなかった場合は関数内でフルパス化されます )
// StartFolderPath の末端に / or \ があっても問題ありません
//extern int CreateRelativePath_( const char *FilePath, const char *StartFolderPath, char *Dest )
//{
//	char filefull[ 512 ], filename[ 256 ], endfull[ 512 ], startfull[ 512 ] ;
//	char endstr[ 256 ], startstr[ 256 ] ;
//	int endlen, startlen, destlen ;
//	char *ep, *sp ;
//
//	// フルパス化
//	ConvertFullPath_( FilePath, filefull ) ;
//	AnalysisFileNameAndDirPath_( filefull, filename, endfull ) ;
//	ConvertFullPath_( StartFolderPath, startfull ) ;
//	startlen = _STRLEN( startfull ) ;
//	if( startfull[ startlen - 1 ] == '\\' || startfull[ startlen - 1 ] == '/' )
//		startfull[ startlen - 1 ] = '\0' ;
//
//	// ドライブ文字列を取得する
//	endlen = AnalysisDriveName_( endfull, endstr ) ;
//	startlen = AnalysisDriveName_( startfull, startstr ) ;
//
//	// ドライブが違ったらファイルパスのフルパスを代入する
//	if( _STRCMP( endstr, startstr ) != 0 )
//	{
//		_STRCPY( Dest, filefull ) ;
//		return 0 ;
//	}
//	ep = &endfull[ endlen ] ;
//	sp = &startfull[ startlen ] ;
//
//	// フォルダ名が違うところまで読み進める
//	for(;;)
//	{
//		// \ or / を飛ばす
//		while( *ep == '\\' || *ep == '/' ) ep ++ ;
//		while( *sp == '\\' || *sp == '/' ) sp ++ ;
//
//		// 終端文字だったらループを抜ける
//		if( *ep == '\0' || *sp == '\0' ) break ;
//
//		// フォルダパスを取得する
//		endlen = AnalysisDirectoryName_( ep, endstr ) ;
//		startlen = AnalysisDirectoryName_( sp, startstr ) ;
//
//		// フォルダパスが違ったらループを抜ける
//		if( _STRCMP( endstr, startstr ) != 0 ) break ;
//		ep += endlen ;
//		sp += startlen ;
//	}
//
//	// お互い同じフォルダだった場合はファイル名を出力
//	if( *ep == '\0' && *sp == '\0' )
//	{
//		_STRCPY( Dest, filename ) ;
//		return 0 ;
//	}
//
//	// 開始フォルダと違うフォルダにある場合はまず開始フォルダが現在のフォルダまで降りるパスの作成
//	destlen = 0 ;
//	if( *sp != '\0' )
//	{
//		Dest[ 0 ] = '\0' ;
//		destlen = 0 ;
//		for(;;)
//		{
//			startlen = AnalysisDirectoryName_( sp, startstr ) ;
//			sp += startlen ;
//			while( *sp == '\\' || *sp == '/' ) sp ++ ;
//			_STRCPY( Dest + destlen, "..\\" ) ;
//			destlen += 3 ;
//			if( *sp == '\0' ) break ;
//		}
//	}
//
//	// 目的のファイルがあるフォルダまでとファイル名をつなげて完成
//	_SPRINTF( Dest + destlen, "%s\\%s", ep, filename ) ;
//	return 0 ;
//}


// 指定のファイルパスを指定のフォルダパスから相対アクセスするための相対パスを作成する
// ( FilePath や StartFolderPath がフルパスではなかった場合は関数内でフルパス化されます )
// StartFolderPath の末端に / or \ があっても問題ありません
extern int CreateRelativePathW_( const wchar_t *FilePath, const wchar_t *StartFolderPath, wchar_t *Dest )
{
	wchar_t filefull[ 512 ], filename[ 256 ], endfull[ 512 ], startfull[ 512 ] ;
	wchar_t endstr[ 256 ], startstr[ 256 ] ;
	int endlen, startlen, destlen ;
	wchar_t *ep, *sp ;

	// フルパス化
	ConvertFullPathW_( FilePath, filefull ) ;
	AnalysisFileNameAndDirPathW_( filefull, filename, endfull ) ;
	ConvertFullPathW_( StartFolderPath, startfull ) ;
	startlen = _WCSLEN( startfull ) ;
	if( startfull[ startlen - 1 ] == L'\\' || startfull[ startlen - 1 ] == L'/' )
	{
		startfull[ startlen - 1 ] = L'\0' ;
	}

	// ドライブ文字列を取得する
	endlen = AnalysisDriveNameW_( endfull, endstr ) ;
	startlen = AnalysisDriveNameW_( startfull, startstr ) ;

	// ドライブが違ったらファイルパスのフルパスを代入する
	if( _WCSCMP( endstr, startstr ) != 0 )
	{
		_WCSCPY( Dest, filefull ) ;
		return 0 ;
	}
	ep = &endfull[ endlen ] ;
	sp = &startfull[ startlen ] ;

	// フォルダ名が違うところまで読み進める
	for(;;)
	{
		// \ or / を飛ばす
		while( *ep == L'\\' || *ep == L'/' ) ep ++ ;
		while( *sp == L'\\' || *sp == L'/' ) sp ++ ;

		// 終端文字だったらループを抜ける
		if( *ep == L'\0' || *sp == L'\0' ) break ;

		// フォルダパスを取得する
		endlen   = AnalysisDirectoryNameW_( ep, endstr ) ;
		startlen = AnalysisDirectoryNameW_( sp, startstr ) ;

		// フォルダパスが違ったらループを抜ける
		if( _WCSCMP( endstr, startstr ) != 0 ) break ;
		ep += endlen ;
		sp += startlen ;
	}

	// お互い同じフォルダだった場合はファイル名を出力
	if( *ep == L'\0' && *sp == L'\0' )
	{
		_WCSCPY( Dest, filename ) ;
		return 0 ;
	}

	// 開始フォルダと違うフォルダにある場合はまず開始フォルダが現在のフォルダまで降りるパスの作成
	destlen = 0 ;
	if( *sp != L'\0' )
	{
		Dest[ 0 ] = L'\0' ;
		destlen = 0 ;
		for(;;)
		{
			startlen = AnalysisDirectoryNameW_( sp, startstr ) ;
			sp += startlen ;
			while( *sp == L'\\' || *sp == L'/' ) sp ++ ;
			_WCSCPY( Dest + destlen, L"..\\" ) ;
			destlen += 3 ;
			if( *sp == L'\0' ) break ;
		}
	}

	// 目的のファイルがあるフォルダまでとファイル名をつなげて完成
	_SWPRINTF( Dest + destlen, L"%s\\%s", ep, filename ) ;
	return 0 ;
}

extern int CreateRelativePathT_( const TCHAR *FilePath, const TCHAR *StartFolderPath, TCHAR *Dest )
{
#ifdef UNICODE
	return CreateRelativePathW_( FilePath, StartFolderPath, Dest ) ;
#else
	wchar_t FilePathBuffer[ FILEPATH_MAX ] ;
	wchar_t StartFolderPathBuffer[ FILEPATH_MAX ] ;
	wchar_t DestBuffer[ FILEPATH_MAX ] ;
	int Result ;

	ConvString( ( const char * )FilePath,        _TCODEPAGE, ( char * )FilePathBuffer,        WCHAR_T_CODEPAGE ) ;
	ConvString( ( const char * )StartFolderPath, _TCODEPAGE, ( char * )StartFolderPathBuffer, WCHAR_T_CODEPAGE ) ;

	Result = CreateRelativePathW_( FilePathBuffer, StartFolderPathBuffer, DestBuffer ) ;

	ConvString( ( const char * )DestBuffer, WCHAR_T_CODEPAGE, ( char * )Dest, _TCODEPAGE ) ;

	return Result ;
#endif
}

// 特定のパス文字列から色々な情報を取得する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が 0 の場合は実際のカレントディレクトリを使用する )
extern int AnalyseFilePathT_(
	const TCHAR *Src,
	TCHAR *FullPath, TCHAR *DirPath, TCHAR *FileName, TCHAR *Name, TCHAR *ExeName, const TCHAR *CurrentDir )
{
	TCHAR full[256], dir[256], fname[256], name[256], exename[256];

	ConvertFullPathT_( Src, full, CurrentDir );
	AnalysisFileNameAndDirPathT_( full, fname, dir );
	AnalysisFileNameAndExeNameT_( fname, name, exename );
	if( FullPath ) _TSTRCPY( FullPath, full    );
	if( DirPath  ) _TSTRCPY( DirPath,  dir     );
	if( FileName ) _TSTRCPY( FileName, fname   );
	if( Name     ) _TSTRCPY( Name,     name    );
	if( ExeName  ) _TSTRCPY( ExeName,  exename );

	return 0;
}

// 特定のパス文字列から色々な情報を取得する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が 0 の場合は実際のカレントディレクトリを使用する )
extern int AnalyseFilePathW_(
	const wchar_t *Src,
	wchar_t *FullPath, wchar_t *DirPath, wchar_t *FileName, wchar_t *Name, wchar_t *ExeName, const wchar_t *CurrentDir )
{
	wchar_t full[256], dir[256], fname[256], name[256], exename[256];

	ConvertFullPathW_( Src, full, CurrentDir );
	AnalysisFileNameAndDirPathW_( full, fname, dir );
	AnalysisFileNameAndExeNameW_( fname, name, exename );
	if( FullPath ) _WCSCPY( FullPath, full    );
	if( DirPath  ) _WCSCPY( DirPath,  dir     );
	if( FileName ) _WCSCPY( FileName, fname   );
	if( Name     ) _WCSCPY( Name,     name    );
	if( ExeName  ) _WCSCPY( ExeName,  exename );

	return 0;
}


// ファイル名も一緒になっていると分かっているパス中からファイル名とディレクトリパスを分割する
// フルパスである必要は無い、ファイル名だけでも良い
//extern int AnalysisFileNameAndDirPath_( const char *Src, char *FileName, char *DirPath )
//{
//	int i, Last ;
//	
//	// ファイル名を抜き出す
//	i = 0 ;
//	Last = -1 ;
//	while( Src[i] != '\0' )
//	{
//		if( CheckMultiByteChar( Src[i], _GET_CODEPAGE() ) == FALSE )
//		{
//			if( Src[i] == '\\' || Src[i] == '/' || Src[i] == '\0' || Src[i] == ':' ) Last = i ;
//			i ++ ;
//		}
//		else
//		{
//			i += 2 ;
//		}
//	}
//	if( FileName != NULL )
//	{
//		if( Last != -1 ) _STRCPY( FileName, &Src[Last+1] ) ;
//		else _STRCPY( FileName, Src ) ;
//	}
//	
//	// ディレクトリパスを抜き出す
//	if( DirPath != NULL )
//	{
//		if( Last != -1 )
//		{
//			_STRNCPY( DirPath, Src, Last ) ;
//			DirPath[Last] = '\0' ;
//		}
//		else
//		{
//			DirPath[0] = '\0' ;
//		}
//	}
//	
//	// 終了
//	return 0 ;
//}

// ファイル名も一緒になっていると分かっているパス中からファイル名とディレクトリパスを分割する
// フルパスである必要は無い、ファイル名だけでも良い
extern int AnalysisFileNameAndDirPathW_( const wchar_t *Src, wchar_t *FileName, wchar_t *DirPath )
{
	int i, Last ;
	
	// ファイル名を抜き出す
	i = 0 ;
	Last = -1 ;
	while( Src[i] != L'\0' )
	{
		if( CHECK_WCHAR_T_DOUBLE( Src[i] ) == FALSE )
		{
			if( Src[i] == L'\\' || Src[i] == L'/' || Src[i] == L'\0' || Src[i] == L':' )
			{
				Last = i ;
			}
			i ++ ;
		}
		else
		{
			i += 2 ;
		}
	}
	if( FileName != NULL )
	{
		if( Last != -1 )
		{
			_WCSCPY( FileName, &Src[Last+1] ) ;
		}
		else
		{
			_WCSCPY( FileName, Src ) ;
		}
	}
	
	// ディレクトリパスを抜き出す
	if( DirPath != NULL )
	{
		if( Last != -1 )
		{
			_WCSNCPY( DirPath, Src, Last ) ;
			DirPath[ Last ] = L'\0' ;
		}
		else
		{
			DirPath[0] = L'\0' ;
		}
	}
	
	// 終了
	return 0 ;
}

extern int AnalysisFileNameAndDirPathT_( const TCHAR *Src, TCHAR *FileName, TCHAR *DirPath )
{
#ifdef UNICODE
	return AnalysisFileNameAndDirPathW_( Src, FileName, DirPath ) ;
#else
	wchar_t SrcBuffer[ FILEPATH_MAX ] ;
	wchar_t FileNameBuffer[ FILEPATH_MAX ] ;
	wchar_t DirPathBuffer[ FILEPATH_MAX ] ;
	int Result ;

	ConvString( ( const char * )Src, _TCODEPAGE, ( char * )SrcBuffer, WCHAR_T_CODEPAGE ) ;

	Result = AnalysisFileNameAndDirPathW_( SrcBuffer, FileNameBuffer, DirPathBuffer ) ;

	if( FileName )
	{
		ConvString( ( const char * )FileNameBuffer, WCHAR_T_CODEPAGE, ( char * )FileName, _TCODEPAGE ) ;
	}

	if( DirPath )
	{
		ConvString( ( const char * )DirPathBuffer, WCHAR_T_CODEPAGE, ( char * )DirPath, _TCODEPAGE ) ;
	}

	return Result ;
#endif
}

// ファイルパスからファイル名と拡張子を取得する
//extern int AnalysisFileNameAndExeName_( const char *Src, char *Name, char *ExeName )
//{
//	char FileName[256], *p, *p2, ename[128], name[128] ;
//
//	// ファイル名のみを取得
//	AnalysisFileNameAndDirPath_( Src, FileName, 0 ) ;
//
//	// 『.』があるかどうかで処理を分岐
//	if( ( p = _STRCHR( FileName, '.' ) ) == NULL )
//	{
//		_STRCPY( name, FileName ) ;
//		ename[0] = '\0' ;
//	}
//	else
//	{
//		for(;;)
//		{
//			p2 = _STRCHR( p + 1, '.' ) ;
//			if( p2 == NULL ) break ;
//			p = p2 ;
//		}
//
//		_STRNCPY( name, FileName, ( int )( p - FileName ) ) ;
//		name[p - FileName] = '\0' ;
//		_STRCPY( ename, p + 1 ) ;
//	}
//
//	if( Name != NULL ) _STRCPY( Name, name ) ;
//	if( ExeName != NULL ) _STRCPY( ExeName, ename ) ;
//
//	// 終了
//	return 0 ;
//}

// ファイルパスからファイル名と拡張子を取得する
extern int AnalysisFileNameAndExeNameW_( const wchar_t *Src, wchar_t *Name, wchar_t *ExeName )
{
	wchar_t FileName[ 256 ] ;
	const wchar_t *p ;
	const wchar_t *p2 ;
	wchar_t ename[128] ;
	wchar_t name[128] ;

	// ファイル名のみを取得
	AnalysisFileNameAndDirPathW_( Src, FileName, 0 ) ;

	// 『.』があるかどうかで処理を分岐
	p = _WCSCHR( FileName, L'.' ) ;
	if( p == NULL )
	{
		_WCSCPY( name, FileName ) ;
		ename[0] = L'\0' ;
	}
	else
	{
		for(;;)
		{
			p2 = _WCSCHR( p + 1, L'.' ) ;
			if( p2 == NULL ) break ;
			p = p2 ;
		}

		_WCSNCPY( name, FileName, ( int )( p - FileName ) ) ;
		name[p - FileName] = L'\0' ;
		_WCSCPY( ename, p + 1 ) ;
	}

	if( Name    != NULL ) _WCSCPY( Name, name ) ;
	if( ExeName != NULL ) _WCSCPY( ExeName, ename ) ;

	// 終了
	return 0 ;
}

// ファイルパスからファイル名と拡張子を取得する
extern int AnalysisFileNameAndExeNameT_( const TCHAR *Src, TCHAR *Name, TCHAR *ExeName )
{
	TCHAR FileName[256], *p, *p2, ename[128], name[128] ;

	// ファイル名のみを取得
	AnalysisFileNameAndDirPathT_( Src, FileName, 0 ) ;

	// 『.』があるかどうかで処理を分岐
	p = ( TCHAR * )_TSTRCHR( FileName, _T( '.' ) ) ;
	if( p == NULL )
	{
		_TSTRCPY( name, FileName ) ;
		ename[0] = _T( '\0' ) ;
	}
	else
	{
		for(;;)
		{
			p2 = ( TCHAR * )_TSTRCHR( p + 1, _T( '.' ) ) ;
			if( p2 == NULL ) break ;
			p = p2 ;
		}

		_TSTRNCPY( name, FileName, ( int )( p - FileName ) ) ;
		name[p - FileName] = _T( '\0' ) ;
		_TSTRCPY( ename, p + 1 ) ;
	}

	if( Name    != NULL ) _TSTRCPY( Name,    name  ) ;
	if( ExeName != NULL ) _TSTRCPY( ExeName, ename ) ;

	// 終了
	return 0 ;
}

// ファイルパスの拡張子を変えた文字列を得る
//extern int GetChangeExeNamePath_( const char *Src, char *Dest, const char *ExeName )
//{
//	char DirPath[256], FileName[128] ;
//
//	AnalysisFileNameAndDirPath_( Src, NULL, DirPath ) ;
//	AnalysisFileNameAndExeName_( Src, FileName, 0 ) ;
//	SetEnMark_( DirPath ) ;
//	_STRCPY( Dest, DirPath ) ;
//	_STRCAT( Dest, FileName ) ;
//	_STRCAT( Dest, "." ) ;
//	_STRCAT( Dest, ExeName ) ;
//	
//	// 終了
//	return 0 ;
//}


// 語尾に『\』がついていない場合は付ける
extern void SetEnMarkT_( TCHAR *PathBuf )
{
	int Len = _TSTRLEN( PathBuf ) ;
	
	if( PathBuf[ Len - 1 ] != '\\' )
	{
		PathBuf[ Len     ] = '\\' ;
		PathBuf[ Len + 1 ] = '\0' ;
	}
}

// 語尾に『\』がついていない場合は付ける
extern void SetEnMarkW_( wchar_t *PathBuf )
{
	int Len = _WCSLEN( PathBuf ) ;
	
	if( PathBuf[ Len - 1 ] != L'\\' )
	{
		PathBuf[ Len     ] = L'\\' ;
		PathBuf[ Len + 1 ] = L'\0' ;
	}
}


// 渡された文字列をフルパス文字列として扱い、ドライブ名( \ or / の前まで )
// 又はネットワークフォルダ名( \ or / の前まで )を取得する、
// ドライブ名だった場合は最後の : も含まれ、ネットワークフォルダだった場合は最初の \\ も含まれます
// 戻り値は取得した文字列の長さ( ドライブ名の場合は最後の :、ネットワークフォルダの場合は \\ も文字列量に含まれます )
// Src はフルパスである必要があります
//extern int AnalysisDriveName_( const char *Src, char *Dest )
//{
//	int len ;
//
//	// ネットワークフォルダの場合は最初の \\ を代入する
//	len = 0 ;
//	if( Src[ 0 ] == '\\' || Src[ 0 ] == '/' )
//	{
//		// ネットワークフォルダ
//		Dest[ 0 ] = Src[ 0 ] ;
//		Dest[ 1 ] = Src[ 1 ] ;
//		Dest += 2 ;
//		Src += 2 ;
//		len += 2 ;
//	}
//
//	// 後は文字列の末端か \ or / が出るまでコピー
//	while( *Src != '\0' && *Src != '\\' && *Src != '/' )
//	{
//		if( CheckMultiByteChar( *Src, _GET_CHARSET() ) )
//		{
//			Dest[ 0 ] = Src[ 0 ] ;
//			Dest[ 1 ] = Src[ 1 ] ;
//			Dest += 2 ;
//			Src += 2 ;
//			len += 2 ;
//		}
//		else
//		{
//			*Dest = *Src ;
//			Dest ++ ;
//			Src ++ ;
//			len ++ ;
//		}
//	}
//
//	// 終端文字をセット
//	*Dest = '\0' ;
//
//	// 文字列長を返す
//	return len ;
//}

// 渡された文字列をフルパス文字列として扱い、ドライブ名( \ or / の前まで )
// 又はネットワークフォルダ名( \ or / の前まで )を取得する、
// ドライブ名だった場合は最後の : も含まれ、ネットワークフォルダだった場合は最初の \\ も含まれます
// 戻り値は取得した文字列の長さ( ドライブ名の場合は最後の :、ネットワークフォルダの場合は \\ も文字列量に含まれます )
// Src はフルパスである必要があります
extern int AnalysisDriveNameW_( const wchar_t *Src, wchar_t *Dest )
{
	int len ;

	// ネットワークフォルダの場合は最初の \\ を代入する
	len = 0 ;
	if( Src[ 0 ] == L'\\' || Src[ 0 ] == L'/' )
	{
		// ネットワークフォルダ
		Dest[ 0 ] = Src[ 0 ] ;
		Dest[ 1 ] = Src[ 1 ] ;
		Dest += 2 ;
		Src += 2 ;
		len += 2 ;
	}

	// 後は文字列の末端か \ or / が出るまでコピー
	while( *Src != L'\0' && *Src != L'\\' && *Src != L'/' )
	{
		if( CHECK_WCHAR_T_DOUBLE( *Src ) )
		{
			Dest[ 0 ] = Src[ 0 ] ;
			Dest[ 1 ] = Src[ 1 ] ;
			Dest += 2 ;
			Src += 2 ;
			len += 2 ;
		}
		else
		{
			*Dest = *Src ;
			Dest ++ ;
			Src ++ ;
			len ++ ;
		}
	}

	// 終端文字をセット
	*Dest = L'\0' ;

	// 文字列長を返す
	return len ;
}


// 渡された文字列をフォルダパス文字列として扱い、フォルダ名( \ or / の前まで )を取得します
// 渡す文字列がフルパスで、最初にドライブ名が書かれていたら正常な結果が得られません
// ../ 等の下位フォルダに降りる文字列があった場合は .. 等が出力されます
// 戻り値は取得した文字列の長さです
//extern int AnalysisDirectoryName_( const char *Src, char *Dest )
//{
//	int len ;
//
//	len = 0 ;
//	while( *Src != '\0' && *Src != '\\' && *Src != '/' )
//	{
//		if( CheckMultiByteChar( *Src, _GET_CHARSET() ) )
//		{
//			Dest[ 0 ] = Src[ 0 ] ;
//			Dest[ 1 ] = Src[ 1 ] ;
//			Dest += 2 ;
//			Src += 2 ;
//			len += 2 ;
//		}
//		else
//		{
//			*Dest = *Src ;
//			Dest ++ ;
//			Src ++ ;
//			len ++ ;
//		}
//	}
//
//	// 終端文字をセット
//	*Dest = '\0' ;
//
//	// 文字列長を返す
//	return len ;
//}

// 渡された文字列をフォルダパス文字列として扱い、フォルダ名( \ or / の前まで )を取得します
// 渡す文字列がフルパスで、最初にドライブ名が書かれていたら正常な結果が得られません
// ../ 等の下位フォルダに降りる文字列があった場合は .. 等が出力されます
// 戻り値は取得した文字列の長さです
extern int AnalysisDirectoryNameW_( const wchar_t *Src, wchar_t *Dest )
{
	int len ;

	len = 0 ;
	while( *Src != L'\0' && *Src != L'\\' && *Src != L'/' )
	{
		if( CHECK_WCHAR_T_DOUBLE( *Src ) )
		{
			Dest[ 0 ] = Src[ 0 ] ;
			Dest[ 1 ] = Src[ 1 ] ;
			Dest += 2 ;
			Src += 2 ;
			len += 2 ;
		}
		else
		{
			*Dest = *Src ;
			Dest ++ ;
			Src ++ ;
			len ++ ;
		}
	}

	// 終端文字をセット
	*Dest = L'\0' ;

	// 文字列長を返す
	return len ;
}









// アーカイブファイルの読み込み機能を使うかどうかを設定する( FALSE:使用しない  TRUE:使用する )
extern int NS_SetUseDXArchiveFlag( int Flag )
{
	int DefaultFlag ;

	// フラグの状態が同じ場合は何もしない
	if( NotUseDXArchive == !Flag ) return 0 ;

	// 標準の読み込み機能が使用されているかどうかを取得しておく
	DefaultFlag = NS_GetStreamFunctionDefault() ;

	// フラグを更新
	NotUseDXArchive = !Flag ;

	// 現在標準の読み込み機能が使われている場合は関数ポインタを更新する
	if( DefaultFlag == TRUE )
	{
		NS_ChangeStreamFunctionW( NULL ) ;
	}

	// 終了
	return 0 ;
}

// ＤＸアーカイブファイルと通常のフォルダのどちらも存在した場合、どちらを優先させるかを設定する( 1:フォルダを優先 0:ＤＸアーカイブファイルを優先(デフォルト) )
extern int NS_SetDXArchivePriority( int Priority )
{
#ifndef DX_NON_DXA
	return DXA_DIR_SetDXArchivePriority( Priority ) ;
#else
	return -1;
#endif
}

// 検索するＤＸアーカイブファイルの拡張子を変更する
extern int NS_SetDXArchiveExtension( const TCHAR *Extension )
{
#ifndef DX_NON_DXA
#ifdef UNICODE
	return DXA_DIR_SetArchiveExtension( Extension ) ;
#else
	wchar_t ExtensionBuffer[ 256 ] ;

	ConvString( ( const char * )Extension, _TCODEPAGE, ( char * )ExtensionBuffer, WCHAR_T_CODEPAGE ) ;

	return DXA_DIR_SetArchiveExtension( ExtensionBuffer ) ;
#endif
#else
	return -1;
#endif
}


// ＤＸアーカイブファイルの鍵文字列を設定する
extern int NS_SetDXArchiveKeyString( const TCHAR *KeyString )
{
#ifndef DX_NON_DXA
#ifdef UNICODE
	char TempBuffer[ 1024 ] ;

	ConvString( ( const char * )KeyString, _TCODEPAGE, TempBuffer, DX_CODEPAGE_SHIFTJIS ) ; 
	return DXA_DIR_SetKeyString( TempBuffer ) ;
#else
	return DXA_DIR_SetKeyString( KeyString ) ;
#endif
#else
	return -1;
#endif
}








#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE
