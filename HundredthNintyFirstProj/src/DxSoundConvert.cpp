// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		�T�E���h�f�[�^�ϊ��v���O����
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

// �C���N���[�h ------------------------------------------------------------------
#include "DxSoundConvert.h"
#include "DxStatic.h"

#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxUseCLib.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` --------------------------------------------------------------------

// �`�����N�h�c�Z�b�g�}�N��
#define RIFFCHUNKID( C1 , C2 , C3 , C4 )	( DWORD )( ((( DWORD )C4)<<24) | ((( DWORD )C3)<<16) | ((( DWORD )C2)<<8) | (C1) ) 

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT		0x0003
#endif

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM				0x0001
#endif

// �\���̒�` --------------------------------------------------------------------

// �T�E���h�ϊ������S�̂Ŏg�p����f�[�^�\����
struct SOUNDCONVERTDATA
{
	int							InitializeFlag ;				// �������t���O

#ifndef DX_NON_ACM
	HMODULE						msacm32DLL ;					// msacm32DLL
	MMRESULT					( WINAPI *acmStreamOpenFunc            )( D_LPHACMSTREAM phas, HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, D_LPWAVEFILTER pwfltr, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen );
	MMRESULT					( WINAPI *acmFormatSuggestFunc         )( HACMDRIVER had, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDst, DWORD cbwfxDst, DWORD fdwSuggest );
	MMRESULT					( WINAPI *acmStreamCloseFunc           )( D_HACMSTREAM has, DWORD fdwClose );
	MMRESULT					( WINAPI *acmMetricsFunc               )( D_HACMOBJ hao, UINT uMetric, LPVOID pMetric );
	MMRESULT					( WINAPI *acmStreamPrepareHeaderFunc   )( D_HACMSTREAM has, D_LPACMSTREAMHEADER pash, DWORD fdwPrepare );
	MMRESULT					( WINAPI *acmStreamConvertFunc         )( D_HACMSTREAM has, D_LPACMSTREAMHEADER pash, DWORD fdwConvert );
	MMRESULT					( WINAPI *acmStreamUnprepareHeaderFunc )( D_HACMSTREAM has, D_LPACMSTREAMHEADER pash, DWORD fdwUnprepare );
	MMRESULT					( WINAPI *acmStreamSizeFunc            )( D_HACMSTREAM has, DWORD cbInput, LPDWORD pdwOutputBytes, DWORD fdwSize );
#endif
} ;

// �������ϐ��錾 --------------------------------------------------------------

// �T�E���h�ϊ������S�̂Ŏg�p����f�[�^
SOUNDCONVERTDATA GSoundConvertData ;

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

static	int CheckNormalWaveFormat( STREAMDATA *Stream ) ;											// �ʏ�̂o�b�l�`�������ׂ�( TRUE:�o�b�l�`��  FALSE:�Ⴄ )

static	int SetupSoundConvert_WAVE(            SOUNDCONV *SoundConv ) ;								// �v�`�u�d�t�@�C���̃Z�b�g�A�b�v�������s��( [��] -1:�G���[ )
static	int TerminateSoundConvert_WAVE(        SOUNDCONV *SoundConv ) ;								// �v�`�u�d�t�@�C���̌�n���������s��
static	int ConvertProcessSoundConvert_WAVE(   SOUNDCONV *SoundConv ) ;								// �ϊ���̃o�b�t�@�Ƀf�[�^���[����
//static	int SetTimeSoundConvert_WAVE(      SOUNDCONV *SoundConv, int Time ) ;					// �ϊ������̈ʒu��ύX����( �~���b�P�� )
static	int SetSampleTimeSoundConvert_WAVE(    SOUNDCONV *SoundConv, int SampleTime ) ;				// �ϊ������̈ʒu��ύX����( �T���v���P�� )
static	int GetSoundConvertDestSize_Fast_WAVE( SOUNDCONV *SoundConv ) ;								// �ϊ���̑�}�̃f�[�^�T�C�Y�𓾂�

#ifndef DX_NON_ACM
static	int SetupSoundConvert_ACM(            SOUNDCONV *SoundConv ) ;								// �`�b�l���g�p�����t�@�C���̃Z�b�g�A�b�v�������s��( [��] -1:�G���[ )
static	int SetupSoundConvert_MP3(            SOUNDCONV *SoundConv ) ;								// �l�o�R�t�@�C���̃Z�b�g�A�b�v�������s��( [��] -1:�G���[ )
static	int TerminateSoundConvert_ACM(        SOUNDCONV *SoundConv ) ;								// �`�b�l���g�p�����t�@�C���̌�n���������s��
static	int ConvertProcessSoundConvert_ACM(   SOUNDCONV *SoundConv ) ;								// �ϊ���̃o�b�t�@�Ƀf�[�^���[����
//static	int SetTimeSoundConvert_ACM(      SOUNDCONV *SoundConv, int Time ) ;					// �ϊ��������ʒu��ύX����( �~���b�P�� )
static	int SetSampleTimeSoundConvert_ACM(    SOUNDCONV *SoundConv, int SampleTime ) ;				// �ϊ������̈ʒu��ύX����( �T���v���P�� )
static	int GetSoundConvertDestSize_Fast_ACM( SOUNDCONV *SoundConv ) ;								// �ϊ���̑�}�̃f�[�^�T�C�Y�𓾂�
#endif

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
static	int TerminateSoundConvert_DSMP3(        SOUNDCONV *SoundConv ) ;							// MP3���g�p�����t�@�C���̌�n���������s��
static	int ConvertProcessSoundConvert_DSMP3(   SOUNDCONV *SoundConv ) ;							// �ϊ���̃o�b�t�@�Ƀf�[�^���[����
//extern	int SetTimeSoundConvert_DSMP3(      SOUNDCONV *SoundConv, int Time ) ;					// �ϊ��������ʒu��ύX����( �~���b�P�� )
static	int SetSampleTimeSoundConvert_DSMP3(    SOUNDCONV *SoundConv, int SampleTime ) ;			// �ϊ������̈ʒu��ύX����( �T���v���P�� )
static	int GetSoundConvertDestSize_Fast_DSMP3( SOUNDCONV *SoundConv ) ;							// �ϊ���̑�}�̃f�[�^�T�C�Y�𓾂�
#endif
#endif
#endif

// �v���O���� --------------------------------------------------------------------

// �������E�I���֐�

