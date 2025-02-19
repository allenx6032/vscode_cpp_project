// ----------------------------------------------------------------------------
//
//		�c�w���C�u�����@���[�r�[�Đ������p�v���O����
//
//				Ver 3.13f
//
// ----------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

#include "DxMovie.h"

#ifndef DX_NON_MOVIE

// �C���N���[�h----------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxFile.h"
#include "DxBaseFunc.h"
#include "DxMemory.h"
#include "DxSound.h"
#include "DxUseCLib.h"
#include "DxLog.h"
#include "DxSystem.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGuid.h"
#include "DxFileWin.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����`------------------------------------------------------------------

// ���[�r�[���X�V����Ԋu
#define MOVIEREFRESHWAIT		(17)

// ���[�r�[�n���h���̗L�����`�F�b�N
#define MOVIEHCHK( HAND, MPOINT )		HANDLECHK(       DX_HANDLETYPE_MOVIE, HAND, *( ( HANDLEINFO ** )&MPOINT ) )
#define MOVIEHCHK_ASYNC( HAND, MPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_MOVIE, HAND, *( ( HANDLEINFO ** )&MPOINT ) )

#define VFW_E_NOT_FOUND                  ((HRESULT)0x80040216L)

#define MOVIE MovieData

// �\���̌^�錾----------------------------------------------------------------

// �f�[�^�錾------------------------------------------------------------------

MOVIEGRAPHMANAGE MovieData ;								// ����֘A�f�[�^

// �N���X�錾 -----------------------------------------------------------------

// �֐��v���g�^�C�v�錾--------------------------------------------------------

#ifndef DX_NON_DSHOW_MOVIE
// ���[�r�[�t�@�C�����I�[�v������  
extern int OpenMovieFunction(
	MOVIEGRAPH * Movie,
	const wchar_t *FileName,
	int *Width,
	int *Height,
	int SurfaceMode,
	int ASyncThread
) ;
#endif

// �v���O�����R�[�h------------------------------------------------------------

// ���[�r�[�֘A�̊Ǘ������̏�����
extern int InitializeMovieManage( void )
{
	if( MOVIE.InitializeFlag == TRUE )
		return -1 ;

	// �n���h���Ǘ����̏�����
	InitializeHandleManage( DX_HANDLETYPE_MOVIE, sizeof( MOVIEGRAPH ), MAX_MOVIE_NUM, InitializeMovieHandle, TerminateMovieHandle, L"Movie" ) ;

	// �������t���O���Ă�
	MOVIE.InitializeFlag = TRUE ;

	// �I��
	return 0 ;
}

// ���[�r�[�֘A�̊Ǘ������̌�n��
extern int TerminateMovieManage( void )
{
	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	// �n���h���Ǘ����̌�n��
	TerminateHandleManage( DX_HANDLETYPE_MOVIE ) ;

	// �������t���O�|��
	MOVIE.InitializeFlag = FALSE ;

	// �I��
	return 0 ;
}

#ifndef DX_NON_DSHOW_MOVIE

// OPENMOVIE_GPARAM �̃f�[�^���Z�b�g����
extern void Graphics_Image_InitOpenMovieGParam( OPENMOVIE_GPARAM *GParam )
{
	GParam->RightAlphaFlag = MOVIE.RightAlphaFlag ;
	GParam->A8R8G8B8Flag = MOVIE.A8R8G8B8Flag ;
	GParam->NotUseYUVFormatSurfaceFlag = MOVIE.NotUseYUVFormatSurfaceFlag ;
}

