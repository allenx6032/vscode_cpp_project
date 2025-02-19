//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用ログプログラム
// 
//  	Ver 3.13f
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ---------------------------------------------------------------
#include "DxLogWin.h"

#ifndef DX_NON_LOG

#include "DxStatic.h"
#include "DxLib.h"
#include "DxLog.h"
#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxGraphics.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGraphicsWin.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// マクロ定義 -----------------------------------------------------------------

// 構造体宣言 -----------------------------------------------------------------

// データ定義 -----------------------------------------------------------------

// 関数宣言 -------------------------------------------------------------------

// プログラム -----------------------------------------------------------------

// エラー書き出しファイルを初期化する処理の環境依存部分
extern int ErrorLogInitialize_Native( void )
{
	HANDLE fp ;
	wchar_t String[ 512 ] ;
	int Len ;

	// ログ出力抑制フラグが立っているか、ユーザー指定のウインドウを
	// 使用している場合は出力を行わない
	if( LogData.NotLogOutFlag == TRUE /* || WinData.UserWindowFlag == TRUE */ ) return 0 ;

	// 日付つきのログ出力が指定されている場合はファイル名を作成
	if( LogData.UseLogDateName == TRUE )
	{
		DATEDATA Date ;
		wchar_t String[128] ;

		// 日付を取得
		NS_GetDateTime( &Date ) ;

		// 文字列の作成
		_WCSCPY( LogData.LogFileName, L"Log" ) ;
		_ITOAW( Date.Year, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Mon, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Day, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Hour, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Min, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L"_" ) ;
		_ITOAW( Date.Sec, String, 10 ) ;
		_WCSCAT( LogData.LogFileName, String ) ;
		_WCSCAT( LogData.LogFileName, L".txt" ) ;
	}
	else
	{
		// それ以外の場合は通常のファイル名
		_WCSCPY( LogData.LogFileName, L"Log.txt" ) ;
	}

	Len = _WCSLEN( LogData.LogOutDirectory ) ;
	_WCSNCPY( String, LogData.LogOutDirectory, Len ) ;
	if( String[ Len - 1 ] != L'\\' )
	{
		String[ Len ] = L'\\' ;
		Len ++ ;
	}
	_WCSCPY( String + Len, LogData.LogFileName ) ;

	// エラーログファイルを再作成する
	DeleteFileW( String ) ;
	fp = CreateFileW( String, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;

	BYTE BOM[ 2 ] = { 0xff, 0xfe } ;
	DWORD WriteSize ;
	WriteFile( fp, &BOM, 2, &WriteSize, NULL ) ;

	if( fp != NULL ) CloseHandle( fp ) ;

	// 終了
	return 0 ;
}

// エラー文書を書き出す
extern int ErrorLogAdd_WCHAR_T( const wchar_t *ErrorStr )
{
	HANDLE fp ;
	DWORD WriteSize, StringLength ;
	wchar_t *Temp, DefaultTemp[256], *d, *AllocTemp = NULL ;
	const wchar_t *p1, *p2 ;

	if( WinAPIData.Win32Func.WinMMDLL == NULL )
		return -1 ;

	StringLength = ( DWORD )_WCSLEN( ErrorStr ) ;
	Temp = DefaultTemp ;

	// もし文字列が余りにも長かったらテンポラリ領域をここで確保する
	if( StringLength > 255 )
	{
		Temp = AllocTemp = ( wchar_t * )NS_DxAlloc( StringLength * 2 * sizeof( wchar_t ) ) ;
	}

	// \n を \r\n に変換しながらコピー
	{
		Temp[0] = L'\0' ;
		p1 = ErrorStr ;
		d = Temp ;

		p2 = _WCSCHR( p1, L'\n' ) ;
		while( p2 != NULL && ( ErrorStr == p2 || p2[-1] != L'\r' ) )
		{
			_MEMCPY( d, p1, ( p2 - p1 ) * sizeof( wchar_t ) ) ;
			d += p2 - p1 ;
			d[0] = L'\r' ;
			d[1] = L'\n' ;
			d[2] = L'\0' ;
			d += 2 ;
			p1 = p2 + 1 ;

			p2 = _WCSCHR( p1, L'\n' ) ;
		}
		_WCSCPY( d, p1 ) ;
		ErrorStr = Temp ;
	}

	// ＤＸライブラリが作成したウインドウである場合のみファイルに出力する
	if( /*WinData.MainWindow != NULL && WinData.UserWindowFlag == FALSE */ true )
	{
		// ログ出力抑制フラグが立っていた場合は出力を行わない
		if( LogData.NotLogOutFlag == FALSE && LogData.LogOutDirectory[0] != L'\0' )
		{
//			wchar_t MotoPath[FILEPATH_MAX] ;
			wchar_t String[ FILEPATH_MAX ] ;
			int Len ;

			// エラーログファイルを開く
//			FGETDIR( MotoPath ) ;
//			FSETDIR( LogData.LogOutDirectory ) ;
			Len = _WCSLEN( LogData.LogOutDirectory ) ;
			_WCSNCPY( String, LogData.LogOutDirectory, Len ) ;
//			_WCSCPY( String, LogData.LogOutDirectory ) ;
			if( String[ Len - 1 ] != L'\\' )
			{
				String[ Len ] = L'\\' ;
				Len ++ ;
			}
			_WCSCPY( String + Len, LogData.LogFileName ) ;
			fp = CreateFileW( String, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;
			if( fp != NULL )
			{
				SetFilePointer( fp, 0, NULL, FILE_END ) ;

				// 規定分だけタブを排出
				if( LogData.ErTabStop == FALSE )
				{
					int i ;	

					// タイムスタンプを出力
					if( LogData.NonUseTimeStampFlag == 0 )
					{
						_SWPRINTF( String, L"%d:", WinAPIData.Win32Func.timeGetTimeFunc() - LogData.LogStartTime ) ;
						WriteFile( fp, String, _WCSLEN( String ) * sizeof( wchar_t ), &WriteSize, NULL ) ;
						OutputDebugStringW( String ) ;
					}

					if( LogData.ErTabNum != 0 )
					{
						for( i = 0 ; i < LogData.ErTabNum ; i ++ )
						{
							String[i] = L'\t' ;
						}
						String[i] = L'\0' ;
						WriteFile( fp, String, LogData.ErTabNum * sizeof( wchar_t ), &WriteSize, NULL ) ;
						OutputDebugStringW( String ) ;
					}
				}

				// エラーログファイルに書き出す
				WriteFile( fp, ErrorStr, _WCSLEN( ErrorStr ) * sizeof( wchar_t ), &WriteSize, NULL ) ;

				// エラーログをアウトプットに書き出す
				OutputDebugStringW( ErrorStr ) ;

				// 最後の文字が改行意外だった場合はタブストップフラグを立てる
				LogData.ErTabStop = ErrorStr[ _WCSLEN( ErrorStr ) - 1 ] != L'\n' ;

				CloseHandle( fp ) ;
			}
//			FSETDIR( MotoPath ) ;
		}
	}
	else
	{
		// エラーログをアウトプットに書き出す
		OutputDebugStringW( ErrorStr ) ;
	}

	// テンポラリ領域を確保していたら開放する
	if( AllocTemp != NULL )
	{
		NS_DxFree( AllocTemp ) ;
	}
	
	// 終了
	return -1 ;
}


// ログ機能の初期化を行うかどうかを取得する
extern int IsInitializeLog( void )
{
	if( WinData.MainWindow == NULL )
	{
		return FALSE ;
	}

	// ハードウエア機能を使っているかどうかで処理を分岐
	if( GSYS.Setting.ValidHardware )
	{
		switch( GRAWIN.Setting.UseGraphicsAPI )
		{
#ifndef DX_NON_DIRECT3D11
		case GRAPHICS_API_DIRECT3D11_WIN32 :
			return D3D11Device_IsValid() ? TRUE : FALSE ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
		case GRAPHICS_API_DIRECT3D9_WIN32 :
			return NS_GetUseDDrawObj() == NULL ? FALSE : TRUE ;
#endif // DX_NON_DIRECT3D9

		default :
			return FALSE ;
		}
	}
	else
	{
		// ＤＸライブラリが終了状態に入っていたら初期化しない
		return NS_GetUseDDrawObj() == NULL || WinData.MainWindow == NULL ? FALSE : TRUE ;
	}

	return TRUE ;
}


#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // DX_NON_LOG