// �T�E���h�f�[�^�ϊ������̏��������s��
extern	int InitializeSoundConvert( void )
{
	// ���ɏ������ς݂̏ꍇ�͉������Ȃ�
	if( GSoundConvertData.InitializeFlag == TRUE )
		return -1 ;

#ifndef DX_NON_ACM
	// MSACM32DLL �����[�h����
	if( GSoundConvertData.msacm32DLL == NULL )
	{
		GSoundConvertData.msacm32DLL = LoadLibrary( _T( "msacm32.dll" ) ) ;
		if( GSoundConvertData.msacm32DLL == NULL )
			return -1 ;

		GSoundConvertData.acmFormatSuggestFunc = ( MMRESULT ( WINAPI * )( HACMDRIVER, LPWAVEFORMATEX, LPWAVEFORMATEX, DWORD, DWORD ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmFormatSuggest" ) ;
		if( GSoundConvertData.acmFormatSuggestFunc == NULL )
			return -1 ;

		GSoundConvertData.acmStreamOpenFunc = ( MMRESULT ( WINAPI * )( D_LPHACMSTREAM, HACMDRIVER, LPWAVEFORMATEX, LPWAVEFORMATEX, D_LPWAVEFILTER, DWORD, DWORD, DWORD ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmStreamOpen" ) ;
		if( GSoundConvertData.acmStreamOpenFunc == NULL )
			return -1 ;

		GSoundConvertData.acmStreamCloseFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, DWORD ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmStreamClose" ) ;
		if( GSoundConvertData.acmStreamCloseFunc == NULL )
			return -1 ;

		GSoundConvertData.acmMetricsFunc = ( MMRESULT ( WINAPI * )( D_HACMOBJ, UINT, LPVOID ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmMetrics" ) ;
		if( GSoundConvertData.acmMetricsFunc == NULL )
			return -1 ;

		GSoundConvertData.acmStreamPrepareHeaderFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, D_LPACMSTREAMHEADER, DWORD ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmStreamPrepareHeader" ) ;
		if( GSoundConvertData.acmStreamPrepareHeaderFunc == NULL )
			return -1 ;

		GSoundConvertData.acmStreamConvertFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, D_LPACMSTREAMHEADER, DWORD ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmStreamConvert" ) ;
		if( GSoundConvertData.acmStreamConvertFunc == NULL )
			return -1 ;

		GSoundConvertData.acmStreamUnprepareHeaderFunc = ( MMRESULT ( WINAPI * )( D_HACMSTREAM, D_LPACMSTREAMHEADER, DWORD ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmStreamUnprepareHeader" ) ;
		if( GSoundConvertData.acmStreamUnprepareHeaderFunc == NULL )
			return -1 ;

		GSoundConvertData.acmStreamSizeFunc = ( MMRESULT  ( WINAPI * )( D_HACMSTREAM, DWORD, LPDWORD, DWORD ) )GetProcAddress( GSoundConvertData.msacm32DLL, "acmStreamSize" ) ;
		if( GSoundConvertData.acmStreamSizeFunc == NULL )
			return -1 ;
	}
#endif

	// �����������t���O�𗧂Ă�
	GSoundConvertData.InitializeFlag = TRUE ;

	// ����I��
	return 0 ;
}

// �T�E���h�f�[�^�ϊ������̏I���������s��
extern	int TerminateSoundConvert( void )
{
	// ���Ɍ�n�������ς݂̏ꍇ�͉������Ȃ�
	if( GSoundConvertData.InitializeFlag == FALSE )
		return -1 ;

#ifndef DX_NON_ACM
	// msacm32.DLL �̉��
	if( GSoundConvertData.msacm32DLL )
	{
		FreeLibrary( GSoundConvertData.msacm32DLL ) ;
		GSoundConvertData.msacm32DLL = NULL ;
		GSoundConvertData.acmFormatSuggestFunc = NULL ;
		GSoundConvertData.acmStreamOpenFunc = NULL ;
		GSoundConvertData.acmStreamCloseFunc = NULL ;
		GSoundConvertData.acmMetricsFunc = NULL ;
		GSoundConvertData.acmStreamPrepareHeaderFunc = NULL ;
		GSoundConvertData.acmStreamConvertFunc = NULL ;
		GSoundConvertData.acmStreamUnprepareHeaderFunc = NULL ;
		GSoundConvertData.acmStreamSizeFunc = NULL ;
	}
#endif

	// �����������t���O��|��
	GSoundConvertData.InitializeFlag = FALSE ;

	// ����I��
	return 0 ;
}

// �ϊ������̃Z�b�g�A�b�v( [��] -1:�G���[ )
extern	int SetupSoundConvert( SOUNDCONV *SoundConv, STREAMDATA *Stream, int DisableReadSoundFunctionMask, int OggVorbisBitDepth, int OggVorbisFromTheoraFile )
{
	// ����
#ifndef DX_NON_OGGVORBIS
	SoundConv->OggVorbisBitDepth = OggVorbisBitDepth ;
	SoundConv->OggVorbisFromTheoraFile = OggVorbisFromTheoraFile ;
#endif
	SoundConv->Stream = *Stream ;
	SoundConv->SeekLockPosition = 0 ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_PCM ) == 0 )
	{
		if( SetupSoundConvert_WAVE( SoundConv ) == 0 ) goto R1 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
#ifndef DX_NON_ACM
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_ACM ) == 0 )
	{
		if( SetupSoundConvert_ACM( SoundConv ) == 0 ) goto R1 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
#endif
#ifndef DX_NON_OGGVORBIS
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_OGG ) == 0 )
	{
		if( SetupSoundConvert_OGG( SoundConv ) == 0 ) goto R1 ;
		Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	}
#endif
#ifndef DX_NON_ACM
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_MP3 ) == 0 )
	{
		if( SetupSoundConvert_MP3( SoundConv ) == 0 ) goto R1 ;
	}
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	if( ( DisableReadSoundFunctionMask & DX_READSOUNDFUNCTION_DSMP3 ) == 0 )
	{
		if( SetupSoundConvert_DSMP3( SoundConv ) == 0 ) goto R1 ;
	}
#endif
#endif
#endif
	return -1 ;
	
R1 :
	// ���ʏ������������s��
	SoundConv->InitializeFlag = FALSE ;
	SoundConv->EndFlag = FALSE ;
	SoundConv->DestData = DXALLOC( ( size_t )SoundConv->DestDataSize ) ;
	if( SoundConv->DestData == NULL )
	{
		TerminateSoundConvert( SoundConv ) ;
		return -1 ;
	}
	SoundConv->DestDataValidSize = 0 ;
	SoundConv->DestDataCompSize = 0 ;
	SoundConv->DestDataCompSizeAll = 0 ;

	// ����������
	SoundConv->InitializeFlag = TRUE ;
	
	// �I��
	return 0 ;
}

/*
// �ϊ�������������Ԃɖ߂�
extern	int SetTimeSoundConvert( SOUNDCONV *SoundConv, int Time )
{
	int res ;
	int BytePos ;

	BytePos = ( SoundConv->OutFormat.nSamplesPerSec * Time / 1000 ) * SoundConv->OutFormat.nBlockAlign ;
	if( _ABS( BytePos - SoundConv->DestDataCompSizeAll ) < 16 ) return 0 ;

	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : res = SetTimeSoundConvert_WAVE( SoundConv, Time ) ; break ;
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : res = SetTimeSoundConvert_ACM( SoundConv, Time ) ; break ;
#endif
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : res = SetTimeSoundConvert_OGG( SoundConv, Time ) ; break ;
#endif
	}
	SoundConv->EndFlag = FALSE ;
	return res ;
}
*/

// �ϊ������̈ʒu��ύX����( �T���v���P�� )
extern int SetSampleTimeSoundConvert( SOUNDCONV *SoundConv, int SampleTime )
{
	int res = -1 ;
	int BytePos ;

	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign ;
	if( _ABS( BytePos - SoundConv->DestDataCompSizeAll ) < 16 ) return 0 ;

	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : res = SetSampleTimeSoundConvert_WAVE( SoundConv, SampleTime ) ; break ;
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : res = SetSampleTimeSoundConvert_ACM( SoundConv, SampleTime ) ; break ;
#endif
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : res = SetSampleTimeSoundConvert_OGG( SoundConv, SampleTime ) ; break ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	case SOUND_METHODTYPE_DSMP3 : res = SetSampleTimeSoundConvert_DSMP3( SoundConv, SampleTime ) ; break ;
#endif
#endif
#endif
	}
	SoundConv->EndFlag = FALSE ;
	return res ;
}

// �V�[�N�\��̈ʒu���Z�b�g���Ă���(�`�b�l�ׂ̈�)
extern	int SetupSeekPosSoundConvert( SOUNDCONV *SoundConv, int SeekPos )
{
	SoundConv->SeekLockPosition = SeekPos * SoundConv->OutFormat.nBlockAlign ;

	return 0 ;
}


// �w��̃T�C�Y��������������( [��] -1:�G���[  0�ȏ�:�ϊ������T�C�Y )
extern	int RunSoundConvert( SOUNDCONV *SoundConv, void *DestBuffer, int DestSize )
{
	int pos, movesize, res = -1, copysize ;
	
	if( SoundConv->InitializeFlag == FALSE ) return -1 ;
	if( SoundConv->EndFlag == TRUE ) return 0 ;
	
	pos = 0 ;
	copysize = 0 ;
	while( DestSize != 0 )
	{
		if( SoundConv->DestDataValidSize == 0 )
		{
			SoundConv->DestDataCompSize = 0 ;
			switch( SoundConv->MethodType )
			{
			case SOUND_METHODTYPE_NORMAL : res = ConvertProcessSoundConvert_WAVE( SoundConv ) ; break ;
#ifndef DX_NON_ACM
			case SOUND_METHODTYPE_ACM : res = ConvertProcessSoundConvert_ACM( SoundConv ) ; break ;
#endif
#ifndef DX_NON_OGGVORBIS
			case SOUND_METHODTYPE_OGG : res = ConvertProcessSoundConvert_OGG( SoundConv ) ; break ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
			case SOUND_METHODTYPE_DSMP3 : res = ConvertProcessSoundConvert_DSMP3( SoundConv ) ; break ;
#endif
#endif
#endif
			}
			if( res == -1 ) break ;
		}
	
		movesize = DestSize ;
		if( movesize > SoundConv->DestDataValidSize ) movesize = SoundConv->DestDataValidSize ;
/*
		int i ;
		int tmp ;
		short *dst, *src ;

		for( i = 0 ; i < movesize / 4 ; i ++ )
		{
			dst = ( short * )( (BYTE *)DestBuffer + pos + i * 4 ) ;
			src = ( short * )( (BYTE *)SoundConv->DestData + SoundConv->DestDataCompSize + i * 4 ) ;

			tmp = src[ 0 ] * 3 / 2 ;
			if( tmp >  32767 ) tmp =  32767 ;
			if( tmp < -32768 ) tmp = -32768 ;
			dst[ 0 ] = ( short )tmp ;

//			NS_DrawPixel( i, ( tmp + 32768 ) / 256, GetColor( 255,255,255 ) ) ;

			tmp = src[ 1 ] * 3 / 2 ;
			if( tmp >  32767 ) tmp =  32767 ;
			if( tmp < -32768 ) tmp = -32768 ;
			dst[ 1 ] = ( short )tmp ;
		}
//		NS_ScreenFlip() ;
//		WaitKey() ;
*/
		_MEMCPY( (BYTE *)DestBuffer + pos,
				(BYTE *)SoundConv->DestData + SoundConv->DestDataCompSize,
				( size_t )movesize ) ;
		SoundConv->DestDataValidSize -= movesize ;
		SoundConv->DestDataCompSize += movesize ;
		SoundConv->DestDataCompSizeAll += movesize ;
		
		DestSize -= movesize ;
		pos += movesize ;
		copysize += movesize ;
	}
	if( DestSize != 0 )
	{
		SoundConv->EndFlag = TRUE ;
	}

	// �I��
	return copysize ;
}

// �ϊ������̌�n�����s��
extern	int TerminateSoundConvert( SOUNDCONV *SoundConv )
{
	if( SoundConv->InitializeFlag == FALSE ) return -1 ;
	
	if( SoundConv->DestData != NULL )
	{
		DXFREE( SoundConv->DestData ) ;
		SoundConv->DestData = NULL ;
	}
	
	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : TerminateSoundConvert_WAVE( SoundConv ) ; break ;
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : TerminateSoundConvert_ACM( SoundConv ) ; break ;
#endif
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : TerminateSoundConvert_OGG( SoundConv ) ; break ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	case SOUND_METHODTYPE_DSMP3 : TerminateSoundConvert_DSMP3( SoundConv ) ; break ;
#endif
#endif
#endif
	}

	SoundConv->InitializeFlag = FALSE ;
	
	// �I��
	return 0 ;	
}

// �ϊ���̉����`�����擾( [��] -1:�G���[  0�ȏ�:�ϊ���̃T�C�Y )
extern	int GetOutSoundFormatInfo( SOUNDCONV *SoundConv, WAVEFORMATEX *OutWaveFormat )
{
	*OutWaveFormat = SoundConv->OutFormat ;
	return 0 ;
}

// �ϊ���̉����`�����擾( [��] -1:�G���[  0�ȏ�:�ϊ���̃T�C�Y )
extern	int GetOutSoundFormat( SOUNDCONV *SoundConv, WAVEFORMATEX *OutWaveFormat )
{
	*OutWaveFormat = SoundConv->OutFormat ;
	
	return 0 ;
}

// �ϊ��������I�����Ă��邩�ǂ������擾����( [��] TRUE:�I������  FALSE:�܂��I�����Ă��Ȃ� )
extern	int GetSoundConvertEndState( SOUNDCONV *SoundConv )
{
	return SoundConv->EndFlag ;
}

