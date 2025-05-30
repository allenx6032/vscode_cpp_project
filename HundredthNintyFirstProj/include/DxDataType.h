// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		データタイプ定義ヘッダファイル
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXDATATYPE_H__
#define __DXDATATYPE_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include <stdio.h>

#if !defined( __ANDROID )
#include "DxDataTypeWin.h"
#endif

#if defined( __ANDROID )
#include "DxDataTypeAndroid.h"
#endif

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

#define SETRECT( Dest, Left, Top, Right, Bottom )	\
	( Dest ).left   = Left ;\
	( Dest ).top    = Top ;\
	( Dest ).right  = Right ;\
	( Dest ).bottom = Bottom ;

// 構造体定義 --------------------------------------------------------------------

// ＲＧＢＡ色構造体
struct RGBCOLOR
{
	unsigned char			Blue, Green, Red, Alpha ;
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // __DXDATATYPE_H__
