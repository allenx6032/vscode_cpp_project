//-----------------------------------------------------------------------------
// 
// 		�c�w���C�u����		Windows�p�������֌W�v���O����
// 
//  	Ver 3.13f
// 
//-----------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

// �C���N���[�h ---------------------------------------------------------------
#include "DxMemoryWin.h"
#include <windows.h>

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` -----------------------------------------------------------------

// �\���̐錾 -----------------------------------------------------------------

// �f�[�^��` -----------------------------------------------------------------

// �֐��錾 -------------------------------------------------------------------

// �v���O���� -----------------------------------------------------------------

// ���b�p�[�֐�

// ���������m�ۂ���
extern void *AllocWrap( size_t AllocSize )
{
	void *AllocMemory ;

//	AllocMemory = malloc( AllocSize ) ;
	AllocMemory = HeapAlloc( GetProcessHeap(), 0, AllocSize ) ;

	return AllocMemory ;
}

// ���������m�ۂ��ĂO�ŏ���������
extern void *CallocWrap( size_t AllocSize )
{
	void *AllocMemory ;

//	AllocMemory = calloc( 1, AllocSize ) ;
	AllocMemory = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, AllocSize ) ;
	
	return AllocMemory ;
}

// �������̍Ċm�ۂ��s��
extern void *ReallocWrap( void *Memory, size_t AllocSize )
{
	void *AllocMemory ;

//	AllocMemory = realloc( Memory, AllocSize ) ;
	AllocMemory = HeapReAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, Memory, AllocSize ) ;
	
	return AllocMemory ;
}

// ���������������
extern void FreeWrap( void *Memory )
{
//	free( Memory ) ;
	HeapFree( GetProcessHeap(), 0, Memory ) ;
}

#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

