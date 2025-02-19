// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		�����R�[�h�֌W�v���O�����w�b�_�t�@�C��
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXCHAR_H__
#define __DXCHAR_H__

// �C���N���[�h ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxStatic.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` --------------------------------------------------------------------

#define DX_CODEPAGE_SHIFTJIS	(932)
#define DX_CODEPAGE_GB2312		(936)
#define DX_CODEPAGE_UHC			(949)
#define DX_CODEPAGE_BIG5		(950)
#define DX_CODEPAGE_UTF16LE		(1200)
#define DX_CODEPAGE_UTF16BE		(1201)
#define DX_CODEPAGE_ASCII		(1252)
#define DX_CODEPAGE_UTF8		(65001)
#define DX_CODEPAGE_UTF32LE		(32766)			// �����ȃR�[�h�����������̂ŏ���ɒ�`
#define DX_CODEPAGE_UTF32BE		(32767)			// �����ȃR�[�h�����������̂ŏ���ɒ�`

// �V�t�gJIS�Q�o�C�g��������
#define CHECK_SHIFTJIS_2BYTE( x )			( ( BYTE )( ( ( ( BYTE )(x) ) ^ 0x20) - ( BYTE )0xa1 ) < 0x3c )

// UTF16LE�T���Q�[�g�y�A����( ���g���G���f�B�A�����p )
#define CHECK_CPU_LE_UTF16LE_4BYTE( x )		( ( ( x ) & 0xfc00 ) == 0xd800 )

// UTF16LE�T���Q�[�g�y�A����( �r�b�O�G���f�B�A�����p )
#define CHECK_CPU_BE_UTF16LE_4BYTE( x )		( ( ( ( ( ( ( WORD )( x ) ) >> 8 ) & 0xff ) | ( ( ( WORD )( x ) << 8 ) & 0xff00 ) ) & 0xfc00 ) == 0xd800 )

// UTF16BE�T���Q�[�g�y�A����( ���g���G���f�B�A�����p )
#define CHECK_CPU_LE_UTF16BE_4BYTE( x )		CHECK_CPU_BE_UTF16LE_4BYTE( x )

// UTF16BE�T���Q�[�g�y�A����( �r�b�O�G���f�B�A�����p )
#define CHECK_CPU_BE_UTF16BE_4BYTE( x )		CHECK_CPU_LE_UTF16LE_4BYTE( x )

// wchar_t �T���Q�[�g�y�A����( UTF-32 or UTF-16 �z�� )
#ifdef WCHAR_T_BE
	#define CHECK_WCHAR_T_DOUBLE( x )	( sizeof( wchar_t ) == 2 && ( ( ( WORD )( x ) & 0x00fc ) == 0x00d8 ) )
#else
	#define CHECK_WCHAR_T_DOUBLE( x )	( sizeof( wchar_t ) == 2 && ( ( ( WORD )( x ) & 0xfc00 ) == 0xd800 ) )
#endif

// �\���̒�` --------------------------------------------------------------------

// UTF-16�Ɗe�����R�[�h�̑Ή��\�̏��
struct CHARCODETABLEINFO
{
	WORD				MultiByteToUTF16[ 0x10000 ] ;		// �e�����R�[�h����UTF-16�ɕϊ����邽�߂̃e�[�u��
	WORD				UTF16ToMultiByte[ 0x10000 ] ;		// UTF-16����e�����R�[�h�ɕϊ����邽�߂̃e�[�u��
} ;

// �����R�[�h�����֌W�Ŏg�p������
struct CHARCODESYSTEM
{
	int					InitializeFlag ;					// �������������s�������ǂ���( TRUE:�s����  FALSE:�s���Ă��Ȃ� )

	CHARCODETABLEINFO	CharCodeCP932Info ;					// Shift-JIS�̕����R�[�h���
	CHARCODETABLEINFO	CharCodeCP936Info ;					// GB2312�̕����R�[�h���
	CHARCODETABLEINFO	CharCodeCP949Info ;					// UHC�̕����R�[�h���
	CHARCODETABLEINFO	CharCodeCP950Info ;					// BIG5�̕����R�[�h���
} ;

// �e�[�u��-----------------------------------------------------------------------

// �������ϐ��錾 --------------------------------------------------------------

extern CHARCODESYSTEM g_CharCodeSystem ;

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

extern	int				InitCharCode( void ) ;																			// �����R�[�h�֘A�����̏��������s��

