//-----------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		Windows用Ogg関係プログラム
// 
//  	Ver 3.13f
// 
//-----------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

#include "DxCompileConfig.h"

#if !defined( DX_NON_OGGVORBIS ) || !defined( DX_NON_OGGTHEORA )

// インクルード----------------------------------------------------------------
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

// マクロ定義------------------------------------------------------------------

// 型定義----------------------------------------------------------------------

// データ宣言------------------------------------------------------------------


// 関数プロトタイプ宣言 -------------------------------------------------------

// プログラム------------------------------------------------------------------

// 環境依存関数

// ハードウエア機能が有効な場合は YUV フォーマットの一時保存用テクスチャを作成する
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

// カレントフレームのRGB画像を作成する( 戻り値  1:作成された  0:されなかった )
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

// 一時バッファの YUV フォーマットのテクスチャを得る
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