// ���[�r�[�t�@�C�����I�[�v������  
extern int OpenMovieFunction(
	MOVIEGRAPH * Movie,
	const wchar_t *FileName,
	int *Width,
	int *Height,
	int SurfaceMode,
	int ASyncThread
)
{
	wchar_t ErStr[256] ;
	D_IAMMultiMediaStream *pAMStream = NULL;
	HRESULT hr ;

	Movie->NowImage.GraphData = NULL;
	Movie->YUVFlag = FALSE ;
	Movie->OverlayDestX = 0 ;
	Movie->OverlayDestY = 0 ;
	Movie->OverlayDestExRate = 0 ;
	Movie->OverlayDispFlag = 0 ;
	Movie->FirstUpdateFlag = FALSE ;
	_MEMSET( &Movie->OverlaySrcRect, 0, sizeof( RECT ) ) ;
	_MEMSET( &Movie->OverlayDestRect, 0, sizeof( RECT ) ) ;

	// �����I�[�o�[���C���g���Ȃ��ꍇ�̓t���J���[�ɂ���
	if( SurfaceMode == DX_MOVIESURFACE_OVERLAY  )
	{
		SurfaceMode = DX_MOVIESURFACE_FULLCOLOR ;
	}

	// ������ʂ��R�Q�r�b�g�J���[���[�h�Ńt���J���[���[�h���w�肵�Ă����ꍇ�̓m�[�}���ɂ���
	if( SurfaceMode == DX_MOVIESURFACE_FULLCOLOR && NS_GetColorBitDepth() == 32 ) SurfaceMode = DX_MOVIESURFACE_NORMAL ;

	SurfaceMode = DX_MOVIESURFACE_FULLCOLOR ;

	// �O���t�B�b�N�r���_�[�I�u�W�F�N�g�̍쐬
	if( ( FAILED( WinAPI_CoCreateInstance_ASync(CLSID_FILTERGRAPH, NULL, CLSCTX_INPROC, IID_IGRAPHBUILDER, (void **)&Movie->pGraph, ASyncThread ) ) ) )
	{
		_WCSCPY( ErStr, L"CoCreateInstance Error : CLSID_FilterGraph\n" ) ;
		goto ERROR_R ;
	}

	// Create the Texture Renderer object
	Movie->pMovieImage = New_D_CMovieRender( NULL, &hr ) ;
    
    // Get a pointer to the IBaseFilter on the TextureRenderer, add it to graph
	hr = Movie->pGraph->AddFilter( Movie->pMovieImage, L"MovieRenderer" ) ;
    if( FAILED( hr ) )
    {
        _WCSCPY( ErStr, L"Could not add renderer filter to graph!\n" ) ;
        return hr;
    }

	// BasicAudio �C���^�[�t�F�C�X�𓾂�
	if( FAILED( Movie->pGraph->QueryInterface( IID_IBASICAUDIO, ( void ** )&Movie->pBasicAudio ) ) )
	{
		_WCSCPY( ErStr, L"QueryInterface Error : IID_IBasicAudio\n" ) ;
		goto ERROR_R ;
	}

	// ���f�B�A�R���g���[���I�u�W�F�N�g���擾����
	if( FAILED( Movie->pGraph->QueryInterface( IID_IMEDIACONTROL, ( void ** )&Movie->pMediaControl ) ) )
	{
		_WCSCPY( ErStr, L"QueryInterface Error : IID_IMediaControl\n" ) ;
		goto ERROR_R ;
	}

	// ���f�B�A�V�[�L���O�I�u�W�F�N�g���擾����
	if( FAILED( Movie->pGraph->QueryInterface( IID_IMEDIASEEKING, ( void ** )&Movie->pMediaSeeking ) ) )
	{
		_WCSCPY( ErStr, L"QueryInterface Error : IID_IMediaSeeking\n" ) ;
		goto ERROR_R ;
	}

   	// �t�@�C�����ۑ�
	_WCSCPY( Movie->FileName, FileName ) ;

	// �f�B���N�g���̋�؂肪 / �ɂȂ��Ă���ӏ��� \ �ɒu��������
	{
		wchar_t *wp ;

		for( wp = Movie->FileName; *wp; wp++ )
		{
			if( *wp == L'/' )
			{
				*wp = '\\' ;
			}
		}
	}

	hr = Movie->pGraph->RenderFile( Movie->FileName, NULL ) ;
	if( FAILED( hr ) )
	{
		_WCSCPY( ErStr, L"RenderFile faired!\n" ) ;

		DWORD_PTR fp ;
		HANDLE FileHandle ;
		void *TempBuffer ;
		size_t CompSize, MoveSize ;
		DWORD WriteSize ;
		size_t FileSize ;
		const DWORD BufferSize = 0x100000 ;

		_WCSCPY( ErStr, L"Movie File Open Error : " ) ;
		_WCSCAT( ErStr, FileName ) ;
		_WCSCAT( ErStr, L"\n" ) ;

		// ���Ƀe���|�����t�@�C�����쐬���Ă���ꍇ�́A
		// �X�Ƀe���|�����t�@�C�����쐬���邱�Ƃ͂��Ȃ�
		if( Movie->UseTemporaryFile == TRUE )
			goto ERROR_R ;

		// �t�@�C�����J���Ȃ�������A�[�J�C�u����Ă���\��������
#ifdef UNICODE
		fp = FOPEN( Movie->FileName ) ;
#else
		fp = FOPEN( FileName ) ;
#endif
		if( fp == 0 ) goto ERROR_R ;
		
		// �J�����ꍇ�̓e���|�����t�@�C���ɏ����o��
		{
			// �t�@�C���T�C�Y�̎擾
			FSEEK( fp, 0L, SEEK_END ) ;
			FileSize = ( size_t )FTELL( fp ) ;
			FSEEK( fp, 0L, SEEK_SET ) ;

			// �ꎞ�I�Ƀf�[�^���i�[����o�b�t�@���m��
			TempBuffer = DXALLOC( BufferSize ) ;
			if( TempBuffer == NULL )
			{
				FCLOSE( fp ) ;
				goto ERROR_R ;
			}

			// �e���|�����t�@�C���̍쐬
			FileHandle = CreateTemporaryFile( Movie->FileName ) ;

			if( FileHandle == NULL )
			{
				FCLOSE( fp ) ;
				DXFREE( TempBuffer ) ;
				goto ERROR_R ;
			}
			Movie->UseTemporaryFile = TRUE ;

			// �e���|�����t�@�C���Ƀf�[�^�������o��
			CompSize = 0 ;
			while( CompSize < FileSize )
			{
				MoveSize = CompSize - FileSize ;
				if( MoveSize > BufferSize ) MoveSize = BufferSize ;

				FREAD( TempBuffer, MoveSize, 1, fp ) ;
				WriteFile( FileHandle, TempBuffer, ( DWORD )MoveSize, &WriteSize, NULL ) ;

				if( MoveSize != WriteSize ) break ;
				CompSize += MoveSize ;
			}

			// �t�@�C������A���������������
			FCLOSE( fp ) ;
			CloseHandle( FileHandle ) ;
			DXFREE( TempBuffer ) ;
		}

		// ���߂ăt�@�C�����J��
		hr = Movie->pGraph->RenderFile( Movie->FileName, NULL ) ;
		if( FAILED( hr ) )
		{
			// �e���|�����t�@�C�����폜
			DeleteFileW( Movie->FileName ) ;
			goto ERROR_R ;
		}
	}

	// �P�t���[��������̎��Ԃ𓾂�
	Movie->pMediaSeeking->GetDuration( &Movie->FrameTime ) ;
	if( Movie->FrameTime == 0 )
	{
		Movie->FrameTime = 10000000 / 60 ;
	}

	// �I�����Ԃ��擾����
	Movie->pMediaSeeking->GetStopPosition( &Movie->StopTime ) ;

	// �摜�C���[�W�̏����Z�b�g����
	Movie->NowImage.Width        = ( int )Movie->pMovieImage->Width ;
	Movie->NowImage.Height       = ( int )Movie->pMovieImage->Height ;
	Movie->NowImage.Pitch        = ( int )Movie->pMovieImage->Pitch ;
	Movie->NowImage.GraphData    = Movie->pMovieImage->ImageBuffer ;
	Movie->NowImageGraphOutAlloc = TRUE ;
	if( Movie->pMovieImage->ImageType == 0 )
	{
		NS_CreateFullColorData( &Movie->NowImage.ColorData ) ;
	}
	else
	if( Movie->pMovieImage->ImageType == 1 && Movie->A8R8G8B8Flag )
	{
		NS_CreateARGB8ColorData( &Movie->NowImage.ColorData ) ;
	}
	else
	{
		NS_CreateXRGB8ColorData( &Movie->NowImage.ColorData ) ;
	}
	Movie->UseNowImage = &Movie->NowImage ;

	if( Width  ) *Width  = ( int )Movie->pMovieImage->Width ;
	if( Height ) *Height = ( int )Movie->pMovieImage->Height ;

	// �I��
	return 0 ;


ERROR_R:

	// �e��b�n�l�I�u�W�F�N�g���I������
	if( pAMStream					){ pAMStream->Release()					; pAMStream = NULL ; }

	if( Movie->pFilter				){ Movie->pFilter->Release()			; Movie->pFilter = NULL ; }
	if( Movie->pAllocator			){ Movie->pAllocator->Release()			; Movie->pAllocator = NULL ; }

	if( Movie->pGraph				){ Movie->pGraph->Release()				; Movie->pGraph = NULL ; }
	if( Movie->pMediaControl		){ Movie->pMediaControl->Release()		; Movie->pMediaControl = NULL ; }
	if( Movie->pMediaSeeking		){ Movie->pMediaSeeking->Release()		; Movie->pMediaSeeking = NULL ; }
	if( Movie->pBasicAudio			){ Movie->pBasicAudio->Release()		; Movie->pBasicAudio = NULL ; }

	return DXST_ERRORLOG_ADDW( ErStr ) ;
}

#endif // DX_NON_DSHOW_MOVIE

// ���[�r�[�t�@�C�����I�[�v������
extern int OpenMovie( const wchar_t *FileName, int *Width, int *Height, int SurfaceMode )
{
	OPENMOVIE_GPARAM GParam ;

	Graphics_Image_InitOpenMovieGParam( &GParam ) ;

	return OpenMovie_UseGParam( &GParam, FileName, Width, Height, SurfaceMode ) ;
}

// ���[�r�[�n���h����������������֐�
extern int InitializeMovieHandle( HANDLEINFO * )
{
	// ���ɉ������Ȃ�
	return 0 ;
}

// ���[�r�[�n���h���̌�n�����s���֐�
extern int TerminateMovieHandle( HANDLEINFO *HandleInfo )
{
	MOVIEGRAPH *Movie = ( MOVIEGRAPH * )HandleInfo ;

#ifndef DX_NON_OGGTHEORA
	// ���� Theora ���g�p���Ă����ꍇ�͂��̊J���������s��
	if( Movie->TheoraFlag )
	{
		TheoraDecode_Terminate( Movie->TheoraHandle ) ;

		if( Movie->TheoraStreamData )
		{
			FCLOSE( Movie->TheoraStreamData ) ;
			Movie->TheoraStreamData = 0 ;
		}

#ifndef DX_NON_SOUND
		NS_DeleteSoundMem( Movie->TheoraVorbisHandle ) ;
		Movie->TheoraVorbisHandle = 0 ;
#endif // DX_NON_SOUND
		Movie->TheoraHandle = 0 ;
		Movie->TheoraFlag = 0 ;
	}
#endif

#ifndef DX_NON_DSHOW_MOVIE
	// �����e���|�����t�@�C�����g�p���Ă����ꍇ�̓e���|�����t�@�C�����폜����
	if( Movie->UseTemporaryFile == TRUE )
	{
		DeleteFileW( Movie->FileName ) ;
		Movie->UseTemporaryFile = FALSE ;
	}
#endif // DX_NON_DSHOW_MOVIE

	if( Movie->NowImage.GraphData != NULL )
	{
		if( Movie->NowImageGraphOutAlloc == FALSE )
		{
			DXFREE( Movie->NowImage.GraphData ) ;
		}
		Movie->NowImage.GraphData = NULL ;
	}

	// �e��b�n�l�I�u�W�F�N�g���I������
//	if( Movie->pMovieImage )		{ Movie->pMovieImage->Release() ;			Movie->pMovieImage = NULL ; }
//	if( Movie->pFilter )			{ Movie->pFilter->Release() ;				Movie->pFilter = NULL ; }
	if( Movie->RefreshEvent )		{ CloseHandle( Movie->RefreshEvent ) ; 		Movie->RefreshEvent = NULL ; }
#ifndef DX_NON_DSHOW_MOVIE
	if( Movie->pBasicAudio )		{ Movie->pBasicAudio->Release() ; 			Movie->pBasicAudio = NULL ; }
	if( Movie->pMediaSeeking )		{ Movie->pMediaSeeking->Release() ; 		Movie->pMediaSeeking = NULL ; }
	if( Movie->pMediaControl )		{ Movie->pMediaControl->Release(); 			Movie->pMediaControl = NULL ; }
	if( Movie->pGraph )				{ Movie->pGraph->Release(); 				Movie->pGraph = NULL ; }
#endif

	// ����I��
	return 0 ;
}

// OpenMovie �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int OpenMovie_UseGParam( OPENMOVIE_GPARAM *GParam, const wchar_t *FileName, int *Width, int *Height, int SurfaceMode, int ASyncThread )
{
	int NewHandle ;
	MOVIEGRAPH * Movie ;

	// �n���h���̍쐬
	NewHandle = AddHandle( DX_HANDLETYPE_MOVIE, ASyncThread, -1 ) ;
	if( NewHandle < 0 )
	{
		return -1 ;
	}

	if( MOVIEHCHK_ASYNC( NewHandle, Movie ) )
	{
		return -1 ;
	}

	// �E�����A���t�@�Ƃ��Ĉ������t���O���Z�b�g����
	Movie->RightAlpha = GParam->RightAlphaFlag ;

	// 32bit�J���[�t�H�[�}�b�g�̓���� A8R8G8B8 �`���Ƃ��Ĉ������ǂ����̃t���O���Z�b�g����
	Movie->A8R8G8B8Flag = GParam->A8R8G8B8Flag ;

	// YUV�t�H�[�}�b�g�̃T�[�t�F�X���g�p���Ȃ����ǂ����̃t���O���Z�b�g����
	Movie->NotUseYUVFormatSurfaceFlag = GParam->NotUseYUVFormatSurfaceFlag ;

#ifndef DX_NON_OGGTHEORA
	// Ogg Theora �Ƃ��ăI�[�v�����悤�Ƃ��Ă݂�
	Movie->TheoraStreamData = FOPEN( FileName ) ;
	Movie->TheoraHandle = TheoraDecode_InitializeStream( &StreamFunctionW, Movie->TheoraStreamData, 10, Movie->NotUseYUVFormatSurfaceFlag, ASyncThread ) ;
	if( Movie->TheoraHandle == 0 )
	{
		FCLOSE( Movie->TheoraStreamData ) ;
		Movie->TheoraStreamData = 0 ;
	}
	if( Movie->TheoraHandle != 0 )
	{
		THEORADECODE_INFO Info ;

		// ���𖄂߂�
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		Movie->Width  = Info.Width ;
		Movie->Height = Info.Height ;
		if( Width  ) *Width  = Info.Width ;
		if( Height ) *Height = Info.Height ;
		Movie->TheoraFrameRate = Info.FrameRate ;
		Movie->TheoraTotalPlayTime = _DTOL( 1000.0 / Info.FrameRate * Info.TotalFrame ) ;

#ifndef DX_NON_SOUND
		LOADSOUND_GPARAM GParam ;

		InitLoadSoundGParam( &GParam ) ;

		// �J������ Thera �p�̃Z�b�g�A�b�v���s��

		// �T�E���h�Đ��p�ɃT�E���h�f�[�^�Ƃ��Ă��ǂݍ���
		GParam.NotInitSoundMemDelete = TRUE ;
		GParam.CreateSoundDataType = DX_SOUNDDATATYPE_FILE ;
		GParam.OggVorbisFromTheoraFile = TRUE ;
		GParam.DisableReadSoundFunctionMask = ~DX_READSOUNDFUNCTION_OGG ;
		Movie->TheoraVorbisHandle = LoadSoundMemBase_UseGParam( &GParam, FileName, 1, -1, FALSE, ASyncThread ) ;
		Movie->TheoraVorbisTotalTime = NS_GetSoundTotalTime( Movie->TheoraVorbisHandle ) ;
		Movie->TheoraVorbisFrequency = NS_GetFrequencySoundMem( Movie->TheoraVorbisHandle ) ;

		// ���[�v�^�C�v�̌���( �����ق�����ɂ��� )
		Movie->TheoraLoopType = Movie->TheoraVorbisTotalTime > Movie->TheoraTotalPlayTime ? 1 : 0 ;

#else // DX_NON_SOUND
		// ���[�v�^�C�v�͓���f�[�^���킹
		Movie->TheoraLoopType = 0 ;
#endif // DX_NON_SOUND

		// �Đ����x�̏�����
		Movie->TheoraPlaySpeedRate = 1.0 ;

		// Theora ���g�p���Ă���t���O�𗧂Ă�
		Movie->TheoraFlag = 1 ;

		// �g�p���ׂ� BASEIMAGE ���Z�b�g
		Movie->UseNowImage = ( BASEIMAGE * )TheoraDecode_GetBaseImage( Movie->TheoraHandle ) ;

		// �T�[�t�F�X���[�h�̓m�[�}���Ƃ������Ƃ�
		Movie->SurfaceMode = DX_MOVIESURFACE_NORMAL ;
	}
	else
#endif
	{
		// �I�[�v���ł��Ȃ������畁�ʂ̃��[�r�[�Ƃ��ĊJ���Ă݂�

#ifndef DX_NON_DSHOW_MOVIE
		// �t�@�C���̃I�[�v��
		Movie->UseTemporaryFile = FALSE ;
		if( OpenMovieFunction( Movie, FileName, Width, Height,SurfaceMode, ASyncThread ) == -1 )
#endif
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xe0\x30\xfc\x30\xd3\x30\xfc\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\xe6\x51\x06\x74\x42\x66\x6b\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x7a\x76\x1f\x75\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"���[�r�[�t�@�C���I�[�v���������ɃG���[���������܂���" @*/ ) ;
			goto ERR ;
		}

		// �g�p���ׂ� BASEIMAGE ���Z�b�g
		Movie->UseNowImage = &Movie->NowImage ;
	}

	// �摜���X�V���ꂽ�t���O��|��
	Movie->NowImageUpdateFlag = FALSE ;

	// �����ꎞ��~�t���O�𗧂Ă�
	Movie->SysPauseFlag = 1 ;

	// �Đ����t���O��|��
	Movie->PlayFlag = FALSE ;

	// �Đ��^�C�v�̓o�b�N�O���E���h�ɂ��Ă���
	Movie->PlayType = DX_PLAYTYPE_BACK ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( NewHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	// �n���h����Ԃ�
	return NewHandle ;

ERR :
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( NewHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	CloseMovie( NewHandle ) ;

	// �G���[�I��
	return -1 ;
}

// ���[�r�[�O���t�B�b�N���I������
extern int CloseMovie( int MovieHandle )
{
	return SubHandle( MovieHandle ) ;
}

// ���[�r�[�̍Đ����J�n����
extern int PlayMovie_( int MovieHandle, int PlayType, int SysPlay )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// PlayType �� -1 �̏ꍇ�͕ύX���Ȃ�
	if( PlayType == -1 ) PlayType = Movie->PlayType ;

	// PlayType �� DX_PLAYTYPE_NORMAL ���w�肵���ꍇ�� DX_PLAYTYPE_BACK �ɂȂ�
	if( PlayType == DX_PLAYTYPE_NORMAL ) PlayType = DX_PLAYTYPE_BACK ;

	// �����ꎞ��~�t���O���|��Ă����牽�����Ȃ�
	if( Movie->SysPauseFlag == 0 ) return 0 ;

	if( SysPlay == 1 && Movie->PlayFlag == FALSE ) return 0 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		int CurFrame ;
		THEORADECODE_INFO Info ;

		// �Đ��J�n���̎��Ԃ��擾
//		Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;

		// �Đ��J�n���̎��Ԃ��擾
		Movie->TheoraPrevTimeCount = NS_GetNowHiPerformanceCount() ;

		// �Đ����Ԃ��Z�b�g
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
		Movie->TheoraPlayNowTime = _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;

		// ���ɍĐ��ς݂̃t���[�����������O�ɍĐ��������Ƃɂ���
//		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
//		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
//		Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;

		// Vorbis �̍Đ����J�n����
#ifndef DX_NON_SOUND
//		NS_SetSoundCurrentTime( _DTOL( 1000.0 / Movie->TheoraFrameRate * CurFrame ), Movie->TheoraVorbisHandle ) ; 
		NS_PlaySoundMem( Movie->TheoraVorbisHandle, Movie->TheoraLoopType == 1 ? PlayType : DX_PLAYTYPE_BACK, FALSE ) ;
#endif // DX_NON_SOUND
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		// �Đ�
		Movie->pMediaControl->Run() ;
	}
