// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		演算プログラムヘッダファイル
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXMATH_H__
#define __DXMATH_H__

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

extern	void	TriangleBarycenter_Base(  const VECTOR   *TrianglePos1, const VECTOR   *TrianglePos2, const VECTOR   *TrianglePos3, const VECTOR   *Position, float  *TrianglePos1Weight, float  *TrianglePos2Weight, float  *TrianglePos3Weight ) ;	// 指定の座標から三角形の重心を求める
extern	void	TriangleBarycenter_BaseD( const VECTOR_D *TrianglePos1, const VECTOR_D *TrianglePos2, const VECTOR_D *TrianglePos3, const VECTOR_D *Position, double *TrianglePos1Weight, double *TrianglePos2Weight, double *TrianglePos3Weight ) ;	// 指定の座標から三角形の重心を求める

extern VECTOR	Get_Triangle_Point_MinPosition(  VECTOR   Point, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;			// 点に一番近い三角形上の座標を得る
extern VECTOR_D	Get_Triangle_Point_MinPositionD( VECTOR_D Point, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;			// 点に一番近い三角形上の座標を得る
extern VECTOR	Get_Line_Point_MinPosition(  VECTOR   Point, VECTOR   LinePos1, VECTOR   LinePos2 ) ;											// 点に一番近い線上の座標を得る
extern VECTOR_D	Get_Line_Point_MinPositionD( VECTOR_D Point, VECTOR_D LinePos1, VECTOR_D LinePos2 ) ;											// 点に一番近い線上の座標を得る
extern int		Get_TwoPlane_Line(  VECTOR   Normal1, float  Dist1, VECTOR   Normal2, float  Dist2, VECTOR   *OutDir, VECTOR   *OutPos ) ;			// 二つの平面が交差する線を得る( 戻り値が -1 の場合は、二つの面は平行 )
extern int		Get_TwoPlane_LineD( VECTOR_D Normal1, double Dist1, VECTOR_D Normal2, double Dist2, VECTOR_D *OutDir, VECTOR_D *OutPos ) ;			// 二つの平面が交差する線を得る( 戻り値が -1 の場合は、二つの面は平行 )
extern int		Get_TwoLine_MinLength_Rate(  VECTOR   Direction1, VECTOR   Position1, float  *OutRate1, VECTOR   Direction2, VECTOR   Position2, float  *OutRate2 ) ;		// 二つの線の最近点のそれぞれの方向ベクトルの拡大値を得る
extern int		Get_TwoLine_MinLength_RateD( VECTOR_D Direction1, VECTOR_D Position1, double *OutRate1, VECTOR_D Direction2, VECTOR_D Position2, double *OutRate2 ) ;		// 二つの線の最近点のそれぞれの方向ベクトルの拡大値を得る

extern void		CreateNormalizePlane(  FLOAT4  *Plane, VECTOR   *Position, VECTOR   *Normal ) ;												// 平面上の点と平面の法線から正規化された平面パラメータを算出する
extern void		CreateNormalizePlaneD( DOUBLE4 *Plane, VECTOR_D *Position, VECTOR_D *Normal ) ;												// 平面上の点と平面の法線から正規化された平面パラメータを算出する
extern void		SetMathScreenSize( int SizeX, int SizeY ) ;																				// DxMath.cpp で使用する画面のサイズを設定する

// 半精度浮動小数点を単精度浮動小数点に変換する
__inline float Float16ToFloat32( WORD Float16 )
{
	unsigned int s ;
	int e ;
	unsigned int f ;
	unsigned int dest ;

	if( Float16 == 0 )
	{
		return	0.0f ;
	}

	s =     Float16 & 0x8000 ;
	e = ( ( Float16 & 0x7c00 ) >> 10 ) - 15 + 127 ;
	f =     Float16 & 0x03ff ;
	dest = ( s << 16 ) | ( ( e << 23 ) & 0x7f800000 ) | ( f << 13 ) ;
	return	*( ( float * )&dest ) ;
}

// 単精度浮動小数点を半精度浮動小数点に変換する
__inline WORD Float32ToFloat16( float Float32 )
{
	unsigned int src = *( ( DWORD * )&Float32 ) ;
	int e ;
	unsigned int s ;
	unsigned int f ;

	if( src == 0 )
	{
		return 0 ;
	}

	e = ( ( src & 0x7f800000 ) >> 23 ) - 127 + 15 ;
	if( e < 0 )
	{
		return 0 ;
	}
	else
	if( e > 31 )
	{
		e = 31 ;
	}

	s = src & 0x80000000 ;
	f = src & 0x007fffff ;
	return ( ( s >> 16 ) & 0x8000 ) | ( ( e << 10 ) & 0x7c00 ) | ( ( f >> 13 ) & 0x03ff ) ;
}

#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // __DXMATH_H__