// �ϊ���̑�}�̃f�[�^�T�C�Y�𓾂�
extern	int GetSoundConvertDestSize_Fast( SOUNDCONV *SoundConv )
{
	switch( SoundConv->MethodType )
	{
	case SOUND_METHODTYPE_NORMAL : return GetSoundConvertDestSize_Fast_WAVE( SoundConv ) ; break ;
#ifndef DX_NON_ACM
	case SOUND_METHODTYPE_ACM : return GetSoundConvertDestSize_Fast_ACM( SoundConv ) ; break ;
#endif
#ifndef DX_NON_OGGVORBIS
	case SOUND_METHODTYPE_OGG : return GetSoundConvertDestSize_Fast_OGG( SoundConv ) ; break ;
#endif
#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3
	case SOUND_METHODTYPE_DSMP3 : return GetSoundConvertDestSize_Fast_DSMP3( SoundConv ) ; break ;
#endif
#endif
#endif
	}
	return 0 ;
}

//extern void *SoundBuf ;
//extern int SoundSize ;

// �����Ȉꊇ�ϊ�
extern	int SoundConvertFast( SOUNDCONV *SoundConv, 
									WAVEFORMATEX *FormatP, void **DestBufferP, int *DestSizeP )
{
	void *Buffer ;
	int BufferSize ;

	// �o�b�t�@�̊m��
	BufferSize = GetSoundConvertDestSize_Fast( SoundConv ) ;

	Buffer = DXALLOC( ( size_t )BufferSize ) ;
	if( Buffer == NULL ) return -1 ;
	
	// �ϊ��ʒu�̃Z�b�g
//	SetTimeSoundConvert( SoundConv, 0 ) ;
	SetSampleTimeSoundConvert( SoundConv, 0 ) ;


	// �f�[�^�`���������k�o�b�l�̏ꍇ�݂̂��̂܂܃R�s�[
	if( SoundConv->MethodType == SOUND_METHODTYPE_NORMAL &&
		( ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer )->SrcFormat.wFormatTag == WAVE_FORMAT_PCM )
	{
		SoundConv->Stream.ReadShred.Read( Buffer, ( size_t )BufferSize, 1, SoundConv->Stream.DataPoint ) ;
	}
	else
	{
		// ����ȊO�̏ꍇ�͕��ʂɓW�J
		BufferSize = RunSoundConvert( SoundConv, Buffer, BufferSize ) ;
	}

	// ����ۑ�
	*FormatP = SoundConv->OutFormat ;
	*DestBufferP = Buffer ;
	*DestSizeP = BufferSize ;
	
	// �I��
	return 0 ;
}















// �t�H�[�}�b�g�ϊ��p

// �ʏ�̂o�b�l�`�������ׂ�( TRUE:�o�b�l�`��  FALSE:�Ⴄ )
extern	int CheckNormalWaveFormat( STREAMDATA *Stream )
{
	___RIFFCHUNK chunk ;
	char type[5] ;
	WAVEFORMAT format ;

	// RIFF�t�@�C�����ǂ������ׂ�
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'R', 'I', 'F', 'F' ) )
		return FALSE ;
		
	// WAVE�t�@�C�����ǂ������ׂ�
	type[4] = '\0' ;
	Stream->ReadShred.Read( type, 4, 1, Stream->DataPoint ) ;
	if( _STRCMP( type, "WAVE" ) != 0 )
		return FALSE ;
		
	// �m�[�}���Ȃo�b�l�`�������ׂ�
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'f', 'm', 't', ' ' ) )
		return FALSE ;

	Stream->ReadShred.Read( &format, sizeof( WAVEFORMAT ), 1, Stream->DataPoint ) ;
	if( format.wFormatTag != WAVE_FORMAT_PCM )
		return FALSE ;
		
	// �����܂ŗ�����ʏ�̃t�H�[�}�b�g�Ƃ�������
	return TRUE ;
}

// �v�`�u�d�t�@�C���̃Z�b�g�A�b�v�������s��( [��] -1:�G���[ )
static	int SetupSoundConvert_WAVE( SOUNDCONV *SoundConv )
{
	___RIFFCHUNK chunk ;
	char type[5] ;
	WAVEFORMATEX *format = NULL ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	Wave->SrcBuffer = NULL ;

	// RIFF�t�@�C�����ǂ������ׂ�
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'R', 'I', 'F', 'F' ) )
		goto ERR ;

	// WAVE�t�@�C�����ǂ������ׂ�
	type[4] = '\0' ;
	Stream->ReadShred.Read( type, 4, 1, Stream->DataPoint ) ;
	if( _STRCMP( type, "WAVE" ) != 0 )
		goto ERR ;

	// �m�[�}���Ȃo�b�l�`�������ׂ�
	{
		// �t�H�[�}�b�g�`�����N������܂ŃX�L�b�v
		while( Stream->ReadShred.Eof( Stream->DataPoint ) == 0 )
		{
			Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
			if( chunk.fcc == RIFFCHUNKID( 'f', 'm', 't', ' ' ) )
			{
				break ;
			}
			Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ;
		}
		if( Stream->ReadShred.Eof( Stream->DataPoint ) != 0 )
			goto ERR ;

		format = (WAVEFORMATEX *)DXALLOC( chunk.cb ) ;
		if( format == NULL ) goto ERR ;

		// �w�b�_�̈ʒu�ƃT�C�Y���Z�b�g
		SoundConv->HeaderPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
		SoundConv->HeaderSize = ( int )chunk.cb ;

		_MEMSET( SoundConv->ConvFunctionBuffer, 0, sizeof( SoundConv->ConvFunctionBuffer ) ) ;

		_MEMSET( &SoundConv->OutFormat, 0, sizeof( SoundConv->OutFormat ) ) ;
		Stream->ReadShred.Read( format, chunk.cb, 1, Stream->DataPoint ) ;
		switch( format->wFormatTag )
		{
		case WAVE_FORMAT_PCM :

			// �`���� WAVEFORMATEX �ł͂Ȃ������ꍇ�͕⊮����
			_MEMCPY( &SoundConv->OutFormat, format, chunk.cb ) ;
			if( chunk.cb == 16 )
			{
				SoundConv->OutFormat.wBitsPerSample = ( WORD )( format->nBlockAlign / format->nChannels * 8 ) ;
				SoundConv->OutFormat.cbSize = 0 ;
			}
			break ;


		case WAVE_FORMAT_IEEE_FLOAT :

			// �o�̓t�H�[�}�b�g���Z�b�g
			SoundConv->OutFormat.nSamplesPerSec  = format->nSamplesPerSec ;
			SoundConv->OutFormat.nChannels       = format->nChannels ;
			SoundConv->OutFormat.wBitsPerSample  = 16 ;
			SoundConv->OutFormat.wFormatTag      = WAVE_FORMAT_PCM ;
			SoundConv->OutFormat.nBlockAlign     = ( WORD )( SoundConv->OutFormat.wBitsPerSample  * SoundConv->OutFormat.nChannels / 8 ) ;
			SoundConv->OutFormat.nAvgBytesPerSec = SoundConv->OutFormat.nBlockAlign * SoundConv->OutFormat.nSamplesPerSec ;

			Wave->SrcSampleNum = SoundConv->OutFormat.nSamplesPerSec ;
			Wave->SrcBufferSize = format->nAvgBytesPerSec ;
			Wave->SrcBuffer = ( BYTE * )DXALLOC( Wave->SrcBufferSize ) ;
			if( Wave->SrcBuffer == NULL )
				goto ERR ;
			break ;


		default :
			goto ERR ;
		}

		// �t�H�[�}�b�g�̃R�s�[
		_MEMCPY( &Wave->SrcFormat, format, chunk.cb ) ;

		// �������̉��
		DXFREE( format ) ;
		format = NULL ;
	}

	// �f�[�^�`�����N��T��
	while( Stream->ReadShred.Eof( Stream->DataPoint ) == 0 )
	{
		Stream->ReadShred.Read( &chunk , sizeof( chunk ) , 1 , Stream->DataPoint ) ;
		if( chunk.fcc == RIFFCHUNKID( 'd', 'a', 't', 'a' ) ) break ;
		Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ; 
	}
	if( Stream->ReadShred.Eof( Stream->DataPoint ) != 0 )
		goto ERR ;

	SoundConv->DataPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	SoundConv->DataSize = ( int )chunk.cb ;

	// �ϊ���̂o�b�l�f�[�^���ꎞ�I�ɕۑ����郁�����̈�̃T�C�Y���Z�b�g
	SoundConv->DestDataSize = ( int )SoundConv->OutFormat.nAvgBytesPerSec ;

	// �^�C�v�Z�b�g
	SoundConv->MethodType = SOUND_METHODTYPE_NORMAL ;

	// ����I��
	return 0 ;

ERR :
	if( Wave->SrcBuffer != NULL )
	{
		DXFREE( Wave->SrcBuffer ) ;
		Wave->SrcBuffer = NULL ;
	}

	if( format != NULL )
	{
		DXFREE( format ) ;
		format = NULL ;
	}

	return -1 ;
}

#ifndef DX_NON_ACM

#ifdef DX_GCC_COMPILE
#define ACM_FORMATSUGGESTF_WFORMATTAG       0x00010000L
#define ACM_FORMATSUGGESTF_NCHANNELS        0x00020000L
#define ACM_FORMATSUGGESTF_NSAMPLESPERSEC   0x00040000L
#define ACM_FORMATSUGGESTF_WBITSPERSAMPLE   0x00080000L

#define ACM_STREAMOPENF_QUERY           0x00000001
#define ACM_STREAMOPENF_ASYNC           0x00000002
#define ACM_STREAMOPENF_NONREALTIME     0x00000004
#endif