#else // DX_NON_DSHOW_MOVIE
	{
	}
#endif

	// �摜���X�V���ꂽ�t���O��|��
	Movie->NowImageUpdateFlag = FALSE ;

	// �����ꎞ��~�t���O��|��
	Movie->SysPauseFlag = 0 ;

	// �Đ��^�C�v��ۑ�����
	Movie->PlayType = PlayType ;

	// �Đ����t���O�𗧂Ă�
	Movie->PlayFlag = TRUE ;

	// �I��
	return 0 ;
}

// ���[�r�[�̍Đ����X�g�b�v����
extern int PauseMovie( int MovieHandle, int SysPause )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// �����ꎞ��~�t���O�������Ă�����Ȃɂ����Ȃ�
	if( Movie->SysPauseFlag == 1 ) return 0 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		// Theora ���g�p���Ă���ꍇ

		// Vorbis �̍Đ����~����
#ifndef DX_NON_SOUND
		NS_StopSoundMem( Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND

		// ���݂̍Đ����ԕ��܂Ńt���[����i�߂Ă���
		UpdateMovie( MovieHandle ) ;
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		if( Movie->pMediaControl == NULL ) return 0 ; 

		// ��~
		Movie->pMediaControl->Pause() ;
	}
#else // DX_NON_DSHOW_MOVIE
	{
	}
#endif

	// �����ꎞ��~�t���O�𗧂Ă�
	Movie->SysPauseFlag = 1 ;

	// �Đ����t���O��|��
	if( SysPause == 0 )
	{
		Movie->PlayFlag = FALSE ;
	}

	// �I��
	return 0 ;
}

// ���[�r�[�̃t���[����i�߂�A�߂����Ƃ͏o���Ȃ�( ���[�r�[����~��ԂŁA���� Ogg Theora �̂ݗL�� )
extern int AddMovieFrame( int MovieHandle, unsigned int FrameNum )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// �摜���X�V���ꂽ�t���O��|��
	Movie->NowImageUpdateFlag = FALSE ;

#ifndef DX_NON_OGGTHEORA
	// Ogg Theora �ł͂Ȃ�������ύX�ł��Ȃ�
	if( Movie->TheoraFlag == FALSE )
		return -1 ;

	// �Đ�����������ύX�ł��Ȃ�
	if( GetMovieState( MovieHandle ) == TRUE )
		return -1 ;

	// �Đ��ʒu��ύX
	TheoraDecode_IncToFrame( Movie->TheoraHandle, ( int )FrameNum ) ;

	// �I��
	return 0 ;
#else
	return -1 ;
#endif
}

// ���[�r�[�̍Đ��ʒu��ݒ肷��(�~���b�P��)
extern int SeekMovie( int MovieHandle, int Time )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// �Đ����~�߂�
	PauseMovie( MovieHandle ) ;

	// �摜���X�V���ꂽ�t���O��|��
	Movie->NowImageUpdateFlag = FALSE ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		int CurFrame ;
		THEORADECODE_INFO Info ;

		// �R���o�[�g�ʒu��ύX����
		TheoraDecode_SeekToTime( Movie->TheoraHandle, Time * 1000 ) ;

		// �Đ��J�n�^�C����ύX����
//		Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
		Movie->TheoraPlayNowTime = _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
//		Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
	
		// �Đ��ʒu��ύX����
#ifndef DX_NON_SOUND
		NS_SetSoundCurrentTime( Time, Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		LONGLONG Now, Stop ;

		if( Movie->pMediaSeeking == NULL ) return 0 ;

		Now = ( LONGLONG )Time * 10000 ;
		Stop = 0 ;
		Movie->pMediaSeeking->SetPositions( &Now, D_AM_SEEKING_AbsolutePositioning, &Stop, D_AM_SEEKING_NoPositioning ) ;
	}
#else // DX_NON_DSHOW_MOVIE
	{
	}
#endif

	// �I��
	return 0 ;
}

// ���[�r�[�̍Đ����x��ݒ肷��( 1.0 = ���{��  2.0 = �Q�{�� )�A�ꕔ�̃t�@�C���t�H�[�}�b�g�݂̂ŗL���ȋ@�\�ł�
extern int SetPlaySpeedRateMovie( int MovieHandle, double SpeedRate )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		Movie->TheoraPlaySpeedRate = SpeedRate ;
#ifndef DX_NON_SOUND
		if( Movie->TheoraVorbisHandle != -1 )
		{
			NS_SetFrequencySoundMem( _DTOL( Movie->TheoraVorbisFrequency * Movie->TheoraPlaySpeedRate ), Movie->TheoraVorbisHandle ) ;
		}
		else
#endif // DX_NON_SOUND
		{
		}
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		if( Movie->pMediaSeeking == NULL ) return 0 ;

		Movie->pMediaSeeking->SetRate( SpeedRate ) ;
	}
#else // DX_NON_DSHOW_MOVIE
	{
	}
#endif

	// �I��
	return 0 ;
}

// ���[�r�[�̍Đ���Ԃ𓾂�
extern int GetMovieState( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
//	OAFilterState PlayFlag ;
#ifndef DX_NON_DSHOW_MOVIE
	LONGLONG Current ;
#endif

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
/*
#ifndef DX_NON_SOUND
		if( Movie->TheoraVorbisHandle != -1 )
		{
			// �T�E���h����~���Ă�����Đ���~
			if( NS_CheckSoundMem( Movie->TheoraVorbisHandle ) == 0 )
			{
				// ���݂̍Đ����ԕ��܂Ńt���[����i�߂Ă���
				UpdateMovie( MovieHandle ) ;

				// �����ꎞ��~�t���O�𗧂Ă�
				Movie->SysPauseFlag = 1 ;

				// �Đ����t���O��|��
				Movie->PlayFlag = FALSE ;

				// �����ꎞ��~�t���O�𗧂Ă�
				Movie->SysPauseFlag = 1 ;
			}
		}
		else
#endif // DX_NON_SOUND
*/
		{
			return Movie->PlayFlag ;
		}
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		D_OAFilterState state ;

		if( Movie->pMediaSeeking == NULL )
			return Movie->PlayFlag ; 

		if( Movie->pMediaControl->GetState( 1000, &state ) != S_OK )
		{
			return Movie->PlayFlag ;
		}

		Movie->pMediaSeeking->GetCurrentPosition( &Current ) ;
		if( ( Movie->PlayType & DX_PLAYTYPE_LOOPBIT ) == 0 && Current >= Movie->StopTime && state == D_State_Stopped )
		{
			Movie->PlayFlag = FALSE ;

			// �����ꎞ��~�t���O�𗧂Ă�
			Movie->SysPauseFlag = 1 ;
		}
	}
#else // DX_NON_DSHOW_MOVIE
	{
	}
#endif

//	if( Movie->pMediaControl->GetState( INFINITE, &PlayFlag ) != S_OK ) return -1 ;
//	return PlayFlag == State_Running ? 1 : 0 ;
	return Movie->PlayFlag ;
}

// ���[�r�[�̃{�����[�����Z�b�g����(0�`10000)
extern int SetMovieVolume( int Volume, int MovieHandle )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		// Vorbis �T�E���h�̉��ʂ��Z�b�g
#ifndef DX_NON_SOUND
		NS_SetVolumeSoundMem( Volume, Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		if( Movie->pBasicAudio == NULL ) return 0 ; 

		// ���ʃZ�b�g
		if( Volume > 10000 ) Volume = 10000 ;
		if( Volume < 0 ) Volume = 0 ;
		Movie->pBasicAudio->put_Volume( -10000 + Volume ) ;
	}
#else // DX_NON_DSHOW_MOVIE
	{
	}
#endif

	// �I��
	return 0 ;
}

// ���[�r�[�̊�{�C���[�W�f�[�^���擾����
extern BASEIMAGE *GetMovieBaseImage( int MovieHandle, int *ImageUpdateFlag )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return NULL ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		// ���[�r�[�̃t���[�����X�V
		UpdateMovie( MovieHandle ) ;

		// NowImage �̓��e���X�V���ꂽ���ǂ����̃t���O��������
		if( ImageUpdateFlag != NULL )
		{
			*ImageUpdateFlag = Movie->NowImageUpdateFlag ;
		}
		Movie->NowImageUpdateFlag = FALSE ;

		// Theora �̃C���[�W��Ԃ�
		return ( BASEIMAGE * )TheoraDecode_GetBaseImage( Movie->TheoraHandle ) ;
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		// �g�p���Ă��Ȃ��ꍇ

		// ���[�r�[�̃t���[�����X�V
		if( GetMovieState( MovieHandle ) == FALSE )
		{
			int Time ;

			Time = TellMovie( MovieHandle ) ;
			PlayMovie_( MovieHandle ) ;
			UpdateMovie( MovieHandle, TRUE ) ;
			PauseMovie( MovieHandle ) ;
			SeekMovie( MovieHandle, Time ) ;
		}
		else
		{
			UpdateMovie( MovieHandle ) ;
		}

		// NowImage �̓��e���X�V���ꂽ���ǂ����̃t���O��������
		if( ImageUpdateFlag != NULL )
		{
			*ImageUpdateFlag = Movie->NowImageUpdateFlag ;
		}
		Movie->NowImageUpdateFlag = FALSE ;

		return &Movie->NowImage ;
	}
#else // DX_NON_DSHOW_MOVIE
	return NULL ;
#endif
}

// ���[�r�[�̑��t���[�����𓾂�( Ogg Theora �ł̂ݗL�� )
extern int GetMovieTotalFrame( int MovieHandle )
{
	MOVIEGRAPH * Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	THEORADECODE_INFO info ;

	// Ogg Theora �ł͂Ȃ�������ύX�ł��Ȃ�
	if( Movie->TheoraFlag == FALSE )
		return -1 ;

	// ���̎擾
	TheoraDecode_GetInfo( Movie->TheoraHandle, &info ) ;

	// ���t���[������Ԃ�
	return info.TotalFrame ;
#else
	return -1 ;
#endif
}

// ���[�r�[�̍Đ��ʒu���擾����(�~���b�P��)
extern int TellMovie( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
#ifndef DX_NON_DSHOW_MOVIE
	D_STREAM_TIME NowTime ;
#endif
	
	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		THEORADECODE_INFO Info ;

		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

		// �t���[�����X�V
		UpdateMovie( MovieHandle ) ;

		// �t���[������Đ����Ԃ�����o��
		return _DTOL( TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) * 1000 / Movie->TheoraFrameRate ) ;
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		if( Movie->pMediaSeeking == NULL ) return 0 ;

		// ���Ԏ擾
		if( Movie->pMediaSeeking->GetCurrentPosition( &NowTime ) != S_OK ) return -1 ;

		// ���Ԃ�Ԃ�
		return _DTOL( (double)NowTime / 10000 ) ;
	}
