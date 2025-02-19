//-----------------------------------------------------------------------------
// 
// 		�c�w���C�u����		Windows�pOgg�֌W�v���O����
// 
//  	Ver 3.13f
// 
//-----------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

#include "DxCompileConfig.h"

#if !defined( DX_NON_OGGVORBIS ) || !defined( DX_NON_OGGTHEORA )

// �C���N���[�h----------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxUseCLib.h"
#include "DxLog.h"
#include "DxGraphicsWin.h"
#include "DxUseCLibOggWin.h"
#include "DxUseCLibOggD3D9.h"
#include "DxUseCLibOggD3D11.h"

//namespace DxLib
//{

// �}�N����`------------------------------------------------------------------

// �^��`----------------------------------------------------------------------

// �f�[�^�錾------------------------------------------------------------------


// �֐��v���g�^�C�v�錾 -------------------------------------------------------

// �v���O����------------------------------------------------------------------

// ���ˑ��֐�

// �n�[�h�E�G�A�@�\���L���ȏꍇ�� YUV �t�H�[�}�b�g�̈ꎞ�ۑ��p�e�N�X�`�����쐬����
extern	int	TheoraDecode_CreateSurface_PF( DECODE_THEORA *DT )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return TheoraDecode_D3D11_CreateSurface_PF( DT ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return TheoraDecode_D3D9_CreateSurface_PF( DT ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

extern	void	TheoraDecode_ReleaseSurface_PF(	DECODE_THEORA *DT )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		TheoraDecode_D3D11_ReleaseSurface_PF( DT ) ;
		break ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		TheoraDecode_D3D9_ReleaseSurface_PF( DT ) ;
		break ;
#endif // DX_NON_DIRECT3D9

	default :
		break ;
	}
}

// �J�����g�t���[����RGB�摜���쐬����( �߂�l  1:�쐬���ꂽ  0:����Ȃ����� )
extern	int		TheoraDecode_SetupImage_PF( DECODE_THEORA *DT, volatile THEORA_STOCKFRAME *Stock, int ASyncThread )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return TheoraDecode_D3D11_SetupImage_PF( DT, Stock, ASyncThread ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return TheoraDecode_D3D9_SetupImage_PF( DT, Stock, ASyncThread ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}

// �ꎞ�o�b�t�@�� YUV �t�H�[�}�b�g�̃e�N�X�`���𓾂�
extern	const D_IDirect3DSurface9 *TheoraDecode_GetYUVImage_PF( DECODE_THEORA *DT )
{
	switch( GRAWIN.Setting.UseGraphicsAPI )
	{
#ifndef DX_NON_DIRECT3D11
	case GRAPHICS_API_DIRECT3D11_WIN32 :
		return TheoraDecode_D3D11_GetYUVImage_PF( DT ) ;
#endif // DX_NON_DIRECT3D11

#ifndef DX_NON_DIRECT3D9
	case GRAPHICS_API_DIRECT3D9_WIN32 :
		return TheoraDecode_D3D9_GetYUVImage_PF( DT ) ;
#endif // DX_NON_DIRECT3D9

	default :
		return 0 ;
	}
}





//}

#endif // #if !defined( DX_NON_OGGVORBIS ) || !defined( DX_NON_OGGTHEORA )