// �`�b�l�t�@�C���̃Z�b�g�A�b�v�������s��( [��] -1:�G���[ )
static	int SetupSoundConvert_ACM( SOUNDCONV *SoundConv )
{
	___RIFFCHUNK chunk ;
	char type[5] ;
	STREAMDATA *Stream = &SoundConv->Stream ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
	int res, i ;
	
	acmdata->Format = NULL ;
	acmdata->AcmStreamHandle[0] = NULL ;
	acmdata->AcmStreamHandle[1] = NULL ;
	acmdata->SrcData = NULL ;
	acmdata->DestDataSampleNum = -1 ;
	acmdata->BeginSeekPosition = 0 ;
	acmdata->BeginSeekCompSrcSize = 0 ;
	acmdata->BeginSeekBuffer = NULL ;

	// RIFF�t�@�C�����ǂ������ׂ�
	Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
	if( chunk.fcc != RIFFCHUNKID( 'R', 'I', 'F', 'F' ) )
		goto ERR ;

	// WAVE�t�@�C�����ǂ������ׂ�
	type[4] = '\0' ;
	Stream->ReadShred.Read( type, 4, 1, Stream->DataPoint ) ;
	if( _STRCMP( type, "WAVE" ) != 0 )
		goto ERR ;

	// �t�H�[�}�b�g���擾����
	{
		// �g����񂪂�������X�L�b�v����
		Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
		if( chunk.fcc == RIFFCHUNKID( 'b', 'e', 'x', 't' ) )
		{
			Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb, SEEK_CUR ) ;
			Stream->ReadShred.Read( &chunk, sizeof( chunk ), 1, Stream->DataPoint ) ;
		}
		if( chunk.fcc != RIFFCHUNKID( 'f', 'm', 't', ' ' ) ) goto ERR ;

		acmdata->Format = (WAVEFORMATEX *)DXALLOC( chunk.cb ) ;
		if( acmdata->Format == NULL ) goto ERR ;

		// �w�b�_�̈ʒu�ƃT�C�Y���Z�b�g
		SoundConv->HeaderPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
		SoundConv->HeaderSize = ( int )chunk.cb ;

		Stream->ReadShred.Read( acmdata->Format, chunk.cb, 1, Stream->DataPoint ) ;

		// �`���� WAVEFORMATEX �ł͂Ȃ������ꍇ�͕⊮����
		if( acmdata->Format->wFormatTag == WAVE_FORMAT_PCM && chunk.cb == 16 )
		{
			acmdata->Format->wBitsPerSample = ( WORD )( acmdata->Format->nBlockAlign / acmdata->Format->nChannels * 8 ) ;
			acmdata->Format->cbSize = 0 ;
		}
	}
	
	// �f�[�^�`�����N��T��
	while( Stream->ReadShred.Eof( Stream->DataPoint ) == 0 )
	{
		Stream->ReadShred.Read( &chunk , sizeof( chunk ) , 1 , Stream->DataPoint ) ;
		if( chunk.fcc == RIFFCHUNKID( 'd', 'a', 't', 'a' ) ) break ;
		Stream->ReadShred.Seek( Stream->DataPoint, chunk.cb + ( ( chunk.cb % 2 != 0 ) ? 1 : 0 ) , SEEK_CUR ) ; 
	}
	
	if( Stream->ReadShred.Eof( Stream->DataPoint ) != 0 )
		goto ERR ;

	// �f�[�^�`�����N�̏����Z�b�g
	SoundConv->DataPos = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	SoundConv->DataSize = ( int )chunk.cb ;

	// �ϊ���̐����t�H�[�}�b�g�𓾂�
	_MEMSET( &SoundConv->OutFormat, 0, sizeof( SoundConv->OutFormat ) ) ;
	SoundConv->OutFormat.wFormatTag = WAVE_FORMAT_PCM ;
	res = ( int )GSoundConvertData.acmFormatSuggestFunc( NULL, acmdata->Format, &SoundConv->OutFormat, sizeof( WAVEFORMATEX ), ACM_FORMATSUGGESTF_WFORMATTAG ) ;
	if( res != 0 ) goto ERR ;
	
	// ���k�f�[�^���ꎞ�I�ɕۑ����郁�����̈�̊m��
//	acmdata->SrcDataSize = AdjustSoundDataBlock( acmdata->Format->nAvgBytesPerSec / STS_CACHEBUFFERSEC, acmdata->Format ) ;
	acmdata->SrcDataSize = SoundConv->DataSize ;  // ���[�v�����̊֌W�ŁA�ہX�f�[�^�����܂�̈���m�ۂ���
	acmdata->SrcData = DXALLOC( ( size_t )acmdata->SrcDataSize ) ;
	if( acmdata->SrcData == NULL ) goto ERR ;
//	acmdata->SrcDataValidSize = 0 ;
	acmdata->SrcDataPosition = 0 ;

	// ���k�f�[�^���ꊇ�œǂݍ���
	Stream->ReadShred.Read( acmdata->SrcData, ( size_t )SoundConv->DataSize, 1, Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, SoundConv->DataPos, STREAM_SEEKTYPE_SET ) ;
//	acmdata->SrcDataValidSize = SoundConv->DataSize ;
	
	// �`�b�l�n���h���̍쐬
	for( i = 0 ; i < 2 ; i ++ )
	{
		res = ( int )GSoundConvertData.acmStreamOpenFunc( &acmdata->AcmStreamHandle[i],
								NULL,
								acmdata->Format,
								&SoundConv->OutFormat,
								NULL,
								0,
								0,
								ACM_STREAMOPENF_NONREALTIME ) ;
		if( res != 0 ) goto ERR ;
	}

	// �ϊ���̂o�b�l�f�[�^���ꎞ�I�ɕۑ����郁�����̈�̃T�C�Y���Z�b�g
	SoundConv->DestDataSize = ( int )SoundConv->OutFormat.nAvgBytesPerSec ;

	// �V�[�N�\��ʒu�܂ŕϊ����Ă��������Ŏg�p���郁�����̈�̊m��
	acmdata->BeginSeekBuffer = DXALLOC( ( size_t )SoundConv->DestDataSize ) ;
	if( acmdata->BeginSeekBuffer == NULL ) goto ERR ;

	// �^�C�v�Z�b�g
	SoundConv->MethodType = SOUND_METHODTYPE_ACM ;

	// ����I��
	return 0 ;

ERR :
	if( acmdata->AcmStreamHandle[0] != NULL )
	{
		GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
		acmdata->AcmStreamHandle[0] = NULL ;
	}

	if( acmdata->AcmStreamHandle[1] != NULL )
	{
		GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
		acmdata->AcmStreamHandle[1] = NULL ;
	}

	if( acmdata->BeginSeekBuffer != NULL )
	{
		DXFREE( acmdata->BeginSeekBuffer ) ;
		acmdata->BeginSeekBuffer = NULL ;
	}

	if( acmdata->Format != NULL )
	{
		DXFREE( acmdata->Format ) ;
		acmdata->Format = NULL ;
	}

	if( acmdata->SrcData != NULL )
	{
		DXFREE( acmdata->SrcData ) ;
		acmdata->SrcData = NULL ;
	}
	
	return -1 ;
}

#endif


#ifndef DX_NON_ACM

// �l�o�R�̃w�b�_�t�@�C�����쐬����Ƃ��ɕK�v�ȃf�[�^�̒�`

// �r�b�g���[�g���̎擾
int MP3BitRateTable[2][3][14] =
{
	// �l�o�d�f�|�P
	{
		// Layer-1
		32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,

		// Layer-2
		32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384,

		// Layer-3
		32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320,
	},

	// �l�o�d�f�|�Q
	{
		// Layer-1
		32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256,

		// Layer-2
		8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 114, 160,

		// Later-3
		8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 114, 160,
	},
};

// �T���v�����O���g��
int MP3FreqTable[2][3] =
{
	// �l�o�d�f�|�P
	44100,48000,32000,

	// �l�o�d�f�|�Q
	22050,24000,16000,
};

// �t���[���w�b�_�\����
struct MP3_FRAMEHEADER
{
	BYTE Layer ;			// 0:Layer-1 1:Layer-2 2:Layer-3
	BYTE MpegV ;			// 0:MPEG1 1:MPEG2
	BYTE BitRateIndex ;
	BYTE SampleRateIndex ;
	BYTE PaddingBit ;
	BYTE ChannelNum ;
	DWORD SampleRate ;
	DWORD BitRate ;
	DWORD FrameSize ;
	DWORD SampleNum ;
} ;

static int AnalysisMP3_FrameHeader( BYTE *Header, MP3_FRAMEHEADER *FrHead )
{
	// �l�o�R�f�[�^�����ׂ�
	if( Header[0] != 0xff || ( Header[1] & 0xf0 ) != 0xf0 ) return -2 ;

	// �l�o�d�f�̃o�[�W�����𓾂�(0:Ver1 1:Ver2)
	FrHead->MpegV = ( BYTE )( ( Header[1] & 0x8 ) == 0x8 ? 0 : 1 ) ;

	// ���C���𓾂�(0:Layer-1 1:Layer-2 2:Layer-3)
	FrHead->Layer = ( BYTE )( 3 - ( ( Header[1] >> 1 ) & 0x3 ) ) ;
	if( FrHead->Layer > 2 ) return -1 ;

	// �r�b�g���[�g�̃C���f�b�N�X�l�𓾂�
	FrHead->BitRateIndex = ( BYTE )( ( Header[2] >> 4 ) & 0xf ) ;
	if( FrHead->BitRateIndex > 14 ) return -1 ;

	// �r�b�g���[�g�𓾂�
	FrHead->BitRate = ( DWORD )MP3BitRateTable[FrHead->MpegV][FrHead->Layer][FrHead->BitRateIndex-1] ;

	// �T���v�����O���g���̃C���f�b�N�X�l�𓾂�
	FrHead->SampleRateIndex = ( BYTE )( ( Header[2] >> 2 ) & 0x3 ) ;
	if( FrHead->SampleRateIndex > 2 ) return -1 ;

	// �T���v�����O���g���𓾂�
	FrHead->SampleRate = ( DWORD )MP3FreqTable[FrHead->MpegV][FrHead->SampleRateIndex] ;

	// �p�f�B���O�r�b�g�𓾂�
	FrHead->PaddingBit = ( BYTE )( ( Header[2] >> 1 ) & 1 ) ;

	// �`�����l�����[�h����`�����l�����𓾂�
	FrHead->ChannelNum = ( BYTE )( ( ( Header[3] >> 6 ) & 0x3 ) == 3 ? 1 : 2 ) ;
	
	// �t���[���T�C�Y���v�Z����
	switch( FrHead->MpegV )
	{
	case 0 : // Mapg-1
		switch( FrHead->Layer )
		{
		case 0 :		// Layer-1
			FrHead->FrameSize = ( ( 12 * ( FrHead->BitRate * 1000 ) / FrHead->SampleRate ) + FrHead->PaddingBit ) * 4 ;
			FrHead->SampleNum = 12 * 8 ;
			break ;
			
		case 1 :		// Layer-2 Layer-3
		case 2 :
			FrHead->FrameSize = ( 144 * ( FrHead->BitRate * 1000 ) / FrHead->SampleRate ) + FrHead->PaddingBit ;
			FrHead->SampleNum = 144 * 8 ;
			break ;
		}
		break ;

	case 1 : // Mapg-2
		switch( FrHead->Layer )
		{
		case 0 :		// Layer-1
			FrHead->FrameSize = ( ( 12 * ( FrHead->BitRate * 1000 ) / FrHead->SampleRate ) + FrHead->PaddingBit ) * 4 ;
			FrHead->SampleNum = 12 * 8 ;
			break ;

		case 1 :		// Layer-2 Layer-3
		case 2 :
			FrHead->FrameSize = ( 72 * ( FrHead->BitRate * 1000 ) / FrHead->SampleRate ) + FrHead->PaddingBit ;
			FrHead->SampleNum = 72 * 8 ;
			break ;
		}
		break ;
	}

	// ����I��	
	return 0 ;
}

// �l�o�R�t�@�C���̃Z�b�g�A�b�v�������s��( [��] -1:�G���[ )
static int SetupSoundConvert_MP3( SOUNDCONV *SoundConv )
{
	BYTE Header[4] ;
//	BYTE Layer, MpegV, BitRateIndex, SampleRateIndex ;
//	BYTE PaddingBit, ChannelNum ;
//	DWORD SampleRate, BitRate, FrameSize ;
	D_MPEGLAYER3WAVEFORMAT *MP3Format = NULL ;
	MP3_FRAMEHEADER MP3Frame ;
	int DataSize, FileSize, /*StartPoint,*/ res, i;
	DWORD LastPoint ;
	DWORD SkipSize, EndSkipSize, StartPos ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	

	acmdata->Format = NULL ;
	acmdata->SrcData = NULL ;
	acmdata->DestDataSampleNum = 0 ;
	acmdata->BeginSeekPosition = 0 ;
	acmdata->BeginSeekCompSrcSize = 0 ;

	// �t�@�C���T�C�Y�𓾂�
	Stream->ReadShred.Seek( Stream->DataPoint, 0, STREAM_SEEKTYPE_END ) ;
	FileSize = ( int )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, STREAM_SEEKTYPE_SET ) ;

	// ID3v2 �^�O������ꍇ�̃X�L�b�v�������s��
	{
		char Tag[4] ;
		BYTE MainVer, SubVer, Flag, SizeData[4] ;

		Tag[3] = '\0' ;
		Stream->ReadShred.Read( Tag, 1, 3, Stream->DataPoint ) ;
		if( _STRCMP( Tag, "ID3" ) == 0 )
		{
			// �o�[�W�����𓾂�
			Stream->ReadShred.Read( &MainVer, 1, 1, Stream->DataPoint ) ;
			Stream->ReadShred.Read( &SubVer, 1, 1, Stream->DataPoint ) ;

			// �t���O�𓾂�
			Stream->ReadShred.Read( &Flag, 1, 1, Stream->DataPoint ) ;

			// �e�ʏ��𓾂�
			Stream->ReadShred.Read( SizeData, 1, 4, Stream->DataPoint ) ;
			SkipSize = ( DWORD )( ( SizeData[0] << 21 ) + ( SizeData[1] << 14 ) + ( SizeData[2] << 7 ) + SizeData[3] + 10 ) ;

			// �t�b�^��񂪂���ꍇ�͍X�ɂP�O�o�C�g�؂�
			if( MainVer == 4 && ( Flag & 0x10 ) != 0 ) SkipSize += 10 ;
		}
		else
		{
			SkipSize = 0 ;
		}
	}

	// �t�@�C���̏I�[�𒲂ׂ�B��� ID3v1 �^�O�����邩�ǂ����ŕω�����
	if( FileSize > 128 )
	{
		char Tag[4] ;

		Tag[3] = '\0' ;
		Stream->ReadShred.Seek( Stream->DataPoint, -128, STREAM_SEEKTYPE_END ) ;
		Stream->ReadShred.Read( Tag, 1, 3, Stream->DataPoint ) ;

		if( _STRCMP( Tag, "TAG" ) == 0 )
		{
			EndSkipSize = 128 ;
		}
		else
		{
			EndSkipSize = 0 ;
		}
	}

	// �f�[�^�̖��[����������
	{
		// �V�[�N����
		Stream->ReadShred.Seek( Stream->DataPoint, SkipSize, STREAM_SEEKTYPE_SET ) ;

		StartPos = 0xffffffff ;

		i = 0;
		for(;;)
		{
			for(;;)
			{
				if( Stream->ReadShred.Read( &Header[0], 1, 1, Stream->DataPoint ) == 0 ) goto R1 ;
				if( Header[0] == 0xff )
				{
					if( Stream->ReadShred.Read( &Header[1], 1, 1, Stream->DataPoint ) == 0 ) goto R1 ;
					if( ( Header[1] & 0xf0 ) == 0xf0 )
					{
						break ;
					}
				}
			}
			
			// �c��̃w�b�_��ǂݍ���
			Stream->ReadShred.Read( &Header[2], 1, 2, Stream->DataPoint ) ;

			// �w�b�_�����
			if( AnalysisMP3_FrameHeader( Header, &MP3Frame ) < 0 ) return -1 ;

			// �����t���[���T�C�Y��0��������G���[
			if( MP3Frame.FrameSize <= 0 ) return -1 ;

			// �����ŏ��̃t���[����������ʒu��ۑ����Ă���
			if( StartPos == 0xffffffff )
			{
				StartPos = ( DWORD )Stream->ReadShred.Tell( Stream->DataPoint ) - 4 ;
			}
/*
			// �l�o�R�f�[�^�����ׂ�
			if( Header[0] != 0xff || ( Header[1] & 0xf0 ) != 0xf0 ) continue ;

			// �l�o�d�f�̃o�[�W�����𓾂�(0:Ver1 1:Ver2)
			MpegV = ( Header[1] & 0x8 ) == 0x8 ? 0 : 1 ;

			// ���C���𓾂�(0:Layer-1 1:Layer-2 2:Layer-3)
			Layer = 3 - ( ( Header[1] >> 1 ) & 0x3 ) ;
			if( Layer > 2 ) return -1 ;

			// �r�b�g���[�g�̃C���f�b�N�X�l�𓾂�
			BitRateIndex = ( Header[2] >> 4 ) & 0xf ;
			if( BitRateIndex > 14 ) return -1 ;

			// �r�b�g���[�g�𓾂�
			BitRate = MP3BitRateTable[MpegV][Layer][BitRateIndex-1] ;

			// �T���v�����O���g���̃C���f�b�N�X�l�𓾂�
			SampleRateIndex = ( Header[2] >> 2 ) & 0x3 ;
			if( SampleRateIndex > 2 ) return -1 ;

			// �T���v�����O���g���𓾂�
			SampleRate = MP3FreqTable[MpegV][SampleRateIndex] ;

			// �p�f�B���O�r�b�g�𓾂�
			PaddingBit = ( Header[2] >> 1 ) & 1 ;

			// �`�����l�����[�h����`�����l�����𓾂�
			ChannelNum = ( ( Header[3] >> 6 ) & 0x3 ) == 3 ? 1 : 2 ;
			
			// �����ŏ��̃t���[����������ʒu��ۑ����Ă���
			if( StartPos == 0xffffffff )
			{
				StartPos = Stream->ReadShred.Tell( Stream->DataPoint ) - 4 ;
			}

			// �t���[���T�C�Y���v�Z����
			switch( MpegV )
			{
			case 0 : // Mapg-1
				switch( Layer )
				{
				case 0 : FrameSize = ( ( 12000 * BitRate / SampleRate ) + PaddingBit ) * 4 ; break ;		// Layer-1
				case 1 :
				case 2 : FrameSize = ( 144000 * BitRate / SampleRate ) + PaddingBit ; break ;				// Layer-2 Layer-3
				}
				break ;

			case 1 : // Mapg-2
				switch( Layer )
				{
				case 0 : FrameSize = ( ( 12000 * BitRate / SampleRate ) + PaddingBit ) * 4 ; break ;		// Layer-1
				case 1 :
				case 2 : FrameSize = ( 72000 * BitRate / SampleRate ) + PaddingBit ; break ;				// Layer-2 Layer-3
				}
				break ;
			}
//			FrameSize += 4 ;
*/
			// ���T���v�����𑝂₷
			acmdata->DestDataSampleNum += MP3Frame.SampleNum ;

			// �f�[�^�̖��[�A�h���X���擾
			{
				LastPoint = ( DWORD )( Stream->ReadShred.Tell( Stream->DataPoint ) /*- 8*/ + MP3Frame.FrameSize - 4 ) ;
				if( LastPoint >= FileSize - EndSkipSize )
				{
					if( LastPoint > FileSize - EndSkipSize ) LastPoint = FileSize - EndSkipSize;
					break ;
				}
				Stream->ReadShred.Seek( Stream->DataPoint, MP3Frame.FrameSize - 4 /*- 8*/, STREAM_SEEKTYPE_CUR ) ;
				i ++ ;
			}
		}
	}

R1 :
	// �t���[���f�[�^��������Ȃ�������G���[
	if( StartPos == 0xffffffff ) return -1 ;

	// �����ʒu���Z�b�g����
