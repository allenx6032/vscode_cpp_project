// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		�V�X�e���v���O����
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

// �C���N���[�h ------------------------------------------------------------------
#include "DxSystem.h"
#include "DxBaseFunc.h"
#include "DxUseCLib.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` --------------------------------------------------------------------

// �\���̒�` --------------------------------------------------------------------

// �e�[�u��-----------------------------------------------------------------------

// �������ϐ��錾 --------------------------------------------------------------

DXSYSTEMDATA DxSysData ;

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

// �v���O���� --------------------------------------------------------------------

// �������E�I���֌W

// DxSysData �֌W�̏��������s��
extern int DxLib_SysInit( void )
{
	// �I�����N�G�X�g�̃t���O��|��
	DxSysData.EndRequestFlag = FALSE ;

	// �I��
	return 0 ;
}

// �I�����N�G�X�g���s��
extern int DxLib_EndRequest( void )
{
	DxSysData.EndRequestFlag = TRUE ;

	// �I��
	return 0 ;
}

// �I�����N�G�X�g�̏�Ԃ��擾����
extern int DxLib_GetEndRequest( void )
{
	return DxSysData.EndRequestFlag ;
}







// ���C�u����������������Ă��邩�ǂ������擾����( �߂�l: TRUE=����������Ă���  FALSE=����Ă��Ȃ� )
extern int NS_DxLib_IsInit( void )
{
	return DxSysData.DxLib_InitializeFlag ;
}






// �G���[�����֐�

// �����t�����C�u�����̃G���[�������s��
extern int DxLib_FmtError( const wchar_t *FormatString , ... )
{
	va_list VaList ;
	wchar_t String[ 1024 ];

	// ���O�o�͗p�̃��X�g���Z�b�g����
	va_start( VaList , FormatString ) ;

	// �ҏW��̕�������擾����
	_VSWPRINTF( String , FormatString , VaList ) ;

	// �ϒ����X�g�̃|�C���^�����Z�b�g����
	va_end( VaList ) ;

	// �G���[�����ɂ܂킷
	return DxLib_Error( String ) ;
}

// �����t�����C�u�����̃G���[�������s��
extern int DxLib_FmtErrorUTF16LE( const char *FormatString , ... )
{
	va_list VaList ;
	char String[ 2048 ];

	// ���O�o�͗p�̃��X�g���Z�b�g����
	va_start( VaList , FormatString ) ;

	// �ҏW��̕�������擾����
	CL_vsprintf( DX_CODEPAGE_UTF16LE, TRUE, CHAR_CODEPAGE, WCHAR_T_CODEPAGE, String, FormatString, VaList ) ;

	// �ϒ����X�g�̃|�C���^�����Z�b�g����
	va_end( VaList ) ;

	// �G���[�����ɂ܂킷
	return DxLib_ErrorUTF16LE( String ) ;
}






// �ǂݍ��ݏ����n�̊֐��Ŕ񓯊��ǂݍ��݂��s�����ǂ�����ݒ肷��( �񓯊��ǂݍ��݂ɑΉ����Ă���֐��̂ݗL�� )( TRUE:�񓯊��ǂݍ��݂��s��  FALSE:�񓯊��ǂݍ��݂��s��Ȃ�( �f�t�H���g ) )
extern int NS_SetUseASyncLoadFlag( int Flag )
{
	DxSysData.ASyncLoadFlag = Flag ;

	// �I��
	return 0 ;
}

// �񓯊��ǂݍ��݂��s�����ǂ������擾����( TRUE:�񓯊��ǂݍ��݂��s��   FALSE:�񓯊��ǂݍ��݂��s��Ȃ� )
extern int GetASyncLoadFlag( void )
{
	return DxSysData.ASyncLoadFlag ? TRUE : FALSE ;
}

// �c�w���C�u�����̃E�C���h�E�֘A�̋@�\���g�p���Ȃ��t���O
extern int NS_SetNotWinFlag( int Flag )
{
	if( Flag == TRUE ) DxSysData.NotDrawFlag = TRUE;
	DxSysData.NotWinFlag = Flag ;

	return 0 ;
}

// �`��@�\���g�����ǂ����̃t���O���Z�b�g����
extern int NS_SetNotDrawFlag( int Flag )
{
	DxSysData.NotDrawFlag = Flag ;

	return 0 ;
}

// �`��@�\���g�����ǂ����̃t���O���擾����
extern int NS_GetNotDrawFlag( void )
{
	return DxSysData.NotDrawFlag ;
}

// �T�E���h�@�\���g�����ǂ����̃t���O���Z�b�g����
extern int NS_SetNotSoundFlag( int Flag )
{
	DxSysData.NotSoundFlag = Flag ;

	return 0;
}

// ���͏�Ԏ擾�@�\���g�����ǂ����̃t���O���Z�b�g����
extern int NS_SetNotInputFlag( int Flag )
{
	DxSysData.NotInputFlag = Flag ;

	return 0;
}


#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE





