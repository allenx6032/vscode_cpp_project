//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用ファイル関係プログラム
// 
//  	Ver 3.13f
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ---------------------------------------------------------------
#include "DxFileWin.h"
#include "DxWindow.h"
#include "DxFile.h"
#include "DxLog.h"
#include "DxChar.h"
#include "DxBaseFunc.h"
#include <windows.h>

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 関数宣言 -------------------------------------------------------------------

DWORD	WINAPI		FileAccessThreadFunction( void *FileAccessThreadData ) ;					// ファイルアクセス専用スレッド用関数
int					SetFilePointer64( HANDLE FileHandle, ULONGLONG Position ) ;					// ファイルのアクセス位置を設定する( 0:成功  -1:失敗 )

// プログラム -----------------------------------------------------------------

// ファイルのアクセス位置を設定する
int SetFilePointer64( HANDLE FileHandle, ULONGLONG Position )
{
	DWORD High, Low ;
	DWORD Result ;

	Low = ( DWORD )Position ;
	High = ( DWORD )( Position >> 32 ) ;
	Result = SetFilePointer( FileHandle, ( LONG )Low, ( LONG * )&High, FILE_BEGIN ) ;
	if( Result == 0xffffffff && GetLastError() != NO_ERROR )
	{
		return -1 ;
	}

	return 0 ;
}