/*	Stream->ReadShred.Seek( Stream->DataPoint, SkipSize, STREAM_SEEKTYPE_SET ) ;

	// �t���[���f�[�^��T��
	{
		while( 1 )
		{
			if( Stream->ReadShred.Read( &Header[0], 1, 1, Stream->DataPoint ) == 0 ) return -1 ;
			if( Header[0] == 0xff )
			{
				if( Stream->ReadShred.Read( &Header[1], 1, 1, Stream->DataPoint ) == 0 ) return -1 ;
				if( ( Header[1] & 0xf0 ) == 0xf0 )
				{
					break ;
				}
			}
		}
	}

	// �c��̃w�b�_��ǂݍ���
	Stream->ReadShred.Read( &Header[2], 1, 2, Stream->DataPoint ) ;
*/
	// �ŏ��̃t���[���̈ʒu�ֈړ�
	Stream->ReadShred.Seek( Stream->DataPoint, StartPos, STREAM_SEEKTYPE_SET ) ;

	// �w�b�_��ǂݍ���
	Stream->ReadShred.Read( &Header[0], 1, 4, Stream->DataPoint ) ;

	// �w�b�_�����
	if( AnalysisMP3_FrameHeader( Header, &MP3Frame ) < 0 ) return -1 ;
/*
	// �l�o�R�f�[�^�����ׂ�
	if( Header[0] != 0xff || ( Header[1] & 0xf0 ) != 0xf0 ) return -1 ;

	// �l�o�d�f�̃o�[�W�����𓾂�(0:Ver1 1:Ver2)
	MpegV = ( Header[1] & 0x8 ) == 0x8 ? 0 : 1 ;

	// ���C���𓾂�(0:Layer-1 1:Layer-2 2:Layer-3)
	Layer = 3 - ( ( Header[1] >> 1 ) & 0x3 ) ;

	// �r�b�g���[�g�̃C���f�b�N�X�l�𓾂�
	BitRateIndex = ( Header[2] >> 4 ) & 0xf ;

	// �r�b�g���[�g�𓾂�
	BitRate = MP3BitRateTable[MpegV][Layer][BitRateIndex-1] ;

	// �T���v�����O���g���̃C���f�b�N�X�l�𓾂�
	SampleRateIndex = ( Header[2] >> 2 ) & 0x3 ;

	// �T���v�����O���g���𓾂�
	SampleRate = MP3FreqTable[MpegV][SampleRateIndex] ;

	// �p�f�B���O�r�b�g�𓾂�
	PaddingBit = ( Header[2] >> 1 ) & 1 ;

	// �`�����l�����[�h����`�����l�����𓾂�
	ChannelNum = ( ( Header[3] >> 6 ) & 0x3 ) == 3 ? 1 : 2 ;

	// �t���[���T�C�Y���v�Z����
	switch( MpegV )
	{
	case 0 : // Mapg-1
		switch( Layer )
		{
		case 0 : FrameSize = ( ( 12000 * BitRate / SampleRate ) + PaddingBit ) * 4 ; break ;		// Layer-1
		case 1 :
		case 2 : FrameSize = ( 144000 * BitRate / SampleRate ) + PaddingBit ; break ;				// Layer-2 Layer-3
		}
		break ;

	case 1 : // Mapg-2
		switch( Layer )
		{
		case 0 : FrameSize = ( ( 12000 * BitRate / SampleRate ) + PaddingBit ) * 4 ; break ;		// Layer-1
		case 1 :
		case 2 : FrameSize = ( 72000 * BitRate / SampleRate ) + PaddingBit ; break ;				// Layer-2 Layer-3
		}
		break ;
	}
//	FrameSize += 4 ;
*/
	// �w�b�_�̃Z�b�g
	{
		// �������̊m��
		int headerSize = sizeof( D_MPEGLAYER3WAVEFORMAT );
		GSoundConvertData.acmMetricsFunc( NULL, D_ACM_METRIC_MAX_SIZE_FORMAT, &headerSize );

		MP3Format = ( D_MPEGLAYER3WAVEFORMAT * )DXALLOC( ( size_t )headerSize ) ;
		if( MP3Format == NULL ) goto ERR ;

		// �l�o�R�v�`�u�d�t�H�[�}�b�g�̃f�[�^���Z�b�g����
		_MEMSET( MP3Format, 0, ( size_t )headerSize ) ;
		MP3Format->wfx.cbSize          = D_MPEGLAYER3_WFX_EXTRA_BYTES ;
		MP3Format->wfx.wFormatTag      = D_WAVE_FORMAT_MPEGLAYER3 ;
		MP3Format->wfx.nChannels       = MP3Frame.ChannelNum ;
		MP3Format->wfx.nSamplesPerSec  = MP3Frame.SampleRate ;
		MP3Format->wfx.nAvgBytesPerSec = MP3Frame.BitRate * 1000 / 8 ;
		MP3Format->wfx.nBlockAlign     = 1 ;
		MP3Format->wfx.wBitsPerSample  = 0 ;
		MP3Format->wID                 = D_MPEGLAYER3_ID_MPEG ;
		MP3Format->fdwFlags            = MP3Frame.PaddingBit/*0*//*MPEGLAYER3_FLAG_PADDING_OFF*/ ;
		MP3Format->nBlockSize          = ( WORD )MP3Frame.FrameSize ;
		MP3Format->nFramesPerBlock     = 1 ;
		MP3Format->nCodecDelay         = 700/*0x0571*/ ;
	}
/*
	// �f�[�^�̃X�^�[�g�A�h���X���擾
	StartPoint = Stream->ReadShred.Tell( Stream->DataPoint ) - 4 ;
*/

	// �L���f�[�^�T�C�Y���擾
	DataSize = ( int )( LastPoint - StartPos ) ;
//	DataSize = LastPoint - StartPoint ;
//	DataSize = FileSize - SkipSize - EndSkipSize ;

	// �t�@�C���|�C���^�̑���
	Stream->ReadShred.Seek( Stream->DataPoint, StartPos, STREAM_SEEKTYPE_SET ) ;
//	Stream->ReadShred.Seek( Stream->DataPoint, StartPoint, STREAM_SEEKTYPE_SET ) ;

	// ���̃Z�b�g
	acmdata->Format = ( WAVEFORMATEX * )MP3Format ;
//	SoundConv->DataPos  = StartPoint ;
	SoundConv->DataPos  = ( int )StartPos ;
	SoundConv->DataSize = DataSize ;

	// �ϊ���̐����t�H�[�}�b�g�𓾂�
	_MEMSET( &SoundConv->OutFormat, 0, sizeof( SoundConv->OutFormat ) ) ;
	SoundConv->OutFormat.wFormatTag = WAVE_FORMAT_PCM ;
	res = ( int )GSoundConvertData.acmFormatSuggestFunc( NULL, acmdata->Format, &SoundConv->OutFormat, sizeof( WAVEFORMATEX ), ACM_FORMATSUGGESTF_WFORMATTAG ) ;
	if( res != 0 ) goto ERR ;

	// ���k�f�[�^���ꎞ�I�ɕۑ����郁�����̈�̊m��
//	acmdata->SrcDataSize = AdjustSoundDataBlock( acmdata->Format->nAvgBytesPerSec * 8/* / STS_CACHEBUFFERSEC*/, acmdata->Format ) ;
	acmdata->SrcDataSize = SoundConv->DataSize ;  // ���[�v�����̊֌W�ŁA�ہX�f�[�^�����܂�̈���m�ۂ���
	acmdata->SrcData = DXALLOC( ( size_t )acmdata->SrcDataSize ) ;
	if( acmdata->SrcData == NULL ) goto ERR ;
//	acmdata->SrcDataValidSize = 0 ;

	// ���k�f�[�^���ꊇ�œǂݍ���
	Stream->ReadShred.Read( acmdata->SrcData, ( size_t )SoundConv->DataSize, 1, Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, StartPos, STREAM_SEEKTYPE_SET ) ;
//	acmdata->SrcDataValidSize = SoundConv->DataSize ;
	acmdata->SrcDataPosition = 0 ;

	// �`�b�l�n���h���̍쐬
	for( i = 0 ; i < 2 ; i ++ )
	{
		res = ( int )GSoundConvertData.acmStreamOpenFunc( &acmdata->AcmStreamHandle[i],
								NULL,
								acmdata->Format,
								&SoundConv->OutFormat,
								NULL,
								0,
								0,
								ACM_STREAMOPENF_NONREALTIME ) ;
		if( res != 0 ) goto ERR ;
	}

	// �ϊ���̂o�b�l�f�[�^���ꎞ�I�ɕۑ����郁�����̈�̃T�C�Y���Z�b�g(�P�b�����������傫�߂ɂƂ��Ă���)
	SoundConv->DestDataSize = ( int )( SoundConv->OutFormat.nAvgBytesPerSec * 4 / 3 ) ;
	
	// �V�[�N�\��ʒu�܂ŕϊ����Ă��������Ŏg�p���郁�����̈�̊m��
	acmdata->BeginSeekBuffer = DXALLOC( ( size_t )SoundConv->DestDataSize ) ;
	if( acmdata->BeginSeekBuffer == NULL ) goto ERR ;

	// �^�C�v�Z�b�g
	SoundConv->MethodType = SOUND_METHODTYPE_ACM ;

	// �I��
	return 0 ;
	
ERR :
	if( acmdata->AcmStreamHandle[0] != NULL )
	{
		GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
		acmdata->AcmStreamHandle[0] = NULL ;
	}

	if( acmdata->AcmStreamHandle[1] != NULL )
	{
		GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
		acmdata->AcmStreamHandle[1] = NULL ;
	}

	if( acmdata->BeginSeekBuffer != NULL )
	{
		DXFREE( acmdata->BeginSeekBuffer ) ;
		acmdata->BeginSeekBuffer = NULL ;
	}

	if( acmdata->Format != NULL )
	{
		DXFREE( acmdata->Format ) ;
		acmdata->Format = NULL ;
	}

	if( acmdata->SrcData != NULL )
	{
		DXFREE( acmdata->SrcData ) ;
		acmdata->SrcData = NULL ;
	}
	
	return -1 ;
}

#endif

// �v�`�u�d�t�@�C���̌�n���������s��
static	int TerminateSoundConvert_WAVE( SOUNDCONV *SoundConv )
{
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	// ���������
	if( Wave->SrcBuffer )
	{
		DXFREE( Wave->SrcBuffer ) ;
		Wave->SrcBuffer = NULL ;
	}

	// �I��
	return 0 ;
}

#ifndef DX_NON_ACM