#else // DX_NON_DSHOW_MOVIE
	return -1 ;
#endif
}
 
// ���[�r�[�̍Đ��ʒu���擾����(�t���[���P��)
extern int TellMovieToFrame( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
#ifndef DX_NON_DSHOW_MOVIE
	D_STREAM_TIME NowTime ;
#endif
	
	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		THEORADECODE_INFO Info ;

		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

		// �t���[�����X�V
		UpdateMovie( MovieHandle ) ;

		// ���݂̃t���[����Ԃ�
		return TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		if( Movie->pMediaSeeking == NULL ) return 0 ;

		// ���Ԏ擾
		if( Movie->pMediaSeeking->GetCurrentPosition( &NowTime ) != S_OK ) return -1 ;

		// ���Ԃ�Ԃ�
		return _DTOL( (double)NowTime / Movie->FrameTime ) ;
	}
#else // DX_NON_DSHOW_MOVIE
	return -1 ;
#endif
}

// ���[�r�[�̍Đ��ʒu��ݒ肷��(�t���[���P��)
extern int SeekMovieToFrame( int MovieHandle, int Frame )
{
	MOVIEGRAPH * Movie ;
//	D_OAFilterState State ;
	
	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// �摜���X�V���ꂽ�t���O��|��
	Movie->NowImageUpdateFlag = FALSE ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		int CurFrame ;
		THEORADECODE_INFO Info ;

		// �R���o�[�g�ʒu��ύX����
		TheoraDecode_SeekToFrame( Movie->TheoraHandle, Frame ) ;

		// �Đ��J�n�^�C����ύX����
//		Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
		Movie->TheoraPlayNowTime = _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
//		Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;
	
		// �Đ��ʒu��ύX����
#ifndef DX_NON_SOUND
		NS_SetSoundCurrentTime( _DTOL( Frame * 1000 / Movie->TheoraFrameRate ), Movie->TheoraVorbisHandle ) ;
#endif // DX_NON_SOUND
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		LONGLONG Now, Stop ;

		if( Movie->pMediaSeeking == NULL ) return 0 ;

		Now = ( D_STREAM_TIME )_DTOL64( (double)Frame * Movie->FrameTime ) ;
		Stop = 0 ;
		Movie->pMediaSeeking->SetPositions( &Now, D_AM_SEEKING_AbsolutePositioning, &Stop, D_AM_SEEKING_NoPositioning ) ;
	}
#else  // DX_NON_DSHOW_MOVIE
	{
	}
#endif

	// �I��
	return 0 ;
}

// ���[�r�[�̂P�t���[��������̎��Ԃ𓾂�
extern LONGLONG GetOneFrameTimeMovie( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		THEORADECODE_INFO Info ;

		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
		return _DTOL( 1000000 / Movie->TheoraFrameRate ) ;
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		return Movie->FrameTime ;
	}
#else // DX_NON_DSHOW_MOVIE
	return -1 ;
#endif
}

// ���[�r�[�O���t�B�b�N�̃f�[�^���擾����
extern MOVIEGRAPH * GetMovieData( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return NULL ;

	return Movie ;
}


// ���[�r�[�Ŏg�p���Ă���T�[�t�F�X�̉�����s��
extern int ReleaseMovieSurface( int MovieHandle )
{
	MOVIEGRAPH * Movie ;
	
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	if( Movie->TheoraFlag )
	{
		TheoraDecode_SurfaceTerminate( Movie->TheoraHandle ) ;
	}
	else
#endif
	{
	}

	// �I��
	return 0 ;
}

