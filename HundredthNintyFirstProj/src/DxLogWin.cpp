//-----------------------------------------------------------------------------
// 
// 		�c�w���C�u����		Windows�p���O�v���O����
// 
//  	Ver 3.13f
// 
//-----------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

// �C���N���[�h ---------------------------------------------------------------
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

// �}�N����` -----------------------------------------------------------------

// �\���̐錾 -----------------------------------------------------------------

// �f�[�^��` -----------------------------------------------------------------

// �֐��錾 -------------------------------------------------------------------

// �v���O���� -----------------------------------------------------------------

// �G���[�����o���t�@�C�������������鏈���̊��ˑ�����
extern int ErrorLogInitialize_Native( void )
{
	HANDLE fp ;
	wchar_t String[ 512 ] ;
	int Len ;

	// ���O�o�͗}���t���O�������Ă��邩�A���[�U�[�w��̃E�C���h�E��
	// �g�p���Ă���ꍇ�͏o�͂��s��Ȃ�
	if( LogData.NotLogOutFlag == TRUE /* || WinData.UserWindowFlag == TRUE */ ) return 0 ;

	// ���t���̃��O�o�͂��w�肳��Ă���ꍇ�̓t�@�C�������쐬
	if( LogData.UseLogDateName == TRUE )
	{
		DATEDATA Date ;
		wchar_t String[128] ;

		// ���t���擾
		NS_GetDateTime( &Date ) ;

		// ������̍쐬
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
		// ����ȊO�̏ꍇ�͒ʏ�̃t�@�C����
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

	// �G���[���O�t�@�C�����č쐬����
	DeleteFileW( String ) ;
	fp = CreateFileW( String, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL ) ;

	BYTE BOM[ 2 ] = { 0xff, 0xfe } ;
	DWORD WriteSize ;
	WriteFile( fp, &BOM, 2, &WriteSize, NULL ) ;

	if( fp != NULL ) CloseHandle( fp ) ;

	// �I��
	return 0 ;
}

// �G���[�����������o��
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

	// ���������񂪗]��ɂ�����������e���|�����̈�������Ŋm�ۂ���
	if( StringLength > 255 )
	{
		Temp = AllocTemp = ( wchar_t * )NS_DxAlloc( StringLength * 2 * sizeof( wchar_t ) ) ;
	}

	// \n �� \r\n �ɕϊ����Ȃ���R�s�[
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

	// �c�w���C�u�������쐬�����E�C���h�E�ł���ꍇ�̂݃t�@�C���ɏo�͂���
	if( /*WinData.MainWindow != NULL && WinData.UserWindowFlag == FALSE */ true )
	{
		// ���O�o�͗}���t���O�������Ă����ꍇ�͏o�͂��s��Ȃ�
		if( LogData.NotLogOutFlag == FALSE && LogData.LogOutDirectory[0] != L'\0' )
		{
//			wchar_t MotoPath[FILEPATH_MAX] ;
			wchar_t String[ FILEPATH_MAX ] ;
			int Len ;

			// �G���[���O�t�@�C�����J��
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

				// �K�蕪�����^�u��r�o
				if( LogData.ErTabStop == FALSE )
				{
					int i ;	

					// �^�C���X�^���v���o��
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

				// �G���[���O�t�@�C���ɏ����o��
				WriteFile( fp, ErrorStr, _WCSLEN( ErrorStr ) * sizeof( wchar_t ), &WriteSize, NULL ) ;

				// �G���[���O���A�E�g�v�b�g�ɏ����o��
				OutputDebugStringW( ErrorStr ) ;

				// �Ō�̕��������s�ӊO�������ꍇ�̓^�u�X�g�b�v�t���O�𗧂Ă�
				LogData.ErTabStop = ErrorStr[ _WCSLEN( ErrorStr ) - 1 ] != L'\n' ;

				CloseHandle( fp ) ;
			}
//			FSETDIR( MotoPath ) ;
		}
	}
	else
	{
		// �G���[���O���A�E�g�v�b�g�ɏ����o��
		OutputDebugStringW( ErrorStr ) ;
	}

	// �e���|�����̈���m�ۂ��Ă�����J������
	if( AllocTemp != NULL )
	{
		NS_DxFree( AllocTemp ) ;
	}
	
	// �I��
	return -1 ;
}


// ���O�@�\�̏��������s�����ǂ������擾����
extern int IsInitializeLog( void )
{
	if( WinData.MainWindow == NULL )
	{
		return FALSE ;
	}

	// �n�[�h�E�G�A�@�\���g���Ă��邩�ǂ����ŏ����𕪊�
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
		// �c�w���C�u�������I����Ԃɓ����Ă����珉�������Ȃ�
		return NS_GetUseDDrawObj() == NULL || WinData.MainWindow == NULL ? FALSE : TRUE ;
	}

	return TRUE ;
}


#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // DX_NON_LOG