// �`�b�l���g�p�����t�@�C���̌�n���������s��
static	int TerminateSoundConvert_ACM( SOUNDCONV *SoundConv )
{
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;

	if( acmdata->Format != NULL )
	{
		DXFREE( acmdata->Format ) ;
		acmdata->Format = NULL ;
	}

	if( acmdata->BeginSeekBuffer != NULL )
	{
		DXFREE( acmdata->BeginSeekBuffer ) ;
		acmdata->BeginSeekBuffer = NULL ;
	}
	
	if( acmdata->AcmStreamHandle[0] != NULL )
	{
		GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
		acmdata->AcmStreamHandle[0] = NULL ;
	}

	if( acmdata->AcmStreamHandle[1] != NULL )
	{
		GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
		acmdata->AcmStreamHandle[1] = NULL ;
	}
	
	if( acmdata->SrcData != NULL )
	{
		DXFREE( acmdata->SrcData ) ;
		acmdata->SrcData = NULL ;
	}

	return 0 ;
}

#endif

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// MP3���g�p�����t�@�C���̌�n���������s��
static int TerminateSoundConvert_DSMP3( SOUNDCONV *SoundConv )
{
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;

	if( dsmp3->PCMBuffer )
	{
		DXFREE( dsmp3->PCMBuffer ) ;
		dsmp3->PCMBuffer = NULL ;
	}

	return 0 ;
}

#endif
#endif
#endif

// �ϊ���̃o�b�t�@�Ƀf�[�^���[����
static	int ConvertProcessSoundConvert_WAVE( SOUNDCONV *SoundConv )
{
	int readsize, pos ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	switch( Wave->SrcFormat.wFormatTag )
	{
	case WAVE_FORMAT_PCM :
		pos = ( int )( Stream->ReadShred.Tell( Stream->DataPoint ) - SoundConv->DataPos ) ;
		if( pos == SoundConv->DataSize ) return -1 ;

		// �ǂݍ��ރf�[�^�T�C�Y�����肷��
		readsize = SoundConv->DataSize - pos ;
		if( SoundConv->DestDataSize < readsize ) readsize = SoundConv->DestDataSize ;

		// �ǂݍ���
		Stream->ReadShred.Read( SoundConv->DestData,
								( size_t )readsize, 1, Stream->DataPoint ) ;
		SoundConv->DestDataValidSize = readsize ;
		break ;

	case WAVE_FORMAT_IEEE_FLOAT :
		{
			short *Dest ;
			float *Src ;
			DWORD i ;
			DWORD SampleNum ;
			int j ;
			int DestI ;

			pos = ( int )( Stream->ReadShred.Tell( Stream->DataPoint ) - SoundConv->DataPos ) ;
			if( pos == SoundConv->DataSize ) return -1 ;

			// �ǂݍ��ރf�[�^�T�C�Y�����肷��
			readsize = SoundConv->DataSize - pos ;
			if( Wave->SrcBufferSize < ( DWORD )readsize ) readsize = ( int )Wave->SrcBufferSize ;

			// �ǂݍ���
			Stream->ReadShred.Read( Wave->SrcBuffer,
									( size_t )readsize, 1, Stream->DataPoint ) ;

			// �����^�ɕϊ�
			Dest = ( short * )SoundConv->DestData ;
			Src = ( float * )Wave->SrcBuffer ;
			SampleNum = ( DWORD )( readsize / Wave->SrcFormat.nBlockAlign ) ;
			for( i = 0 ; i < SampleNum ; i ++ )
			{
				for( j = 0 ; j < Wave->SrcFormat.nChannels ; j ++, Dest ++, Src ++ )
				{
					DestI = _FTOL( *Src * 32768.0f ) ;
					if( DestI < -32768 )
					{
						*Dest = -32768 ;
					}
					else
					if( DestI > 32767 )
					{
						*Dest = 32767 ;
					}
					else
					{
						*Dest = ( short )DestI ;
					}
				}
			}
			SoundConv->DestDataValidSize = ( int )( SampleNum * SoundConv->OutFormat.nBlockAlign ) ;
		}
		break ;
	}

	return 0 ;
}

#ifndef DX_NON_ACM

