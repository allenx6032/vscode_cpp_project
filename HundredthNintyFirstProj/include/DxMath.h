// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		���Z�v���O�����w�b�_�t�@�C��
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXMATH_H__
#define __DXMATH_H__

// �C���N���[�h ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` --------------------------------------------------------------------

// �\���̒�` --------------------------------------------------------------------

// �e�[�u��-----------------------------------------------------------------------

// �������ϐ��錾 --------------------------------------------------------------

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

extern	void	TriangleBarycenter_Base(  const VECTOR   *TrianglePos1, const VECTOR   *TrianglePos2, const VECTOR   *TrianglePos3, const VECTOR   *Position, float  *TrianglePos1Weight, float  *TrianglePos2Weight, float  *TrianglePos3Weight ) ;	// �w��̍��W����O�p�`�̏d�S�����߂�
extern	void	TriangleBarycenter_BaseD( const VECTOR_D *TrianglePos1, const VECTOR_D *TrianglePos2, const VECTOR_D *TrianglePos3, const VECTOR_D *Position, double *TrianglePos1Weight, double *TrianglePos2Weight, double *TrianglePos3Weight ) ;	// �w��̍��W����O�p�`�̏d�S�����߂�

extern VECTOR	Get_Triangle_Point_MinPosition(  VECTOR   Point, VECTOR   TrianglePos1, VECTOR   TrianglePos2, VECTOR   TrianglePos3 ) ;			// �_�Ɉ�ԋ߂��O�p�`��̍��W�𓾂�
extern VECTOR_D	Get_Triangle_Point_MinPositionD( VECTOR_D Point, VECTOR_D TrianglePos1, VECTOR_D TrianglePos2, VECTOR_D TrianglePos3 ) ;			// �_�Ɉ�ԋ߂��O�p�`��̍��W�𓾂�
extern VECTOR	Get_Line_Point_MinPosition(  VECTOR   Point, VECTOR   LinePos1, VECTOR   LinePos2 ) ;											// �_�Ɉ�ԋ߂�����̍��W�𓾂�
extern VECTOR_D	Get_Line_Point_MinPositionD( VECTOR_D Point, VECTOR_D LinePos1, VECTOR_D LinePos2 ) ;											// �_�Ɉ�ԋ߂�����̍��W�𓾂�
extern int		Get_TwoPlane_Line(  VECTOR   Normal1, float  Dist1, VECTOR   Normal2, float  Dist2, VECTOR   *OutDir, VECTOR   *OutPos ) ;			// ��̕��ʂ�����������𓾂�( �߂�l�� -1 �̏ꍇ�́A��̖ʂ͕��s )
extern int		Get_TwoPlane_LineD( VECTOR_D Normal1, double Dist1, VECTOR_D Normal2, double Dist2, VECTOR_D *OutDir, VECTOR_D *OutPos ) ;			// ��̕��ʂ�����������𓾂�( �߂�l�� -1 �̏ꍇ�́A��̖ʂ͕��s )
extern int		Get_TwoLine_MinLength_Rate(  VECTOR   Direction1, VECTOR   Position1, float  *OutRate1, VECTOR   Direction2, VECTOR   Position2, float  *OutRate2 ) ;		// ��̐��̍ŋߓ_�̂��ꂼ��̕����x�N�g���̊g��l�𓾂�
extern int		Get_TwoLine_MinLength_RateD( VECTOR_D Direction1, VECTOR_D Position1, double *OutRate1, VECTOR_D Direction2, VECTOR_D Position2, double *OutRate2 ) ;		// ��̐��̍ŋߓ_�̂��ꂼ��̕����x�N�g���̊g��l�𓾂�

extern void		CreateNormalizePlane(  FLOAT4  *Plane, VECTOR   *Position, VECTOR   *Normal ) ;												// ���ʏ�̓_�ƕ��ʂ̖@�����琳�K�����ꂽ���ʃp�����[�^���Z�o����
extern void		CreateNormalizePlaneD( DOUBLE4 *Plane, VECTOR_D *Position, VECTOR_D *Normal ) ;												// ���ʏ�̓_�ƕ��ʂ̖@�����琳�K�����ꂽ���ʃp�����[�^���Z�o����
extern void		SetMathScreenSize( int SizeX, int SizeY ) ;																				// DxMath.cpp �Ŏg�p�����ʂ̃T�C�Y��ݒ肷��

// �����x���������_��P���x���������_�ɕϊ�����
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

// �P���x���������_�𔼐��x���������_�ɕϊ�����
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
