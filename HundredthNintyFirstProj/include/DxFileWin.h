// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		Windows�p�t�@�C���֌W�v���O�����w�b�_�t�@�C��
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXFILEWIN_H__
#define __DXFILEWIN_H__

// �C���N���[�h ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxFile.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` --------------------------------------------------------------------

// �\���̒�` --------------------------------------------------------------------

// �t�@�C���A�N�Z�X��p�X���b�h�p�\����
struct FILEACCESSTHREAD
{
	DWORD					ThreadID ;							// �X���b�h�h�c
	HANDLE					ThreadHandle ;						// �X���b�h�n���h��
	HANDLE					Handle ;							// �t�@�C���A�N�Z�X�p�n���h��
	HANDLE					FuncEvent ;							// �w�ߎ󂯎��p�n���h��
	HANDLE					CompEvent ;							// �w�ߊ����p�n���h��
	int						Function ;							// �w��( FILEACCESSTHREAD_FUNCTION_OPEN �� )

	int						EndFlag ;							// �I���������A�t���O
	int						ErrorFlag ;							// �G���[�����t���O

	wchar_t					FilePath[FILEPATH_MAX] ;			// �t�@�C���p�X
	void					*ReadBuffer ;						// �ǂݍ��ރf�[�^���i�[����o�b�t�@�[
	ULONGLONG				ReadPosition ;						// �ǂݍ��ރf�[�^�̈ʒu
	DWORD					ReadSize ;							// �ǂݍ��ރf�[�^�̃T�C�Y(�ǂݍ��߂��f�[�^�̃T�C�Y)
	ULONGLONG				SeekPoint ;							// �t�@�C���|�C���^���ړ�����ʒu 

	BYTE					*CacheBuffer ;						// �L���b�V���o�b�t�@�ւ̃|�C���^
	ULONGLONG				CachePosition ;						// �L���b�V�����Ă���t�@�C���ʒu
	ULONGLONG				CacheSize ;							// �L���b�V���o�b�t�@�̗L���f�[�^�T�C�Y
} ;

// �t�@�C���A�N�Z�X�����p�\����
struct WINFILEACCESS
{
	HANDLE					Handle ;							// �t�@�C���A�N�Z�X�p�n���h��
	int						UseThread ;							// �X���b�h���g�p���邩�ǂ���
	int						UseCacheFlag ;						// �L���b�V�����g�p���邩�ǂ���
	int						UseASyncReadFlag ;					// �񓯊��ǂݍ��݂��s�����ǂ���
	int						EofFlag ;							// �I�[�`�F�b�N�t���O
	ULONGLONG				Position ;							// �A�N�Z�X�ʒu
	ULONGLONG				Size ;								// �T�C�Y

	FILEACCESSTHREAD		ThreadData ;						// �ǂݍ��ݐ�p�X���b�h�̃f�[�^
} ;

// �e�[�u��-----------------------------------------------------------------------

// �������ϐ��錾 --------------------------------------------------------------

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

extern	HANDLE		CreateTemporaryFile( wchar_t *TempFileNameBuffer ) ;								// �e���|�����t�@�C�����쐬����

#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // __DXFILEWIN_H__