// �ϊ���̃o�b�t�@�Ƀf�[�^���[����
static	int ConvertProcessSoundConvert_ACM( SOUNDCONV *SoundConv )
{
	D_ACMSTREAMHEADER header ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
//	STREAMDATA *Stream = &SoundConv->Stream ;
	DWORD ConvSize = 0 ;

	// �V�[�N�\��ʒu�ƃT�uACM�n���h���ŕϊ����������Ă���ʒu���Ⴄ�ꍇ�͏������s��
	if( SoundConv->SeekLockPosition != acmdata->BeginSeekPosition )
	{
		// �ǂ��z���Ă��܂��Ă���ꍇ�͍ŏ������蒼��
		if( acmdata->BeginSeekPosition > SoundConv->SeekLockPosition )
		{
			GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
			GSoundConvertData.acmStreamOpenFunc( &acmdata->AcmStreamHandle[1], NULL,
							acmdata->Format, &SoundConv->OutFormat,
							NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ;
			acmdata->BeginSeekPosition = 0 ;
			acmdata->BeginSeekCompSrcSize = 0 ;
		}

		if( SoundConv->SeekLockPosition - 1024 * 10 > acmdata->BeginSeekPosition )
		{
			// �ϊ��ς݂̈ʒu��i�߂�
			_MEMSET( &header, 0, sizeof( D_ACMSTREAMHEADER ) ) ;
			header.cbStruct		= sizeof( D_ACMSTREAMHEADER ) ;
			header.pbSrc		= (BYTE *)acmdata->SrcData + acmdata->BeginSeekCompSrcSize ;
			if( acmdata->SrcDataSize - acmdata->BeginSeekCompSrcSize > 2048 ) header.cbSrcLength = 2048 ;
			else 	                                                          header.cbSrcLength = ( DWORD )( acmdata->SrcDataSize - acmdata->BeginSeekCompSrcSize ) ;
			header.pbDst		= (BYTE *)acmdata->BeginSeekBuffer ;
			header.cbDstLength	= ( DWORD )SoundConv->DestDataSize ;
			if( (signed)( header.cbDstLength + acmdata->BeginSeekPosition ) > SoundConv->SeekLockPosition ) 
				header.cbDstLength = ( DWORD )( SoundConv->SeekLockPosition - acmdata->BeginSeekPosition ) ;

			if( GSoundConvertData.acmStreamPrepareHeaderFunc( acmdata->AcmStreamHandle[1], &header, 0 ) != 0 )
				return -1 ;

			// �W�J
			GSoundConvertData.acmStreamConvertFunc( acmdata->AcmStreamHandle[1], &header, D_ACM_STREAMCONVERTF_BLOCKALIGN ) ;

			// �W�J�������������������W��i�߂�
			acmdata->BeginSeekPosition += header.cbDstLengthUsed ;
			acmdata->BeginSeekCompSrcSize += header.cbSrcLengthUsed ;

			// �w�b�_�̉���
			GSoundConvertData.acmStreamUnprepareHeaderFunc( acmdata->AcmStreamHandle[1], &header, 0 ) ;
		}
	}

	// �I�[�܂ŗ��Ă�����I��
	if( acmdata->SrcDataPosition == acmdata->SrcDataSize )
		return -1 ;

	// �ϊ����̃f�[�^������������X�g���[������ǂݍ��݂�
//(���[�v�����̊֌W�Ńf�[�^���ہX�ǂݍ���ł��邽�߂��̏����͕K�v�Ȃ��Ȃ���)
/*	if( acmdata->SrcDataValidSize < acmdata->SrcDataSize )
	{
		int pos, readsize ;

		pos = Stream->ReadShred.Tell( Stream->DataPoint ) - SoundConv->DataPos ; 
		if( pos == SoundConv->DataSize && acmdata->SrcDataValidSize == 0 )
			return -1 ;

		readsize = SoundConv->DataSize - pos ;
		if( readsize != 0 )
		{
			if( readsize > acmdata->SrcDataSize - acmdata->SrcDataValidSize )
				readsize = acmdata->SrcDataSize - acmdata->SrcDataValidSize ;

			Stream->ReadShred.Read( (BYTE *)acmdata->SrcData + acmdata->SrcDataValidSize,
									readsize, 1, Stream->DataPoint ) ;
			acmdata->SrcDataValidSize += readsize ;
		}
	}
*/

	// �W�J�̏���
	_MEMSET( &header, 0, sizeof( D_ACMSTREAMHEADER ) ) ;
	header.cbStruct		= sizeof( D_ACMSTREAMHEADER ) ;
//	header.pbSrc		= (BYTE *)acmdata->SrcData ;
//	header.cbSrcLength	= acmdata->SrcDataValidSize ;
	header.pbSrc		= (BYTE *)acmdata->SrcData + acmdata->SrcDataPosition ;
	if( acmdata->SrcDataSize - acmdata->SrcDataPosition > 2048 ) header.cbSrcLength = 2048 ;
	else 	                                                     header.cbSrcLength = ( DWORD )( acmdata->SrcDataSize - acmdata->SrcDataPosition ) ;
	header.pbDst		= (BYTE *)SoundConv->DestData ;
	header.cbDstLength	= ( DWORD )SoundConv->DestDataSize ;


	if( GSoundConvertData.acmStreamPrepareHeaderFunc( acmdata->AcmStreamHandle[0], &header, 0 ) != 0 )
		return -1 ;

	// �W�J
	GSoundConvertData.acmStreamConvertFunc( acmdata->AcmStreamHandle[0], &header, D_ACM_STREAMCONVERTF_BLOCKALIGN ) ;

	// �f�[�^�̃Z�b�g
	ConvSize = header.cbDstLengthUsed ;
/*	if( acmdata->DestDataSampleNum != -1 )
	{
		if( ( SoundConv->DestDataCompSize + ConvSize ) / SoundConv->OutFormat.nBlockAlign >= acmdata->DestDataSampleNum )
		{
			ConvSize = ( acmdata->DestDataSampleNum - SoundConv->DestDataCompSize / SoundConv->OutFormat.nBlockAlign ) * SoundConv->OutFormat.nBlockAlign ;
		}
	}
*/
	SoundConv->DestDataValidSize = ( int )header.cbDstLengthUsed ;
	acmdata->SrcDataPosition += header.cbSrcLengthUsed ;
//	acmdata->SrcDataValidSize -= header.cbSrcLengthUsed ;

	// �w�b�_�̉���
	GSoundConvertData.acmStreamUnprepareHeaderFunc( acmdata->AcmStreamHandle[0], &header, 0 ) ;

	// �g�p���ꂽ�f�[�^�̃T�C�Y���O�ŁA���R���o�[�g�T�C�Y���O�������特�����I�����Ă���
	// �\��������̂ŁA���̏ꍇ�̓R���o�[�g�I��
	if( ConvSize == 0 )
	{
		SoundConv->DestDataValidSize = 0 ;
		acmdata->SrcDataPosition = acmdata->SrcDataSize ;
//		acmdata->SrcDataValidSize = 0 ;
		return -1 ;
	}

	// �c�����������X���C�h������
//(���[�v�����̊֌W�Ńf�[�^���ہX�ǂݍ���ł��邽�߂��̏����͕K�v�Ȃ��Ȃ���)
/*	if( acmdata->SrcDataValidSize != 0 )
		_MEMMOVE( acmdata->SrcData,
				 (BYTE *)acmdata->SrcData + header.cbSrcLengthUsed,
				 acmdata->SrcDataValidSize ) ;
*/

	// �I��
	return 0 ;
}

#endif

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// �ϊ���̃o�b�t�@�Ƀf�[�^���[����
static int ConvertProcessSoundConvert_DSMP3( SOUNDCONV *SoundConv )
{
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;
	int readsize ;

	// �S�ē]���������Ă�����I��
	if( dsmp3->PCMValidDataSize == dsmp3->PCMDestCopySize ) return -1 ;
	
	// �]������f�[�^�T�C�Y�����肷��
	readsize = ( int )( dsmp3->PCMValidDataSize - dsmp3->PCMDestCopySize ) ;
	if( SoundConv->DestDataSize < readsize ) readsize = SoundConv->DestDataSize ;
	
	// �]������
	_MEMCPY( SoundConv->DestData, ( BYTE * )dsmp3->PCMBuffer + dsmp3->PCMDestCopySize, ( size_t )readsize ) ;
	SoundConv->DestDataValidSize = readsize ;

	// �]���������I�t�Z�b�g��i�߂�
	dsmp3->PCMDestCopySize += readsize ;

	return 0 ;
}

#endif
#endif
#endif

// �ϊ������̈ʒu��ύX����( �T���v���P�� )
static int SetSampleTimeSoundConvert_WAVE( SOUNDCONV *SoundConv, int SampleTime )
{
	int BytePos ;
	STREAMDATA *Stream = &SoundConv->Stream ;
	
	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign + SoundConv->DataPos ;
	Stream->ReadShred.Seek( Stream->DataPoint, BytePos, SEEK_SET ) ;

	SoundConv->DestDataValidSize = 0 ;
	SoundConv->DestDataCompSize = 0 ;
	SoundConv->DestDataCompSizeAll = BytePos ;

	return 0 ;
}

#ifndef DX_NON_ACM

// �ϊ������̈ʒu��ύX����( �T���v���P�� )
static int SetSampleTimeSoundConvert_ACM( SOUNDCONV *SoundConv, int SampleTime )
{
	int BytePos, CurPos, res, DelByte ;
	STREAMDATA *Stream = &SoundConv->Stream ;
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;

	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign ;
	CurPos = SoundConv->DestDataCompSizeAll ;
	
	// ���x���݉𓀂��Ă��镔���̎w�肾�����ꍇ�͏����𕪊�
	if( BytePos >= CurPos && BytePos <= CurPos + SoundConv->DestDataValidSize )
	{
		DelByte = BytePos - CurPos ;
		SoundConv->DestDataCompSize += DelByte ;
		SoundConv->DestDataCompSizeAll += DelByte ;
		SoundConv->DestDataValidSize -= DelByte ;
		
		// �I��
		return 0 ;
	}
	
	// �����ϊ��������ς�ł��܂��Ă���n�_�ɖ߂�ꍇ�͂`�b�l�n���h�����쐬���Ȃ����A
	// �X�g���[���|�C���g���f�[�^�̍ŏ��ɖ߂�
	if( BytePos < CurPos )
	{
		// ������̂`�b�l�n���h�����w��̈ʒu�Ɠ������A������ʒu�I�Ɏ�O�ł���ꍇ�͂�������g�p����
		if( BytePos >= acmdata->BeginSeekPosition )
		{
			D_HACMSTREAM temp ;

			// �`�b�l�n���h���̌���
			temp = acmdata->AcmStreamHandle[0] ;
			acmdata->AcmStreamHandle[0] = acmdata->AcmStreamHandle[1] ;
			acmdata->AcmStreamHandle[1] = temp ;

			// ������
			acmdata->SrcDataPosition = acmdata->BeginSeekCompSrcSize ;
			SoundConv->DestDataCompSize = acmdata->BeginSeekPosition ;
			SoundConv->DestDataCompSizeAll = acmdata->BeginSeekPosition ;
			SoundConv->DestDataValidSize = 0 ;

			// ���܂Ń��C���������`�b�l�n���h���͏�����
			GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[1], 0 ) ;
			GSoundConvertData.acmStreamOpenFunc( &acmdata->AcmStreamHandle[1], NULL,
							acmdata->Format, &SoundConv->OutFormat,
							NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ;
			acmdata->BeginSeekCompSrcSize = 0 ;
			acmdata->BeginSeekPosition = 0 ;
		}
		else
		{
			// �T�u���ʖڂȏꍇ�̓��C������蒼���Ĉꂩ��ǂ������Ȃ�
			GSoundConvertData.acmStreamCloseFunc( acmdata->AcmStreamHandle[0], 0 ) ;
			GSoundConvertData.acmStreamOpenFunc( &acmdata->AcmStreamHandle[0], NULL,
							acmdata->Format, &SoundConv->OutFormat,
							NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME ) ;
			Stream->ReadShred.Seek( Stream->DataPoint, SoundConv->DataPos, SEEK_SET ) ;
			acmdata->SrcDataPosition = 0 ;
//			acmdata->SrcDataValidSize = 0 ;

			SoundConv->DestDataValidSize = 0 ;
			SoundConv->DestDataCompSize = 0 ;
			SoundConv->DestDataCompSizeAll = 0 ;
		}
	}
	
	// �w��̈ʒu�܂ŉ𓀏�����i�߂�
	res = 0 ;
	while( SoundConv->DestDataCompSizeAll + SoundConv->DestDataValidSize < BytePos )
	{
		SoundConv->DestDataCompSizeAll += SoundConv->DestDataValidSize ;
		SoundConv->DestDataValidSize = 0 ;
		SoundConv->DestDataCompSize = 0 ;
		res = ConvertProcessSoundConvert_ACM( SoundConv ) ;
		if( res == -1 ) break ;
	}
	
	// �I�[�ɓ��B���Ă��܂�����ϊ��I����Ԃɂ���
	if( res == -1 )
	{
		SoundConv->EndFlag = TRUE ;
		SoundConv->DestDataValidSize = 0 ;
		return 0 ;
	}
	
	DelByte = BytePos - SoundConv->DestDataCompSizeAll ;
	SoundConv->DestDataValidSize -= DelByte ;
	SoundConv->DestDataCompSize += DelByte ;
	SoundConv->DestDataCompSizeAll += DelByte ;

	// �I��
	return 0 ;
}

#endif

#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// �ϊ������̈ʒu��ύX����( �T���v���P�� )
static	int SetSampleTimeSoundConvert_DSMP3( SOUNDCONV *SoundConv, int SampleTime )
{
	int BytePos ;
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;
	
	BytePos = SampleTime * SoundConv->OutFormat.nBlockAlign ;

	SoundConv->DestDataValidSize   = 0 ;
	SoundConv->DestDataCompSize    = 0 ;
	SoundConv->DestDataCompSizeAll = BytePos ;
	dsmp3->PCMDestCopySize = ( DWORD )BytePos ;

	return 0 ;
}

#endif
#endif
#endif


// �ϊ���̑�}�̃f�[�^�T�C�Y�𓾂�
static	int GetSoundConvertDestSize_Fast_WAVE( SOUNDCONV *SoundConv )
{
	SOUNDCONV_WAVE *Wave = ( SOUNDCONV_WAVE * )SoundConv->ConvFunctionBuffer ;

	switch( Wave->SrcFormat.wFormatTag )
	{
	case WAVE_FORMAT_PCM :
		return SoundConv->DataSize ;

	case WAVE_FORMAT_IEEE_FLOAT :
		return SoundConv->DataSize / ( Wave->SrcFormat.wBitsPerSample / SoundConv->OutFormat.wBitsPerSample ) ;
	}

	return -1 ;
}

#ifndef DX_NON_ACM

// �ϊ���̑�}�̃f�[�^�T�C�Y�𓾂�
static	int GetSoundConvertDestSize_Fast_ACM( SOUNDCONV *SoundConv )
{
//	SOUNDCONV_ACM *acmdata = &SoundConv->AcmTypeData ;
	SOUNDCONV_ACM *acmdata = (SOUNDCONV_ACM *)SoundConv->ConvFunctionBuffer ;
	DWORD size ;
	
	if( acmdata->DestDataSampleNum != -1 )
	{
		size = (DWORD)( acmdata->DestDataSampleNum * SoundConv->OutFormat.nBlockAlign ) ;
	}
	else
	{
		GSoundConvertData.acmStreamSizeFunc( acmdata->AcmStreamHandle[0], ( DWORD )SoundConv->DataSize, &size, D_ACM_STREAMSIZEF_SOURCE ) ;
	}
	return ( int )size ;
}

#endif


#ifndef DX_NON_MOVIE
#ifndef DX_NON_DSHOW_MOVIE
#ifndef DX_NON_DSHOW_MP3

// �ϊ���̑�}�̃f�[�^�T�C�Y�𓾂�
static int GetSoundConvertDestSize_Fast_DSMP3( SOUNDCONV *SoundConv )
{
	SOUNDCONV_DSMP3 *dsmp3 = ( SOUNDCONV_DSMP3 * )SoundConv->ConvFunctionBuffer ;
	return ( int )dsmp3->PCMValidDataSize ;
}

#endif
#endif
#endif



#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