// ���[�r�[�̍X�V���s��
extern int UpdateMovie( int MovieHandle, int /*AlwaysFlag*/ )
{
	MOVIEGRAPH * Movie ;
//	LONGLONG NowFrame, OldFrame ;
	
	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

#ifndef DX_NON_OGGTHEORA
	// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( Movie->TheoraFlag )
	{
		// Theora ���g�p���Ă���ꍇ
		int NowFrame, CurFrame, AddFrame ;
		LONGLONG NowTime ;
		THEORADECODE_INFO Info ;

		// Theora �̏����擾
		TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

		// ���̍Đ����ԕ��t���[����i�߂�
		if( Movie->SysPauseFlag == 0 )
		{
			// �Đ����Ԃ͉������L���ȏꍇ�͉�������擾����
#ifndef DX_NON_SOUND
/*			if( Movie->TheoraVorbisHandle != -1 && NS_CheckSoundMem( Movie->TheoraVorbisHandle ) == 1 )
			{
				NowTime = NS_GetSoundCurrentTime( Movie->TheoraVorbisHandle ) * 1000 ;
				NowFrame = _DTOL( ( double )( NowTime * Movie->TheoraFrameRate ) / 1000000 ) ;

				// �������特���擾�����ꍇ�́A�f���̍ő�t���[���𒴂��Ă��܂����ꍇ�͌��݃t���[�����f���̍ő�t���[���ɂ���
				if( NowFrame >= Info.TotalFrame )
				{
					NowFrame = Info.TotalFrame - 1 ;
				}
			}
			else*/
#endif // DX_NON_SOUND
			{
				LONGLONG Temp64_1, Temp64_2 ;
				DWORD Temp128[ 4 ] ;

				NowTime = NS_GetNowHiPerformanceCount() ;

				// �Đ��ςݎ��Ԃ�i�߂�
				if( Movie->TheoraPlaySpeedRate < 0.999999999 || Movie->TheoraPlaySpeedRate > 1.0000000001 )
				{
					Temp64_1 = NowTime - Movie->TheoraPrevTimeCount ;
					Temp64_2 = _DTOL( Movie->TheoraPlaySpeedRate * 0x10000 ) ;
					_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

					Temp64_2 = 0x10000 ;
					_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

					Movie->TheoraPlayNowTime += Temp64_1 ;
				}
				else
				{
					Movie->TheoraPlayNowTime += NowTime - Movie->TheoraPrevTimeCount ;
				}

				Movie->TheoraPrevTimeCount = NowTime ;

				// ���݂̃t���[���̎Z�o
				{
					Temp64_1 = Movie->TheoraPlayNowTime ;
					Temp64_2 = _DTOL( Movie->TheoraFrameRate * 0x10000 ) ;
					_MUL128_1( ( DWORD * )&Temp64_1, ( DWORD * )&Temp64_2, Temp128 ) ;

#if defined( DX_GCC_COMPILE ) || defined( __ANDROID )
					Temp64_2 = 65536000000LL /* 1000000 * 0x10000 */ ;
#else
					Temp64_2 = 65536000000   /* 1000000 * 0x10000 */ ;
#endif
					_DIV128_1( Temp128, ( DWORD * )&Temp64_2, ( DWORD * )&Temp64_1 ) ;

					NowFrame = ( int )Temp64_1 ;
//					NowFrame = _DTOL( ( double )( ( NowTime - Movie->TheoraPlayTime ) * Movie->TheoraFrameRate ) / 1000000 ) ;
				}

				// ���[�v�w�肪���邩�ǂ����ő��t���[�����𒴂��Ă���ꍇ�̏����𕪊򂷂�
				if( Info.TotalFrame <= NowFrame )
				{
					if( Movie->PlayType & DX_PLAYTYPE_LOOPBIT )
					{
						// ���[�v����ꍇ�͑��t���[�����Ŋ������]����o��
						NowFrame %= Info.TotalFrame ;
					}
					else
					{
						// ���[�v���Ȃ��ꍇ�͍ŏI�t���[���Ŏ~�܂�
						NowFrame = Info.TotalFrame - 1 ;
					}
				}
			}

			// ���[�r�[�̍Đ��t���[�����擾����
			CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;

			// ��]�̃t���[�������݃o�b�t�@�Ɋi�[����Ă���t���[���ƈႤ�ꍇ�̓o�b�t�@���X�V����
			if( CurFrame != NowFrame )
			{
				Movie->NowImageUpdateFlag = TRUE ;

				// ���Z����t���[�������Z�o
				if( NowFrame < CurFrame )
				{
					// ���[�v����ꍇ
					AddFrame = Info.TotalFrame - CurFrame + NowFrame ;

#ifndef DX_NON_SOUND
					// ���[�v����ꍇ�ŁA�Đ��^�C�v�������̏ꍇ�̓T�E���h���ēx�Đ����J�n����
					if( Movie->TheoraLoopType == 0 )
					{
						NS_PlaySoundMem( Movie->TheoraVorbisHandle, DX_PLAYTYPE_BACK ) ;
					}
#endif // DX_NON_SOUND
				}
				else
				{
					AddFrame = NowFrame - CurFrame ;
				}

				// �t���[����i�߂�
				TheoraDecode_IncToFrame( Movie->TheoraHandle, AddFrame ) ;

				// �ŏI�X�V���Ԃ�ۑ�
				Movie->RefreshTime = ( int )( NowTime / 1000 ) ;
			}
			else
			{
				// ��]�̃t���[�������t���[�����I�[�ɒB���Ă��Ċ����[�v�w��ł������T�E���h�������ꍇ�͂����ōĐ��I��
				if( NowFrame == Info.TotalFrame - 1 &&
					( Movie->PlayType & DX_PLAYTYPE_LOOPBIT ) == 0
#ifndef DX_NON_SOUND
					&& ( Movie->TheoraVorbisHandle == -1 || NS_CheckSoundMem( Movie->TheoraVorbisHandle ) != 1 )
#endif // DX_NON_SOUND
					)
				{
					Movie->PlayFlag = FALSE ;

					// �����ꎞ��~�t���O�𗧂Ă�
					Movie->SysPauseFlag = 1 ;
				}
			}
		}

		// �C���[�W�̍\�z
		if( Movie->UpdateFunction )
		{
			Movie->UpdateFunction( Movie, Movie->UpdateFunctionData ) ;

			// �ŏ��̍X�V���s��ꂽ�t���O���Z�b�g����
			Movie->FirstUpdateFlag = TRUE ;
		}
	}
	else
#endif
#ifndef DX_NON_DSHOW_MOVIE
	{
		LONGLONG Now, Stop ;

		if( Movie->pMovieImage->NewImageSet )
		{
			Movie->NowImageUpdateFlag = TRUE ;

			Movie->pMovieImage->NewImageSet = 0 ;
			if( Movie->UpdateFunction )
			{
				Movie->NowImage.Width        = ( int )Movie->pMovieImage->Width ;
				Movie->NowImage.Height       = ( int )Movie->pMovieImage->Height ;
				Movie->NowImage.Pitch        = ( int )Movie->pMovieImage->Pitch ;
				Movie->NowImage.GraphData    = Movie->pMovieImage->ImageBuffer ;
				Movie->NowImageGraphOutAlloc = TRUE ;
				if( Movie->pMovieImage->ImageType == 0 )
				{
					NS_CreateFullColorData( &Movie->NowImage.ColorData ) ;
				}
				else
				if( Movie->pMovieImage->ImageType == 1 && Movie->A8R8G8B8Flag )
				{
					NS_CreateARGB8ColorData( &Movie->NowImage.ColorData ) ;
				}
				else
				{
					NS_CreateXRGB8ColorData( &Movie->NowImage.ColorData ) ;
				}
				Movie->UseNowImage = &Movie->NowImage ;
				Movie->UpdateFunction( Movie, Movie->UpdateFunctionData ) ;
			}
		}

		if( Movie->pMediaSeeking && Movie->pMediaControl )
		{
			Movie->pMediaSeeking->GetCurrentPosition( &Now ) ;
			if( Now >= Movie->StopTime )
			{
				if( Movie->PlayType & DX_PLAYTYPE_LOOPBIT )
				{
					Now = 0 ;
					Stop = 0 ;
					Movie->pMediaSeeking->SetPositions( &Now, D_AM_SEEKING_AbsolutePositioning, &Stop, D_AM_SEEKING_NoPositioning ) ;
					Movie->pMediaControl->Run() ;
				}
				else
				{
					// ��~
					Movie->pMediaControl->Pause() ;

					Movie->PlayFlag = FALSE ;

					// �����ꎞ��~�t���O�𗧂Ă�
					Movie->SysPauseFlag = 1 ;
				}
			}
		}
	}
#else // DX_NON_DSHOW_MOVIE
	{
	}
#endif

	// �I��
	return 0 ;
}

// ���[�r�[�̍X�V���ɌĂяo���R�[���o�b�N�֐���o�^����
extern int SetCallbackMovie( int MovieHandle, void ( *Callback )( MOVIEGRAPH *Movie, void *Data ), void *Data )
{
	MOVIEGRAPH *Movie ;

	// ���[�r�[�f�[�^�n���h�����擾
	if( MOVIEHCHK( MovieHandle, Movie ) )
		return -1 ;

	// �A�h���X���Z�b�g
	Movie->UpdateFunction = Callback ;
	Movie->UpdateFunctionData = Data ;

	// �I��
	return 0 ;
}