extern	int				GetCodePageUnitSize(	int CodePage ) ;														// �w��̃R�[�h�y�[�W�̏��ŏ��T�C�Y���擾����( �߂�l�F�o�C�g�� )
extern	int				GetCharBytes(			const char *CharCode, int CodePage ) ;									// �P�����̃o�C�g�����擾����( �߂�l�F�P�����̃o�C�g�� )
extern	DWORD			GetCharCode(			const char *CharCode, int CodePage, int *CharBytes ) ;					// �P�����̕����R�[�h�ƕ����̃o�C�g�����擾����
extern	int				PutCharCode(			DWORD CharCode, int CodePage, char *Dest ) ;							// �����R�[�h��ʏ�̕�����ɕϊ�����A�I�[�Ƀk�������͏������܂Ȃ�( �߂�l�F�������񂾃o�C�g�� )
extern	DWORD			ConvCharCode(			DWORD SrcCharCode, int SrcCodePage, int DestCodePage ) ;				// �����R�[�h���w��̃R�[�h�y�[�W�̕����ɕϊ�����
extern	int				ConvCharCodeString(		const DWORD *Src, int SrcCodePage, DWORD *Dest, int DestCodePage ) ;	// �P�����S�o�C�g�̔z����A�ʃR�[�h�y�[�W�̂P�����S�o�C�g�̔z��ɕϊ�����( �߂�l�F�ϊ���̃T�C�Y�A�k�������܂�( �P�ʁF�o�C�g ) )
extern	int				StringToCharCodeString( const char  *Src, int SrcCodePage, DWORD  *Dest ) ;						// ��������P�����S�o�C�g�̔z��ɕϊ�����( �߂�l�F�ϊ���̃T�C�Y�A�k�������܂�( �P�ʁF�o�C�g ) )
extern	int				CharCodeStringToString( const DWORD *Src, char *Dest, int DestCodePage ) ;						// �P�����S�o�C�g�̔z��𕶎���ɕϊ�����( �߂�l�F�ϊ���̃T�C�Y�A�k�������܂�( �P�ʁF�o�C�g ) )
extern	int				ConvString(				const char *Src, int SrcCodePage, char *Dest, int DestCodePage ) ;		// ��������w��̃R�[�h�y�[�W�̕�����ɕϊ�����( �߂�l�F�ϊ���̃T�C�Y�A�k�������܂�( �P�ʁF�o�C�g ) )
extern	int				GetStringCharNum(		const char *String, int CodePage ) ;									// ������Ɋ܂܂�镶�������擾����
extern	const char *	GetStringCharAddress(	const char *String, int CodePage, int Index ) ;							// �w��ԍ��̕����̃A�h���X���擾����
extern	DWORD			GetStringCharCode(		const char *String, int CodePage, int Index ) ;							// �w��ԍ��̕����̃R�[�h���擾����

extern	void			CL_strcpy(            int CodePage, char *Dest, const char *Src ) ;
extern	void			CL_strncpy(           int CodePage, char *Dest, const char *Src, int Num ) ;
extern	void			CL_strcat(            int CodePage, char *Dest, const char *Src ) ;
extern	const char *	CL_strstr(            int CodePage, const char *Str1, const char *Str2 ) ;
extern	int				CL_strlen(            int CodePage, const char *Str ) ;
extern	int				CL_strcmp(            int CodePage, const char *Str1, const char *Str2 ) ;
extern	int				CL_stricmp(           int CodePage, const char *Str1, const char *Str2 ) ;
extern	int				CL_strcmp_str2_ascii( int CodePage, const char *Str1, const char *Str2 ) ;
extern	int				CL_strncmp(           int CodePage, const char *Str1, const char *Str2, int Size ) ;
extern	const char *	CL_strchr(            int CodePage, const char *Str, DWORD CharCode ) ;
extern	const char *	CL_strrchr(           int CodePage, const char *Str, DWORD CharCode ) ;
extern	char *			CL_strupr(            int CodePage, char *Str ) ;
extern	int				CL_vsprintf(          int CodePage, int IsWChar, int CharCodePage, int WCharCodePage, char *Buffer, const char *FormatString, va_list Arg ) ;
extern	int				CL_sprintf(           int CodePage, int IsWChar, int CharCodePage, int WCharCodePage, char *Buffer, const char *FormatString, ... ) ;
extern	char *			CL_itoa(              int CodePage, int Value, char *Buffer, int Radix ) ;
extern	int				CL_atoi(              int CodePage, const char *Str ) ;
extern	double			CL_atof(              int CodePage, const char *Str ) ;
extern	int				CL_vsscanf(           int CodePage, int IsWChar, int CharCodePage, int WCharCodePage, const char *String, const char *FormatString, va_list Arg ) ;
extern	int				CL_sscanf(            int CodePage, int IsWChar, int CharCodePage, int WCharCodePage, const char *String, const char *FormatString, ... ) ;

#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // __DXCHAR_H__