// テンポラリファイルを作成する
extern HANDLE CreateTemporaryFile( wchar_t *TempFileNameBuffer )
{
	wchar_t String1[FILEPATH_MAX], String2[FILEPATH_MAX] ;
	HANDLE FileHandle ;
	int Length ;

	// テンポラリファイルのディレクトリパスを取得する
	if( GetTempPathW( 256, String1 ) == 0 ) return NULL ;

	// 文字列の最後に￥マークをつける
	Length = _WCSLEN( String1 ) ;
	if( String1[ Length - 1 ] != L'\\' )
	{
		String1[ Length     ]  = L'\\' ;
		String1[ Length + 1 ]  = L'\0' ;
	}

	// テンポラリファイルのファイル名を作成する
	if( GetTempFileNameW( String1, L"tmp", 0, String2 ) == 0 ) return NULL ;

	// フルパスに変換
	ConvertFullPathW_( String2, String1 ) ;

	// テンポラリファイルを開く
	DeleteFileW( String1 ) ;
	FileHandle = CreateFileW( String1, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;
	if( FileHandle == NULL )
	{
		return NULL ;
	}

	// テンポラリファイル名を保存
	if( TempFileNameBuffer != NULL )
	{
		_WCSCPY( TempFileNameBuffer, String1 ) ;
	}

	// ハンドルを返す
	return FileHandle ;
}



// ファイルアクセス専用スレッド用関数
DWORD WINAPI FileAccessThreadFunction( void *FileAccessThreadData )
{
	FILEACCESSTHREAD *dat = (FILEACCESSTHREAD *)FileAccessThreadData ;
	DWORD res, ReadSize ;

	for(;;)
	{
		for(;;)
		{
			// キャッシュを使用すかどうかで処理を分岐
			if( dat->CacheBuffer )
			{
				// 指令が来るまでちょっと待つ
				res = WaitForSingleObject( dat->FuncEvent, 100 ) ;

				// 指令が来てい無い場合でファイルが開いている場合はキャッシング処理を行う
				if( res == WAIT_TIMEOUT && dat->Handle != NULL )
				{
					// もしキャッシュが一杯だったら何もしない
					if( dat->CacheSize != FILEACCESSTHREAD_DEFAULT_CACHESIZE )
					{
						// 読み込み開始位置セット
						SetFilePointer64( dat->Handle, dat->CachePosition + dat->CacheSize ) ;

						// 読み込み
						ReadFile( dat->Handle, &dat->CacheBuffer[dat->CacheSize], ( DWORD )( FILEACCESSTHREAD_DEFAULT_CACHESIZE - dat->CacheSize ), &ReadSize, NULL ) ;
						
						// 有効なサイズを増やす
						dat->CacheSize += ( LONGLONG )ReadSize ;
					}
				}
				else
				{
					break ;
				}
			}
			else
			{
				// 指令が来るまで待つ
				res = WaitForSingleObject( dat->FuncEvent, INFINITE ) ;
				if( res == WAIT_TIMEOUT && dat->Handle != NULL ) continue;
				break;
			}
		}

//		WaitForSingleObject( dat->FuncEvent, INFINITE ) ;

		// イベントのシグナル状態を解除する
		ResetEvent( dat->FuncEvent ) ;
		ResetEvent( dat->CompEvent ) ;

		// 指令が来たら判断する
		switch( dat->Function )
		{
		case FILEACCESSTHREAD_FUNCTION_OPEN :
			dat->Handle = CreateFileW( dat->FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;
			if( dat->Handle == INVALID_HANDLE_VALUE )
			{
				dat->ErrorFlag = TRUE ;
				goto END ;
			}
			break ;

		case FILEACCESSTHREAD_FUNCTION_CLOSE :
			CloseHandle( dat->Handle ) ;
			dat->Handle = NULL ;
			break ;

		case FILEACCESSTHREAD_FUNCTION_READ :
			// キャッシュと読み込み位置が一致している場合はキャッシュからデータを転送する
			if( dat->CacheBuffer && dat->ReadPosition == dat->CachePosition && dat->CacheSize != 0 )
			{
				DWORD MoveSize ;

				// 転送するサイズを調整
				MoveSize = dat->ReadSize ;
				if( MoveSize > dat->CacheSize ) MoveSize = ( DWORD )dat->CacheSize ;

				// 転送
				_MEMCPY( dat->ReadBuffer, dat->CacheBuffer, MoveSize ) ;

				// 読み込みサイズと読み込み位置を移動する
				dat->ReadBuffer = (void *)( (BYTE *)dat->ReadBuffer + MoveSize ) ;
				dat->ReadPosition += MoveSize ;
				dat->ReadSize -= MoveSize ;
				
				// キャッシュの情報も更新
				dat->CachePosition += MoveSize ;
				dat->CacheSize     -= MoveSize ;
				if( dat->CacheSize != 0 ) _MEMMOVE( &dat->CacheBuffer[0], &dat->CacheBuffer[MoveSize], ( size_t )dat->CacheSize ) ;
			}

			// 希望のデータが全て読めていない場合は更にファイルから読み込む
			if( dat->ReadSize != 0 )
			{
				SetFilePointer64( dat->Handle, dat->ReadPosition ) ;
				ReadFile( dat->Handle, dat->ReadBuffer, dat->ReadSize, &dat->ReadSize, NULL ) ;

				// キャッシュを初期化する
				if( dat->CacheBuffer )
				{
					dat->CachePosition = dat->ReadPosition + dat->ReadSize ;
					dat->CacheSize = 0 ;
				}
			}
			break ;

		case FILEACCESSTHREAD_FUNCTION_SEEK :
			SetFilePointer64( dat->Handle, dat->SeekPoint ) ;

			// キャッシュを初期化する
			if( dat->CacheBuffer )
			{
				dat->CachePosition = (DWORD)dat->SeekPoint ;
				dat->CacheSize = 0 ;
			}
			break ;

		case FILEACCESSTHREAD_FUNCTION_EXIT :
			if( dat->Handle != NULL ) CloseHandle( dat->Handle ) ;
			dat->Handle = NULL ;
			goto END ;
		}

		// 指令が完了したら完了イベントをシグナル状態にする
		SetEvent( dat->CompEvent ) ;
	}

END:
	// エラー時の為に完了イベントをシグナル状態にする
	SetEvent( dat->CompEvent ) ;
	dat->EndFlag = TRUE ;
	ExitThread( 1 ) ;

	return 0 ;
}



// ファイルアクセス関数
extern DWORD_PTR ReadOnlyFileAccessOpen( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag )
{
	WINFILEACCESS *FileAccess ;
	DWORD Code ;
	DWORD High ;

//	UseCacheFlag = UseCacheFlag ;
	BlockReadFlag = BlockReadFlag ;

	FileAccess = ( WINFILEACCESS * )DXALLOC( sizeof( WINFILEACCESS ) ) ;
	if( FileAccess == NULL ) return 0 ;

	_MEMSET( FileAccess, 0, sizeof( WINFILEACCESS ) ) ;

//	// キャッシュを使用するかどうかをスレッドを使用するかどうかにしてしまう
//	FileAccess->UseThread = UseCacheFlag ;

	// キャッシュを使用するかどうかのフラグをセット
	FileAccess->UseCacheFlag = UseCacheFlag ;
	FileAccess->ThreadData.CacheBuffer = NULL;

	// 非同期読み書きフラグをセット
	FileAccess->UseASyncReadFlag = UseASyncReadFlag ;

	// キャッシュ、若しくは非同期読み書きを行う場合はスレッドを使用する
	FileAccess->UseThread = FileAccess->UseCacheFlag || FileAccess->UseASyncReadFlag ;

	// スレッドを使用する場合としない場合で処理を分岐
	if( FileAccess->UseThread == TRUE )
	{
		// スレッドを使用する場合はファイルアクセス専用スレッドを立てる

		// 最初にファイルを開けるかどうか確かめておく
		FileAccess->Handle = CreateFileW( Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;
		if( FileAccess->Handle == INVALID_HANDLE_VALUE )
		{
			DXFREE( FileAccess ) ;
			return 0 ;
		}
		FileAccess->Size = GetFileSize( FileAccess->Handle, &High ) ;
		FileAccess->Size |= ( ( ULONGLONG )High ) << 32 ;
		CloseHandle( FileAccess->Handle ) ;
		FileAccess->Handle = NULL ;

		// キャッシュ用メモリの確保
		if( FileAccess->UseCacheFlag )
		{
			FileAccess->ThreadData.CacheBuffer = (BYTE *)DXALLOC( FILEACCESSTHREAD_DEFAULT_CACHESIZE );
			if( FileAccess->ThreadData.CacheBuffer == NULL )
			{
				DXFREE( FileAccess->ThreadData.CacheBuffer ) ;
				DXFREE( FileAccess ) ;
				DXST_ERRORLOG_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\xad\x30\xe3\x30\xc3\x30\xb7\x30\xe5\x30\x28\x75\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ファイル読み込みキャッシュ用メモリの確保に失敗しました\n" @*/ ) ;
				return 0 ;
			}
		}

		// 専用スレッドデータを初期化する
		FileAccess->ThreadData.Handle = NULL ;
		FileAccess->ThreadData.ThreadHandle = NULL ;
		FileAccess->ThreadData.FuncEvent = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
		FileAccess->ThreadData.CompEvent = CreateEvent( NULL, TRUE, TRUE, NULL ) ;

		FileAccess->ThreadData.ThreadHandle = CreateThread(
												NULL,
												0,
												(LPTHREAD_START_ROUTINE)FileAccessThreadFunction, 
												&FileAccess->ThreadData,
												0,
												&FileAccess->ThreadData.ThreadID ) ;
		if( FileAccess->ThreadData.ThreadHandle == NULL )
		{
			if( FileAccess->ThreadData.CacheBuffer ) DXFREE( FileAccess->ThreadData.CacheBuffer ) ;
			CloseHandle( FileAccess->ThreadData.FuncEvent ) ;
			CloseHandle( FileAccess->ThreadData.CompEvent ) ;
			DXFREE( FileAccess ) ;
			DXST_ERRORLOG_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xa2\x30\xaf\x30\xbb\x30\xb9\x30\x02\x5c\x28\x75\xb9\x30\xec\x30\xc3\x30\xc9\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ファイルアクセス専用スレッドの作成に失敗しました\n" @*/ ) ;
			return 0 ;
		}
		SetThreadPriority( FileAccess->ThreadData.ThreadHandle, THREAD_PRIORITY_LOWEST ) ;

		// ファイルオープン指令はここで完了してしまう
		FileAccess->ThreadData.Function = FILEACCESSTHREAD_FUNCTION_OPEN ;
		_WCSCPY( FileAccess->ThreadData.FilePath, Path ) ;

		ResetEvent( FileAccess->ThreadData.CompEvent ) ;
		SetEvent( FileAccess->ThreadData.FuncEvent ) ;

		// 指令が終了するまで待つ
		WaitForSingleObject( FileAccess->ThreadData.CompEvent, INFINITE ) ;
		if( FileAccess->ThreadData.ErrorFlag == TRUE )
		{
			if( FileAccess->ThreadData.CacheBuffer ) DXFREE( FileAccess->ThreadData.CacheBuffer ) ;
			CloseHandle( FileAccess->ThreadData.FuncEvent ) ;
			CloseHandle( FileAccess->ThreadData.CompEvent ) ;
			do
			{
				Sleep( 0 ) ;
				GetExitCodeThread( FileAccess->ThreadData.ThreadHandle, &Code ) ;
			}while( Code == STILL_ACTIVE ) ;
			CloseHandle( FileAccess->ThreadData.ThreadHandle ) ;
			DXFREE( FileAccess ) ;
			DXST_ERRORLOG_ADDUTF16LE( "\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"ファイルのオープンに失敗しました\n" @*/ ) ;
			return 0 ;
		}
	}
	else
	{
		// スレッドを使用しない場合はこの場でファイルを開く
		FileAccess->Handle = CreateFileW( Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;
		if( FileAccess->Handle == INVALID_HANDLE_VALUE )
		{
			DXFREE( FileAccess ) ;
			return 0 ;
		}
		FileAccess->Size = GetFileSize( FileAccess->Handle, &High ) ;
		FileAccess->Size |= ( ( ULONGLONG )High ) << 32 ;
	}
	FileAccess->EofFlag = FALSE ;
	FileAccess->Position = 0 ;

	return ( DWORD_PTR )FileAccess ;
}

extern int ReadOnlyFileAccessClose( DWORD_PTR Handle )
{
	WINFILEACCESS *FileAccess = ( WINFILEACCESS * )Handle ;
	BOOL Result;
	DWORD Code ;

	// スレッドを使用する場合としない場合で処理を分岐
	if( FileAccess->UseThread == TRUE )
	{
		// これ以前の指令が出ていた場合の為に指令完了イベントがシグナル状態になるまで待つ
		WaitForSingleObject( FileAccess->ThreadData.CompEvent, INFINITE ) ;

		// スレッドに終了指令を出す
		FileAccess->ThreadData.Function = FILEACCESSTHREAD_FUNCTION_EXIT ;
		ResetEvent( FileAccess->ThreadData.CompEvent ) ;
		SetEvent( FileAccess->ThreadData.FuncEvent ) ;

		// 指令が終了するまで待つ
		WaitForSingleObject( FileAccess->ThreadData.CompEvent, INFINITE ) ;

		// スレッドが終了するのを待つ
		do
		{
			Sleep(1);
			GetExitCodeThread( FileAccess->ThreadData.ThreadHandle, &Code );
		}while( Code == STILL_ACTIVE );

		// キャッシュを使用していた場合はキャッシュ用メモリを開放する
		if( FileAccess->ThreadData.CacheBuffer )
			DXFREE( FileAccess->ThreadData.CacheBuffer ) ;

		// イベントやスレッドを閉じる
		CloseHandle( FileAccess->ThreadData.ThreadHandle ) ;
		CloseHandle( FileAccess->ThreadData.CompEvent ) ;
		CloseHandle( FileAccess->ThreadData.FuncEvent ) ;
		Result = 0 ;
	}
	else
	{
		// 使用していない場合はこの場でハンドルを閉じて終了
		Result = CloseHandle( FileAccess->Handle ) ;
	}
	DXFREE( FileAccess ) ;

	return Result != 0 ? 0 : -1/*EOF*/ ;
}

extern LONGLONG ReadOnlyFileAccessTell( DWORD_PTR Handle )
{
	WINFILEACCESS *FileAccess = ( WINFILEACCESS * )Handle ;

	return ( LONGLONG )FileAccess->Position ;
}

extern int ReadOnlyFileAccessSeek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType )
{
	WINFILEACCESS *FileAccess = ( WINFILEACCESS * )Handle ;
	ULONGLONG Pos = 0 ;
	int Result ;

	switch( SeekType )
	{
	case SEEK_CUR :
		if( FileAccess->Position + SeekPoint < 0 )
		{
			Pos = 0 ;
		}
		else
		{
			Pos = FileAccess->Position + SeekPoint ;
		}
		break ;

	case SEEK_END :
		if( FileAccess->Size + SeekPoint < 0 )
		{
			Pos = 0 ;
		}
		else
		{
			Pos = FileAccess->Size + SeekPoint ;
		}
		break ;

	case SEEK_SET :
		if( SeekPoint < 0 )
		{
			Pos = 0 ;
		}
		else
		{
			Pos = ( ULONGLONG )SeekPoint ;
		}
		break ;
	}

	// スレッドを使用しているかどうかで処理を分岐
	if( FileAccess->UseThread == TRUE )
	{
		// これ以前の指令が出ていた場合の為に指令完了イベントがシグナル状態になるまで待つ
		WaitForSingleObject( FileAccess->ThreadData.CompEvent, INFINITE ) ;

		// スレッドにファイル位置変更指令を出す
		FileAccess->ThreadData.Function = FILEACCESSTHREAD_FUNCTION_SEEK ;
		FileAccess->ThreadData.SeekPoint = Pos ;
		ResetEvent( FileAccess->ThreadData.CompEvent ) ;
		SetEvent( FileAccess->ThreadData.FuncEvent ) ;
	}
	else
	{
		// ファイルアクセス位置を変更する
		Result = SetFilePointer64( FileAccess->Handle, Pos ) ;
		if( Result == -1 ) return -1 ;
	}

	// 位置を保存しておく
	FileAccess->Position = Pos ;

	// 終端チェックフラグを倒す
	FileAccess->EofFlag = FALSE ;

	// 終了
	return 0 ;
}

extern size_t ReadOnlyFileAccessRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle )
{
	WINFILEACCESS *FileAccess = ( WINFILEACCESS * )Handle ;
	DWORD Result, BytesRead ;

	if( BlockSize == 0 ) return 0 ;

	// 終端チェック
	if( FileAccess->Position == FileAccess->Size )
	{
		FileAccess->EofFlag = TRUE ;
		return 0 ;
	}

	// 項目数調整
	if( BlockSize * DataNum + FileAccess->Position > FileAccess->Size )
	{
		DataNum = ( size_t )( ( FileAccess->Size - FileAccess->Position ) / BlockSize ) ;
	}
	
	if( DataNum == 0 )
	{
		FileAccess->EofFlag = TRUE ;
		return 0 ;
	}

	// スレッドを使用しているかどうかで処理を分岐
	if( FileAccess->UseThread == TRUE )
	{
		// これ以前の指令が出ていた場合の為に指令完了イベントがシグナル状態になるまで待つ
		WaitForSingleObject( FileAccess->ThreadData.CompEvent, INFINITE ) ;

		// スレッドにファイル読み込み指令を出す
		FileAccess->ThreadData.Function = FILEACCESSTHREAD_FUNCTION_READ ;
		FileAccess->ThreadData.ReadBuffer = Buffer ;
		FileAccess->ThreadData.ReadPosition = FileAccess->Position ;
		FileAccess->ThreadData.ReadSize = ( DWORD )( BlockSize * DataNum ) ;
		ResetEvent( FileAccess->ThreadData.CompEvent ) ;
		SetEvent( FileAccess->ThreadData.FuncEvent ) ;

		// 非同期かどうかで処理を分岐
		if( FileAccess->UseASyncReadFlag == FALSE )
		{
			// 同期読み込みの場合は指令が完了するまで待つ
			WaitForSingleObject( FileAccess->ThreadData.CompEvent, INFINITE ) ;
		}

		BytesRead = ( DWORD )( BlockSize * DataNum ) ;
		Result = 1 ;
	}
	else
	{
		Result = ( DWORD )ReadFile( FileAccess->Handle, Buffer, DWORD( BlockSize * DataNum ), &BytesRead, NULL ) ;
	}

	FileAccess->Position += ( DWORD )( DataNum * BlockSize ) ;
	return Result != 0 ? BytesRead / BlockSize : 0 ;
}

extern int ReadOnlyFileAccessEof( DWORD_PTR Handle )
{
	WINFILEACCESS *FileAccess = (WINFILEACCESS *)Handle ;

	return FileAccess->EofFlag ? EOF : 0 ;
}

extern int ReadOnlyFileAccessIdleCheck( DWORD_PTR Handle )
{
	WINFILEACCESS *FileAccess = (WINFILEACCESS *)Handle ;

	if( FileAccess->UseThread == TRUE )
	{
		return WaitForSingleObject( FileAccess->ThreadData.CompEvent, 0 ) == WAIT_TIMEOUT ? FALSE : TRUE ;
	}
	else
	{
		return TRUE ;
	}
}

extern int ReadOnlyFileAccessChDir( const wchar_t *Path )
{
	return SetCurrentDirectoryW( Path ) ;
}

extern int ReadOnlyFileAccessGetDir( wchar_t *Buffer )
{
	return ( int )GetCurrentDirectoryW( FILEPATH_MAX, Buffer ) ;
}

static void _WIN32_FIND_DATA_To_FILEINFO( WIN32_FIND_DATAW *FindData, FILEINFOW *FileInfo )
{
	// ファイル名のコピー
	_WCSCPY( FileInfo->Name, FindData->cFileName );

	// ディレクトリかどうかのフラグをセット
	FileInfo->DirFlag = (FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ? TRUE : FALSE;

	// ファイルのサイズをセット
	FileInfo->Size = ( LONGLONG )( ( ( ( ULONGLONG )FindData->nFileSizeHigh ) << 32 ) + FindData->nFileSizeLow ) ;

	// ファイルタイムを保存
	_FileTimeToLocalDateData( &FindData->ftCreationTime, &FileInfo->CreationTime );
	_FileTimeToLocalDateData( &FindData->ftLastWriteTime, &FileInfo->LastWriteTime );
}

// 戻り値: -1=エラー  -1以外=FindHandle
extern DWORD_PTR ReadOnlyFileAccessFindFirst( const wchar_t *FilePath, FILEINFOW *Buffer )
{
	WIN32_FIND_DATAW FindData ;
	HANDLE FindHandle ;

	FindHandle = FindFirstFileW( FilePath, &FindData ) ;
	if( FindHandle == INVALID_HANDLE_VALUE )
	{
		return ( DWORD_PTR )-1;
	}

	if( Buffer )
	{
		_WIN32_FIND_DATA_To_FILEINFO( &FindData, Buffer ) ;
	}

	return (DWORD_PTR)FindHandle;
}

// 戻り値: -1=エラー  0=成功
extern int ReadOnlyFileAccessFindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer )
{
	WIN32_FIND_DATAW FindData ;

	if( FindNextFileW( (HANDLE)FindHandle, &FindData ) == 0 )
	{
		return -1 ;
	}

	if( Buffer )
	{
		_WIN32_FIND_DATA_To_FILEINFO( &FindData, Buffer ) ;
	}

	return 0 ;
}

// 戻り値: -1=エラー  0=成功
extern int ReadOnlyFileAccessFindClose( DWORD_PTR FindHandle )
{
	// ０以外が返ってきたら成功
	return FindClose( (HANDLE)FindHandle ) != 0 ? 0 : -1;
}







#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