// ���[�r�[�̍Đ���Ԃ��~����
extern int DisableMovieAll( void )
{
	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	// ���ׂẴ��[�r�[�ɂ��ď������s��
	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ;

#ifndef DX_NON_OGGTHEORA
		// Theora ���g�p���Ă��邩�ǂ����ŏ����𕪊�
		if( Movie->TheoraFlag )
		{
			// �������Ȃ�
/*
			// �����ꎞ��~�t���O���|��Ă����珈��
			if( Movie->SysPauseFlag == 0 )
			{
				// Vorbis �̍Đ����~����
				NS_StopSoundMem( Movie->TheoraVorbisHandle ) ;

				// ���݂̍Đ����ԕ��܂Ńt���[����i�߂Ă���
				{
					int NowFrame, CurFrame ;
					LONGLONG NowTime ;
					THEORADECODE_INFO Info ;

					// Theora �̏����擾
					TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;

					// ���̍Đ����ԕ��t���[����i�߂�
					NowTime = NS_GetNowHiPerformanceCount() ;
					NowFrame = _DTOL( ( double )( ( NowTime - Movie->TheoraPlayTime ) * Movie->TheoraFrameRate ) / 1000000 ) ;
					CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
					if( CurFrame != NowFrame )
					{
						if( NowFrame < CurFrame )
						{
							TheoraDecode_SeekToFrame( Movie->TheoraHandle, NowFrame ) ;
						}
						else
						{
							TheoraDecode_IncToFrame( Movie->TheoraHandle, NowFrame - CurFrame ) ;
						}
					}

					// �C���[�W�̍\�z
					TheoraDecode_SetupImage( Movie->TheoraHandle ) ;
				}

				// �����ꎞ��~�t���O�𗧂Ă�
				Movie->SysPauseFlag = 1 ;
			}
*/		}
		else
#endif
#ifndef DX_NON_DSHOW_MOVIE
		{
/*			if( Movie )
			{
				// ���݂̍Đ����Ԃ�ۑ�
				Movie->pMediaSeeking->GetCurrentPosition( &Movie->BackUpTime ) ;

				// �e��b�n�l�I�u�W�F�N�g���I������

				if( Movie->pMediaControl		){ Movie->pMediaControl->Release() ;		Movie->pMediaControl = NULL ;		}
				if( Movie->pMediaSeeking		){ Movie->pMediaSeeking->Release() ;		Movie->pMediaSeeking = NULL ;		}
				if( Movie->pBasicAudio			){ Movie->pBasicAudio->Release() ;			Movie->pBasicAudio = NULL ;			}
				if( Movie->pMovieImage			){ Movie->pMovieImage->Release() ;			Movie->pMovieImage = NULL ;			}
				if( Movie->pFilter				){ Movie->pFilter->Release() ;				Movie->pFilter = NULL ;				}
				if( Movie->pGraph				){ Movie->pGraph->Release() ;				Movie->pGraph = NULL ;				}
			}
*/		}
#else  // DX_NON_DSHOW_MOVIE
		{
		}
#endif
	}

	// �I��
	return 0 ;
}


// ���[�r�[�̍Đ���Ԃ𕜌�����
extern int RestoreMovieAll( void )
{
	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	// ���ׂẴ��[�r�[�ɂ��ď������s��
	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ;

#ifndef DX_NON_OGGTHEORA
		// Theora ���ǂ����ŏ����𕪊�
		if( Movie->TheoraFlag )
		{
			// �������Ȃ�
/*
			// �Đ����t���O�������Ă���ꍇ�̂ݏ���
			if( Movie->PlayFlag )
			{
				// �����ꎞ��~�t���O�������Ă����珈��
				if( Movie->SysPauseFlag == 1 )
				{
					int CurFrame ;
					THEORADECODE_INFO Info ;

					// �Đ��J�n���̎��Ԃ��擾
					Movie->TheoraPlayTime = NS_GetNowHiPerformanceCount() ;

					// ���ɍĐ��ς݂̃t���[�����������O�ɍĐ��������Ƃɂ���
					TheoraDecode_GetInfo( Movie->TheoraHandle, &Info ) ;
					CurFrame = TheoraDecode_GetCurrentFrame( Movie->TheoraHandle ) ;
					Movie->TheoraPlayTime -= _DTOL( 1000000.0 / Movie->TheoraFrameRate * CurFrame ) ;

					// Vorbis �̍Đ����J�n����
					NS_PlaySoundMem( Movie->TheoraVorbisHandle, DX_PLAYTYPE_BACK, FALSE ) ;

					// �����ꎞ��~�t���O��|��
					Movie->SysPauseFlag = 0 ;
				}
			}
*/
		}
		else
#endif
#ifndef DX_NON_DSHOW_MOVIE
		{
/*			if( Movie )
			{
				LONGLONG Now, Stop ;

				// �ăI�[�v���������s��
				if( OpenMovieFunction( Movie, Movie->FileName, NULL, NULL, Movie->SurfaceMode ) == -1 )
					return -1 ;

				// �폜�O�̍Đ��|�C���g�ɐݒ�
				Now = Movie->BackUpTime ;
				Stop = 0 ;
				Movie->pMediaSeeking->SetPositions( &Now, D_AM_SEEKING_AbsolutePositioning, &Stop, D_AM_SEEKING_NoPositioning ) ;

				// �Đ����t���O�������Ă���ꍇ�͍Đ�
				if( Movie->SysPauseFlag == 0 )
				{
					Movie->pMediaControl->Run() ;
				}
			}
*/		}
#else // DX_NON_DSHOW_MOVIE
		{
		}
#endif
	}

	// �I��
	return 0 ;

}

// ���ׂẴ��[�r�[�O���t�B�b�N���X�^�[�g
extern int PlayMovieAll( void )
{
	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ; 

		PlayMovie_( Movie->HandleInfo.Handle, -1, 1 ) ;
	}

	// �I��
	return 0 ;
}

// ���ׂẴ��[�r�[�O���t�B�b�N���X�g�b�v
extern int PauseMovieAll( void )
{
	MOVIEGRAPH *Movie ;
	int i ;

	if( MOVIE.InitializeFlag == FALSE )
		return -1 ;

	for( i = HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MOVIE ].AreaMax ; i ++ )
	{
		Movie = ( MOVIEGRAPH * )HandleManageArray[ DX_HANDLETYPE_MOVIE ].Handle[ i ] ;
		if( Movie == NULL ) continue ; 

		PauseMovie( Movie->HandleInfo.Handle, 1 ) ;
	}

	// �I��
	return 0 ;
}

// ����t�@�C���̉E�������A���t�@���Ƃ��Ĉ������ǂ������Z�b�g����( TRUE:�A���t�@���Ƃ��Ĉ��� )
extern int NS_SetMovieRightImageAlphaFlag( int Flag )
{
	MOVIE.RightAlphaFlag = Flag;
	return TRUE;
}

// �ǂݍ��ޓ���t�@�C����32bit�J���[�������ꍇ�AA8R8G8B8 �`���Ƃ��Ĉ������ǂ������Z�b�g����A
// 32bit�J���[�ł͂Ȃ�����t�@�C���ɑ΂��Ă͖���( Flag  TRUE:A8R8G8B8�Ƃ��Ĉ���  FALSE:X8R8G8B8�Ƃ��Ĉ���( �f�t�H���g ) )
extern int NS_SetMovieColorA8R8G8B8Flag( int Flag )
{
	MOVIE.A8R8G8B8Flag = Flag;
	return TRUE;
}

// �x�t�u�t�H�[�}�b�g�̃T�[�t�F�X���g�p�ł���ꍇ�͂x�t�u�t�H�[�}�b�g�̃T�[�t�F�X���g�p���邩�ǂ�����ݒ肷��( TRUE:�g�p����( �f�t�H���g ) FALSE:�q�f�a�t�H�[�}�b�g�̃T�[�t�F�X���g�p���� )
extern int NS_SetMovieUseYUVFormatSurfaceFlag( int Flag )
{
	MOVIE.NotUseYUVFormatSurfaceFlag = Flag ? FALSE : TRUE ;
	return TRUE;
}


#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif
