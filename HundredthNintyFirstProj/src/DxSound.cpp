// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		�c�����������r������������v���O����
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

#include "DxSound.h"

#ifndef DX_NON_SOUND

// �C���N���[�h ------------------------------------------------------------------
#include "DxFile.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxMemory.h"
#include "DxUseCLib.h"
#include "DxASyncLoad.h"
#include "DxLog.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGuid.h"
#include "DxFileWin.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` -------------------------------------

// �ȈՃA�N�Z�X�p�}�N��
#define DSOUND								DX_DirectSoundData
#define MIDI								DX_MidiData

// �\�t�g�T�E���h�֌W�̒�`
#define SSND_PLAYER_SEC_DIVNUM				(256)		// �P�b�̕�����
#define SSND_PLAYER_STRM_BUFSEC				(256)		// �v���C���[�� DirectSoundBuffer �̃T�C�Y( 1 / SSND_PLAYER_SEC_DIVNUM�@�b )
#define SSND_PLAYER_STRM_ONECOPYSEC			(8)			// �v���C���[�� DirectSoundBuffer �̃T�C�Y( 1 / SSND_PLAYER_SEC_DIVNUM�@�b )
#define SSND_PLAYER_STRM_SAKICOPYSEC		(256 / 5)	// �v���C���[�̍Đ����ɉ����f�[�^���s�W�J���Ă����ő厞��( �P�ʂ� SSND_PLAYER_SEC_DIVNUM����1�b )
#define SSND_PLAYER_STRM_MINSAKICOPYSEC		(256 / 20)	// �v���C���[�̍Đ����ɉ����f�[�^���s�W�J���Ă����ŏ�����( �P�ʂ� SSND_PLAYER_SEC_DIVNUM����1�b )

// �X�g���[���֌W�̒�`
#define STS_DIVNUM							(256)		// �P�b�̕�����
#define STS_BUFSEC							(256)		// �X�g���[�����T�E���h�Đ����Ɋm�ۂ���T�E���h�o�b�t�@�̎���( �P�ʂ� STS_DIVNUM����1�b )
#define STS_ONECOPYSEC						(8)			// �X�g���[�����T�E���h�Đ����Ɉ�x�ɓ]������T�E���h�f�[�^�̒���( 1/STS_ONECOPYSEC �b )
#define STS_SAKICOPYSEC						(192)		// �X�g���[�����T�E���h�Đ����ɉ����f�[�^���s�W�J���Ă����ő厞��( �P�ʂ� STS_DIVNUM����1�b )
#define STS_MINSAKICOPYSEC					(96)		// �X�g���[�����T�E���h�Đ����ɉ����f�[�^���s�W�J���Ă����ŏ�����( �P�ʂ� STS_DIVNUM����1�b )
#define STS_CACHEBUFFERSEC					(8)			// �X�g���[�����T�E���h�Đ����ɁA�t�@�C������̃X�g���[���̂Ƃ��Ƀ�������Ɋm�ۂ���ꎞ�o�b�t�@�̒���( 1/STS_CACHEBUFFERSEC �b )

#define DSMP3ONEFILLSEC						(8)			// DirectShow ���g�����l�o�R�ϊ��̃t���O��ӂ肪�J�o�[���鎞��( 1/DSMP3ONEFILLSEC �b )

#define SOUNDSIZE( size, align )			( ( ( size ) + ( align - 1 ) ) / ( align ) * ( align ) )

// �L���v�`���ŕۑ�����T�E���h�̃T���v�����O���[�g
#define CAPTURE_SAMPLERATE					(44100)

// �W���v�`�u�d�t�@�C���̃w�b�_�̃T�C�Y�ƕW���v�`�u�d�t�@�C���� WAVEFORMATEX �\���̂̃T�C�Y
#define NORMALWAVE_HEADERSIZE				(46)
#define NORMALWAVE_FORMATSIZE				(18)

// �l�h�c�h�ŏ��{�����[��
#define DM_MIN_VOLUME						(-10000)


// �G���[�`�F�b�N�p�}�N��
#define SOUNDHCHK( HAND, SPOINT )			HANDLECHK(       DX_HANDLETYPE_SOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SOUNDHCHK_ASYNC( HAND, SPOINT )		HANDLECHK_ASYNC( DX_HANDLETYPE_SOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// �G���[�`�F�b�N�p�}�N��
#define SSND_MASKHCHK( HAND, SPOINT )		HANDLECHK(       DX_HANDLETYPE_SOFTSOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define SSND_MASKHCHK_ASYNC( HAND, SPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_SOFTSOUND, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// �G���[�`�F�b�N�p�}�N��
#define MIDI_MASKHCHK( HAND, SPOINT )		HANDLECHK(       DX_HANDLETYPE_MUSIC, HAND, *( ( HANDLEINFO ** )&SPOINT ) )
#define MIDI_MASKHCHK_ASYNC( HAND, SPOINT )	HANDLECHK_ASYNC( DX_HANDLETYPE_MUSIC, HAND, *( ( HANDLEINFO ** )&SPOINT ) )

// �f�V�x���l���� XAudio2 �̗��l�ɕϊ�����֐�
__inline float D_XAudio2DecibelsToAmplitudeRatio( float Decibels )
{
    return _POW( 10.0f, Decibels / 20.0f ) ;
}

// XAudio2 �̗��l����f�V�x���l�ɕϊ�����֐�
__inline float D_XAudio2AmplitudeRatioToDecibels( float Volume )
{
    if( Volume == 0 )
    {
        return -3.402823466e+38f ;
    }
    return 20.0f * ( float )_LOG10( Volume ) ;
}

// �\���̒�` --------------------------------------------------------------------

static const D_X3DAUDIO_DISTANCE_CURVE_POINT D_X3DAudioDefault_LinearCurvePoints[2] = { 0.0f, 1.0f, 1.0f, 0.0f };
static const D_X3DAUDIO_DISTANCE_CURVE       D_X3DAudioDefault_LinearCurve          = { ( D_X3DAUDIO_DISTANCE_CURVE_POINT * )& D_X3DAudioDefault_LinearCurvePoints[ 0 ], 2 } ;

static const D_X3DAUDIO_CONE Listener_DirectionalCone = { DX_PI_F * 5.0f / 6.0f, DX_PI_F * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f } ;

static const D_X3DAUDIO_DISTANCE_CURVE_POINT D_Emitter_LFE_CurvePoints[3]			= { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f } ;
static const D_X3DAUDIO_DISTANCE_CURVE       D_Emitter_LFE_Curve					= { ( D_X3DAUDIO_DISTANCE_CURVE_POINT * )&D_Emitter_LFE_CurvePoints[ 0 ], 3 } ;

static const D_X3DAUDIO_DISTANCE_CURVE_POINT D_Emitter_Reverb_CurvePoints[3]		= { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f } ;
static const D_X3DAUDIO_DISTANCE_CURVE       D_Emitter_Reverb_Curve					= { ( D_X3DAUDIO_DISTANCE_CURVE_POINT * )&D_Emitter_Reverb_CurvePoints[ 0 ], 3 } ;

static const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS D_PRESET_PARAMS[ D_XAUDIO2FX_PRESET_NUM ] =
{
	D_XAUDIO2FX_I3DL2_PRESET_FOREST,
	D_XAUDIO2FX_I3DL2_PRESET_DEFAULT,
	D_XAUDIO2FX_I3DL2_PRESET_GENERIC,
	D_XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,
	D_XAUDIO2FX_I3DL2_PRESET_ROOM,
	D_XAUDIO2FX_I3DL2_PRESET_BATHROOM,
	D_XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,
	D_XAUDIO2FX_I3DL2_PRESET_STONEROOM,
	D_XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,
	D_XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,
	D_XAUDIO2FX_I3DL2_PRESET_CAVE,
	D_XAUDIO2FX_I3DL2_PRESET_ARENA,
	D_XAUDIO2FX_I3DL2_PRESET_HANGAR,
	D_XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,
	D_XAUDIO2FX_I3DL2_PRESET_HALLWAY,
	D_XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,
	D_XAUDIO2FX_I3DL2_PRESET_ALLEY,
	D_XAUDIO2FX_I3DL2_PRESET_CITY,
	D_XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,
	D_XAUDIO2FX_I3DL2_PRESET_QUARRY,
	D_XAUDIO2FX_I3DL2_PRESET_PLAIN,
	D_XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,
	D_XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,
	D_XAUDIO2FX_I3DL2_PRESET_UNDERWATER,
	D_XAUDIO2FX_I3DL2_PRESET_SMALLROOM,
	D_XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,
	D_XAUDIO2FX_I3DL2_PRESET_LARGEROOM,
	D_XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,
	D_XAUDIO2FX_I3DL2_PRESET_LARGEHALL,
	D_XAUDIO2FX_I3DL2_PRESET_PLATE,
} ;

// �������ϐ��錾 --------------------------------------------------------------

// �c�����������r���������f�[�^
DIRECTSOUNDDATA DX_DirectSoundData ;

// �l�h�c�h�Ǘ��p�f�[�^
MIDIDATA DX_MidiData ;	

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

static	int CreateDirectSoundBuffer( WAVEFORMATEX *WaveFormat, DWORD BufferSize ,
									 int SoundType, int BufferNum,
									 int SoundHandle, int SrcSoundHandle = -1, int ASyncThread = FALSE ) ;	// �V���� DirectSoundBuffer �I�u�W�F�N�g�̍쐬
static  int _GetDirectSoundBuffer( int SoundHandle, SOUND * Sound, SOUNDBUFFER **BufferP = NULL, bool BufferGet = true ) ;	// �g�p�\�ȃT�E���h�o�b�t�@���擾����(-1:�T�E���h�o�b�t�@�̎g�p���𓾂��Ȃ�����)
static	DWORD WINAPI StreamSoundThreadFunction( void * ) ;											// �X�g���[���T�E���h�����p�X���b�h



// �⏕�n�֐�
static	int AdjustSoundDataBlock( int Length, SOUND * Sound ) ;										// �T�C�Y�␳

// �\�t�g�E�G�A�Ő��䂷��T�E���h�f�[�^�n���h���֌W
//static int AddSoftSoundData( int IsPlayer, int Channels, int BitsPerSample, int SamplesPerSec, int SampleNum ) ;	// �\�t�g�E�G�A�Ő��䂷��T�E���h�f�[�^�n���h���̒ǉ�
//static int SubSoftSoundData( int SoftSoundHandle, int CheckIsPlayer ) ;											// �\�t�g�E�G�A�Ő��䂷��T�E���h�f�[�^�n���h���̍폜
static	int _SoftSoundPlayerProcess( SOFTSOUND *SPlayer ) ;														// �\�t�g�E�G�A�Ő��䂷��T�E���h�f�[�^�v���C���[�̒������

// �⏕�n
extern	int CreateWaveFileImage( 	void **DestBufferP, int *DestBufferSizeP,
									const WAVEFORMATEX *Format, int FormatSize,
									const void *WaveData, int WaveDataSize ) ;						// �����t�H�[�}�b�g�Ɣg�`�C���[�W����v�`�u�d�t�@�C���C���[�W���쐬����
static	int FileFullRead( const wchar_t *FileName, void **BufferP, int *SizeP ) ;						// �t�@�C�����ۂ܂郁�����ɓǂݍ���
extern	int StreamFullRead( STREAMDATA *Stream, void **BufferP, int *SizeP ) ;						// �X�g���[���f�[�^���ۂ܂郁�����ɓǂݍ���
static	int SoundTypeChangeToStream( int SoundHandle ) ;											// �T�E���h�n���h���̃^�C�v���X�g���[���n���h���ł͂Ȃ��ꍇ�ɃX�g���[���n���h���ɕύX����
static	int MilliSecPositionToSamplePosition( int SamplesPerSec, int MilliSecTime ) ;				// �~���b�P�ʂ̐��l���T���v���P�ʂ̐��l�ɕϊ�����
static	int SamplePositionToMilliSecPosition( int SamplesPerSec, int SampleTime ) ;					// �T���v���P�ʂ̐��l���~���b�P�ʂ̐��l�ɕϊ�����
static	int _PlaySetupSoundMem( SOUND * Sound, int TopPositionFlag ) ;								// �T�E���h�n���h���̍Đ��������s��( -1:�G���[ 0:����I�� 1:�Đ�����K�v�Ȃ� )
static	int	_PlaySetupStreamSoundMem( int SoundHandle, SOUND * Sound, int PlayType, int TopPositionFlag, SOUNDBUFFER **DestSBuffer ) ;	// �X�g���[���T�E���h�n���h���̍Đ��������s��( -1:�G���[ 0:����I�� 1:�Đ�����K�v�Ȃ� )
static	int _CreateSoundHandle( int Is3DSound, int NotInitSoundMemDelete, int ASyncThread ) ;									// �T�E���h�n���h�����쐬����

// �g�`�f�[�^�p
static	WAVEDATA	*AllocWaveData( int Size, int UseDoubleSizeBuffer = FALSE ) ;
static	int			ReleaseWaveData( WAVEDATA *Data ) ;
static	WAVEDATA	*DuplicateWaveData( WAVEDATA *Data ) ;

//�T�E���h�o�b�t�@�p
static HRESULT SoundBuffer_Initialize(          SOUNDBUFFER *Buffer, D_DSBUFFERDESC *Desc, SOUNDBUFFER *Src, int Is3DSound ) ;
static HRESULT SoundBuffer_Duplicate(           SOUNDBUFFER *Buffer, SOUNDBUFFER *Src, int Is3DSound ) ;
static HRESULT SoundBuffer_Terminate(           SOUNDBUFFER *Buffer ) ;
static HRESULT SoundBuffer_Play(                SOUNDBUFFER *Buffer, int Loop ) ;
static HRESULT SoundBuffer_Stop(                SOUNDBUFFER *Buffer, int EffectStop = FALSE ) ;
static HRESULT SoundBuffer_GetStatus(           SOUNDBUFFER *Buffer, DWORD *Status ) ;
static HRESULT SoundBuffer_Lock(                SOUNDBUFFER *Buffer, DWORD WritePos, DWORD WriteSize, void **LockPos1, DWORD *LockSize1, void **LockPos2, DWORD *LockSize2 ) ;
static HRESULT SoundBuffer_Unlock(              SOUNDBUFFER *Buffer, void *LockPos1, DWORD LockSize1, void *LockPos2, DWORD LockSize2 ) ;
static HRESULT SoundBuffer_SetFrequency(        SOUNDBUFFER *Buffer, DWORD Frequency ) ;
static HRESULT SoundBuffer_GetFrequency(        SOUNDBUFFER *Buffer, LPDWORD Frequency ) ;
static HRESULT SoundBuffer_SetPan(              SOUNDBUFFER *Buffer, LONG Pan ) ;
static HRESULT SoundBuffer_GetPan(              SOUNDBUFFER *Buffer, LPLONG Pan ) ;
static HRESULT SoundBuffer_RefreshVolume(       SOUNDBUFFER *Buffer ) ;
static HRESULT SoundBuffer_SetVolumeAll(        SOUNDBUFFER *Buffer, LONG Volume ) ;
static HRESULT SoundBuffer_SetVolume(           SOUNDBUFFER *Buffer, int Channel, LONG Volume ) ;
static HRESULT SoundBuffer_GetVolume(           SOUNDBUFFER *Buffer, int Channel, LPLONG Volume ) ;
static HRESULT SoundBuffer_GetCurrentPosition(  SOUNDBUFFER *Buffer, LPDWORD PlayPos, LPDWORD WritePos ) ;
static HRESULT SoundBuffer_SetCurrentPosition(  SOUNDBUFFER *Buffer, DWORD NewPos ) ;
static HRESULT SoundBuffer_CycleProcess(        SOUNDBUFFER *Buffer ) ;
static HRESULT SoundBuffer_FrameProcess(        SOUNDBUFFER *Buffer, int Sample, short *DestBuf ) ;
static HRESULT SoundBuffer_Set3DPosition(       SOUNDBUFFER *Buffer, VECTOR *Position ) ;
static HRESULT SoundBuffer_Set3DRadius(         SOUNDBUFFER *Buffer, float Radius ) ;
static HRESULT SoundBuffer_Set3DInnerRadius(    SOUNDBUFFER *Buffer, float Radius ) ;
static HRESULT SoundBuffer_Set3DVelocity(       SOUNDBUFFER *Buffer, VECTOR *Velocity ) ;
static HRESULT SoundBuffer_Set3DFrontPosition(  SOUNDBUFFER *Buffer, VECTOR *FrontPosition, VECTOR *UpVector ) ;
static HRESULT SoundBuffer_Set3DConeAngle(      SOUNDBUFFER *Buffer, float InnerAngle, float OuterAngle ) ;
static HRESULT SoundBuffer_Set3DConeVolume(     SOUNDBUFFER *Buffer, float InnerAngleVolume, float OuterAngleVolume ) ;
static HRESULT SoundBuffer_Refresh3DSoundParam( SOUNDBUFFER *Buffer, int AlwaysFlag = FALSE ) ;
static HRESULT SoundBuffer_SetReverbParam(      SOUNDBUFFER *Buffer, SOUND3D_REVERB_PARAM *Param ) ;
static HRESULT SoundBuffer_SetPresetReverbParam( SOUNDBUFFER *Buffer, int PresetNo ) ;

// XAudio2�֌W
static HRESULT D_XAudio2CreateReverb( IUnknown** ppApo, DWORD Flags = 0 ) ;
static HRESULT D_XAudio2CreateVolumeMeter( IUnknown** ppApo, DWORD Flags = 0 ) ;
static void D_ReverbConvertI3DL2ToNative( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS* pNative ) ;
static void D_ReverbConvertI3DL2ToNative2_8( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS2_8* pNative ) ;

// �v���O���� --------------------------------------------------------------------

// �c�����������r���������������A�I���֐�

BOOL CALLBACK DSEnum( LPGUID /*lpGuid*/, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID /*lpContext*/ )
{
	DXST_ERRORLOGFMT_ADDW(( L"Module Name : %15s   Description : %s ", lpcstrModule , lpcstrDescription )) ;

	return TRUE ;
}


// �c�����������r��������������������
extern int InitializeDirectSound( void )
{
	HRESULT hr ;
	WAVEFORMATEX wfmtx ;
	D_DSCAPS caps ;

	if( DSOUND.InitializeFlag ) return -1 ;
	if( DSOUND.DirectSoundObject != NULL ) return -1 ;

INITSTART:

	// �o�̓��[�g���Z�b�g
	if( DSOUND.OutputSmaplesPerSec == 0 ) DSOUND.OutputSmaplesPerSec = 44100 ;

	// �R�c�T�E���h�̂P���[�g����ݒ�
	if( DSOUND._3DSoundOneMetreEnable == FALSE )
	{
		DSOUND._3DSoundOneMetre = 1.0f ;
	}

	// �Đ����[�h�ɂ���ď����𕪊�
	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		if( DSOUND.EnableXAudioFlag )
		{
			int i ;
			const wchar_t *X3DAudioDLLName[] =
			{
				L"X3DAudio1_7.dll",
				//L"X3DAudio1_6.dll",
				//L"X3DAudio1_5.dll",
				//L"X3DAudio1_4.dll",
				//L"X3DAudio1_3.dll",
				//L"X3DAudio1_2.dll",
				//L"X3DAudio1_1.dll",
				//L"X3DAudio1_0.dll",
				NULL,
			} ;
			GUID *XAudioGuidList[][ 2 ] = 
			{
				{
					&CLSID_XAUDIO2_7,
					&CLSID_XAUDIO2_7_DEBUG,
				},
				//{
				//	&CLSID_XAUDIO2_6,
				//	&CLSID_XAUDIO2_6_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_5,
				//	&CLSID_XAUDIO2_5_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_4,
				//	&CLSID_XAUDIO2_4_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_3,
				//	&CLSID_XAUDIO2_3_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_2,
				//	&CLSID_XAUDIO2_2_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_1,
				//	&CLSID_XAUDIO2_1_DEBUG,
				//},
				//{
				//	&CLSID_XAUDIO2_0,
				//	&CLSID_XAUDIO2_0_DEBUG,
				//},
				{
					NULL,
					NULL
				},
			} ;
			DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"XAudio2 �̏��������s���܂�\n" @*/ ) ;

			DXST_ERRORLOG_TABADD ;

			// �ŏ���XAudio2_8.dll�����������ׂ�
			DSOUND.XAudio2_8DLL = LoadLibraryW( L"XAudio2_8.dll" ) ;
			if( DSOUND.XAudio2_8DLL != NULL )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x5f\x00\x38\x00\x2e\x00\x64\x00\x6c\x00\x6c\x00\x20\x00\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"XAudio2_8.dll ���g�p���܂�\n" @*/ ) ;

				// �������ꍇ�̏���
				DSOUND.XAudio2CreateFunc          = ( HRESULT ( WINAPI * )( D_IXAudio2_8 **, DWORD, D_XAUDIO2_PROCESSOR ) )GetProcAddress( DSOUND.XAudio2_8DLL, "XAudio2Create" ) ;
				DSOUND.CreateAudioVolumeMeterFunc = ( HRESULT ( WINAPI * )( IUnknown** ppApo ) )GetProcAddress( DSOUND.XAudio2_8DLL, "CreateAudioVolumeMeter" ) ;
				DSOUND.CreateAudioReverbFunc      = ( HRESULT ( WINAPI * )( IUnknown** ppApo ) )GetProcAddress( DSOUND.XAudio2_8DLL, "CreateAudioReverb" ) ;
				DSOUND.X3DAudioInitializeFunc     = ( void ( __cdecl * )( DWORD, float, D_X3DAUDIO_HANDLE ) )GetProcAddress( DSOUND.XAudio2_8DLL, "X3DAudioInitialize" ) ;
				DSOUND.X3DAudioCalculateFunc      = ( void ( __cdecl * )( const D_X3DAUDIO_HANDLE, const D_X3DAUDIO_LISTENER *, const D_X3DAUDIO_EMITTER *, DWORD, D_X3DAUDIO_DSP_SETTINGS * ) )GetProcAddress( DSOUND.XAudio2_8DLL, "X3DAudioCalculate" ) ;

				DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x68\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"XAudio2 �C���^�[�t�F�[�X�̎擾�Ə��������s���܂�....  " @*/ ) ;

				hr = DSOUND.XAudio2CreateFunc( &DSOUND.XAudio2_8Object, 0, D_XAUDIO2_DEFAULT_PROCESSOR ) ;
				if( FAILED( hr ) )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�������Ɏ��s���܂���\n" @*/ ) ;
					DXST_ERRORLOGFMT_ADDA(( "\x83\x47\x83\x89\x81\x5b\x83\x52\x81\x5b\x83\x68 %x"/*@ "�G���[�R�[�h %x" @*/, hr )) ;
					DSOUND.XAudio2_8Object->Release() ;
					DSOUND.XAudio2_8Object = NULL ;

					DSOUND.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}
				DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x20\x00\x56\x00\x65\x00\x72\x00\x32\x00\x2e\x00\x38\x00\x0a\x00\x00"/*@ L"���� Ver2.8\n" @*/ ) ;

				hr = DSOUND.XAudio2_8Object->CreateMasteringVoice( &DSOUND.XAudio2_8MasteringVoiceObject, D_XAUDIO2_DEFAULT_CHANNELS, DSOUND.OutputSmaplesPerSec ) ;
				if( FAILED( hr ) )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x4d\x00\x61\x00\x73\x00\x74\x00\x65\x00\x72\x00\x69\x00\x6e\x00\x67\x00\x56\x00\x6f\x00\x69\x00\x63\x00\x65\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2MasteringVoice�̍쐬�Ɏ��s���܂���\n" @*/ ) ;
					DSOUND.XAudio2_8Object->Release() ;
					DSOUND.XAudio2_8Object = NULL ;

					DSOUND.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}

				// ���o�[�u�v�Z�p�p�����[�^����
				for( i = 0 ; i < D_XAUDIO2FX_PRESET_NUM ; i ++ )
				{
					D_ReverbConvertI3DL2ToNative2_8( &D_PRESET_PARAMS[ i ], &DSOUND.XAudio2_8ReverbParameters[ i ] ) ;
				}
			}
			else
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"XAudio2 �C���^�[�t�F�[�X�̎擾���s���܂�....  " @*/ ) ;
				for( i = 0 ; XAudioGuidList[ i ][ 0 ] != NULL ; i ++ )
				{
					hr = WinAPIData.Win32Func.CoCreateInstanceFunc( *XAudioGuidList[ i ][ 0 ], NULL, CLSCTX_ALL, IID_IXAUDIO2, ( LPVOID *)&DSOUND.XAudio2Object );
					if( !FAILED( hr ) )
					{
						break ;
					}
				}
				if( XAudioGuidList[ i ][ 0 ] != NULL )
				{
					DXST_ERRORLOGFMT_ADDUTF16LE(( "\x10\x62\x9f\x52\x20\x00\x56\x00\x65\x00\x72\x00\x32\x00\x2e\x00\x25\x00\x64\x00\x0a\x00\x00"/*@ L"���� Ver2.%d\n" @*/, 7 - i )) ;
					DXST_ERRORLOG_ADDUTF16LE(( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"���������C���^�[�t�F�[�X�̏���������...  " @*/ )) ;
					hr = DSOUND.XAudio2Object->Initialize( 0 ) ;
					if( FAILED( hr ) )
					{
						DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�������Ɏ��s���܂���\n" @*/ ) ;
						DXST_ERRORLOGFMT_ADDA(( "\x83\x47\x83\x89\x81\x5b\x83\x52\x81\x5b\x83\x68 %x"/*@ "�G���[�R�[�h %x" @*/, hr )) ;
						DSOUND.XAudio2Object->Release() ;
						DSOUND.XAudio2Object = NULL ;

						DSOUND.EnableXAudioFlag = FALSE ;
						goto INITSTART ;
					}
					DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����\n" @*/ ) ;

					DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x33\x00\x44\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x2e\x00\x44\x00\x4c\x00\x4c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x2d\x4e\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"X3DAudio.DLL �̓ǂݍ��ݒ�... " @*/ ) ;
					for( i = 0 ; X3DAudioDLLName[ i ] != NULL ; i ++ )
					{
						DSOUND.X3DAudioDLL = LoadLibraryW( X3DAudioDLLName[ i ] ) ;
						if( DSOUND.X3DAudioDLL != NULL )
							break ;
					}
					if( DSOUND.X3DAudioDLL == NULL )
					{
						DXST_ERRORLOG_ADDUTF16LE( "\x31\x59\x57\x65\x01\x30\x58\x00\x33\x00\x44\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x01\x30\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x6f\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"���s�AX3DAudio�AXAudio2�͎g�p���܂���\n" @*/ ) ;
						DSOUND.XAudio2Object->Release() ;
						DSOUND.XAudio2Object = NULL ;

						DSOUND.EnableXAudioFlag = FALSE ;
						goto INITSTART ;
					}
					else
					{
						DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����\n" @*/ ) ;

						DSOUND.X3DAudioInitializeFunc = ( void ( __cdecl * )( DWORD, float, D_X3DAUDIO_HANDLE ) )GetProcAddress( DSOUND.X3DAudioDLL, "X3DAudioInitialize" ) ;
						DSOUND.X3DAudioCalculateFunc  = ( void ( __cdecl * )( const D_X3DAUDIO_HANDLE, const D_X3DAUDIO_LISTENER *, const D_X3DAUDIO_EMITTER *, DWORD, D_X3DAUDIO_DSP_SETTINGS * ) )GetProcAddress( DSOUND.X3DAudioDLL, "X3DAudioCalculate" ) ;
					}
				}
				else
				{
					DSOUND.XAudio2Object = NULL ;
					DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2�C���^�[�t�F�[�X�̎擾�Ɏ��s���܂���\n" @*/ ) ;

					DSOUND.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}

				hr = DSOUND.XAudio2Object->CreateMasteringVoice( &DSOUND.XAudio2MasteringVoiceObject, D_XAUDIO2_DEFAULT_CHANNELS, DSOUND.OutputSmaplesPerSec ) ;
				if( FAILED( hr ) )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x4d\x00\x61\x00\x73\x00\x74\x00\x65\x00\x72\x00\x69\x00\x6e\x00\x67\x00\x56\x00\x6f\x00\x69\x00\x63\x00\x65\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2MasteringVoice�̍쐬�Ɏ��s���܂���\n" @*/ ) ;

					if( DSOUND.X3DAudioDLL != NULL )
					{
						FreeLibrary( DSOUND.X3DAudioDLL ) ;
						DSOUND.X3DAudioInitializeFunc = NULL ;
						DSOUND.X3DAudioCalculateFunc = NULL ;
						DSOUND.X3DAudioDLL = NULL ;
					}

					DSOUND.XAudio2Object->Release() ;
					DSOUND.XAudio2Object = NULL ;

					DSOUND.EnableXAudioFlag = FALSE ;
					goto INITSTART ;
				}

				// ���o�[�u�v�Z�p�p�����[�^����
				for( i = 0 ; i < D_XAUDIO2FX_PRESET_NUM ; i ++ )
				{
					D_ReverbConvertI3DL2ToNative( &D_PRESET_PARAMS[ i ], &DSOUND.XAudio2ReverbParameters[ i ] ) ;
				}
			}
		}
		else
		{
			DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"DirectSound �̏��������s���܂�\n" @*/ ) ;
			DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"DirectSound �C���^�[�t�F�[�X�̎擾���s���܂�....  " @*/ ) ; 
			hr = WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTSOUND, NULL, CLSCTX_ALL, IID_IDIRECTSOUND, ( LPVOID *)&DSOUND.DirectSoundObject );
			if( !FAILED( hr ) )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����\n" @*/ ) ;
				DXST_ERRORLOG_ADDUTF16LE( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x20\x00\x00"/*@ L"���������C���^�[�t�F�[�X�̏���������...  " @*/ ) ;
				hr = DSOUND.DirectSoundObject->Initialize( NULL ) ;
				if( FAILED( hr ) )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�������Ɏ��s���܂���\n" @*/ ) ;
					DXST_ERRORLOGFMT_ADDA(( "\x83\x47\x83\x89\x81\x5b\x83\x52\x81\x5b\x83\x68 %x"/*@ "�G���[�R�[�h %x" @*/, hr )) ;
					DSOUND.DirectSoundObject->Release() ;
					DSOUND.DirectSoundObject = NULL ;
					return -1 ;
				}
				DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����\n" @*/ ) ;
			}
			else
			{
				DSOUND.DirectSoundObject = NULL ;
				DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\xa4\x30\xf3\x30\xbf\x30\xfc\x30\xd5\x30\xa7\x30\xfc\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound�C���^�[�t�F�[�X�̎擾�Ɏ��s���܂���\n" @*/ ) ;
				return -1 ;
			}
	/*
			DXST_ERRORLOG_ADDUTF16LE( L"DirectSound �̏��������s���܂�\n" ) ; 
			hr = DirectSoundCreate( NULL , &DSOUND.DirectSoundObject , NULL ) ;
			if( hr != D_DS_OK )
			{
				DSOUND.DirectSoundObject = NULL ;
				DXST_ERRORLOG_ADDUTF16LE( L"DirectSound�̏������Ɏ��s���܂���\n" ) ;
				return -1 ;
			}
	*/

			// �������x�����Z�b�g����
			hr = DSOUND.DirectSoundObject->SetCooperativeLevel( GetDesktopWindow(), D_DSSCL_PRIORITY ) ;
			if( hr != D_DS_OK )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x6e\x30\x54\x53\xbf\x8a\xec\x30\xd9\x30\xeb\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound�̋������x���̐ݒ�Ɏ��s���܂���\n" @*/ ) ;
				DSOUND.DirectSoundObject->Release() ;
				DSOUND.DirectSoundObject = NULL ;
				return -1 ;
			}

			// �v���C�}���T�E���h�o�b�t�@���쐬����
			{
				D_DSBUFFERDESC dsbdesc ;
				DWORD ChNum , Rate , Byte ;

				// �쐬�X�e�[�^�X���Z�b�g����
				_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
				dsbdesc.dwSize = sizeof( dsbdesc ) ;
				dsbdesc.dwFlags = ( DWORD )( D_DSBCAPS_PRIMARYBUFFER | ( DSOUND.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : 0 ) ) ;
				hr = DSOUND.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &DSOUND.PrimarySoundBuffer , NULL ) ;
				if( hr != D_DS_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x6e\x30\xd7\x30\xe9\x30\xa4\x30\xde\x30\xea\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound�̃v���C�}���T�E���h�o�b�t�@�̍쐬�Ɏ��s���܂���\n" @*/ ) ;
					DSOUND.DirectSoundObject->Release() ;
					DSOUND.DirectSoundObject = NULL ;
					return -1 ;
				}

				// ���𓾂�
				_MEMSET( &caps, 0, sizeof( caps ) ) ;
				caps.dwSize = sizeof( caps ) ;
				DSOUND.DirectSoundObject->GetCaps( &caps ) ;


				// �v���C�}���o�b�t�@�̃t�H�[�}�b�g���Z�b�g����
				ChNum = ( DWORD )( caps.dwFlags & D_DSCAPS_PRIMARYSTEREO ? 2 : 1 ) ;
				Rate  = caps.dwMaxSecondarySampleRate < 44100 ? caps.dwMaxSecondarySampleRate : 44100  ;
				Byte  = ( DWORD )( caps.dwFlags & D_DSCAPS_PRIMARY16BIT  ? 2 : 1 ) ;

				_MEMSET( &wfmtx, 0, sizeof( wfmtx ) ) ;
				wfmtx.wFormatTag		= WAVE_FORMAT_PCM ;											// PCM�t�H�[�}�b�g
				wfmtx.nChannels			= ( WORD )ChNum ;											// �`�����l���Q���X�e���I
				wfmtx.nSamplesPerSec	= Rate ;													// �Đ����[�g
				wfmtx.wBitsPerSample	= ( WORD )( Byte * 8 ) ;									// �P���ɂ�����f�[�^�r�b�g��
				wfmtx.nBlockAlign		= ( WORD )( wfmtx.wBitsPerSample / 8 * wfmtx.nChannels ) ;	// �P�w���c�ɂ�����f�[�^�o�C�g��
				wfmtx.nAvgBytesPerSec	= wfmtx.nSamplesPerSec * wfmtx.nBlockAlign ;				// �P�b�ɂ�����f�[�^�o�C�g��
				hr = DSOUND.PrimarySoundBuffer->SetFormat( &wfmtx ) ;
				if( hr != D_DS_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x6e\x30\xd7\x30\xe9\x30\xa4\x30\xde\x30\xea\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound�̃v���C�}���T�E���h�o�b�t�@�̃t�H�[�}�b�g�ݒ�Ɏ��s���܂���\n" @*/ ) ;
				}

			}
		}
		break ;

	case DX_MIDIMODE_DM :
		DSOUND.EnableXAudioFlag = FALSE ;

		// �c�����������l���������ɂ��Đ��̏ꍇ�̏���
		{
			D_IDirectMusic *DMusic ;
			D_IDirectMusic8 *DMusic8 ;
			GUID MidiGuid = {0};
			bool MidiGuidValid ;
				
			DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x38\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x4c\x88\x44\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"DirectSound8 �̏��������s���܂�\n" @*/ ) ; 

			// DirectMusic �I�u�W�F�N�g�̍쐬
//			if( FAILED(	WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DirectMusicLoader , NULL , 
//											CLSCTX_INPROC , IID_IDirectMusicLoader8 , ( void ** )&DSOUND.DirectMusicLoaderObject ) ) )
			if( FAILED(	WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTMUSICLOADER, NULL , 
											CLSCTX_INPROC , IID_IDIRECTMUSICLOADER8 , ( void ** )&DSOUND.DirectMusicLoaderObject ) ) )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x4c\x00\x6f\x00\x61\x00\x64\x00\x65\x00\x72\x00\x38\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusicLoader8 �I�u�W�F�N�g�̍쐬�Ɏ��s���܂���\n" @*/ ) ;
				DXST_ERRORLOG_ADDUTF16LE( "\x4d\x52\xd0\x30\xfc\x30\xb8\x30\xe7\x30\xf3\x30\x67\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x6b\x30\xfb\x79\x8a\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"�O�o�[�W�����ł̏����������Ɉڂ�܂�\n" @*/ ) ;
				DSOUND.SoundMode = DX_MIDIMODE_MCI ;
				return InitializeDirectSound() ;
			}

//			if( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DirectMusicPerformance , NULL , 
//											CLSCTX_INPROC , IID_IDirectMusicPerformance8 , ( void ** )&DSOUND.DirectMusicPerformanceObject ) ) )
			if( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTMUSICPERFORMANCE, NULL , 
											CLSCTX_INPROC , IID_IDIRECTMUSICPERFORMANCE8 , ( void ** )&DSOUND.DirectMusicPerformanceObject ) ) )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x50\x00\x65\x00\x72\x00\x66\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6e\x00\x63\x00\x65\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusicPerformance �I�u�W�F�N�g�̍쐬�Ɏ��s���܂���\n" @*/ ) ;
				TerminateDirectSound() ;
				return -1 ;
			}

			MidiGuidValid = false ;
			// MIDI�����̗�
			{
				D_DMUS_PORTCAPS Param ;
				WCHAR wStr[D_DMUS_MAX_DESCRIPTION+3] ;
				int i ;

				if( FAILED( WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTMUSIC, NULL,
												CLSCTX_INPROC_SERVER, IID_IDIRECTMUSIC, (LPVOID*)&DMusic ) ) )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusic �I�u�W�F�N�g�̎擾�Ɏ��s���܂���\n" @*/ ) ;
					TerminateDirectSound() ;
					return -1 ;
				}
			
//				if( FAILED( DMusic->QueryInterface( IID_IDirectMusic8, (void **)&DMusic8 ) ) )
				if( FAILED( DMusic->QueryInterface( IID_IDIRECTMUSIC8, (void **)&DMusic8 ) ) )
				{
					DMusic->Release() ;
					DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x38\x00\x20\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusic8 �I�u�W�F�N�g�̎擾�Ɏ��s���܂���\n" @*/ ) ;
					TerminateDirectSound() ;
					return -1 ;
				}
				
				_MEMSET( &Param, 0, sizeof( Param ) ) ;
				Param.dwSize = sizeof(D_DMUS_PORTCAPS) ;
				
				for( i = 0 ; DMusic8->EnumPort( ( DWORD )i, &Param ) != S_FALSE ; i ++ )
				{
					_WCSCPY( wStr, Param.wszDescription ) ;
					wStr[D_DMUS_MAX_DESCRIPTION] = L'\0' ;
					DXST_ERRORLOGFMT_ADDW(( L"%s  Device%d:%s ", Param.dwClass == D_DMUS_PC_OUTPUTCLASS ? L"Output port" : L"Input port", i, wStr )) ;
				}
				
				DMusic8->Release() ;
				DMusic->Release() ;
			}
	
			// DirectMusicPerformance�I�u�W�F�N�g�̏�����
			{
				D_DMUS_AUDIOPARAMS AudioParams ;

				_MEMSET( &AudioParams, 0, sizeof( AudioParams ) ) ;
				AudioParams.dwSize = sizeof( AudioParams ) ;
				AudioParams.fInitNow = TRUE ;
				AudioParams.dwValidData = D_DMUS_AUDIOPARAMS_SAMPLERATE | D_DMUS_AUDIOPARAMS_FEATURES ;
				AudioParams.dwSampleRate = 44100 ;
				AudioParams.dwFeatures = D_DMUS_AUDIOF_ALL ;
				if( MidiGuidValid == true )
				{
					AudioParams.dwValidData |= D_DMUS_AUDIOPARAMS_DEFAULTSYNTH  ;
					AudioParams.clsidDefaultSynth = MidiGuid ;
				}

				DSOUND.DirectSoundObject = NULL ;
				DMusic = NULL ;
				if( DSOUND.DirectMusicPerformanceObject->InitAudio(
											&DMusic ,									// IDirectMusic�C���^�[�t�F�C�X�͕s�v�B
											&DSOUND.DirectSoundObject,					// IDirectSound�C���^�[�t�F�C�X�|�C���^��n���B
											NULL ,										// �E�B���h�E�̃n���h���B
											D_DMUS_APATH_SHARED_STEREOPLUSREVERB ,		// �f�t�H���g�̃I�[�f�B�I�p�X�^�C�v
											64 ,										// �p�t�H�[�}���X�`�����l���̐��B
											D_DMUS_AUDIOF_ALL ,							// �V���Z�T�C�U�̋@�\�B
											&AudioParams								// �I�[�f�B�I�p�����[�^�ɂ̓f�t�H���g���g�p�B
										) != S_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x50\x00\x65\x00\x72\x00\x66\x00\x6f\x00\x72\x00\x6d\x00\x61\x00\x6e\x00\x63\x00\x65\x00\xaa\x30\xd6\x30\xb8\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectMusicPerformance�I�u�W�F�N�g�̏������Ɏ��s���܂���\n" @*/ ) ;
					TerminateDirectSound() ;
					return -1 ;
				}
			}

			// �ő剹�ʂ̎擾
			DSOUND.DirectMusicPerformanceObject->GetGlobalParam( GUID_PERFMASTERVOLUME , ( void * )&DSOUND.MaxVolume , sizeof( long ) ) ;
		}
		break ;
	}

#ifndef DX_NON_BEEP
	// �r�[�v���̃Z�b�g
	NS_SetBeepFrequency( 8000 ) ;
#endif

	if( DSOUND.EnableXAudioFlag )
	{
		// ���o��
		if( DSOUND.XAudio2_8DLL == NULL )
		{
			D_XAUDIO2_DEVICE_DETAILS Details ;

			DSOUND.XAudio2Object->GetDeviceDetails( 0, &Details ) ;

			DXST_ERRORLOG_TABADD ;

#ifndef DX_GCC_COMPILE
			DXST_ERRORLOGFMT_ADDW(( L"Device Name : %s", Details.DisplayName )) ;
#endif
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x70\x65\x3a\x00\x25\x00\x64\x00\x00"/*@ L"�`�����l����:%d" @*/, Details.OutputFormat.Format.nChannels )) ;
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xd3\x30\xc3\x30\xc8\x30\xbe\x7c\xa6\x5e\x3a\x00\x25\x00\x64\x00\x62\x00\x69\x00\x74\x00\x00"/*@ L"�r�b�g���x:%dbit" @*/, Details.OutputFormat.Format.wBitsPerSample )) ;
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xec\x30\xfc\x30\xc8\x30\x3a\x00\x25\x00\x64\x00\x48\x00\x7a\x00\x00"/*@ L"�T���v�����O���[�g:%dHz" @*/, Details.OutputFormat.Format.nSamplesPerSec )) ;

			DXST_ERRORLOG_TABSUB ;

			DSOUND.XAudio2OutputChannelMask = Details.OutputFormat.dwChannelMask ;
			DSOUND.OutputChannels = Details.OutputFormat.Format.nChannels ;
		}
		else
		{
			int i ;

			DSOUND.XAudio2_8MasteringVoiceObject->GetChannelMask( &DSOUND.XAudio2OutputChannelMask ) ;
			DSOUND.OutputChannels = 0 ;
			for( i = 0 ; i < 32 ; i ++ )
			{
				if( ( DSOUND.XAudio2OutputChannelMask & ( 1 << i ) ) != 0 )
					DSOUND.OutputChannels ++ ;
			}
		}

		// X3DAudio �̏�����
		DSOUND.X3DAudioInitializeFunc( DSOUND.XAudio2OutputChannelMask, D_X3DAUDIO_SPEED_OF_SOUND, DSOUND.X3DAudioInstance ) ;
	}
	else
	{
		// �����o�b�t�@�̏�����
		{
			D_DSBUFFERDESC dsbdesc ;
			WAVEFORMATEX wfmtx ;
			HRESULT hr ;

			// �o�b�t�@�����X�e�[�^�X�̃Z�b�g
			_MEMSET( &wfmtx, 0, sizeof( wfmtx ) ) ;
			wfmtx.wFormatTag		= WAVE_FORMAT_PCM ;											// PCM�t�H�[�}�b�g
			wfmtx.nChannels			= 2 ;														// �`�����l���Q���X�e���I
			wfmtx.nSamplesPerSec	= 44100 ;													// �Đ����[�g
			wfmtx.wBitsPerSample	= 2 * 8 ;													// �P���ɂ�����f�[�^�r�b�g��
			wfmtx.nBlockAlign		= ( WORD )( wfmtx.wBitsPerSample / 8 * wfmtx.nChannels ) ;	// �P�w���c�ɂ�����f�[�^�o�C�g��
			wfmtx.nAvgBytesPerSec	= wfmtx.nSamplesPerSec * wfmtx.nBlockAlign ;				// �P�b�ɂ�����f�[�^�o�C�g��

			_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
			dsbdesc.dwSize			= sizeof( dsbdesc ) ;
			dsbdesc.dwFlags			= ( DWORD )( D_DSBCAPS_GLOBALFOCUS | ( DSOUND.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
			dsbdesc.dwBufferBytes	= ( DWORD )( wfmtx.nBlockAlign * 100 ) ;
			dsbdesc.lpwfxFormat		= &wfmtx ;

			hr = DSOUND.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &DSOUND.NoSoundBuffer , NULL ) ;
			if( hr != D_DS_OK )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x21\x71\xf3\x97\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����o�b�t�@�̍쐬�Ɏ��s���܂���" @*/ ) ;
				DSOUND.NoSoundBuffer = NULL ;
				goto R1 ;
			}

			// �����Ŗ��߂�
			{
				LPVOID write1 ;
				DWORD length1 ;
				LPVOID write2 ;
				DWORD length2 ;
				DWORD ltemp, i , j ;
				signed short *p ;

				hr = DSOUND.NoSoundBuffer->Lock( 0 , dsbdesc.dwBufferBytes, &write1 , &length1 , &write2 , &length2 , 0 ) ;		// �o�b�t�@�̃��b�N
				if( hr != D_DS_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x21\x71\xf3\x97\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\xed\x30\xc3\x30\xaf\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����o�b�t�@�̃��b�N�Ɏ��s���܂���" @*/ ) ;
					DSOUND.NoSoundBuffer->Release() ;
					DSOUND.NoSoundBuffer = NULL ;
				}

				j = 0 ;
				p = ( signed short * )write1 ;
				ltemp = length1 >> 1 ;
				for( i = 0 ; i < ltemp ; i += 1, j ++, p ++ ) *p = 0 ;

				if( write2 != 0 )
				{
					p = ( signed short * )write2 ;
					ltemp = length2 >> 1 ;
					for( i = 0 ; i < ltemp ; i ++, j ++, p ++ ) *p = 0 ;
				}

				hr = DSOUND.NoSoundBuffer->Unlock( write1, length1, write2, length2 ) ;								// �o�b�t�@�̃��b�N����
			}

			// �����o�b�t�@�̍Đ�
			DSOUND.NoSoundBuffer->Play( 0, 0, D_DSBPLAY_LOOPING ) ;
		}

R1 :
		// ���\�\��
		if( DSOUND.DirectSoundObject )
		{
			typedef HRESULT ( WINAPI *DIRECTSOUNDENUMERATEFUNC )( LPD_DSENUMCALLBACKA, LPVOID lpContext ) ;
			DIRECTSOUNDENUMERATEFUNC DirectSoundEnumerateFunc = NULL ;
			HINSTANCE DSoundDLL = NULL ;
	 
			DXST_ERRORLOG_TABADD ;
			_MEMSET( &caps, 0, sizeof( caps ) ) ;
			caps.dwSize = sizeof( caps ) ;
			DSOUND.DirectSoundObject->GetCaps( &caps ) ;

			DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x92\x30\x17\x52\x19\x63\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"DirectSound �f�o�C�X��񋓂��܂�\n" @*/ ) ;
			DXST_ERRORLOG_TABADD ;
			
			// DirectSound DLL �����[�h����
			if( ( DSoundDLL = LoadLibraryA( "DSOUND.DLL" ) ) == NULL )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x53\x00\x4f\x00\x55\x00\x4e\x00\x44\x00\x2e\x00\x44\x00\x4c\x00\x4c\x00\x20\x00\x6e\x30\xed\x30\xfc\x30\xc9\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DSOUND.DLL �̃��[�h�Ɏ��s���܂���\n" @*/ ) ;
				DXST_ERRORLOG_TABSUB ;
				goto ENUMEND ;
			}
		
			// �񋓗p�֐��̎擾
			if( ( DirectSoundEnumerateFunc = ( DIRECTSOUNDENUMERATEFUNC )GetProcAddress( DSoundDLL, "DirectSoundEnumerateW" ) ) == NULL )
			{
				FreeLibrary( DSoundDLL );
				DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x17\x52\x19\x63\x28\x75\xa2\x95\x70\x65\x6e\x30\xdd\x30\xa4\x30\xf3\x30\xbf\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound �f�o�C�X�̗񋓗p�֐��̃|�C���^�擾�Ɏ��s���܂���\n" @*/ ) ;
				DXST_ERRORLOG_TABSUB ;
				goto ENUMEND ;
			}

			// ��
			DirectSoundEnumerateFunc( DSEnum , NULL ) ;

			DXST_ERRORLOG_TABSUB ;

			DXST_ERRORLOGFMT_ADDUTF16LE(( "\x00\x67\x27\x59\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xec\x30\xfc\x30\xc8\x30\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x48\x00\x7a\x00\x20\x00\x00\x67\x0f\x5c\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xec\x30\xfc\x30\xc8\x30\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x48\x00\x7a\x00\x20\x00\x00"/*@ L"�ő�T���v�����O���[�g:%.2fKHz �ŏ��T���v�����O���[�g:%.2fKHz " @*/,
						( double )caps.dwMaxSecondarySampleRate / 1000 ,
						( double )caps.dwMinSecondarySampleRate / 1000 )) ;

			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xcf\x7d\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x42\x00\x20\x00\x7a\x7a\x4d\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x3a\x00\x25\x00\x2e\x00\x32\x00\x66\x00\x4b\x00\x42\x00\x20\x00\x0a\x00\x00"/*@ L"���T�E���h�������̈�:%.2fKB �󂫃T�E���h�������̈�:%.2fKB \n" @*/,
						( double )caps.dwTotalHwMemBytes / 0x100 ,
						( double )caps.dwFreeHwMemBytes / 0x100 )) ;

			DXST_ERRORLOG_ADDUTF16LE( "\x29\x52\x28\x75\xef\x53\xfd\x80\xb5\x30\xf3\x30\xd7\x30\xea\x30\xf3\x30\xb0\x30\xbe\x7c\xa6\x5e\x0a\x00\x00"/*@ L"���p�\�T���v�����O���x\n" @*/ ) ;
			DXST_ERRORLOGFMT_ADDW(( L"  Primary    16bit = %s  8bit = %s ",
				( caps.dwFlags & D_DSCAPS_PRIMARY16BIT ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_PRIMARY8BIT  ? L"OK" : L"NO" ) )) ;

			DXST_ERRORLOGFMT_ADDW(( L"  Secondary  16bit = %s  8bit = %s \n",
				( caps.dwFlags & D_DSCAPS_SECONDARY16BIT ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_SECONDARY8BIT  ? L"OK" : L"NO" ) )) ;

			DXST_ERRORLOG_ADDUTF16LE( "\x29\x52\x28\x75\xef\x53\xfd\x80\xc1\x30\xe3\x30\xf3\x30\xcd\x30\xeb\x30\x0a\x00\x00"/*@ L"���p�\�`�����l��\n" @*/ ) ;
			DXST_ERRORLOGFMT_ADDW(( L"  Primary    MONO = %s   STEREO = %s ",
				( caps.dwFlags & D_DSCAPS_PRIMARYMONO   ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_PRIMARYSTEREO ? L"OK" : L"NO" ) )) ;

			DXST_ERRORLOGFMT_ADDW(( L"  Secondary  MONO = %s   STEREO = %s \n",
				( caps.dwFlags & D_DSCAPS_SECONDARYMONO   ? L"OK" : L"NO" ) ,
				( caps.dwFlags & D_DSCAPS_SECONDARYSTEREO ? L"OK" : L"NO" ) )) ;

			DXST_ERRORLOG_TABSUB ;

			// DirectSound DLL ���������
			FreeLibrary( DSoundDLL );
		}
	}
ENUMEND :
	// �쐬���鉹�̃f�[�^�^�C�v���Z�b�g
	DSOUND.CreateSoundDataType = DX_SOUNDDATATYPE_MEMNOPRESS ;

#ifndef DX_NON_OGGVORBIS
	// �n�����u�����������̂o�b�l�f�R�[�h���́A�r�b�g�[�x���P�Ubit�ɃZ�b�g
	DSOUND.OggVorbisBitDepth = 2 ;
#endif

	// �T�E���h�n���h���Ǘ���񏉊���
	InitializeHandleManage( DX_HANDLETYPE_SOUND, sizeof( SOUND ), MAX_SOUND_NUM, InitializeSoundHandle, TerminateSoundHandle, L"Sound" ) ;

	// �\�t�g�E�G�A�ň����g�`�f�[�^�n���h���Ǘ���񏉊���
	InitializeHandleManage( DX_HANDLETYPE_SOFTSOUND, sizeof( SOFTSOUND ), MAX_SOFTSOUND_NUM, InitializeSoftSoundHandle, TerminateSoftSoundHandle, L"SoftSound" ) ;

	// �l�h�c�h�n���h���Ǘ���񏉊���
	InitializeHandleManage( DX_HANDLETYPE_MUSIC, sizeof( MIDIHANDLEDATA ), MAX_MUSIC_NUM, InitializeMidiHandle, TerminateMidiHandle, L"Music" ) ;

	// �n���h�����X�g��������
	InitializeHandleList( &DSOUND._3DSoundListFirst, &DSOUND._3DSoundListLast ) ;
	InitializeHandleList( &DSOUND.StreamSoundListFirst, &DSOUND.StreamSoundListLast ) ;
	InitializeHandleList( &DSOUND.SoftSoundPlayerListFirst, &DSOUND.SoftSoundPlayerListLast ) ;
	InitializeHandleList( &DSOUND.PlayFinishDeleteSoundListFirst, &DSOUND.PlayFinishDeleteSoundListLast ) ;
	InitializeHandleList( &DSOUND.Play3DSoundListFirst, &DSOUND.Play3DSoundListLast ) ;

	// �R�c�T�E���h�����p�̃��X�i�[����������
	DSOUND.X3DAudioListenerConeData = Listener_DirectionalCone ;
	DSOUND.X3DAudioListenerData.Position.x = 0.0f ;
	DSOUND.X3DAudioListenerData.Position.y = 0.0f ;
	DSOUND.X3DAudioListenerData.Position.z = 0.0f ;
	DSOUND.X3DAudioListenerData.Velocity.x = 0.0f ;
	DSOUND.X3DAudioListenerData.Velocity.y = 0.0f ;
	DSOUND.X3DAudioListenerData.Velocity.z = 0.0f ;
	DSOUND.X3DAudioListenerData.OrientFront.x = 0.0f ;
	DSOUND.X3DAudioListenerData.OrientFront.y = 0.0f ;
	DSOUND.X3DAudioListenerData.OrientFront.z = 1.0f ;
	DSOUND.X3DAudioListenerData.OrientTop.x = 0.0f ;
	DSOUND.X3DAudioListenerData.OrientTop.y = 1.0f ;
	DSOUND.X3DAudioListenerData.OrientTop.z = 0.0f ;
	DSOUND.X3DAudioListenerData.pCone = &DSOUND.X3DAudioListenerConeData ;
	DSOUND.ListenerInfo.Position = VGet( 0.0f, 0.0f, 0.0f ) ;
	DSOUND.ListenerInfo.FrontDirection = VGet( 0.0f, 0.0f, 1.0f ) ;
	DSOUND.ListenerInfo.Velocity = VGet( 0.0f, 0.0f, 0.0f ) ;
	DSOUND.ListenerInfo.InnerAngle = DSOUND.X3DAudioListenerConeData.InnerAngle ;
	DSOUND.ListenerInfo.OuterAngle = DSOUND.X3DAudioListenerConeData.OuterAngle ;
	DSOUND.ListenerInfo.InnerVolume = DSOUND.X3DAudioListenerConeData.InnerVolume ;
	DSOUND.ListenerInfo.OuterVolume = DSOUND.X3DAudioListenerConeData.OuterVolume ;
	DSOUND.ListenerSideDirection = VGet( 1.0f, 0.0f, 0.0f ) ;

	// �������t���O�𗧂Ă�
	DSOUND.InitializeFlag = TRUE ;

	NS_InitSoundMem() ;
	NS_InitSoftSound() ;
	NS_InitSoftSoundPlayer() ;

#ifndef DX_NON_MULTITHREAD

	// �X�g���[���T�E���h�Đ��p�̏������s���X���b�h�̍쐬
	DSOUND.StreamSoundThreadHandle = NULL ;
	if( WinData.ProcessorNum > 1 )
	{
		DSOUND.StreamSoundThreadHandle = CreateThread( NULL, 0, StreamSoundThreadFunction, NULL, CREATE_SUSPENDED, &DSOUND.StreamSoundThreadID ) ;
		SetThreadPriority( DSOUND.StreamSoundThreadHandle, THREAD_PRIORITY_TIME_CRITICAL ) ;
		ResumeThread( DSOUND.StreamSoundThreadHandle ) ;
	}

#endif // DX_NON_MULTITHREAD
	
	if( DSOUND.EnableXAudioFlag )
	{
		DXST_ERRORLOG_TABSUB ;
		DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x41\x00\x75\x00\x64\x00\x69\x00\x6f\x00\x32\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"XAudio2 �̏������͐���ɏI�����܂���\n" @*/ ) ;
	}
	else
	{
		DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound �̏������͐���ɏI�����܂���\n" @*/ ) ;
	}

	// �I��
	return 0 ;
}

// �c�����������r���������̎g�p���I������
extern int TerminateDirectSound( void )
{
	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

#ifndef DX_NON_MULTITHREAD

	// �X�g���[���T�E���h�Đ��p�X���b�h���I������
	if( DSOUND.StreamSoundThreadHandle != NULL )
	{
		DSOUND.StreamSoundThreadEndFlag = 1 ;
		while( NS_ProcessMessage() == 0 && DSOUND.StreamSoundThreadEndFlag == 1 ) Sleep( 2 ) ;
	}

#endif // DX_NON_MULTITHREAD

	// �T�E���h�L���v�`���������I������
	EndSoundCapture() ;

	// ���ׂẴT�E���h�f�[�^���������
	NS_InitSoundMem() ;

	// �S�Ẵ\�t�g�T�E���h�f�[�^���������
	NS_InitSoftSound() ;
	NS_InitSoftSoundPlayer() ;

	// ���ׂĂ̂l�h�c�h�f�[�^���������
	NS_InitMusicMem() ;

	// �e���|�����t�@�C���̍폜
	DeleteFileW( MIDI.FileName ) ;

	// �T�E���h�n���h���Ǘ����̌�n��
	TerminateHandleManage( DX_HANDLETYPE_SOUND ) ;

	// �\�t�g�T�E���h�n���h���Ǘ����̌�n��
	TerminateHandleManage( DX_HANDLETYPE_SOFTSOUND ) ;

	// �l�h�c�h�n���h���Ǘ����̌�n��
	TerminateHandleManage( DX_HANDLETYPE_MUSIC ) ;

	// �Đ����[�h�ɂ���ď����𕪊�
	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		// �ʏ�̍Đ����[�h�̎��̏���

		// PlayWav�֐��ōĐ�����Ă���T�E���h�͂Ȃ��̂�-1���Z�b�g���Ă���
		DSOUND.PlayWavSoundHandle = -1 ;

		break ;

	case DX_MIDIMODE_DM :
		// DirectMusic �ɂ�鉉�t���s���Ă����ꍇ�̏���

		// ���ׂẴT�E���h�̍Đ����~�߂�
		if( DSOUND.DirectMusicPerformanceObject )
		{
			DSOUND.DirectMusicPerformanceObject->Stop( NULL , NULL , 0 , 0 ) ;
			DSOUND.DirectMusicPerformanceObject->CloseDown() ;
			DSOUND.DirectMusicPerformanceObject->Release() ;

			DSOUND.DirectMusicPerformanceObject = NULL ;
		}

		// ���[�_�[���������
		if( DSOUND.DirectMusicLoaderObject )
		{
			DSOUND.DirectMusicLoaderObject->Release() ; 
			DSOUND.DirectMusicLoaderObject = NULL ;
		}

		break ;
	}

	// XAudio2 ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( DSOUND.EnableXAudioFlag )
	{
		// �}�X�^�����O�{�C�X�̉��
		if( DSOUND.XAudio2MasteringVoiceObject != NULL )
		{
			DSOUND.XAudio2MasteringVoiceObject->DestroyVoice() ;
			DSOUND.XAudio2MasteringVoiceObject = NULL ;
		}
		if( DSOUND.XAudio2_8MasteringVoiceObject != NULL )
		{
			DSOUND.XAudio2_8MasteringVoiceObject->DestroyVoice() ;
			DSOUND.XAudio2_8MasteringVoiceObject = NULL ;
		}

		// XAudio2�̉��
		if( DSOUND.XAudio2Object != NULL )
		{
			DSOUND.XAudio2Object->Release() ;
			DSOUND.XAudio2Object = NULL ;
		}

		// XAudio2.dll�̌�n������
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			FreeLibrary( DSOUND.XAudio2_8DLL ) ;
			DSOUND.XAudio2_8DLL = NULL ;
			DSOUND.XAudio2CreateFunc = NULL ;
			DSOUND.CreateAudioVolumeMeterFunc = NULL ;
			DSOUND.CreateAudioReverbFunc = NULL ;
			DSOUND.X3DAudioInitializeFunc = NULL ;
			DSOUND.X3DAudioCalculateFunc = NULL ;
		}

		// X3DAudio�̌�n������
		if( DSOUND.X3DAudioDLL != NULL )
		{
			FreeLibrary( DSOUND.X3DAudioDLL ) ;
			DSOUND.X3DAudioDLL = NULL ;
			DSOUND.X3DAudioInitializeFunc = NULL ;
			DSOUND.X3DAudioCalculateFunc = NULL ;
		}
	}
	else
	{
		// �v���C�}���T�E���h�o�b�t�@�̍Đ���~�A�j��
		if( DSOUND.PrimarySoundBuffer )
		{
			DSOUND.PrimarySoundBuffer->Stop() ;
			DSOUND.PrimarySoundBuffer->Release() ;
			DSOUND.PrimarySoundBuffer = NULL ;
		}

		// �����o�b�t�@�̍Đ���~�A�j��
		if( DSOUND.NoSoundBuffer )
		{
			DSOUND.NoSoundBuffer->Stop() ;
			DSOUND.NoSoundBuffer->Release() ;
			DSOUND.NoSoundBuffer = NULL ;
		}
		
		// �r�[�v���o�b�t�@�̉��
		if( DSOUND.BeepSoundBuffer )
		{
			DSOUND.BeepSoundBuffer->Release() ;
			DSOUND.BeepSoundBuffer = NULL ;
		}

		// �c�����������r���������I�u�W�F�N�g�̉��
		if( DSOUND.DirectSoundObject > (D_IDirectSound *)1 )
		{
			DSOUND.DirectSoundObject->Release() ;
		}
		DSOUND.DirectSoundObject = NULL ;


		DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x20\x00\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSound �̏I�������͐���ɏI�����܂���\n" @*/ ) ;
	}

	// �������t���O��|��
	DSOUND.InitializeFlag = FALSE ;

	// �I��
	return 0 ;
}









// �T�E���h�f�[�^�Ǘ��n�֐�

// InitSoundMem �Ń������ɓǂݍ���WAVE�f�[�^���폜���邩�ǂ������`�F�b�N����֐�
static int DeleteCancelCheckInitSoundMemFunction( HANDLEINFO *HandleInfo )
{
	SOUND * Sound = ( SOUND * )HandleInfo ;

	return Sound->NotInitSoundMemDelete ;
}

// �������ɓǂ݂���WAVE�f�[�^���폜���A����������
extern int NS_InitSoundMem( int /*LogOutFlag*/ )
{
	int Ret ;

	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	// �n���h����������
	Ret = AllHandleSub( DX_HANDLETYPE_SOUND, DeleteCancelCheckInitSoundMemFunction ) ;

	// PlayWav�p�T�E���h�n���h����������
	DSOUND.PlayWavSoundHandle = -1 ;

	// �I��
	return 0 ;
}

// �V�����T�E���h�f�[�^�̈���m�ۂ���
extern int NS_AddSoundData( int Handle )
{
	int Result ;

	if( DSOUND.InitializeFlag == FALSE )
	{
		return -1 ;
	}

	Result = AddHandle( DX_HANDLETYPE_SOUND, FALSE, Handle ) ;

	return Result ;
}

// �T�E���h�n���h���̏�����
extern int InitializeSoundHandle( HANDLEINFO *HandleInfo )
{
	SOUND *Sound = ( SOUND * )HandleInfo ;
	int i ;

	// ������
	Sound->PresetReverbParam = 0 ;
	NS_Get3DPresetReverbParamSoundMem( &Sound->ReverbParam, DX_REVERB_PRESET_DEFAULT ) ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->BaseVolume[ i ] = 0 ;
	}
	Sound->BasePan = 0 ;
	Sound->BaseFrequency = 0 ;
	Sound->Base3DPosition = VGet( 0.0f, 0.0f, 0.0f ) ;
	Sound->Base3DRadius = 14.0f ;
	Sound->Base3DVelocity = VGet( 0.0f, 0.0f, 0.0f ) ;

	// �T�E���h�̍Đ����I��������n���h���������I�ɍ폜���邩�ǂ����̃t���O��|��
	Sound->PlayFinishDeleteFlag = FALSE ;

	// �f�[�^�̐���������
	Sound->Stream.FileNum = 0 ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̌�n��
extern int TerminateSoundHandle( HANDLEINFO *HandleInfo )
{
	SOUND *Sound = ( SOUND * )HandleInfo ;
	int i ;

	// ���̍Đ����~�߂�
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		NS_StopStreamSoundMem( HandleInfo->Handle ) ;
	}
	else
	{
		if( Sound->Buffer[0].Valid ) NS_StopSoundMem( HandleInfo->Handle ) ;
	}

	if( Sound->Is3DSound != FALSE )
	{
		// �R�c�T�E���h�������ꍇ�͂R�c�T�E���h���X�g����O��
		SubHandleList( &Sound->_3DSoundList ) ;

		// �Đ������X�g�ɒǉ�����Ă�����O��
		if( Sound->AddPlay3DSoundList )
		{
			Sound->AddPlay3DSoundList = FALSE ;
			SubHandleList( &Sound->Play3DSoundList ) ;
		}
	}

	// �Đ����I��������폜����ݒ�ɂȂ��Ă�����A�Đ����I��������폜����T�E���h�̃��X�g����O��
	if( Sound->PlayFinishDeleteFlag )
	{
		SubHandleList( &Sound->PlayFinishDeleteSoundList ) ;
	}

	// �T�E���h�^�C�v�ɂ���ď����𕪊�
	switch( Sound->Type )
	{
	case DX_SOUNDTYPE_NORMAL :
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			if( Sound->Buffer[ i ].Valid ) SoundBuffer_Terminate( &Sound->Buffer[ i ] ) ;
		}

		// �T�E���h�f�[�^���������
		if( Sound->Normal.WaveDataUseCount )
		{
			*Sound->Normal.WaveDataUseCount = *Sound->Normal.WaveDataUseCount - 1 ;
			if( *Sound->Normal.WaveDataUseCount == 0 )
			{
				if( Sound->Normal.WaveData != NULL )
				{
					DXFREE( Sound->Normal.WaveData ) ;
				}
				DXFREE( Sound->Normal.WaveDataUseCount ) ;
			}
		}
		Sound->Normal.WaveDataUseCount = NULL ;
		Sound->Normal.WaveData = NULL ;
		break ;

	case DX_SOUNDTYPE_STREAMSTYLE :
		{
			int i ;
			STREAMFILEDATA *PlayData ;
			SOUND *UniSound = NULL ;
			
			// �T�E���h�o�b�t�@�����L���Ă���ꍇ�͏����𕪊�
			if( Sound->Stream.BufferBorrowSoundHandle != -1 &&
				SOUNDHCHK( Sound->Stream.BufferBorrowSoundHandle, UniSound ) == false )
			{
				// �T�E���h�o�b�t�@���g�p���Ă���̐������炷
				UniSound->Stream.BufferBorrowSoundHandleCount-- ;
				
				// �J�E���g���O�ɂȂ��Ă��āA�������ȊO�̃T�E���h�n���h���ŁA
				// �폜�҂������Ă����ꍇ�͍폜����
				if( Sound->Stream.BufferBorrowSoundHandle != HandleInfo->Handle &&
					UniSound->Stream.DeleteWaitFlag == TRUE )
				{
					NS_DeleteSoundMem( Sound->Stream.BufferBorrowSoundHandle ) ;
				}
			}
			
			// �T�E���h�o�b�t�@�����̃n���h���Ŏg���Ă�����폜�҂���Ԃɂ���
			if( Sound->Stream.BufferBorrowSoundHandleCount != 0 )
			{
				Sound->Stream.DeleteWaitFlag = TRUE ;

				// �폜�L�����Z���̖߂�l
				return 1 ;
			}

			// �X�g���[���T�E���h���X�g����O��
			SubHandleList( &Sound->Stream.StreamSoundList ) ;

			// �o�^����Ă��邷�ׂẴT�E���h�f�[�^�̗̈�����
			PlayData = Sound->Stream.File ;
			for( i = 0 ; i < Sound->Stream.FileNum ; i ++, PlayData ++ )
			{
				TerminateSoundConvert( &PlayData->ConvData ) ;
			
				if( PlayData->FileImage != NULL )
					DXFREE( PlayData->FileImage ) ;

				if( Sound->Stream.FileLibraryLoadFlag[ i ] == TRUE &&
					PlayData->DataType == DX_SOUNDDATATYPE_FILE &&
					PlayData->ConvData.Stream.DataPoint )
					PlayData->ConvData.Stream.ReadShred.Close( PlayData->ConvData.Stream.DataPoint ) ;

				if( PlayData->MemStream.DataPoint != NULL )
					PlayData->MemStream.ReadShred.Close( PlayData->MemStream.DataPoint ) ;
			}

			if( Sound->Buffer[0].Valid ) SoundBuffer_Terminate( &Sound->Buffer[0] ) ;
		}
		break ;
	}

	// �I��
	return 0 ;
}

// �V���� DirectSoundBuffer �I�u�W�F�N�g�̍쐬
static int CreateDirectSoundBuffer( WAVEFORMATEX *WaveFormat , DWORD BufferSize, int SoundType , int BufferNum , int SoundHandle, int SrcSoundHandle, int ASyncThread )
{
	int i ;
	SOUND *Sound ;
	SOUND *SrcSound = NULL ;
	D_DSBUFFERDESC dsbdesc ;
	HRESULT hr ;
	
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			return -1 ;
	}

	// ���n���h��������ꍇ�̏���
	if( SrcSoundHandle != -1 )
	{
		if( SOUNDHCHK( SrcSoundHandle, SrcSound ) )
			return -1 ;

		// �^�C�v���X�g���[����������G���[
		if( SoundType != DX_SOUNDTYPE_NORMAL ) return -1 ;

		// �p�����[�^�͌��n���h����������Ă���
		WaveFormat = &SrcSound->BufferFormat ;
		BufferSize = ( DWORD )SrcSound->Normal.WaveSize ;
	}

	// �^�C�v�ɂ���ď����𕪊�
	switch( SoundType )
	{
	case DX_SOUNDTYPE_NORMAL :	// �m�[�}���T�E���h
		{
			// �Z�J���_���T�E���h�o�b�t�@���쐬

			// �����쐬
			for( i = 0 ; i < BufferNum ; i ++ )
			{
				// �\�[�X�n���h���������ꍇ�͂P�Ԗڈȍ~�͓����o�b�t�@�𕡐�����
				if( i == 0 && SrcSound == NULL )
				{
NORMAL_CREATEBUFFER :
					// �o�b�t�@�����X�e�[�^�X�̃Z�b�g
					_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
					dsbdesc.dwSize			= sizeof( dsbdesc ) ;
					dsbdesc.dwFlags			= ( DWORD )( D_DSBCAPS_GLOBALFOCUS | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | D_DSBCAPS_CTRLFREQUENCY | ( DSOUND.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
					dsbdesc.dwBufferBytes	= BufferSize ;
					dsbdesc.lpwfxFormat		= WaveFormat ;
//					hr = DSOUND.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &Sound->Buffer[ i ] , NULL ) ;
					hr = SoundBuffer_Initialize( &Sound->Buffer[ i ], &dsbdesc, NULL, Sound->Is3DSound ) ;
					
					// �쐬�Ɏ��s�����ꍇ�́A�܂߂�@�\�����炵�čēx�쐬����
					if( hr != D_DS_OK )
					{
						// �o�b�t�@�����X�e�[�^�X�̃Z�b�g
						_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
						dsbdesc.dwSize			= sizeof( dsbdesc ) ;
						dsbdesc.dwFlags			= ( DWORD )( D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | ( DSOUND.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
						dsbdesc.dwBufferBytes	= BufferSize ;
						dsbdesc.lpwfxFormat		= WaveFormat ;

						if( SoundBuffer_Initialize( &Sound->Buffer[ i ], &dsbdesc, NULL, Sound->Is3DSound ) != D_DS_OK )
						{
							i -- ;
							while( i != -1 )
							{
								if( Sound->Buffer[ i ].Valid ) SoundBuffer_Terminate( &Sound->Buffer[ i ] ) ;

								i -- ;
							}
							DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectSoundBuffer �̍쐬�Ɏ��s���܂���\n" @*/ ) ;
							return -1 ;
						}
					}
					
					// �������ǂ����t���O��|��
					Sound->Normal.BufferDuplicateFlag[ i ] = FALSE ;
				}
				else
				{
					if( SrcSound )
					{
						hr = SoundBuffer_Duplicate( &Sound->Buffer[ i ], &SrcSound->Buffer[ 0 ], Sound->Is3DSound ) ;
//						hr = DSOUND.DirectSoundObject->DuplicateSoundBuffer( Sound->Buffer[ 0 ], &Sound->Buffer[ i ] ) ;
					}
					else
					{
						hr = SoundBuffer_Duplicate( &Sound->Buffer[ i ], &Sound->Buffer[ 0 ], Sound->Is3DSound ) ;
//						hr = DSOUND.DirectSoundObject->DuplicateSoundBuffer( Sound->Buffer[ 0 ], &Sound->Buffer[ i ] ) ;
					}

					// �������ǂ����t���O���Ƃ肠�����|���Ă���
					Sound->Normal.BufferDuplicateFlag[ i ] = FALSE ;
//					Sound->Normal.BufferDuplicateFlag[ i ] = hr == D_DS_OK ? TRUE : FALSE ;

					// �����Ɏ��s�����ꍇ�͕����̃T�E���h�o�b�t�@���g�p���Ȃ�
					if( hr != D_DS_OK )
					{
						// �\�[�X�T�E���h�������āA���O�Ŏ��s��������n���h��������
						if( SrcSound != NULL && i == 0 )
						{
							goto NORMAL_CREATEBUFFER ;
						}
						else
						{
							BufferNum = i ;
							break ;
						}
					}
					
					// �����Ɏ��s�����ꍇ�͕��ʂ̃o�b�t�@���쐬����
/*					if( hr != D_DS_OK )
					{
						// �������ǂ����t���O�𗧂Ă�
						Sound->Normal.BufferDuplicateFlag[ i ] = TRUE ;
						
						// �o�b�t�@�����X�e�[�^�X�̃Z�b�g
						_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
						dsbdesc.dwSize			= sizeof( dsbdesc ) ;
						dsbdesc.dwFlags			= D_DSBCAPS_GLOBALFOCUS | D_DSBCAPS_STATIC | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | D_DSBCAPS_CTRLFREQUENCY ;
						dsbdesc.dwBufferBytes	= BufferSize ;
						dsbdesc.lpwfxFormat		= WaveFormat ;
						hr = DSOUND.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &Sound->Buffer[ i ] , NULL ) ;
						
						// �쐬�Ɏ��s�����ꍇ�́A�܂߂�@�\�����炵�čēx�쐬����
						if( hr != D_DS_OK )
						{
							// �o�b�t�@�����X�e�[�^�X�̃Z�b�g
							_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
							dsbdesc.dwSize			= sizeof( dsbdesc ) ;
							dsbdesc.dwFlags			= D_DSBCAPS_STATIC | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME ;
							dsbdesc.dwBufferBytes	= BufferSize ;
							dsbdesc.lpwfxFormat		= WaveFormat ;

							if( DSOUND.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &Sound->Buffer[ i ] , NULL ) != D_DS_OK )
							{
								i -- ;
								while( i != -1 )
								{
									if( Sound->Buffer[ i ] ) Sound->Buffer[ i ]->Release() ;
									Sound->Buffer[ i ] = NULL ;

									i -- ;
								}
								return DxLib_Error( _T( "DirectSoundBuffer �̍쐬�Ɏ��s���܂���" ) ;
							}
						}
					}
*/				}
			}

			// �T�E���h�o�b�t�@����ۑ�
			Sound->ValidBufferNum = BufferNum ;
		}
		break ;

	case DX_SOUNDTYPE_STREAMSTYLE :		// �X�g���[�����T�E���h
		// �o�b�t�@�����X�e�[�^�X�̃Z�b�g
		_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
		dsbdesc.dwSize			= sizeof( dsbdesc ) ;
		dsbdesc.dwFlags			= ( DWORD )( D_DSBCAPS_GLOBALFOCUS | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | D_DSBCAPS_CTRLFREQUENCY | D_DSBCAPS_GETCURRENTPOSITION2 | ( DSOUND.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
		dsbdesc.dwBufferBytes	= SOUNDSIZE( STS_BUFSEC * WaveFormat->nAvgBytesPerSec / STS_DIVNUM, WaveFormat->nBlockAlign ) ;
		dsbdesc.lpwfxFormat		= WaveFormat ;

//		if( DSOUND.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &Sound->Buffer[ 0 ] , NULL ) != D_DS_OK )
		if( SoundBuffer_Initialize( &Sound->Buffer[ 0 ], &dsbdesc, NULL, Sound->Is3DSound ) != D_DS_OK )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa8\x98\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�X�g���[�����T�E���h�o�b�t�@�̍쐬�Ɏ��s���܂���\n" @*/ ) ;
			return -1 ;
		}

		// �T�E���h�o�b�t�@����ۑ�
		Sound->ValidBufferNum = 1 ;
		break ;
	}

	// �f�[�^��ۑ�
//	Sound->BufferFormat.nAvgBytesPerSec = WaveFormat->nAvgBytesPerSec ;
	Sound->Type = SoundType ;

	// �X�g���[���`���̏ꍇ�̓X�g���[���T�E���h�n���h�����X�g�ɒǉ�
	if( SoundType == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// �N���e�B�J���Z�N�V�����̎擾
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		AddHandleList( &DSOUND.StreamSoundListFirst, &Sound->Stream.StreamSoundList, SoundHandle, Sound ) ;

		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;
	}

	// �I��
	return 0 ;
}

// �g�p�\�ȃT�E���h�o�b�t�@���擾����(-1:�T�E���h�o�b�t�@�̎g�p���𓾂��Ȃ�����)
static int _GetDirectSoundBuffer( int SoundHandle, SOUND * Sound, SOUNDBUFFER **BufferP, bool BufferGet )
{
	SOUND * UniSound = NULL ;

	// �X�g���[���ł͂Ȃ�������g�p���͓�����
	if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE )
	{
		if( BufferP ) *BufferP = &Sound->Buffer[ 0 ] ;
		return 0 ;
	}
	
	// ���L�o�b�t�@���g�p���Ă��邩�ǂ����ŕ���
	if( BufferP ) *BufferP = &Sound->Buffer[0] ;
	if( Sound->Stream.BufferBorrowSoundHandle == -1 )
	{
		// �g�p���Ă��Ȃ��ꍇ�́A�g�p����Ă��Ȃ���Ύg�p���𓾂���
		if( Sound->Stream.BufferUseSoundHandle == -1 )
		{
			Sound->Stream.BufferUseSoundHandle = SoundHandle ;
			return 0 ;
		}
		
		// �g�p����Ă���ꍇ�Ŏg�p���Ă���̂������ł͂Ȃ��A
		// �g�p���𓾂�w��������ꍇ�͎g�p���𓾂��Ȃ�
		if( SoundHandle != Sound->Stream.BufferUseSoundHandle && BufferGet == false ) return -1 ;
		
		// �g�p���������Ă���̂������ł͂Ȃ��ꍇ�͍Đ����X�g�b�v����
		if( SoundHandle != Sound->Stream.BufferUseSoundHandle )
			NS_StopStreamSoundMem( Sound->Stream.BufferUseSoundHandle ) ;

		// �g�p���𓾂�
		Sound->Stream.BufferUseSoundHandle = SoundHandle ;
		return 0 ;
	}
	else
	{
		// �؂�悪������������g�p���𓾂��Ȃ�
		if( SOUNDHCHK( Sound->Stream.BufferBorrowSoundHandle, UniSound ) == true ) return -1 ;
		if( BufferP ) *BufferP = &UniSound->Buffer[0] ;
		
		// �N���g�p���Ă��Ȃ��ꍇ�͎g�p���𓾂邱�Ƃ��o����
		if( UniSound->Stream.BufferUseSoundHandle == -1 )
		{
			UniSound->Stream.BufferUseSoundHandle = SoundHandle ;
			return 0 ;
		}
		
		// �g�p����Ă���ꍇ�Ŏg�p���Ă���̂������ł͂Ȃ��A
		// �g�p���𓾂�w��������ꍇ�͎g�p���𓾂��Ȃ�
		if( SoundHandle != UniSound->Stream.BufferUseSoundHandle && BufferGet == false ) return -1 ;

		// �g�p���������Ă���̂������ł͂Ȃ��ꍇ�͍Đ����X�g�b�v����
		if( SoundHandle != UniSound->Stream.BufferUseSoundHandle )
			NS_StopStreamSoundMem( UniSound->Stream.BufferUseSoundHandle ) ;

		// �g�p���𓾂�
		UniSound->Stream.BufferUseSoundHandle = SoundHandle ;
		return 0 ;
	}
}


// AddStreamSoundMem �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int AddStreamSoundMem_UseGParam(
	LOADSOUND_GPARAM *GParam,
	STREAMDATA *Stream,
	int LoopNum,
	int SoundHandle,
	int StreamDataType,
	int *CanStreamCloseFlag,
	int UnionHandle,
	int ASyncThread
)
{
	DWORD State ;
	SOUND * Sound ;
	STREAMFILEDATA *PlayData ;
	WAVEFORMATEX Format ;
	SOUND * UniSound;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// ��ł��f�[�^�����݂���ꍇ�̓`�F�b�N����������
	if( Sound->Stream.FileNum > 0 )
	{
		// �X�g���[�����T�E���h�ł͂Ȃ������ꍇ�͏I��
		if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa8\x98\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x67\x30\x6f\x30\x6a\x30\x44\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x4c\x30\x21\x6e\x55\x30\x8c\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x41\x00\x64\x00\x64\x00\x53\x00\x72\x00\x65\x00\x61\x00\x6d\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x0a\x00\x00"/*@ L"�X�g���[�����T�E���h�ł͂Ȃ��T�E���h�f�[�^���n����܂��� in AddSreamSound\n" @*/ ) ;
			goto ERR ;
		}

		// ���ɍĐ����̏ꍇ���I��
		if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == 0 )
		{
//			SBuffer->GetStatus( &State ) ;
			SoundBuffer_GetStatus( SBuffer, &State ) ;

			if( State & D_DSBSTATUS_PLAYING )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\xe2\x65\x6b\x30\x8d\x51\x1f\x75\x2d\x4e\x6e\x30\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6b\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\xfd\x8f\xa0\x52\x57\x30\x88\x30\x46\x30\x68\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"���ɍĐ����̃X�g���[���T�E���h�f�[�^�Ƀf�[�^��ǉ����悤�Ƃ��܂���\n" @*/ ) ;
				goto ERR ;
			}
		}

		// �����X�g���[���T�E���h����f�[�^�̗̈�ɗ]�T���Ȃ��ꍇ���I��
		if( Sound->Stream.FileNum == STS_SETSOUNDNUM - 1 )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa8\x98\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x6e\x30\x36\x52\xa1\x5f\xc7\x30\xfc\x30\xbf\x30\x18\x98\xdf\x57\x4c\x30\x44\x30\x63\x30\x71\x30\x44\x30\x67\x30\x59\x30\x0a\x00\x00"/*@ L"�X�g���[�����T�E���h�̐���f�[�^�̈悪�����ς��ł�\n" @*/ ) ;
			goto ERR ;
		}
	}

	PlayData = &Sound->Stream.File[ Sound->Stream.FileNum ] ;
	_MEMSET( PlayData, 0, sizeof( STREAMFILEDATA ) ) ;

	// �f�[�^�^�C�v��ۑ�
	PlayData->DataType = StreamDataType ;

	// ���[�v�J�n�ʒu�͖���
	PlayData->LoopStartSamplePosition = -1 ;

	// �X�g���[���|�C���^��擪�ʒu��
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;

	// �V�����v�`�u�d�t�@�C���̃��[�h�A�f�[�^�̃^�C�v�ɂ���ď����𕪊�
	switch( StreamDataType )
	{
	case DX_SOUNDDATATYPE_MEMNOPRESS :
		// ��������ɒ��ڍĐ��\�ȃf�[�^��ێ�����ꍇ
		{
			STREAMDATA MemStream ;
			void *WaveImage ;
			int WaveSize ;

			// �o�b�l�`���ɕϊ�
			{
				SOUNDCONV ConvData ;
				int res ;
				void *SrcBuffer ;
				int SrcSize ;
				
				// �ہX�������ɓǂݍ���
				if( StreamFullRead( Stream, &SrcBuffer, &SrcSize ) < 0 )
				{
					DXST_ERRORLOGFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����f�[�^�̓ǂݍ��݂Ɏ��s���܂���" @*/ )) ;
					goto ERR ;
				}
				
				MemStream.DataPoint = MemStreamOpen( SrcBuffer, ( unsigned int )SrcSize ) ;
				MemStream.ReadShred = *GetMemStreamDataShredStruct() ;
			
				SetupSoundConvert( &ConvData, &MemStream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
									,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
				) ;
				res = SoundConvertFast( &ConvData, &Format, &PlayData->FileImage, &PlayData->FileImageSize ) ;

				TerminateSoundConvert( &ConvData ) ;
				MemStreamClose( MemStream.DataPoint ) ;
				DXFREE( SrcBuffer ) ;

				if( res < 0 )
				{
					DXST_ERRORLOGFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\x09\x59\xdb\x63\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����f�[�^�̕ϊ��Ɏ��s���܂���" @*/ )) ;
					goto ERR ;
				}
			}

			// �v�`�u�d�t�@�C�����ł����グ��
			if( CreateWaveFileImage( &WaveImage, &WaveSize,
								 &Format, sizeof( WAVEFORMATEX ),
								 PlayData->FileImage, PlayData->FileImageSize ) < 0 )
				goto ERR ;

			// �W�J���ꂽ�f�[�^���X�g���[���Ƃ��čēx�J��				
			PlayData->MemStream.DataPoint = MemStreamOpen( WaveImage, ( unsigned int )WaveSize ) ;
			PlayData->MemStream.ReadShred = *GetMemStreamDataShredStruct() ;
			SetupSoundConvert( &PlayData->ConvData, &PlayData->MemStream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
				,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
			) ;

			// �������A�h���X�̓���ւ�
			DXFREE( PlayData->FileImage ) ;
			PlayData->FileImage = WaveImage ;
			PlayData->FileImageSize = WaveSize ;

			// �X�g���[������t���O�𗧂Ă�
			*CanStreamCloseFlag = TRUE ;
		}
		break ;

	case DX_SOUNDDATATYPE_MEMPRESS :
		// �f�[�^�̓�������ɑ��݂��āA���k����Ă���ꍇ
		{
			// �ہX�������ɓǂݍ���
			if( StreamFullRead( Stream, &PlayData->FileImage, &PlayData->FileImageSize ) < 0 )
			{
				DXST_ERRORLOGFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����f�[�^�̓ǂݍ��݂Ɏ��s���܂���" @*/ )) ;
				goto ERR ;
			}

			// �W�J���ꂽ�f�[�^���X�g���[���Ƃ��čēx�J��				
			PlayData->MemStream.DataPoint = MemStreamOpen( PlayData->FileImage, ( unsigned int )PlayData->FileImageSize ) ;
			PlayData->MemStream.ReadShred = *GetMemStreamDataShredStruct() ;
			if( SetupSoundConvert( &PlayData->ConvData, &PlayData->MemStream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
									,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
									) < 0 )
			{
				MemStreamClose( PlayData->MemStream.DataPoint ) ;
				DXFREE( PlayData->FileImage ) ;
				PlayData->FileImage = NULL ;
				
				DXST_ERRORLOGFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����f�[�^�̃I�[�v���Ɏ��s���܂���" @*/ )) ;
				goto ERR ;
			}


			// �X�g���[������t���O�𗧂Ă�
			*CanStreamCloseFlag = TRUE ;
		}
		break ;

	case DX_SOUNDDATATYPE_FILE :
		// �f�[�^���t�@�C�����璀���ǂݍ���ł���ꍇ
		{
			// �g��Ȃ��f�[�^�� NULL ���Z�b�g���Ă���
			PlayData->FileImage = NULL ;
			PlayData->FileImageSize = 0 ;
			PlayData->MemStream.DataPoint = NULL ;

			// �W�J���ꂽ�f�[�^���X�g���[���Ƃ��čēx�J��				
			if( SetupSoundConvert( &PlayData->ConvData, Stream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
									,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
								) < 0 )
			{
				DXST_ERRORLOGFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xaa\x30\xfc\x30\xd7\x30\xf3\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����f�[�^�̃I�[�v���Ɏ��s���܂���" @*/ )) ;
				goto ERR ;
			}

			// �X�g���[������t���O��|��
			*CanStreamCloseFlag = FALSE ;
		}
		break ;
	}
	
	// ��ԍŏ��̃f�[�^�̏ꍇ�͏����𕪊�
	if( Sound->Stream.FileNum == 0 )
	{
		// �t�H�[�}�b�g���擾
		GetOutSoundFormatInfo( &PlayData->ConvData, &Sound->BufferFormat ) ;

		// �T�E���h�o�b�t�@�����L���邩�ǂ����𒲂ׂ�
		UniSound = NULL;
		while( UnionHandle != -1 )
		{
			// �����ȃn���h���̏ꍇ�͖���
			if( SOUNDHCHK( UnionHandle, UniSound ) )
			{
				UniSound = NULL;
				UnionHandle = -1;
				break;
			}

			// �t�H�[�}�b�g���Ⴄ���A�I���W�i���̃o�b�t�@���Ȃ����A�X�g���[���`���łȂ��ꍇ���A�E�g
			if( UniSound->Type != DX_SOUNDTYPE_STREAMSTYLE ||
				UniSound->Stream.BufferBorrowSoundHandle != -1 ||
				_MEMCMP( &Sound->BufferFormat, &UniSound->BufferFormat, sizeof( WAVEFORMATEX ) ) != 0 )
			{
				UniSound = NULL;
				UnionHandle = -1;
				break;
			}
			break;
		}

		// �T�E���h�o�b�t�@�����L���邩�ǂ����ŏ����𕪊�
		if( UniSound == NULL )
		{
			// �T�E���h�o�b�t�@���쐬
			CreateDirectSoundBuffer( &Sound->BufferFormat, SOUNDSIZE( STS_BUFSEC * Sound->BufferFormat.nAvgBytesPerSec / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ), DX_SOUNDTYPE_STREAMSTYLE, 1, SoundHandle, -1, ASyncThread ) ;
			Sound->Stream.SoundBufferSize = SOUNDSIZE( STS_BUFSEC * Sound->BufferFormat.nAvgBytesPerSec / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
			Sound->BaseFrequency          = ( int )Sound->BufferFormat.nSamplesPerSec ;

			// ���L�����Z�b�g
			Sound->Stream.BufferBorrowSoundHandle = -1 ;
		}
		else
		{
			// �N���e�B�J���Z�N�V�����̎擾
			CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

			// �X�g���[���T�E���h�n���h�����X�g�ɒǉ�
			Sound->Type = DX_SOUNDTYPE_STREAMSTYLE ;
			Sound->ValidBufferNum = 1 ;
			AddHandleList( &DSOUND.StreamSoundListFirst, &Sound->Stream.StreamSoundList, SoundHandle, Sound ) ;

			// �N���e�B�J���Z�N�V�����̉��
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

			// ���L�����Z�b�g
			Sound->Stream.BufferBorrowSoundHandle = UnionHandle ;
			UniSound->Stream.BufferBorrowSoundHandleCount ++ ;
		}
		Sound->Stream.BufferBorrowSoundHandleCount = 0 ;
		Sound->Stream.BufferUseSoundHandle = -1 ;
		Sound->Stream.DeleteWaitFlag = FALSE ;
	}
	else
	{
		// ��ڈȍ~�̏ꍇ�̓o�b�t�@�̃t�H�[�}�b�g�ƈ�v���Ă��邩���ׂ�
		GetOutSoundFormatInfo( &PlayData->ConvData, &Format ) ;
		if( _MEMCMP( &Format, &Sound->BufferFormat, sizeof( WAVEFORMATEX ) ) != 0 )
		{
			TerminateSoundConvert( &PlayData->ConvData ) ;
			if( PlayData->MemStream.DataPoint != NULL )
			{
				MemStreamClose( PlayData->MemStream.DataPoint ) ;
				PlayData->MemStream.DataPoint = NULL ;
			}
			if( PlayData->FileImage != NULL )
			{
				DXFREE( PlayData->FileImage ) ;
				PlayData->FileImage = NULL ;
			}

			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xd5\x30\xa9\x30\xfc\x30\xde\x30\xc3\x30\xc8\x30\x6e\x30\x55\x90\x46\x30\xf3\x97\xf0\x58\xc7\x30\xfc\x30\xbf\x30\x6f\x30\x0c\x54\x42\x66\x6b\x30\x7f\x4f\x46\x30\x53\x30\x68\x30\x6f\x30\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x00"/*@ L"�t�H�[�}�b�g�̈Ⴄ�����f�[�^�͓����Ɏg�����Ƃ͏o���܂���" @*/ )) ;
			goto ERR ;
		}
	}

	// ���[�v�̐���ۑ�
	PlayData->LoopNum = ( char )LoopNum ;

	// �v���C�f�[�^�̐��𑝂₷
	Sound->Stream.FileNum ++ ;
	
	// �X�g���[���f�[�^�̑��T���v�������X�V
	{
		int i ;
		Sound->Stream.TotalSample = 0 ;
		for( i = 0 ; i < Sound->Stream.FileNum ; i ++ )
			Sound->Stream.TotalSample += GetSoundConvertDestSize_Fast( &Sound->Stream.File[i].ConvData ) / Sound->BufferFormat.nBlockAlign ;
	}

	// �I��
	return 0 ;

ERR :

	return -1 ;
}

// �X�g���[�����T�E���h�f�[�^�ɃT�E���h�f�[�^��ǉ�����
extern int NS_AddStreamSoundMem( STREAMDATA *Stream, int LoopNum, int SoundHandle, int StreamDataType, int *CanStreamCloseFlag, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return AddStreamSoundMem_UseGParam( &GParam, Stream, LoopNum, SoundHandle, StreamDataType, CanStreamCloseFlag, UnionHandle, FALSE ) ;
}

// AddStreamSoundMemToFile �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int AddStreamSoundMemToFile_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *WaveFile,
	int LoopNum,
	int SoundHandle,
	int StreamDataType,
	int UnionHandle,
	int ASyncThread
)
{
	DWORD_PTR fp ;
	SOUND * Sound ;
	int CanStreamCloseFlag ;
	STREAMDATA Stream ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// �t�@�C�����J��
	fp = FOPEN( WaveFile ) ;
	if( fp == 0 )
	{
		DXST_ERRORLOGFMT_ADDW(( L"Sound File Open Error : %s \n" , WaveFile )) ;
		goto ERR ;
	}

	// �X�g���[���f�[�^�����Z�b�g����
	Stream.DataPoint = (void *)fp ;
	Stream.ReadShred = *GetFileStreamDataShredStruct() ;

	// �X�g���[���n���h����ǉ�
	if( AddStreamSoundMem_UseGParam( GParam, &Stream, LoopNum, SoundHandle, StreamDataType, &CanStreamCloseFlag, UnionHandle, ASyncThread ) == -1 )
	{
		FCLOSE( fp ) ;
		DXST_ERRORLOGFMT_ADDW(( L"Sound File Setup Error : %s \n" , WaveFile )) ;
		goto ERR ;
	}

	// �t�@�C������Ă��ǂ��Ƃ̂��B����������t�@�C�������
	if( CanStreamCloseFlag == TRUE )
	{
		FCLOSE( fp ) ;
	}

	// ���C�u�����Ń��[�h�����A�t���O�𗧂Ă�
	Sound->Stream.FileLibraryLoadFlag[ Sound->Stream.FileNum - 1 ] = TRUE ;

	// �I��
	return 0 ;

ERR :

	// �G���[�I��
	return -1 ;
}

// AddStreamSoundMemToMem �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int AddStreamSoundMemToMem_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const void *FileImageBuffer,
	int ImageSize,
	int LoopNum,
	int SoundHandle,
	int StreamDataType,
	int UnionHandle,
	int ASyncThread
)
{
	void *StreamHandle ;
	SOUND * Sound ;
	int CanStreamCloseFlag ;
	STREAMDATA Stream ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// �f�[�^�^�C�v���t�@�C������̒����ǂݍ��݂������ꍇ�̓������ɃC���[�W��u���A�ɕύX
	if( StreamDataType == DX_SOUNDDATATYPE_FILE ) StreamDataType = DX_SOUNDDATATYPE_MEMPRESS ;

	// �������C���[�W�X�g���[�����J��
	StreamHandle = MemStreamOpen( FileImageBuffer, ( unsigned int )ImageSize ) ;
	if( StreamHandle == NULL )
	{
		DXST_ERRORLOGFMT_ADDUTF16LE(( "\x37\xff\x21\xff\x36\xff\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x0a\x00\x00"/*@ L"�v�`�u�t�@�C���̃X�g���[���C���[�W�n���h���̍쐬�Ɏ��s���܂��� \n" @*/ )) ;
		goto ERR ;
	}

	// �X�g���[���f�[�^�����Z�b�g
	Stream.DataPoint = StreamHandle ;
	Stream.ReadShred = *GetMemStreamDataShredStruct() ;

	// �X�g���[���n���h����ǉ�
	if( AddStreamSoundMem_UseGParam( GParam, &Stream, LoopNum, SoundHandle, StreamDataType, &CanStreamCloseFlag, UnionHandle, ASyncThread ) == -1 )
	{
		DXST_ERRORLOGFMT_ADDUTF16LE(( "\x37\xff\x21\xff\x36\xff\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x0a\x00\x00"/*@ L"�v�`�u�t�@�C���̃X�g���[���C���[�W�̃Z�b�g�A�b�v�Ɏ��s���܂��� \n" @*/ )) ;
		goto ERR ;
	}

	// �X�g���[������Ă��ǂ��Ƃ̂��B����������X�g���[�������
	if( CanStreamCloseFlag == TRUE )
	{
		MemStreamClose( StreamHandle ) ;
	}

	// ���C�u�����Ń��[�h�����A�t���O�𗧂Ă�
	Sound->Stream.FileLibraryLoadFlag[ Sound->Stream.FileNum - 1 ] = TRUE ;

	// �I��
	return 0 ;

ERR:

	return -1 ;
}

// �X�g���[�����T�E���h�f�[�^�ɃT�E���h�f�[�^��ǉ�����
extern int NS_AddStreamSoundMemToFile( const TCHAR *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
#ifdef UNICODE
	return AddStreamSoundMemToFile_WCHAR_T(
		WaveFile, LoopNum, SoundHandle, StreamDataType, UnionHandle
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveFile, return -1 )

	Result = AddStreamSoundMemToFile_WCHAR_T(
		UseWaveFileBuffer, LoopNum, SoundHandle, StreamDataType, UnionHandle
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveFile )

	return Result ;
#endif
}

// �X�g���[�����T�E���h�f�[�^�ɃT�E���h�f�[�^��ǉ�����
extern int AddStreamSoundMemToFile_WCHAR_T( const wchar_t *WaveFile, int LoopNum,  int SoundHandle, int StreamDataType, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return AddStreamSoundMemToFile_UseGParam( &GParam, WaveFile, LoopNum, SoundHandle, StreamDataType, UnionHandle, FALSE ) ;
}

// �X�g���[�����T�E���h�f�[�^�ɃT�E���h�f�[�^��ǉ�����
extern int NS_AddStreamSoundMemToMem( const void *FileImageBuffer, int ImageSize, int LoopNum, int SoundHandle, int StreamDataType, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return AddStreamSoundMemToMem_UseGParam( &GParam, FileImageBuffer, ImageSize, LoopNum, SoundHandle, StreamDataType, UnionHandle, FALSE ) ;
}



// SetupStreamSoundMem �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int SetupStreamSoundMem_UseGParam( int SoundHandle, int ASyncThread )
{
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			goto ERR ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			goto ERR ;
	}

	// �X�g���[������Ȃ������牽�������I��
	if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE ) goto END ;

	// �Đ����������牽�������I��
	if( ASyncThread == FALSE && NS_CheckStreamSoundMem( SoundHandle ) == 1 ) goto END ;

	// �g�p���������ꍇ�͉��������I��
	if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) goto END ;

	// �������������Ă����牽�������I��
	if( Sound->Stream.StartSetupCompFlag == TRUE ) goto END ;

	// �Đ��ʒu��擪�ɃZ�b�g
//	SBuffer->SetCurrentPosition( 0 ) ;
	SoundBuffer_SetCurrentPosition( SBuffer, 0 ) ;

	// �p�����[�^��������
	Sound->Stream.SoundBufferCompCopyOffset	 = -800 ;
	Sound->Stream.SoundBufferCopyStartOffset = -1 ;
	Sound->Stream.PrevCheckPlayPosition      = 0 ;
	Sound->Stream.EndWaitFlag                = FALSE ;

	// ���k�f�[�^��W�J���鏀�����s��
	{
		STREAMFILEDATA *PlayData ;

		PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;
//		SetTimeSoundConvert( &PlayData->ConvData, Sound->Stream.FileCompCopyLength * 1000 / Sound->BufferFormat.nAvgBytesPerSec ) ;
		SetSampleTimeSoundConvert( &PlayData->ConvData, Sound->Stream.FileCompCopyLength / Sound->BufferFormat.nBlockAlign ) ;
	}

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// ��������
	Sound->Stream.StartSetupCompFlag = TRUE ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �ŏ��̃T�E���h�f�[�^����������
	ProcessStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;

END :

	// �I��
	return 0 ;

ERR :

	// �I��
	return 0 ;
}

// �X�g���[�����T�E���h�f�[�^�̍Đ��������s��
extern int NS_SetupStreamSoundMem( int SoundHandle )
{
	return SetupStreamSoundMem_UseGParam( SoundHandle, FALSE ) ;
}


// �X�g���[���T�E���h�n���h���̍Đ��������s��( -1:�G���[ 0:����I�� 1:�Đ�����K�v�Ȃ� )
static	int	_PlaySetupStreamSoundMem( int SoundHandle, SOUND * Sound, int PlayType, int TopPositionFlag, SOUNDBUFFER **DestSBuffer )
{
	SOUNDBUFFER *SBuffer ;
	int i ;

	// �g�p����T�E���h�o�b�t�@���擾
	_GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer );
	if( DestSBuffer ) *DestSBuffer = SBuffer ;
	
	// �Đ����œ������ł��Ȃ��ꍇ�͍����ŏI��
	if( TopPositionFlag == FALSE && NS_CheckStreamSoundMem( SoundHandle ) == 1 )
		return 1 ;

	// �Đ����̏ꍇ�͎~�߂�
	NS_StopStreamSoundMem( SoundHandle ) ;

	// �Đ��^�C�v��������番��
	if( Sound->PlayType != PlayType )
	{
		// �ꍇ�ɂ���Ă͓������Đ��ɂ���
		if( ( Sound->PlayType & DX_PLAYTYPE_LOOPBIT ) != ( PlayType & DX_PLAYTYPE_LOOPBIT ) &&
			Sound->Stream.AllPlayFlag == TRUE )
		{
			Sound->Stream.AllPlayFlag = FALSE ;
			Sound->Stream.FileCompCopyLength = 0 ;
			Sound->Stream.CompPlayWaveLength = 0 ;
			Sound->Stream.LoopPositionValidFlag = FALSE ;
			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive = 0 ;
			Sound->Stream.StartSetupCompFlag = FALSE ;		// �����t���O��|��
		}
	}

	// ������̍Đ��������ꍇ�̏���
	if( TopPositionFlag == TRUE )
	{
		Sound->Stream.AllPlayFlag = FALSE ;
		if( Sound->Stream.CompPlayWaveLength != 0 )
		{
			Sound->Stream.FileCompCopyLength = 0 ;
			Sound->Stream.CompPlayWaveLength = 0 ;
			Sound->Stream.LoopPositionValidFlag = FALSE ;
			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive = 0 ;
			Sound->Stream.StartSetupCompFlag = FALSE ;		// �����t���O��|��
		}
	}

	// �Đ��^�C�v��ۑ�
	Sound->PlayType = PlayType ;

	// �Đ�����
	NS_SetupStreamSoundMem( SoundHandle ) ;

	// �{�����[�����Z�b�g����
	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		if( Sound->ValidNextPlayVolume[ i ] )
		{
			SoundBuffer_SetVolumeAll( &Sound->Buffer[ 0 ], ( LONG )Sound->NextPlayVolume[ i ] ) ;
			Sound->ValidNextPlayVolume[ i ] = 0 ;
		}
		else
		{
			SoundBuffer_SetVolumeAll( &Sound->Buffer[ 0 ], ( LONG )Sound->BaseVolume[ i ] ) ;
		}
	}

	// �p�����Z�b�g����
	if( Sound->ValidNextPlayPan )
	{
		SoundBuffer_SetPan( &Sound->Buffer[ 0 ], ( LONG )Sound->NextPlayPan ) ;
		Sound->ValidNextPlayPan = 0 ;
	}
	else
	{
		SoundBuffer_SetPan( &Sound->Buffer[ 0 ], ( LONG )Sound->BasePan ) ;
	}

	// ���g�����Z�b�g����
	if( Sound->ValidNextPlayFrequency )
	{
		SoundBuffer_SetFrequency( &Sound->Buffer[ 0 ], ( DWORD )Sound->NextPlayFrequency ) ;
		Sound->ValidNextPlayFrequency = 0 ;
	}
	else
	{
		SoundBuffer_SetFrequency( &Sound->Buffer[ 0 ], ( DWORD )Sound->BaseFrequency ) ;
	}

	// �R�c�T�E���h�̏ꍇ�͂R�c�T�E���h�p�����[�^�̍X�V���s��
	if( Sound->Is3DSound != FALSE )
	{
		VECTOR *Position ;
		float Radius ;
		VECTOR *Velocity ;

		if( Sound->ValidNextPlay3DPosition )
		{
			Position = &Sound->NextPlay3DPosition ;
			Sound->ValidNextPlay3DPosition = 0 ;
		}
		else
		{
			Position = &Sound->Base3DPosition ;
		}

		if( Sound->ValidNextPlay3DRadius )
		{
			Radius = Sound->NextPlay3DRadius ;
			Sound->ValidNextPlay3DRadius = 0 ;
		}
		else
		{
			Radius = Sound->Base3DRadius ;
		}

		if( Sound->ValidNextPlay3DVelocity )
		{
			Velocity = &Sound->NextPlay3DVelocity ;
			Sound->ValidNextPlay3DVelocity = 0 ;
		}
		else
		{
			Velocity = &Sound->Base3DVelocity ;
		}
		SoundBuffer_Set3DPosition( &Sound->Buffer[ 0 ], Position ) ;
		SoundBuffer_Set3DRadius(   &Sound->Buffer[ 0 ], Radius ) ;
		SoundBuffer_Set3DVelocity( &Sound->Buffer[ 0 ], Velocity ) ;

		SoundBuffer_Refresh3DSoundParam( &Sound->Buffer[ 0 ] ) ;
	}

	// ����I��
	return 0 ;
}

// �T�E���h�n���h�����쐬����
static int _CreateSoundHandle( int Is3DSound, int NotInitSoundMemDelete, int ASyncThread )
{
	int SoundHandle ;
	SOUND *Sound ;

	SoundHandle = AddHandle( DX_HANDLETYPE_SOUND, ASyncThread, -1 ) ;
	if( SoundHandle == -1 )
		return -1 ;

	if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
		return -1 ;

	Sound->Is3DSound = Is3DSound ;
	Sound->NotInitSoundMemDelete = NotInitSoundMemDelete ;

	if( Sound->Is3DSound != FALSE )
	{
		// �R�c�T�E���h�`���̏ꍇ�͂R�c�T�E���h�n���h�����X�g�ɒǉ�
		AddHandleList( &DSOUND._3DSoundListFirst, &Sound->_3DSoundList, SoundHandle, Sound ) ;
	}

	return SoundHandle ;
}

// �X�g���[�����T�E���h�f�[�^�̍Đ��J�n
extern int NS_PlayStreamSoundMem( int SoundHandle , int PlayType, int TopPositionFlag )
{
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// �G���[�I��
		return -1 ;
	}

	// �Đ��̏���
	if( _PlaySetupStreamSoundMem( SoundHandle, Sound, PlayType, TopPositionFlag, &SBuffer ) != 0 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// �G���[�I��
		return -1 ;
	}

	// �Đ��J�n
//	SBuffer->Play( 0 , 0 , D_DSBPLAY_LOOPING ) ; 
	SoundBuffer_Play( SBuffer, TRUE ) ;

	// �R�c�T�E���h�̏ꍇ�͍Đ����̂R�c�T�E���h���X�g�ɒǉ�����
	if( Sound->Is3DSound != FALSE )
	{
		if( Sound->AddPlay3DSoundList == FALSE )
		{
			Sound->AddPlay3DSoundList = TRUE ;
			AddHandleList( &DSOUND.Play3DSoundListFirst, &Sound->Play3DSoundList, SoundHandle, Sound ) ;
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �Đ��X�e�[�^�X�ɂ���Ă͍Đ��I����҂�
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		while( NS_ProcessMessage() == 0 && NS_CheckSoundMem( SoundHandle ) == 1 ){ Sleep( 1 ) ; }
	}

	// �I��
	return 0 ;
}

// �X�g���[�����T�E���h�f�[�^�̍Đ���Ԃ𓾂�
extern int NS_CheckStreamSoundMem( int SoundHandle )
{
	DWORD State ;
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �T�E���h�o�b�t�@�̎g�p���������ꍇ�͏��Ȃ��Ƃ��Đ��͍s���Ă��Ȃ�
	if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return 0 ;

	// �V�X�e�����Ŏ~�߂Ă���ꍇ�͎~�߂�O�̏�Ԃ�Ԃ�
	if( Sound->BufferPlayStateBackupFlagValid[ 0 ] )
	{
		return Sound->BufferPlayStateBackupFlag[ 0 ] ? 1 : 0 ;
	}
	else
	{
		// �X�e�[�^�X���擾���Ē��ׂ�
//		SBuffer->GetStatus( &State ) ;
		SoundBuffer_GetStatus( SBuffer, &State ) ;
		if( State & D_DSBSTATUS_PLAYING ) return 1 ;
	}

	// �����܂ł��Ă���΍Đ��͂���Ă��Ȃ�
	return 0 ;
}



// �X�g���[�����T�E���h�f�[�^�̍Đ��I��
extern int NS_StopStreamSoundMem( int SoundHandle )
{
	SOUND *Sound, *UniSound = NULL ;
	DWORD State ;
	HRESULT hr ;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// �G���[�I��
		return -1 ;
	}

	// �Đ����Ԃ��X�V����
	RefreshStreamSoundPlayCompLength( SoundHandle ) ;

	// �n���h���̎g�p���������ꍇ�͏��Ȃ��Ƃ��Đ����ł͂Ȃ�
	if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		return 0 ;
	}
	
	// �Đ����̏ꍇ�͎~�߂�
	if( Sound->Stream.BufferBorrowSoundHandle != -1 )
	{
		if( SOUNDHCHK( Sound->Stream.BufferBorrowSoundHandle, UniSound ) == false )
		{
			UniSound->Stream.BufferUseSoundHandle = -1;
		}
	}

//	hr = SBuffer->GetStatus( &State ) ;
	hr = SoundBuffer_GetStatus( SBuffer, &State ) ;
	if( hr != D_DS_OK ) return -1 ;
	if( State & D_DSBSTATUS_PLAYING )
	{
//		SBuffer->Stop() ;
		SoundBuffer_Stop( SBuffer, TRUE ) ;

		Sound->BufferPlayStateBackupFlagValid[ 0 ] = FALSE ;
		Sound->BufferPlayStateBackupFlag[ 0 ] = FALSE ;
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̍Đ��ʒu���o�C�g�P�ʂŕύX����(�Đ����~�܂��Ă��鎞�̂ݗL��)
extern int NS_SetStreamSoundCurrentPosition( int Byte, int SoundHandle )
{
	SOUND * sd ;
//	STREAMFILEDATA * pl ;
//	int i, pos ;
//	int datasize ;
//	WAVEFORMATEX *wf ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// �G���[�I��
		return -1 ;
	}

	if( Byte / sd->BufferFormat.nBlockAlign > sd->Stream.TotalSample )
		goto ERR ;

	// �t�@�C������ȏ�̏ꍇ�͂܂����Ή�
	if( sd->Stream.FileNum > 1 ) goto ERR ;

/*
***	���k�f�[�^�̏ꍇ�͐���Ɍv�Z�ł��Ȃ����� ****

	// �Đ��ʒu�̎Z�o
	pl = sd->Stream.File ;
	pos = Byte ;
	for( i = 0 ; i < sd->Stream.FileNum ; i ++, pl ++ )
	{
		wf = &sd->BufferFormat ;
		datasize = pl->ConvData.DataSize ;
		if( pos < datasize ) break ;
		pos -= datasize ;
	}
	if( i == sd->Stream.FileNum ) return -1 ;
*/
	sd->Stream.FileLoopCount         = 0 ;
	sd->Stream.LoopPositionValidFlag = FALSE ;
//	sd->Stream.FileActive            = i ;
	sd->Stream.FileActive            = 0 ;
	sd->Stream.CompPlayWaveLength    = ( DWORD )Byte ;
	sd->Stream.FileCompCopyLength    = Byte ;

	// ���������t���O��|��
	sd->Stream.StartSetupCompFlag = FALSE ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;

ERR :
	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	return -1 ;
}

// �T�E���h�n���h���̍Đ��ʒu���o�C�g�P�ʂŎ擾����
extern int NS_GetStreamSoundCurrentPosition( int SoundHandle )
{
	SOUND * sd ;
//	STREAMFILEDATA * pl ;
	int /*i,*/ pos ;
//	WAVEFORMATEX *wf ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// �G���[�I��
		return -1 ;
	}

	// �Đ��ʒu�̎Z�o
/*	pl = sd->Stream.File ;
	pos = 0 ;
	for( i = 0 ; i < sd->Stream.FileNum ; i ++, pl ++ )
	{
		wf = &sd->BufferFormat ;
		pos += pl->ConvData.DataSize ;
	}
	pos += sd->Stream.FileCompCopyLength ;
*/
	// �Đ����Ԃ̍X�V
	RefreshStreamSoundPlayCompLength( SoundHandle ) ;

	// �Đ����Ԃ̎擾
	pos = ( int )sd->Stream.CompPlayWaveLength ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �l��Ԃ�
	return pos ;
}

// �T�E���h�n���h���̍Đ��ʒu���~���b�P�ʂŐݒ肷��(���k�`���̏ꍇ�͐������ݒ肳��Ȃ��ꍇ������)
extern int NS_SetStreamSoundCurrentTime( int Time, int SoundHandle )
{
	SOUND * sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �Đ��ʒu�̕ύX
	return NS_SetStreamSoundCurrentPosition( ( int )( MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, Time ) * sd->BufferFormat.nBlockAlign ), SoundHandle ) ;
}

// �T�E���h�n���h���̍Đ��ʒu���~���b�P�ʂŎ擾����(���k�`���̏ꍇ�͐������l���Ԃ��Ă��Ȃ��ꍇ������)
extern int NS_GetStreamSoundCurrentTime( int SoundHandle )
{
	SOUND * sd ;
//	STREAMFILEDATA * pl ;
	int /*i,*/ time ;
//	int datasize ;
//	WAVEFORMATEX *wf ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// �G���[�I��
		return -1 ;
	}

	// �Đ����Ԃ̍X�V
	RefreshStreamSoundPlayCompLength( SoundHandle ) ;

	// �Đ����Ԃ��~���b�P�ʂɕϊ�
//	time = _DTOL( (double)sd->Stream.CompPlayWaveLength / sd->BufferFormat.nAvgBytesPerSec * 1000 ) ;
	time = SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, ( int )( sd->Stream.CompPlayWaveLength / sd->BufferFormat.nBlockAlign ) ) ;
	
	// �Đ����Ԃ��ŏ��Ɏ擾�������̒��������������Ƃ�����̂�
	// �����Đ����Ԃ̕��������Ȃ��Ă��܂����ꍇ�͕␳����
	if( time > NS_GetSoundTotalTime( SoundHandle ) )
		time = NS_GetSoundTotalTime( SoundHandle ) ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �l��Ԃ�
	return time ;
}

// �T�E���h�n���h���̍Đ��������Ԃ��X�V����
extern int RefreshStreamSoundPlayCompLength( int SoundHandle, int CurrentPosition, int ASyncThread )
{
	SOUND * sd ;
	DWORD CurPosition ;
	int UpdateFlag ;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, sd ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, sd ) )
			return -1 ;
	}

	// �X�g���[���Đ��p�f�[�^�ł͂Ȃ��ꍇ�͉��������I��
	if( sd->Type != DX_SOUNDTYPE_STREAMSTYLE ) return 0 ;
	
	// �T�E���h�o�b�t�@�̎g�p���������Ȃ��ꍇ�͏��Ȃ��Ƃ��Đ����ł͂Ȃ�
	if( _GetDirectSoundBuffer( SoundHandle, sd, &SBuffer, false ) == -1 ) return 0 ;
	
	// �Đ��������������Ă��Ȃ������I��
	if( sd->Stream.StartSetupCompFlag == FALSE ) return 0 ;

	// ���݂̍Đ��ʒu���擾����
	if( CurrentPosition == -1 )
	{
//		SBuffer->GetCurrentPosition( &CurPosition , NULL );
		SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition , NULL ) ;
	}
	else
	{
		CurPosition = ( DWORD )CurrentPosition ;
	}

	// �O��̈ʒu�Ƃ̍��������Z����
	if( sd->Stream.PrevCheckPlayPosition != CurPosition )
	{
		UpdateFlag = 0 ;

		// ���[�v�w�肪����ꍇ�Ɩ����ꍇ�ŏ����𕪊�
		if( sd->Stream.LoopPositionValidFlag == TRUE )
		{
			if( sd->Stream.PrevCheckPlayPosition < CurPosition )
			{
				if( sd->Stream.LoopPosition > sd->Stream.PrevCheckPlayPosition &&
					sd->Stream.LoopPosition <= CurPosition )
				{
					sd->Stream.CompPlayWaveLength = CurPosition - sd->Stream.LoopPosition ;
					UpdateFlag = 1 ;
				}
			}
			else
			{
				if( sd->Stream.LoopPosition > sd->Stream.PrevCheckPlayPosition )
				{
					sd->Stream.CompPlayWaveLength = sd->Stream.SoundBufferSize - sd->Stream.LoopPosition + CurPosition ;
					UpdateFlag = 1 ;
				}
				else
				if( sd->Stream.LoopPosition < CurPosition )
				{
					sd->Stream.CompPlayWaveLength = CurPosition - sd->Stream.LoopPosition ;
					UpdateFlag = 1 ;
				}
			}
		}
		if( UpdateFlag == 1 )
		{
			sd->Stream.CompPlayWaveLength += sd->Stream.LoopAfterCompPlayWaveLength ;
			sd->Stream.LoopPositionValidFlag = FALSE ;
		}
		else
		{
			if( sd->Stream.PrevCheckPlayPosition < CurPosition )
			{
				sd->Stream.CompPlayWaveLength += CurPosition - sd->Stream.PrevCheckPlayPosition ;
			}
			else
			{
				sd->Stream.CompPlayWaveLength += ( sd->Stream.SoundBufferSize - sd->Stream.PrevCheckPlayPosition ) + CurPosition ;
			}
		}
		
		// ����擾�����ʒu���Z�b�g����
		sd->Stream.PrevCheckPlayPosition = CurPosition ;
	}

	// �I��
	return 0 ;
}

// �X�g���[���T�E���h�����p�X���b�h
static	DWORD WINAPI StreamSoundThreadFunction( void * )
{
	for(;;)
	{
		if( DSOUND.StreamSoundThreadEndFlag == 1 ) break ;
		if( DSOUND.InitializeFlag == FALSE ) break ;

		// �N���e�B�J���Z�N�V�����̎擾
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		// �X�g���[�~���O����
		NS_ProcessStreamSoundMemAll() ;

		// �Đ����I��������n���h�����폜���鏈�����s��
		ProcessPlayFinishDeleteSoundMemAll() ;

		// �R�c�T�E���h���Đ����Ă���T�E���h�n���h���ɑ΂��鏈�����s��
		ProcessPlay3DSoundMemAll() ;

		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;


		// �N���e�B�J���Z�N�V�����̎擾
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		// �X�g���[�~���O����
		ST_SoftSoundPlayerProcessAll() ;

		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		// �҂�
		Sleep( 10 ) ;
	}

	// �X���b�h�I��
	DSOUND.StreamSoundThreadEndFlag = 2 ;
	ExitThread( 0 ) ;

	return 0 ;
}


// ProcessStreamSoundMem �֐��̕⏕�֐�
// �����f�[�^����������
void NoneSoundDataCopy( SOUND * Sound, SOUNDBUFFERLOCKDATA *LockData, DWORD MoveLength )
{
	DWORD MLen, MOff ;
	BYTE *WOff ;

	MOff = 0 ;

	while( MoveLength != 0 )
	{
		MLen = MoveLength ;

		if( LockData->Valid != 0 )
		{
			WOff = LockData->WriteP + LockData->Offset ;

			if( MLen > LockData->Valid ) MLen = LockData->Valid ;

			if( Sound->BufferFormat.wBitsPerSample == 8 )	_MEMSET( WOff, 127, MLen ) ;
			else											_MEMSET( WOff, 0,   MLen ) ;

			LockData->Valid -= MLen ;
			LockData->Offset += MLen ;
			MoveLength -= MLen ;
			MOff += MLen ;
		}
		else
		{
			if( LockData->Valid2 == 0 ) break ;

			WOff = LockData->WriteP2 + LockData->Offset2 ;

			if( MLen > LockData->Valid2 ) MLen = LockData->Valid2 ;

			if( Sound->BufferFormat.wBitsPerSample == 8 )	_MEMSET( WOff, 127, MLen ) ;
			else											_MEMSET( WOff, 0,   MLen ) ;

			LockData->Valid2 -= MLen ;
			LockData->Offset2 += MLen ;
			MoveLength -= MLen ;
			MOff += MLen ;
		}
	}
}


// ProcessStreamSoundMem �֐��̕⏕�֐�
// �����f�[�^����������
int SoundDataCopy( SOUNDBUFFERLOCKDATA *LockData, SOUNDCONV *ConvData, DWORD MoveLength )
{
	DWORD MLen, MAllLen ;
	BYTE *WOff ;

	MAllLen = 0 ;

	while( MoveLength != 0 )
	{
		MLen = MoveLength ;

		if( LockData->Valid != 0 )
		{
			WOff = LockData->WriteP + LockData->Offset ;

			if( MLen > LockData->Valid ) MLen = LockData->Valid ;
			MLen = ( DWORD )RunSoundConvert( ConvData, WOff, ( int )MLen ) ;
			if( MLen == 0 ) break ;

			LockData->Valid -= MLen ;
			LockData->Offset += MLen ;
			MoveLength -= MLen ;
			MAllLen += MLen ;
		}
		else
		{
			WOff = LockData->WriteP2 + LockData->Offset2 ;

			if( MLen > LockData->Valid2 ) MLen = LockData->Valid2 ;
			MLen = ( DWORD )RunSoundConvert( ConvData, WOff, ( int )MLen ) ;
			if( MLen == 0 ) break ;

			LockData->Valid2 -= MLen ;
			LockData->Offset2 += MLen ;
			MoveLength -= MLen ;
			MAllLen += MLen ;
		}
	}
	
	return ( int )MAllLen ;
}

// ProcessStreamSoundMem�֐��̕⏕�֐�
// ���̃t�@�C���Ɉڂ邩�ǂ������`�F�b�N����
int StreamSoundNextData( SOUND * Sound, SOUNDBUFFERLOCKDATA *LockData, int CurrentPosition )
{
	STREAMFILEDATA *PlayData ;
		
	// ���[�v���𑝂₷
	Sound->Stream.FileLoopCount ++ ;

	// �������[�v���ǂ����ŏ����𕪊�
	if( Sound->Stream.File[Sound->Stream.FileActive].LoopNum == -1 )
	{
		WAVEFORMATEX *wfmt = &Sound->BufferFormat ;

		PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;

		// ���Ƀ��[�v�ʒu���Z�b�g���Ă���ꍇ�͈�x���[�v����܂ő҂�
		if( Sound->Stream.LoopPositionValidFlag == TRUE )
			return -1 ;

		// �Đ��ʒu���Z�b�g����
		Sound->Stream.FileCompCopyLength = PlayData->LoopSamplePosition * wfmt->nBlockAlign ;

		// �Đ��A�h���X�X�V�p�����Z�b�g
		Sound->Stream.LoopAfterCompPlayWaveLength = ( DWORD )( PlayData->LoopSamplePosition * wfmt->nBlockAlign ) ;
		Sound->Stream.LoopPositionValidFlag       = TRUE ;
		if( LockData->Valid == 0 )
		{
			Sound->Stream.LoopPosition = LockData->Length2 - LockData->Valid2 ;
		}
		else
		{
			Sound->Stream.LoopPosition = LockData->StartOffst + LockData->Length - LockData->Valid ;
		}
	}
	else
	{
		// ���[�v�񐔂��K��l�ɂ��Ă����玟�̃f�[�^��
		if( Sound->Stream.FileLoopCount > Sound->Stream.File[Sound->Stream.FileActive].LoopNum  )
		{
			// ���[�v�Đ��w��Ń��[�v����������������ꍇ�A���Ƀ��[�v�ʒu���Z�b�g���Ă���ꍇ�͈�x���[�v����܂ő҂�
			if( Sound->Stream.FileNum - 1 == Sound->Stream.FileActive &&
				Sound->PlayType == DX_PLAYTYPE_LOOP &&
				Sound->Stream.LoopPositionValidFlag == TRUE )
				return -1 ;

			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive ++ ;
			if( Sound->Stream.FileNum == Sound->Stream.FileActive )
			{
				Sound->Stream.AllPlayFlag = TRUE ;
			
				if( Sound->PlayType == DX_PLAYTYPE_LOOP )
				{
					Sound->Stream.FileActive = 0  ;
	//				Sound->Stream.CompPlayWaveLength = 0 ;

					Sound->Stream.LoopAfterCompPlayWaveLength = 0 ;
					Sound->Stream.LoopPositionValidFlag = TRUE ;
					if( LockData->Valid == 0 )
					{
						Sound->Stream.LoopPosition = LockData->Length2 - LockData->Valid2 ;
					}
					else
					{
						Sound->Stream.LoopPosition = LockData->StartOffst + LockData->Length - LockData->Valid ;
					}
				}
				else
				{
					// �I���t���O�𗧂Ă�
					Sound->Stream.EndWaitFlag = TRUE ;

					// �I���I�t�Z�b�g���Z�b�g����
					if( LockData->Valid == 0 )
					{
						Sound->Stream.EndOffset = LockData->Length2 - LockData->Valid2 ;
					}
					else
					{
						Sound->Stream.EndOffset = LockData->StartOffst + LockData->Length - LockData->Valid ;
					}
					Sound->Stream.EndStartOffset = ( DWORD )CurrentPosition ;
					NoneSoundDataCopy( Sound, LockData, ( DWORD )AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec / STS_ONECOPYSEC ), Sound ) ) ;

					return 0 ;
				}
			}
		}

		// �Đ����̏�����
		Sound->Stream.FileCompCopyLength = 0 ;

	}

	// ���k�f�[�^��W�J���鏀�����s��
	PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;
//	SetTimeSoundConvert( &PlayData->ConvData,  Sound->Stream.FileCompCopyLength * 1000 / Sound->BufferFormat.nAvgBytesPerSec ) ;
	SetSampleTimeSoundConvert( &PlayData->ConvData,  Sound->Stream.FileCompCopyLength / Sound->BufferFormat.nBlockAlign ) ;

	// �I��
	return 0 ;
}


// ProcessStreamSoundMem �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int ProcessStreamSoundMem_UseGParam( int SoundHandle, int ASyncThread )
{
	SOUND * Sound ;
	STREAMFILEDATA *PlayData ;
	DWORD State ;
	DWORD CurPosition = 0 ;
	SOUNDBUFFERLOCKDATA LockData ;
	int MoveByte, MoveByte2, MoveStartOffset ;
	int MoveB ;
	SOUNDBUFFER *SBuffer ;
	int BreakFlag ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;
	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �G���[�`�F�b�N
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
			return -1 ;
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
			return -1 ;
	}

	// �Đ��`�����X�g���[���^�C�v����Ȃ������炱���ŏI��
	if( Sound->Type != DX_SOUNDTYPE_STREAMSTYLE ) return 0 ;
	
	// �T�E���h�o�b�t�@�̎g�p�����Ȃ��ꍇ�������ŏI��
	if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return 0 ;
	
	// �Đ��������������Ă��Ȃ������牽�������I��
	if( Sound->Stream.StartSetupCompFlag == FALSE ) return 0 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �ēx�n���h���`�F�b�N
	if( ASyncThread )
	{
		if( SOUNDHCHK_ASYNC( SoundHandle, Sound ) )
		{
			// �N���e�B�J���Z�N�V�����̉��
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

			// �G���[�I��
			return -1 ;
		}
	}
	else
	{
		if( SOUNDHCHK( SoundHandle, Sound ) )
		{
			// �N���e�B�J���Z�N�V�����̉��
			CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

			// �G���[�I��
			return -1 ;
		}
	}

	// �Đ��������Ԃ̍X�V
	RefreshStreamSoundPlayCompLength( SoundHandle, -1, ASyncThread ) ;

	// �G���h�t���O�������Ă���ꍇ�͏I�����菈�����s��
	if( Sound->Stream.EndWaitFlag )
	{
		// ���݂̍Đ��ʒu�𓾂�
//		SBuffer->GetCurrentPosition( &CurPosition , NULL );
		SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition, NULL ) ;
  
		// �I������
		if(
		    (
			  Sound->Stream.EndStartOffset   < Sound->Stream.EndOffset &&
			  (
			    Sound->Stream.EndStartOffset > CurPosition ||
				Sound->Stream.EndOffset      < CurPosition
			  )
			) ||
			(
			  Sound->Stream.EndOffset        < CurPosition &&
			  Sound->Stream.EndStartOffset   > CurPosition &&
			  Sound->Stream.EndStartOffset   > Sound->Stream.EndOffset
			)
		  )
		{
			// �Đ����Ƃ߂�
//			SBuffer->Stop() ;
			SoundBuffer_Stop( SBuffer ) ;

			// �G���h�t���O��|��
			Sound->Stream.EndWaitFlag = FALSE ;

			// �Đ����ԏ�������������
			Sound->Stream.FileCompCopyLength = 0 ;
			Sound->Stream.CompPlayWaveLength = 0 ;
			Sound->Stream.LoopPositionValidFlag = FALSE ;
			Sound->Stream.FileLoopCount = 0 ;
			Sound->Stream.FileActive = 0 ;
			Sound->Stream.StartSetupCompFlag = FALSE ;		// �����t���O��|��
		}
		else
		{
			MoveByte = ( int )AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec / STS_ONECOPYSEC ), Sound ) ;
			MoveStartOffset = Sound->Stream.SoundBufferCompCopyOffset ;

			// �]�����ׂ����ǂ����̔���
			{
				DWORD C, S ;

//				SBuffer->GetCurrentPosition( &CurPosition , NULL );
				SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition, NULL ) ;

				C = ( DWORD )Sound->Stream.SoundBufferCompCopyOffset ;
				S = ( DWORD )Sound->Stream.SoundBufferCopyStartOffset ;
				if( ( C < S && ( S > CurPosition && C < CurPosition ) ) || 
					( C > S && ( S > CurPosition || C < CurPosition ) ) )
				{
					goto ERR ;
				}
			}

			MoveB = MoveByte ;

			// ���b�N����
			{
				LockData.StartOffst = ( DWORD )MoveStartOffset ;

				if( Sound->Stream.SoundBufferCompCopyOffset > (int)SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_BUFSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) )
					Sound->Stream.SoundBufferCompCopyOffset = Sound->Stream.SoundBufferCompCopyOffset ;

//				SBuffer->Lock( MoveStartOffset, MoveByte,
//								( void ** )&(LockData.WriteP), &LockData.Length,
//								( void ** )&(LockData.WriteP2), &LockData.Length2, 0 ) ; 
				SoundBuffer_Lock( SBuffer, ( DWORD )MoveStartOffset, ( DWORD )MoveByte,
								( void ** )&(LockData.WriteP), &LockData.Length,
								( void ** )&(LockData.WriteP2), &LockData.Length2 ) ; 
				LockData.Offset  = 0 ;
				LockData.Offset2 = 0 ;
				LockData.Valid  = LockData.Length ;
				LockData.Valid2 = LockData.Length2 ;
			}

			// �f�[�^�]������
			while( MoveByte != 0 )
			{
				// �g�p����f�[�^���Z�b�g
				PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;

				// �]���o�C�g�̃Z�b�g
				MoveByte2 = MoveByte ;

				// �]��
				NoneSoundDataCopy( Sound, &LockData, ( DWORD )MoveByte2 ) ;
				MoveByte -= MoveByte2 ;
			}

			// ���b�N����
//			SBuffer->Unlock( ( void * )LockData.WriteP, LockData.Length,
//							( void * )LockData.WriteP2, LockData.Length2 ) ; 
			SoundBuffer_Unlock( SBuffer, ( void * )LockData.WriteP, LockData.Length,
										( void * )LockData.WriteP2, LockData.Length2 ) ; 

			// ���ɓ]���������s���I�t�Z�b�g�̕ύX
			{
				Sound->Stream.SoundBufferCompCopyOffset += MoveB ;
				if( Sound->Stream.SoundBufferCompCopyOffset >= (int)SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_BUFSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) )
					Sound->Stream.SoundBufferCompCopyOffset -= (int)SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_BUFSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;

				Sound->Stream.SoundBufferCopyStartOffset = ( int )( Sound->Stream.SoundBufferCompCopyOffset - SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_SAKICOPYSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ) ;
				if( Sound->Stream.SoundBufferCopyStartOffset < 0 )
					Sound->Stream.SoundBufferCopyStartOffset += SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_BUFSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
			}
		}

		// �I��
		goto END ;
	}
	else
	{
		// �]����������
		{
			// ���݉��t����Ă��邩�`�F�b�N
			State = 0 ;
			//SBuffer->GetStatus( &State ) ;
			SoundBuffer_GetStatus( SBuffer, &State ) ;

			// ���������ł��Ȃ��A���t���ł��Ȃ��ꍇ�͂����ŏI��
			if( ( State & D_DSBSTATUS_PLAYING ) == 0 )
			{
				if( Sound->Stream.SoundBufferCompCopyOffset == -800 )
				{
					MoveByte = AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec * STS_SAKICOPYSEC / STS_DIVNUM ), Sound ) ;
					MoveStartOffset = 0 ;
				}
				else
				{
					goto END ;
				}
			}
			else
			{
				DWORD SakiyomiSec ;
				MoveByte = AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec / STS_ONECOPYSEC ), Sound ) ;
				MoveStartOffset = Sound->Stream.SoundBufferCompCopyOffset ;

				// �]�����ׂ����ǂ����̔���A���݂̐�ǂ݂̕b���̎Z�o
				{
					DWORD C, S ;

//					SBuffer->GetCurrentPosition( &CurPosition , NULL );
					SoundBuffer_GetCurrentPosition( SBuffer, &CurPosition, NULL ) ;
					
					C = ( DWORD )Sound->Stream.SoundBufferCompCopyOffset ;
					S = ( DWORD )Sound->Stream.SoundBufferCopyStartOffset ;
					if( ( C < S && ( S > CurPosition && C < CurPosition ) ) || 
						( C > S && ( S > CurPosition || C < CurPosition ) ) )
					{
						goto ERR ;
					}

					if( C < S )
					{
						if( CurPosition < C ) CurPosition += SOUNDSIZE( STS_BUFSEC * Sound->BufferFormat.nAvgBytesPerSec / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
						C += SOUNDSIZE( STS_BUFSEC * Sound->BufferFormat.nAvgBytesPerSec / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
					}
					SakiyomiSec = ( C - CurPosition ) * 0x200 / Sound->BufferFormat.nAvgBytesPerSec ;
				}

				// ��ǂݕb�����K���������Ă�����K��̕b���܂ň�C�ɓ]��
				if( SakiyomiSec < STS_MINSAKICOPYSEC * 0x200 / STS_DIVNUM )
				{
					MoveByte = AdjustSoundDataBlock( ( int )( Sound->BufferFormat.nAvgBytesPerSec * ( STS_MINSAKICOPYSEC * 0x200 / STS_DIVNUM - SakiyomiSec ) / 0x200 ), Sound ) ;
				}
			}
		}

		MoveB = MoveByte ;

		// ���b�N����
		{
			LockData.StartOffst = ( DWORD )MoveStartOffset ;

//			SBuffer->Lock( MoveStartOffset, MoveByte,
//							( void ** )&(LockData.WriteP), &LockData.Length,
//							( void ** )&(LockData.WriteP2), &LockData.Length2, 0 ) ; 
			SoundBuffer_Lock( SBuffer, ( DWORD )MoveStartOffset, ( DWORD )MoveByte,
							( void ** )&(LockData.WriteP), &LockData.Length,
							( void ** )&(LockData.WriteP2), &LockData.Length2 ) ; 
			LockData.Offset  = 0 ;
			LockData.Offset2 = 0 ;
			LockData.Valid  = LockData.Length ;
			LockData.Valid2 = LockData.Length2 ;
		}

		// �f�[�^�]������
		while( MoveByte != 0 && Sound->Stream.EndWaitFlag == FALSE )
		{
			// �g�p����f�[�^���Z�b�g
			PlayData = &Sound->Stream.File[ Sound->Stream.FileActive ] ;

			// �]���o�C�g�̃Z�b�g
			MoveByte2 = MoveByte ;

			// ���[�v�J�n�ʒu���ݒ肳��Ă���ꍇ�͂��̈ʒu����܂ł̓f�[�^���Z�b�g���Ȃ�
			if( PlayData->LoopStartSamplePosition != -1 && MoveByte2 + Sound->Stream.FileCompCopyLength > PlayData->LoopStartSamplePosition * Sound->BufferFormat.nBlockAlign )
			{
				MoveByte2 = PlayData->LoopStartSamplePosition * Sound->BufferFormat.nBlockAlign - Sound->Stream.FileCompCopyLength ;
				if( MoveByte2 <= 0 )
					MoveByte2 = 0 ;
			}

			// �]��
			MoveByte2 = SoundDataCopy( &LockData, &PlayData->ConvData, ( DWORD )MoveByte2 ) ;

			// �]���ʂ��O�o�C�g�̏ꍇ�͎��̃t�@�C���Ɉڂ�
			BreakFlag = FALSE ;
			if( MoveByte2 == 0 )
			{
//				CurPosition = 0;
				if( StreamSoundNextData( Sound, &LockData, ( int )CurPosition ) < 0 )
				{
					BreakFlag = TRUE ;
				}
			}
			else
			{
				MoveByte -= MoveByte2 ;
				Sound->Stream.FileCompCopyLength += MoveByte2 ;
			}

			// ���[�v���ׂ��ʒu�ɗ��Ă���̂Ɏ��̍Đ��f�[�^�Ɉڍs���Ă��Ȃ��ꍇ�͍Đ����[�v�҂�( Sound->Stream.LoopPositionValidFlag �� TRUE )�̏�ԂȂ̂ŁA���[�v���甲����
			if( ( PlayData == &Sound->Stream.File[ Sound->Stream.FileActive ] &&
				  PlayData->LoopStartSamplePosition != -1 &&
				  Sound->Stream.FileCompCopyLength >= PlayData->LoopStartSamplePosition * Sound->BufferFormat.nBlockAlign ) ||
				BreakFlag )
				break ;
		}
		MoveB -= MoveByte ;

		// ���b�N����
//		SBuffer->Unlock( ( void * )LockData.WriteP,  LockData.Length,
//						  ( void * )LockData.WriteP2, LockData.Length2 ) ; 
		SoundBuffer_Unlock( SBuffer, ( void * )LockData.WriteP,  LockData.Length,
						  			( void * )LockData.WriteP2, LockData.Length2 ) ; 

		// ���ɓ]���������s���I�t�Z�b�g�̕ύX
		if( Sound->Stream.SoundBufferCompCopyOffset == -800 )
		{
//			Sound->Stream.SoundBufferCompCopyOffset = SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_SAKICOPYSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
			Sound->Stream.SoundBufferCompCopyOffset = MoveB ;
			Sound->Stream.SoundBufferCopyStartOffset = 0 ;
		}
		else
		{
			Sound->Stream.SoundBufferCompCopyOffset += MoveB ;
			if( Sound->Stream.SoundBufferCompCopyOffset >= (int)SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_BUFSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) )
				Sound->Stream.SoundBufferCompCopyOffset -= (int)SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_BUFSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;

			Sound->Stream.SoundBufferCopyStartOffset = ( int )( Sound->Stream.SoundBufferCompCopyOffset - SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_SAKICOPYSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ) ;
			if( Sound->Stream.SoundBufferCopyStartOffset < 0 )
				Sound->Stream.SoundBufferCopyStartOffset += SOUNDSIZE( Sound->BufferFormat.nAvgBytesPerSec * STS_BUFSEC / STS_DIVNUM, Sound->BufferFormat.nBlockAlign ) ;
		}
	}

END :

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;

ERR :

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[�I��
	return -1 ;
}

// �S�Ẳ��̈ꎞ��~��Ԃ�ύX����
extern int PauseSoundMemAll( int PauseFlag )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	DWORD State ;
	int i ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// ��~���邩�J�n���邩�ŏ����𕪊�
	if( PauseFlag )
	{
		// ��~����ꍇ

		for( List = HandleManageArray[ DX_HANDLETYPE_SOUND ].ListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			Sound = ( SOUND * )List->Data ;

			for( i = 0 ; i < MAX_SOUNDBUFFER_NUM ; i ++ )
			{
				if( Sound->Buffer[ i ].Valid == FALSE || Sound->Buffer[ i ].DSBuffer == NULL )
					continue ;

				// ���ɏ�ԕۑ��ς݂̏ꍇ�͉������Ȃ�
				if( Sound->BufferPlayStateBackupFlagValid[ i ] )
					continue ;

				// �T�E���h�o�b�t�@�̍Đ���Ԃ�ۑ�
				if( SoundBuffer_GetStatus( &Sound->Buffer[ i ], &State ) == D_DS_OK )
				{
					Sound->BufferPlayStateBackupFlagValid[ i ] = TRUE ;

					if( State & D_DSBSTATUS_PLAYING )
					{
						Sound->BufferPlayStateBackupFlag[ i ] = TRUE ;

						// �Đ�����Ă�����Đ����~�߂�
						SoundBuffer_Stop( &Sound->Buffer[ i ], TRUE ) ;
					}
					else
					{
						Sound->BufferPlayStateBackupFlag[ i ] = FALSE ;
					}
				}
				else
				{
					Sound->BufferPlayStateBackupFlagValid[ i ] = FALSE ;
				}
			}
		}
	}
	else
	{
		// �ĊJ����ꍇ

		for( List = HandleManageArray[ DX_HANDLETYPE_SOUND ].ListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			Sound = ( SOUND * )List->Data ;

			for( i = 0 ; i < MAX_SOUNDBUFFER_NUM ; i ++ )
			{
				if( Sound->Buffer[ i ].Valid == FALSE || Sound->Buffer[ i ].DSBuffer == NULL ) continue ;

				// �T�E���h�o�b�t�@�̍Đ���Ԃ��L���ŁA���Đ����Ă����ꍇ�͍Đ����ĊJ����
				if( Sound->BufferPlayStateBackupFlagValid[ i ] &&
					Sound->BufferPlayStateBackupFlag[ i ] )
				{
					SoundBuffer_Play( &Sound->Buffer[ i ], Sound->Buffer[ i ].Loop ) ;
				}

				Sound->BufferPlayStateBackupFlagValid[ i ] = FALSE ;
			}
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �X�g���[���T�E���h�̍Đ������֐�
extern int NS_ProcessStreamSoundMem( int SoundHandle )
{
	return ProcessStreamSoundMem_UseGParam( SoundHandle, FALSE ) ;
}

// �L���ȃX�g���[���T�E���h�̂��ׂčĐ������֐��ɂ�����
extern int NS_ProcessStreamSoundMemAll( void )
{
	HANDLELIST *List ;
	SOUND *Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	for( List = DSOUND.StreamSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		// �Đ��������������Ă��Ȃ������牽����������
		Sound = ( SOUND * )List->Data ;
		if( Sound->Stream.StartSetupCompFlag == FALSE ) continue ;

		NS_ProcessStreamSoundMem( List->Handle ) ;
	}

/*	int i , Num ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	Num = 0 ;
	for( i = 0 ; i < MAX_SOUND_NUM && Num < DSOUND.SoundNum ; i ++ )
	{
		if( DSOUND.Sound[ i ] )
		{
			if( DSOUND.Sound[ i ]->Type == DX_SOUNDTYPE_STREAMSTYLE )
				NS_ProcessStreamSoundMem( i | DX_HANDLETYPE_MASK_SOUND | ( DSOUND.Sound[ i ]->ID << DX_HANDLECHECK_ADDRESS ) ) ;
			Num ++ ;
		}
	}
*/

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �Đ����̂��ׂĂ̂R�c�T�E���h�̃p�����[�^���X�V����
extern int Refresh3DSoundParamAll()
{
	HANDLELIST *List ;
	SOUND *Sound ;
	DWORD State ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	for( List = DSOUND._3DSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		// �o�b�t�@�[�̐������J��Ԃ�
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			// �p�����[�^�X�V�\��
			Sound->Buffer[ i ].EmitterDataChangeFlag = TRUE ;

			// �Đ����������瑦���ɍX�V
			SoundBuffer_GetStatus( &Sound->Buffer[ i ], &State ) ;
			if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
			{
				SoundBuffer_Refresh3DSoundParam( &Sound->Buffer[ i ] ) ;
			}
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �Đ����I��������T�E���h�n���h�����폜����T�E���h�̏������s��
extern int ProcessPlayFinishDeleteSoundMemAll( void )
{
	HANDLELIST *List ;
//	SOUND *Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

LOOPSTART:

	for( List = DSOUND.PlayFinishDeleteSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
//		Sound = ( SOUND * )List->Data ;

		// �Đ����ł͂Ȃ�������폜
		if( NS_CheckSoundMem( List->Handle ) == 0 )
		{
			NS_DeleteSoundMem( List->Handle ) ;
			goto LOOPSTART ;
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �R�c�T�E���h���Đ����Ă���T�E���h�n���h���ɑ΂��鏈�����s��
extern int ProcessPlay3DSoundMemAll( void )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;
	int Valid ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

LOOPSTART:
	for( List = DSOUND.Play3DSoundListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		Valid = FALSE ;
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			if( Sound->Buffer[ i ].Valid == 0 )
			{
				continue ;
			}

			if( SoundBuffer_CycleProcess( &Sound->Buffer[ i ] ) == D_DS_OK )
			{
				Valid = TRUE ;
			}
		}

		if( Valid == FALSE )
		{
			Sound->AddPlay3DSoundList = FALSE ;
			SubHandleList( &Sound->Play3DSoundList ) ;
			goto LOOPSTART ;
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// LoadSoundMemBase �̎������֐�
extern int LoadSoundMem2_Static(
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const wchar_t *WaveName1,
	const wchar_t *WaveName2,
	int ASyncThread
)
{
	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	// ��ڂ̃T�E���h�f�[�^�̒ǉ�
	if( AddStreamSoundMemToFile_UseGParam( GParam, WaveName1, 0,  SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// �Q�ڂ̃T�E���h�f�[�^�̒ǉ�
	if( AddStreamSoundMemToFile_UseGParam( GParam, WaveName2, -1, SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// �Đ�����
	SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;

	// ����I��
	return 0 ;

ERR :
	// �G���[�I��
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD

// LoadSoundMem2 �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadSoundMem2_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoundHandle ;
	const wchar_t *WaveName1 ;
	const wchar_t *WaveName2 ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	WaveName1 = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	WaveName2 = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadSoundMem2_Static( GParam, SoundHandle, WaveName1, WaveName2, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoundHandle ) ;
	}
}

#endif // DX_NON_ASYNCLOAD

// LoadSoundMem2 �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int LoadSoundMem2_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *WaveName1,
	const wchar_t *WaveName2,
	int ASyncLoadFlag
)
{
	int SoundHandle = -1 ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, FALSE ) ;
	if( SoundHandle == -1 )
		goto ERR ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath1[ 1024 ] ;
		wchar_t FullPath2[ 1024 ] ;

		ConvertFullPathW_( WaveName1, FullPath1 ) ;
		ConvertFullPathW_( WaveName2, FullPath2 ) ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath1 ) ; 
		AddASyncLoadParamString( NULL, &Addr, FullPath2 ) ; 

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadSoundMem2_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath1 ) ; 
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath2 ) ; 

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMem2_Static( GParam, SoundHandle, WaveName1, WaveName2, FALSE ) < 0 )
			goto ERR ;
	}

	// �n���h����Ԃ�
	return SoundHandle ;

ERR :
	SubHandle( SoundHandle ) ;
	SoundHandle = -1 ;

	// �I��
	return -1 ;
}

// �O�t���ƃ��[�v���ɕ����ꂽ�T�E���h�f�[�^�̍쐬
extern int NS_LoadSoundMem2( const TCHAR *WaveName1 , const TCHAR *WaveName2 )
{
#ifdef UNICODE
	return LoadSoundMem2_WCHAR_T(
		WaveName1 , WaveName2
	) ;
#else
	int Result = -1 ;

	TCHAR_TO_WCHAR_T_STRING_BEGIN( WaveName1 )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( WaveName2 )

	TCHAR_TO_WCHAR_T_STRING_SETUP( WaveName1, goto ERR )
	TCHAR_TO_WCHAR_T_STRING_SETUP( WaveName2, goto ERR )

	Result = LoadSoundMem2_WCHAR_T(
		UseWaveName1Buffer , UseWaveName2Buffer
	) ;

ERR :

	TCHAR_TO_WCHAR_T_STRING_END( WaveName1 )
	TCHAR_TO_WCHAR_T_STRING_END( WaveName2 )

	return Result ;
#endif
}

// �O�t���ƃ��[�v���ɕ����ꂽ�T�E���h�f�[�^�̍쐬
extern int LoadSoundMem2_WCHAR_T( const wchar_t *WaveName1 , const wchar_t *WaveName2 )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMem2_UseGParam( &GParam, WaveName1, WaveName2, GetASyncLoadFlag() ) ;
}

// ��ɂa�f�l��ǂݍ��ނ̂ɓK�����֐�
extern int NS_LoadBGM( const TCHAR *WaveName )
{
#ifdef UNICODE
	return LoadBGM_WCHAR_T(
		WaveName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadBGM_WCHAR_T(
		UseWaveNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// ��ɂa�f�l��ǂݍ��ނ̂ɓK�����֐�
extern int LoadBGM_WCHAR_T( const wchar_t *WaveName )
{
	int Type = DSOUND.CreateSoundDataType, SoundHandle ;

	if( _WCSICMP( WaveName + _WCSLEN( WaveName ) - 3, L"wav" ) == 0 )
	{
		NS_SetCreateSoundDataType( DX_SOUNDDATATYPE_FILE ) ;
	}
	else
	{
		NS_SetCreateSoundDataType( DX_SOUNDDATATYPE_MEMPRESS ) ;
	}

	SoundHandle = LoadSoundMem_WCHAR_T( WaveName, 1 ) ;
	NS_SetCreateSoundDataType( Type ) ;

	return SoundHandle ;
}

// LoadSoundMem2ByMemImage �̎������֐�
static int LoadSoundMem2ByMemImage_Static( 
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const void *FileImageBuffer1,
	int ImageSize1,
	const void *FileImageBuffer2,
	int ImageSize2,
	int ASyncThread
)
{
	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	// ��ڂ̃T�E���h�f�[�^�̒ǉ�
	if( AddStreamSoundMemToMem_UseGParam( GParam, FileImageBuffer1, ImageSize1,  0, SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// �Q�ڂ̃T�E���h�f�[�^�̒ǉ�
	if( AddStreamSoundMemToMem_UseGParam( GParam, FileImageBuffer2, ImageSize2, -1, SoundHandle, GParam->CreateSoundDataType, -1, ASyncThread ) == -1 ) goto ERR ;

	// �Đ�����
	SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;

	// ����I��
	return 0 ;

ERR :
	// �G���[�I��
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadSoundMem2ByMemImage �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadSoundMem2ByMemImage_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoundHandle ;
	const void *FileImageBuffer1 ;
	int ImageSize1 ;
	const void *FileImageBuffer2 ;
	int ImageSize2 ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImageBuffer1 = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ImageSize1 = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImageBuffer2 = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ImageSize2 = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoundMem2ByMemImage_Static( GParam, SoundHandle, FileImageBuffer1, ImageSize1, FileImageBuffer2, ImageSize2, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadSoundMem2ByMemImage �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int LoadSoundMem2ByMemImage_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const void *FileImageBuffer1,
	int ImageSize1,
	const void *FileImageBuffer2,
	int ImageSize2,
	int ASyncLoadFlag
)
{
	int SoundHandle = -1 ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, FALSE ) ;
	if( SoundHandle == -1 )
		goto ERR ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImageBuffer1 ) ; 
		AddASyncLoadParamInt( NULL, &Addr, ImageSize1 ) ; 
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImageBuffer2 ) ; 
		AddASyncLoadParamInt( NULL, &Addr, ImageSize2 ) ; 

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadSoundMem2ByMemImage_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImageBuffer1 ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, ImageSize1 ) ; 
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImageBuffer2 ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, ImageSize2 ) ; 

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMem2ByMemImage_Static( GParam, SoundHandle, FileImageBuffer1, ImageSize1, FileImageBuffer2, ImageSize2, FALSE ) < 0 )
			goto ERR ;
	}

	// �n���h����Ԃ�
	return SoundHandle ;

ERR :
	SubHandle( SoundHandle ) ;
	SoundHandle = -1 ;

	// �I��
	return -1 ;
}

// �O�t���ƃ��[�v���ɕ����ꂽ�T�E���h�f�[�^�̍쐬
extern int NS_LoadSoundMem2ByMemImage( const void *FileImageBuffer1, int ImageSize1, const void *FileImageBuffer2, int ImageSize2 )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMem2ByMemImage_UseGParam( &GParam, FileImageBuffer1, ImageSize1, FileImageBuffer2, ImageSize2, GetASyncLoadFlag() ) ;
}


	
// LOADSOUND_GPARAM �̃f�[�^���Z�b�g����
extern void InitLoadSoundGParam( LOADSOUND_GPARAM *GParam )
{
	GParam->NotInitSoundMemDelete = FALSE ;												// InitSoundMem �ō폜���Ȃ����ǂ����̃t���O( TRUE:InitSoundMem�ł͍폜���Ȃ�  FALSE:InitSoundMem�ō폜���� )
	GParam->Create3DSoundFlag = DSOUND.Create3DSoundFlag ;								// 3D�T�E���h���쐬���邩�ǂ����̃t���O( TRUE:�R�c�T�E���h���쐬����  FALSE:�R�c�T�E���h���쐬���Ȃ� )
	GParam->CreateSoundDataType = DSOUND.CreateSoundDataType ;							// �쐬����T�E���h�f�[�^�̃f�[�^�^�C�v
	GParam->DisableReadSoundFunctionMask = DSOUND.DisableReadSoundFunctionMask ;		// �g�p���Ȃ��ǂݍ��ݏ����̃}�X�N
#ifndef DX_NON_OGGVORBIS
	GParam->OggVorbisBitDepth = DSOUND.OggVorbisBitDepth ;								// �n�����u�����������g�p���̃r�b�g�[�x(1:8bit 2:16bit)
	GParam->OggVorbisFromTheoraFile = DSOUND.OggVorbisFromTheoraFile ;					// Ogg Theora �t�@�C������ Vorbis �f�[�^���Q�Ƃ��邩�ǂ����̃t���O( TRUE:Theora �t�@�C������ Vorbis �f�[�^���Q�Ƃ��� )
#endif
}

// LoadSoundMemBase �̎������֐�
static int LoadSoundMemBase_Static(
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const wchar_t *WaveName,
	int BufferNum,
	int UnionHandle,
	int ASyncThread
)
{
	void *SrcBuffer = NULL ;
	int SrcSize ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	if( BufferNum > MAX_SOUNDBUFFER_NUM ) BufferNum = MAX_SOUNDBUFFER_NUM ;

	// �f�[�^�^�C�v���������ɓW�J����ȊO�̏ꍇ�A�X�g���[���f�[�^�Ƃ��ăT�E���h�f�[�^���쐬����
	if( GParam->CreateSoundDataType != DX_SOUNDDATATYPE_MEMNOPRESS )
	{
		// �X�g���[���n�̃T�E���h�f�[�^�̒ǉ�
		if( AddStreamSoundMemToFile_UseGParam( GParam, WaveName, 0, SoundHandle, GParam->CreateSoundDataType, UnionHandle, ASyncThread ) == -1 )
			goto ERR ;

		// �Đ�����
		SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;
	}
	else
	{
		// �ہX�������ɓǂݍ���
		if( FileFullRead( WaveName, &SrcBuffer, &SrcSize ) < 0 )
		{
			DXST_ERRORLOGFMT_ADDW(( L"Sound File Load Error : %s", WaveName )) ;
			goto ERR ;
		}

		// ����������ǂݍ��ފ֐��ɓn��
		if( LoadSoundMemByMemImageBase_UseGParam( GParam, FALSE, SoundHandle, SrcBuffer, SrcSize, BufferNum, -1, FALSE, ASyncThread ) < 0 )
		{
			DXST_ERRORLOGFMT_ADDW(( L"Sound File Setup Error : %s" , WaveName )) ;
			goto ERR ;
		}

		// �������̉��
		DXFREE( SrcBuffer ) ;
		SrcBuffer = NULL ;
	}

	// ����I��
	return 0 ;

ERR :
	if( SrcBuffer != NULL ) DXFREE( SrcBuffer ) ;

	return -1 ;
}


#ifndef DX_NON_ASYNCLOAD
// LoadSoundMemBase �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadSoundMemBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoundHandle ;
	const wchar_t *WaveName ;
	int BufferNum ;
	int UnionHandle ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	WaveName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	BufferNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	UnionHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoundMemBase_Static( GParam, SoundHandle, WaveName, BufferNum, UnionHandle, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadSoundMemBase �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int LoadSoundMemBase_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *WaveName,
	int BufferNum,
	int UnionHandle,
	int ASyncLoadFlag,
	int ASyncThread 
)
{
	int SoundHandle = -1 ;
	wchar_t FullPath[ 1024 ] ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, ASyncThread ) ;
	if( SoundHandle == -1 )
		goto ERR ;

	ConvertFullPathW_( WaveName, FullPath ) ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag && ASyncThread == FALSE )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ; 
		AddASyncLoadParamInt( NULL, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, UnionHandle ) ;

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadSoundMemBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, UnionHandle ) ;

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMemBase_Static( GParam, SoundHandle, FullPath, BufferNum, UnionHandle, ASyncThread ) < 0 )
			goto ERR ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	// �n���h����Ԃ�
	return SoundHandle ;

ERR :
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	SubHandle( SoundHandle ) ;
	SoundHandle = -1 ;

	// �I��
	return -1 ;
}

// �T�E���h�f�[�^��ǉ�����
extern int NS_LoadSoundMemBase( const TCHAR *WaveName, int BufferNum, int UnionHandle )
{
#ifdef UNICODE
	return LoadSoundMemBase_WCHAR_T(
		WaveName, BufferNum, UnionHandle
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadSoundMemBase_WCHAR_T(
		UseWaveNameBuffer, BufferNum, UnionHandle
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// �T�E���h�f�[�^��ǉ�����
extern int LoadSoundMemBase_WCHAR_T( const wchar_t *WaveName, int BufferNum, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMemBase_UseGParam( &GParam, WaveName, BufferNum, UnionHandle, GetASyncLoadFlag() ) ;
}

// �T�E���h�f�[�^��ǉ�����
extern int NS_LoadSoundMem( const TCHAR *WaveName, int BufferNum, int UnionHandle )
{
#ifdef UNICODE
	return LoadSoundMem_WCHAR_T(
		WaveName, BufferNum, UnionHandle
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadSoundMem_WCHAR_T(
		UseWaveNameBuffer, BufferNum, UnionHandle
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// �T�E���h�f�[�^��ǉ�����
extern int LoadSoundMem_WCHAR_T( const wchar_t *WaveName, int BufferNum, int UnionHandle )
{
	return LoadSoundMemBase_WCHAR_T( WaveName, BufferNum, UnionHandle ) ;
}

// �����Đ����w��^�T�E���h�ǉ��֐�
extern int NS_LoadSoundMemToBufNumSitei( const TCHAR *WaveName, int BufferNum )
{
#ifdef UNICODE
	return LoadSoundMemToBufNumSitei_WCHAR_T(
		WaveName, BufferNum
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( WaveName, return -1 )

	Result = LoadSoundMemToBufNumSitei_WCHAR_T(
		UseWaveNameBuffer, BufferNum
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( WaveName )

	return Result ;
#endif
}

// �����Đ����w��^�T�E���h�ǉ��֐�
extern int LoadSoundMemToBufNumSitei_WCHAR_T( const wchar_t *WaveName, int BufferNum )
{
	return LoadSoundMemBase_WCHAR_T( WaveName, BufferNum, -1 ) ;
}

// �T�E���h�����\�[�X����ǂݍ���
extern int NS_LoadSoundMemByResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int BufferNum )
{
#ifdef UNICODE
	return LoadSoundMemByResource_WCHAR_T(
		ResourceName, ResourceType, BufferNum
	) ;
#else
	int Result = -1 ;

	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )

	Result = LoadSoundMemByResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer, BufferNum
	) ;

ERR :

	TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_END( ResourceType )

	return Result ;
#endif
}

// �T�E���h�����\�[�X����ǂݍ���
extern int LoadSoundMemByResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType, int BufferNum )
{
	void *Image ;
	int ImageSize ;

	// ���\�[�X�̏����擾
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 )
	{
		return -1 ;
	}

	// �n���h���̍쐬
	return NS_LoadSoundMemByMemImageBase( Image, ImageSize, BufferNum ) ;
}

// �����T�E���h�f�[�^���g�p����T�E���h�n���h�����쐬����
extern int NS_DuplicateSoundMem( int SrcSoundHandle, int BufferNum )
{
	int i ;
	int Handle = -1 ;
	HRESULT hr ;
	LPVOID write1, write2 ;
	DWORD length1, length2 ;
	SOUND *Sound, *SrcSound ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	// �G���[����
	if( SOUNDHCHK( SrcSoundHandle, SrcSound ) )
		goto ERR ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͕����ł��Ȃ�
	if( SrcSound->Type == DX_SOUNDTYPE_STREAMSTYLE )
		goto ERR ;

	if( BufferNum > MAX_SOUNDBUFFER_NUM ) BufferNum = MAX_SOUNDBUFFER_NUM ;

	// �V�����T�E���h�f�[�^�̒ǉ�
	Handle = _CreateSoundHandle( SrcSound->Is3DSound, SrcSound->NotInitSoundMemDelete, FALSE ) ;
	if( Handle < 0 )
	{
		goto ERR ;
	}

	Sound = ( SOUND * )HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ Handle & DX_HANDLEINDEX_MASK ] ;

	// �t�H�[�}�b�g��ۑ�
	Sound->BufferFormat = SrcSound->BufferFormat ;

	// WaveData �̃|�C���^��ۑ�
	Sound->Normal.WaveData = SrcSound->Normal.WaveData ;
	Sound->Normal.WaveSize = SrcSound->Normal.WaveSize ;

	// WaveData �̎Q�Ɛ��𑝂₷
	Sound->Normal.WaveDataUseCount = SrcSound->Normal.WaveDataUseCount ;
	*Sound->Normal.WaveDataUseCount = *Sound->Normal.WaveDataUseCount + 1 ;

	// �R�c�T�E���h�̏ꍇ�̓o�b�t�@���͂P
	if( Sound->Is3DSound )
		BufferNum = 1 ;

	// DirectSoundBuffer ���쐬
	if( ( CreateDirectSoundBuffer( &Sound->BufferFormat, ( DWORD )Sound->Normal.WaveSize, DX_SOUNDTYPE_NORMAL, BufferNum, Handle, SrcSoundHandle, FALSE ) ) == -1 )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\x07\x89\xfd\x88\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x6e\x30\x20\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x42\x00\x75\x00\x66\x00\x66\x00\x65\x00\x72\x00\x20\x00\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�����T�E���h�� DirectSoundBuffer �̍쐬�Ɏ��s���܂���\n" @*/ ) ;
		goto ERR ;
	}
	Sound->BaseFrequency = ( int )Sound->BufferFormat.nSamplesPerSec ;

	// �T�E���h�f�[�^���T�E���h�o�b�t�@�ɃR�s�[����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		// �ʂ̃T�E���h�o�b�t�@�̕����������ꍇ�͓]����Ƃ̕K�v�������̂œ]�����Ȃ�
		// ( ����ɓ��삵�Ȃ��������������߂Ƃ肠�����t���O�͖��� )
//		if( Sound->Normal.BufferDuplicateFlag[ i ] == TRUE ) continue ;

		hr = SoundBuffer_Lock( &Sound->Buffer[ i ], 0, ( DWORD )Sound->Normal.WaveSize,
								&write1, &length1,
								&write2, &length2 ) ;
		if( hr != D_DS_OK )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x31\x00\x00"/*@ L"�T�E���h�f�[�^�̓]���Ɏ��s���܂���_1" @*/ ) ;
			goto ERR ;
		}

		_MEMCPY( write1, Sound->Normal.WaveData, length1 ) ;
		if( write2 != 0 ) _MEMCPY( write2, (BYTE *)Sound->Normal.WaveData + length1, length2 ) ;

		hr = SoundBuffer_Unlock( &Sound->Buffer[ i ], write1, length1, write2, length2 ) ;
		if( hr != D_DS_OK )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x32\x00\x00"/*@ L"�T�E���h�f�[�^�̓]���Ɏ��s���܂���_2" @*/ ) ;
			goto ERR ;
		}
	}

	// �I���A�T�E���h�f�[�^�n���h����Ԃ�
	return Handle ;

ERR :
	if( Handle != -1 )
	{
		NS_DeleteSoundMem( Handle ) ;
	}
	
	return -1 ;
}

// LoadSoundMemByMemImageBase �̎������֐�
static int LoadSoundMemByMemImageBase_Static(
	LOADSOUND_GPARAM *GParam,
	int SoundHandle,
	const void *FileImageBuffer,
	int ImageSize,
	int BufferNum,
	int UnionHandle,
	int ASyncThread
)
{
	int i ;
	WAVEFORMATEX Format ;
	void *SoundBuffer ;
	int SoundSize ;
	HRESULT hr ;
	LPVOID write1, write2 ;
	DWORD length1, length2 ;
	SOUND * Sound ;
	SOUNDCONV ConvData ;
	STREAMDATA Stream ;

	if( DSOUND.DirectSoundObject == NULL || BufferNum > MAX_SOUNDBUFFER_NUM )
		return -1 ;

//	_MEMSET( &ConvData, 0, sizeof( ConvData ) );
	ConvData.InitializeFlag = FALSE ;
	Stream.DataPoint = NULL ;
	SoundBuffer = NULL ;

	Sound = ( SOUND * )HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ SoundHandle & DX_HANDLEINDEX_MASK ] ;

//	// �R�c�T�E���h�̏ꍇ�̓o�b�t�@�̐��͈����
//	if( Sound->Is3DSound != FALSE )
//		BufferNum = 1 ;

	// �f�[�^�^�C�v���������ɓW�J����ȊO�̏ꍇ�A�X�g���[���f�[�^�Ƃ��ăT�E���h�f�[�^���쐬����
	if( GParam->CreateSoundDataType != DX_SOUNDDATATYPE_MEMNOPRESS )
	{
STREAM_TYPE :
		if( AddStreamSoundMemToMem_UseGParam(
				GParam,
				FileImageBuffer,
				ImageSize,
				0,
				SoundHandle,
				GParam->CreateSoundDataType,
				UnionHandle,
				ASyncThread
			) == -1 )
			goto ERR ;

		// �Đ�����
		SetupStreamSoundMem_UseGParam( SoundHandle, ASyncThread ) ;
	}
	else
	{
		// �o�b�l�`���ɕϊ�
		{
			Stream.DataPoint = MemStreamOpen( FileImageBuffer, ( unsigned int )ImageSize ) ;
			Stream.ReadShred = *GetMemStreamDataShredStruct() ;
			if( SetupSoundConvert( &ConvData, &Stream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
								,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
								) < 0 )
			{
				DXST_ERRORLOGFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\x30\xff\x23\xff\x2d\xff\x78\x30\x6e\x30\x09\x59\xdb\x63\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����t�@�C���̂o�b�l�ւ̕ϊ��Ɏ��s���܂���" @*/ )) ;
				goto ERR ;
			}
			if( SoundConvertFast( &ConvData, &Format, &SoundBuffer, &SoundSize ) < 0 )
			{
				DXST_ERRORLOGFMT_ADDUTF16LE(( "\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�����t�@�C�����i�[���郁�����̈�̊m�ۂɎ��s���܂���" @*/ )) ;
				goto ERR ;
			}

			TerminateSoundConvert( &ConvData ) ;
			MemStreamClose( Stream.DataPoint ) ;
			Stream.DataPoint = NULL ;
		}

		// 3���ȏ�̃T�E���h�f�[�^�̓X�g���[���`���ōĐ�����
		if( SoundSize / Format.nAvgBytesPerSec > 60 * 3 )
		{
			if( SoundBuffer != NULL )
			{
				DXFREE( SoundBuffer ) ;
				SoundBuffer = NULL ;
			}

			GParam->CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
			goto STREAM_TYPE ;
		}

		// DirectSoundBuffer ���쐬
//		DXST_ERRORLOGFMT_ADDW(( L"format:%d  channel:%d  samplespersec:%d  AvgBytes:%d  blockalign:%d  BitsPerSample:%d  cbsize:%d\n",
//				Format.wFormatTag, Format.nChannels, Format.nSamplesPerSec, Format.nAvgBytesPerSec, Format.nBlockAlign, Format.wBitsPerSample, Format.cbSize )) ;
		if( ( CreateDirectSoundBuffer( &Format, ( DWORD )SoundSize, DX_SOUNDTYPE_NORMAL, BufferNum, SoundHandle, -1, ASyncThread ) ) == -1 )
		{
			// DirectSoundBuffer �̍쐬�Ɏ��s������X�g���[���`���ŊJ�����Ƃ��Ă݂�
			if( SoundBuffer != NULL )
			{
				DXFREE( SoundBuffer ) ;
				SoundBuffer = NULL ;
			}

			GParam->CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
			goto STREAM_TYPE ;
		}
		Sound->BaseFrequency = ( int )Format.nSamplesPerSec ;

		// �Q�Ɛ���ۑ����郁�����̊m��
		Sound->Normal.WaveDataUseCount = ( int * )DXALLOC( sizeof( int ) ) ;
		if( Sound->Normal.WaveDataUseCount == NULL )
		{
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xc2\x53\x67\x71\x70\x65\x92\x30\xdd\x4f\x58\x5b\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�T�E���h�f�[�^�̎Q�Ɛ���ۑ����郁�����̈�̊m�ۂɎ��s���܂���" @*/ )) ;
			goto ERR ;
		}

		Sound->BufferFormat = Format ;			// �t�H�[�}�b�g��ۑ�
		Sound->Normal.WaveData = SoundBuffer ;	// �f�[�^�|�C���^��ۑ�
		Sound->Normal.WaveSize = SoundSize ;	// �f�[�^�T�C�Y��ۑ�
		*Sound->Normal.WaveDataUseCount = 1 ;
		SoundBuffer = NULL ;

		// �T�E���h�f�[�^���T�E���h�o�b�t�@�ɃR�s�[����
		{
			for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
			{
				// �ʂ̃T�E���h�o�b�t�@�̕����������ꍇ�͓]����Ƃ̕K�v�������̂œ]�����Ȃ�
				if( Sound->Normal.BufferDuplicateFlag[ i ] == TRUE ) continue ;
			
				hr = SoundBuffer_Lock( &Sound->Buffer[ i ], 0, ( DWORD )SoundSize,
										&write1, &length1,
										&write2, &length2 ) ;
				if( hr != D_DS_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x31\x00\x00"/*@ L"�T�E���h�f�[�^�̓]���Ɏ��s���܂���_1" @*/ ) ;
					goto ERR ;
				}

				_MEMCPY( write1, Sound->Normal.WaveData, length1 ) ;
				if( write2 != 0 ) _MEMCPY( write2, (BYTE *)Sound->Normal.WaveData + length1, length2 ) ;

				hr = SoundBuffer_Unlock( &Sound->Buffer[ i ], write1, length1, write2, length2 ) ;
				if( hr != D_DS_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xe2\x8e\x01\x90\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x5f\x00\x32\x00\x00"/*@ L"�T�E���h�f�[�^�̓]���Ɏ��s���܂���_2" @*/ ) ;
					goto ERR ;
				}
			}
		}
	}
	
	// ����I��
	return 0 ;

ERR :
	if( SoundBuffer != NULL ) DXFREE( SoundBuffer ) ;
	if( Stream.DataPoint != NULL ) MemStreamClose( Stream.DataPoint ) ;
	TerminateSoundConvert( &ConvData ) ;
	
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadSoundMemByMemImageBase �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadSoundMemByMemImageBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int CreateSoundHandle ;
	int SoundHandle ;
	const void *FileImageBuffer ;
	int ImageSize ;
	int BufferNum ;
	int UnionHandle ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	CreateSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	SoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImageBuffer = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	ImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	BufferNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	UnionHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoundMemByMemImageBase_Static( GParam, SoundHandle, FileImageBuffer, ImageSize, BufferNum, UnionHandle, TRUE ) ;
	DecASyncLoadCount( SoundHandle ) ;
	if( Result < 0 )
	{
		if( CreateSoundHandle )
		{
			SubHandle( SoundHandle ) ;
		}
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadSoundMemByMemImageBase �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int LoadSoundMemByMemImageBase_UseGParam(
	LOADSOUND_GPARAM *GParam,
	int CreateSoundHandle,
	int SoundHandle,
	const void *FileImageBuffer,
	int ImageSize,
	int BufferNum,
	int UnionHandle,
	int ASyncLoadFlag,
	int ASyncThread
)
{
	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	if( CreateSoundHandle )
	{
		SoundHandle = _CreateSoundHandle( GParam->Create3DSoundFlag, GParam->NotInitSoundMemDelete, ASyncThread ) ;
		if( SoundHandle == -1 )
		{
			goto ERR ;
		}
	}
	else
	{
#ifndef DX_NON_ASYNCLOAD
		if( ASyncThread )
		{
			IncASyncLoadCount( SoundHandle, -1 ) ;
		}
#endif // DX_NON_ASYNCLOAD
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag && ASyncThread == FALSE )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, CreateSoundHandle ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImageBuffer ) ; 
		AddASyncLoadParamInt( NULL, &Addr, ImageSize ) ;
		AddASyncLoadParamInt( NULL, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, UnionHandle ) ;

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadSoundMemByMemImageBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, CreateSoundHandle ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoundHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImageBuffer ) ; 
		AddASyncLoadParamInt( AParam->Data, &Addr, ImageSize ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, BufferNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, UnionHandle ) ;

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( SoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoundMemByMemImageBase_Static( GParam, SoundHandle, FileImageBuffer, ImageSize, BufferNum, UnionHandle, ASyncThread ) < 0 )
			goto ERR ;
	}

#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	// �n���h����Ԃ�
	return CreateSoundHandle ? SoundHandle : 0 ;

ERR :
#ifndef DX_NON_ASYNCLOAD
	if( ASyncThread )
	{
		DecASyncLoadCount( SoundHandle ) ;
	}
#endif // DX_NON_ASYNCLOAD

	if( CreateSoundHandle )
	{
		SubHandle( SoundHandle ) ;
		SoundHandle = -1 ;
	}

	// �I��
	return -1 ;
}

// ��������ɓW�J���ꂽ�t�@�C���C���[�W����n���h�����쐬����(�x�[�X�֐�)
extern int NS_LoadSoundMemByMemImageBase( const void *FileImageBuffer, int ImageSize, int BufferNum, int UnionHandle )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;

	return LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, FileImageBuffer, ImageSize, BufferNum, UnionHandle, GetASyncLoadFlag(), FALSE ) ;
}

// ��������ɓW�J���ꂽ�t�@�C���C���[�W����n���h�����쐬����
extern int NS_LoadSoundMemByMemImage( const void *FileImageBuffer , int ImageSize, int UnionHandle ) 
{
	return NS_LoadSoundMemByMemImageBase( FileImageBuffer, ImageSize, 1, UnionHandle ) ;
}

// ��������ɓW�J���ꂽ�t�@�C���C���[�W����n���h�����쐬����2
extern int NS_LoadSoundMemByMemImage2( const void *UData, int UDataSize, const WAVEFORMATEX *UFormat, int UHeaderSize )
{
	void *WaveImage ;
	int WaveImageSize ;
	int res, Handle ;
	
	res = CreateWaveFileImage( &WaveImage, &WaveImageSize,
								 UFormat, UHeaderSize,
								 UData, UDataSize ) ;
	if( res < 0 ) return -1 ;
	
	Handle = NS_LoadSoundMemByMemImageBase( WaveImage, WaveImageSize, 1 ) ;
	
	DXFREE( WaveImage ) ;
	
	return Handle ;
}

// ��������ɓW�J���ꂽ�t�@�C���C���[�W����n���h�����쐬����(�x�[�X�֐�)
extern int NS_LoadSoundMemByMemImageToBufNumSitei( const void *FileImageBuffer, int ImageSize, int BufferNum )
{
	return NS_LoadSoundMemByMemImageBase( FileImageBuffer, ImageSize, BufferNum ) ;
}







// �������ɓǂݍ���WAVE�f�[�^���폜����
extern int NS_DeleteSoundMem( int SoundHandle, int )
{
	return SubHandle( SoundHandle ) ;
}

// �T�E���h�n���h���̍Đ��������s��( -1:�G���[ 0:����I�� 1:�Đ�����K�v�Ȃ� )
static	int _PlaySetupSoundMem( SOUND * Sound, int TopPositionFlag )
{
	DWORD State ;
	ULONGLONG Position ;
	HRESULT hr ;
	int i ;
	int j ;
//	D_XAUDIO2_VOICE_STATE XAState ;
	LONG Volume[ SOUNDBUFFER_MAX_CHANNEL_NUM ] ;
	LONG Pan ;
	DWORD Frequency ;

	// �O��̍Đ��ʒu���擾����
	{
		DWORD Temp ;

//		Sound->Buffer[ Sound->Normal.BackPlayBufferNo ]->GetCurrentPosition( &Position, NULL ) ;
		SoundBuffer_GetCurrentPosition( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], &Temp, NULL ) ;
		Position = Temp ;
	}

	// �Đ�����T�E���h�o�b�t�@�̃Z�b�g
	i = Sound->Normal.BackPlayBufferNo ;

	// ����Đ�����T�E���h�o�b�t�@�̃Z�b�g
	i ++ ;
	if( i >= Sound->ValidBufferNum )
		i = 0 ;

	// �O��̍Đ��ʒu�̏����X�V����
	Sound->Normal.BackPlayBufferNo = i ;

	// �Đ����������ꍇ�͎~�߂�
	{
//		hr = Sound->Buffer[ i ]->GetStatus( &State ) ;
		hr = SoundBuffer_GetStatus( &Sound->Buffer[ i ], &State ) ;
		if( hr != D_DS_OK ) return -1 ;
		if( State & D_DSBSTATUS_PLAYING )
		{
			// �擪����̍Đ��w��ł͂Ȃ��ꍇ�͉��������I��
			if( TopPositionFlag == FALSE )
				return 1 ;

			// �Đ���~
//			Sound->Buffer[ i ]->Stop() ;
			SoundBuffer_Stop( &Sound->Buffer[ i ] ) ;
		}

		// �Đ��ʒu��擪�ɂ���
//		if( TopPositionFlag == TRUE )	Sound->Buffer[i]->SetCurrentPosition( 0 ) ;
//		else							Sound->Buffer[i]->SetCurrentPosition( Position ) ;
		if( TopPositionFlag == TRUE )	SoundBuffer_SetCurrentPosition( &Sound->Buffer[i], 0 ) ;
		else							SoundBuffer_SetCurrentPosition( &Sound->Buffer[i], ( DWORD )Position ) ;
	}

	// �{�����[�����Z�b�g����
	for( j = 0 ; j < SOUNDBUFFER_MAX_CHANNEL_NUM ; j ++ )
	{
		if( Sound->ValidNextPlayVolume[ j ] )
		{
			Volume[ j ] = ( LONG )Sound->NextPlayVolume[ j ] ;
			Sound->ValidNextPlayVolume[ j ] = 0 ;
		}
		else
		{
			Volume[ j ] = ( LONG )Sound->BaseVolume[ j ] ;
		}
	}

	// �p�����Z�b�g����
	if( Sound->ValidNextPlayPan )
	{
		Pan = ( LONG )Sound->NextPlayPan ;
		Sound->ValidNextPlayPan = 0 ;
	}
	else
	{
		Pan = ( LONG )Sound->BasePan ;
	}

	// ���g�����Z�b�g����
	if( Sound->ValidNextPlayFrequency )
	{
		Frequency = ( DWORD )Sound->NextPlayFrequency ;
		Sound->ValidNextPlayFrequency = 0 ;
	}
	else
	{
		Frequency = ( DWORD )Sound->BaseFrequency ;
	}

	for( j = 0 ; j < SOUNDBUFFER_MAX_CHANNEL_NUM ; j ++ )
	{
		SoundBuffer_SetVolume( &Sound->Buffer[ i ], j, Volume[ j ] ) ;
	}
	SoundBuffer_SetPan(       &Sound->Buffer[ i ], Pan ) ;
	SoundBuffer_SetFrequency( &Sound->Buffer[ i ], Frequency ) ;

	// �R�c�T�E���h�̏ꍇ�͂R�c�T�E���h�p�����[�^�̍X�V���s��
	if( Sound->Is3DSound != FALSE )
	{
		VECTOR *Position ;
		float Radius ;
		VECTOR *Velocity ;

		if( Sound->ValidNextPlay3DPosition )
		{
			Position = &Sound->NextPlay3DPosition ;
			Sound->ValidNextPlay3DPosition = 0 ;
		}
		else
		{
			Position = &Sound->Base3DPosition ;
		}

		if( Sound->ValidNextPlay3DRadius )
		{
			Radius = Sound->NextPlay3DRadius ;
			Sound->ValidNextPlay3DRadius = 0 ;
		}
		else
		{
			Radius = Sound->Base3DRadius ;
		}

		if( Sound->ValidNextPlay3DVelocity )
		{
			Velocity = &Sound->NextPlay3DVelocity ;
			Sound->ValidNextPlay3DVelocity = 0 ;
		}
		else
		{
			Velocity = &Sound->Base3DVelocity ;
		}
		SoundBuffer_Set3DPosition( &Sound->Buffer[ i ], Position ) ;
		SoundBuffer_Set3DRadius(   &Sound->Buffer[ i ], Radius ) ;
		SoundBuffer_Set3DVelocity( &Sound->Buffer[ i ], Velocity ) ;

		SoundBuffer_Refresh3DSoundParam( &Sound->Buffer[ i ] ) ;
	}

	// ����I��
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^���Đ�����
extern int NS_PlaySoundMem( int SoundHandle , int PlayType, int TopPositionFlag )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_PlayStreamSoundMem( SoundHandle , PlayType, TopPositionFlag ) ;
	}

	// �Đ��������s��
	if( _PlaySetupSoundMem( Sound, TopPositionFlag ) == 1 )
		return 0 ;

	// �Đ�
	{
//		if( Sound->Buffer[ i ]->Play( 0 , 0 , PlayType == DX_PLAYTYPE_LOOP ? D_DSBPLAY_LOOPING : 0 ) != D_DS_OK )
		if( SoundBuffer_Play( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], PlayType == DX_PLAYTYPE_LOOP ) != D_DS_OK )
			return -1 ;
	}

	// �R�c�T�E���h�̏ꍇ�͍Đ����̂R�c�T�E���h���X�g�ɒǉ�����
	if( Sound->Is3DSound != FALSE )
	{
		// �N���e�B�J���Z�N�V�����̎擾
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

		if( Sound->AddPlay3DSoundList == FALSE )
		{
			Sound->AddPlay3DSoundList = TRUE ;
			AddHandleList( &DSOUND.Play3DSoundListFirst, &Sound->Play3DSoundList, SoundHandle, Sound ) ;
		}

		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;
	}

	// �Đ��X�e�[�^�X�ɂ���Ă͍Đ��I����҂�
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		while( NS_ProcessMessage() == 0 && NS_CheckSoundMem( SoundHandle ) == 1 ){ Sleep( 1 ) ; }
	}

	// �I��
	return 0 ;
}


// �������ɓǂݍ��񂾕�����WAVE�f�[�^���Ȃ�ׂ������ɍĐ�����
extern int NS_MultiPlaySoundMem( const int *SoundHandleList, int SoundHandleNum, int PlayType, int TopPositionFlag )
{
	SOUND **SoundList, *SoundBuf[ 256 ], **SoundTempBuf = NULL, *Sound ;
	SOUNDBUFFER **SBufferList, *SBufferBuf[ 256 ], **SBufferTempBuf = NULL, *SBuffer ;
	int i, Loop ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �T�E���h�|�C���^���X�g������Ȃ������烁�������m�ۂ���
	if( SoundHandleNum > 256 )
	{
		SoundTempBuf = ( SOUND ** )DXALLOC( ( sizeof( SOUND * ) + sizeof( SOUNDBUFFER * ) ) * SoundHandleNum ) ;
		if( SoundTempBuf == NULL )
		{
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\x25\x00\x64\x00\x20\x00\x0b\x50\x6e\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xc7\x30\xfc\x30\xbf\x30\x6e\x30\xdd\x30\xa4\x30\xf3\x30\xbf\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"%d �̃T�E���h�f�[�^�̃|�C���^���i�[���郁�����̈�̊m�ۂɎ��s���܂���" @*/ , SoundHandleNum )) ;
			goto END ;
		}
		SBufferTempBuf = ( SOUNDBUFFER ** )( SoundTempBuf + SoundHandleNum ) ;

		SBufferList = SBufferTempBuf ;
		SoundList = SoundTempBuf ;
	}
	else
	{
		SBufferList = SBufferBuf ;
		SoundList = SoundBuf ;
	}

	// ���[�v���ǂ������擾���Ă���
	Loop = PlayType == DX_PLAYTYPE_LOOP ;

	// �Đ��̏���
	for( i = 0 ; i < SoundHandleNum ; i ++ )
	{
		// �G���[����
		if( SOUNDHCHK( SoundHandleList[ i ], Sound ) )
		{
			SoundList[ i ] = NULL ;
		}
		else
		{
			SoundList[ i ] = Sound ;

			if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
			{
				_PlaySetupStreamSoundMem( SoundHandleList[ i ], Sound, PlayType, TopPositionFlag, &SBufferList[ i ] ) ;
			}
			else
			{
				_PlaySetupSoundMem( Sound, TopPositionFlag ) ;
				SBufferList[ i ] = &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ] ;
			}
		}
	}

	// ��ĂɍĐ�
	if( DSOUND.EnableSoundCaptureFlag )
	{
		for( i = 0 ; i < SoundHandleNum ; i ++ )
		{
			Sound = SoundList[ i ] ;
			if( Sound == NULL ) continue ;
			SBuffer = SBufferList[ i ] ;
			SBuffer->State = TRUE ;
			SBuffer->Loop = ( Loop || Sound->Type == DX_SOUNDTYPE_STREAMSTYLE ) ? 1 : 0 ;
		}
	}
	else
	{
		for( i = 0 ; i < SoundHandleNum ; i ++ )
		{
			Sound = SoundList[ i ] ;
			if( Sound == NULL ) continue ;
			SBuffer = SBufferList[ i ] ;
			SBuffer->DSBuffer->Play( 0, 0, ( DWORD )( ( Loop || Sound->Type == DX_SOUNDTYPE_STREAMSTYLE ) ? D_DSBPLAY_LOOPING : 0 ) ) ;
		}
	}

	// �Đ��X�e�[�^�X�ɂ���Ă͍Đ��I����҂�
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		while( NS_ProcessMessage() == 0 )
		{
			for( i = 0 ; i < SoundHandleNum ; i ++ )
			{
				if( SoundList[ i ] == NULL ) continue ;
				if( NS_CheckSoundMem( SoundHandleList[ i ] ) == 1 ) break ;
			}
			if( i == SoundHandleNum ) break ;
			Sleep( 1 ) ;
		}
	}

END :

	// �������̉��
	if( SoundTempBuf )
	{
		DXFREE( SoundTempBuf ) ;
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}


// �������ɓǂݍ���WAVE�f�[�^�̍Đ����~�߂�
extern int NS_StopSoundMem( int SoundHandle )
{
	SOUND * Sound ;
	HRESULT hr ;
	DWORD state ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_StopStreamSoundMem( SoundHandle ) ;
	}

	// �Đ���~
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		Sound->BufferPlayStateBackupFlagValid[ i ] = FALSE ;
		Sound->BufferPlayStateBackupFlag[ i ] = FALSE ;

//		hr = Sound->Buffer[ i ]->GetStatus( &state ) ;
		hr = SoundBuffer_GetStatus( &Sound->Buffer[ i ], &state ) ;
		if( hr != D_DS_OK ) return -1 ;
		if( state & D_DSBSTATUS_PLAYING  )
		{
//			Sound->Buffer[ i ]->Stop() ;
			SoundBuffer_Stop( &Sound->Buffer[ i ], TRUE ) ;
		}
	}

	// �I��
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^���Đ��������ׂ�
extern int NS_CheckSoundMem( int SoundHandle )
{
	DWORD State ;
	SOUND * Sound ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_CheckStreamSoundMem( SoundHandle ) ;
	}
	
	// �X�e�[�^�X���擾���Ē��ׂ�
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		// �V�X�e�����Ŏ~�߂Ă���ꍇ�͎~�߂�O�̏�Ԃ�Ԃ�
		if( Sound->BufferPlayStateBackupFlagValid[ i ] )
		{
			return Sound->BufferPlayStateBackupFlag[ i ] ? 1 : 0 ;
		}
		else
		{
//			Sound->Buffer[ i ]->GetStatus( &State ) ;
			SoundBuffer_GetStatus( &Sound->Buffer[ i ], &State ) ;
			if( State & D_DSBSTATUS_PLAYING ) return 1 ;
		}
	}

	// �����܂ł��Ă���΍Đ��͂���Ă��Ȃ�
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^�̍Đ��Ƀp����ݒ肷��
extern int NS_SetPanSoundMem( int PanPal , int SoundHandle )
{
	int i ;
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �Đ�������Ă��Ȃ��ꍇ�� SetNextPlayPanSoundMem ���Ă�
//	if( NS_CheckSoundMem( SoundHandle ) == 0 )
//	{
//		return NS_SetNextPlayPanSoundMem( PanPal, SoundHandle ) ;
//	}

	if( PanPal >  10000 ) PanPal =  10000 ;
	if( PanPal < -10000 ) PanPal = -10000 ;

	// �p�����Z�b�g����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
//		Sound->Buffer[ i ]->SetPan( ( LONG )PanPal ) ;
		SoundBuffer_SetPan( &Sound->Buffer[ i ], ( LONG )PanPal ) ;
	}
//	SoundBuffer_SetPan( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], ( LONG )PanPal ) ;
	Sound->BasePan = PanPal ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̃p����ݒ肷��( 0 �` 255 )
extern int NS_ChangePanSoundMem( int PanPal, int SoundHandle )
{
	int i ;
	int pan ;
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( PanPal >  255 ) PanPal =  255 ;
	if( PanPal < -255 ) PanPal = -255 ;

	if( PanPal == 0 )
	{
		pan = 0 ;
	}
	else
	{
		if( PanPal > 0 )
		{
			if( PanPal == 255 )
			{
				pan = 10000 ;
			}
			else
			{
				if( DSOUND.OldVolumeTypeFlag )
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan > 10000 )
				{
					pan = 10000 ;
				}
			}
		}
		else
		{
			if( PanPal == -255 )
			{
				pan = -10000 ;
			}
			else
			{
				if( DSOUND.OldVolumeTypeFlag )
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan < -10000 )
				{
					pan = -10000 ;
				}
			}
		}
	}

	// �p�����Z�b�g����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		SoundBuffer_SetPan( &Sound->Buffer[ i ], ( LONG )pan ) ;
	}

	Sound->BasePan = pan ;

	// �I��
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^�̃p�����擾����
extern int NS_GetPanSoundMem( int SoundHandle )
{
	SOUND * Sound ;
	LONG Result ;
	HRESULT hr ;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// �g�p���������ꍇ�̓G���[
		if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return -1 ;
	
		// �p�����擾����
		hr = SoundBuffer_GetPan( SBuffer, &Result ) ;
	}
	else
	{
		hr = SoundBuffer_GetPan( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], &Result ) ;
	}

	// �I��
	return Result ; 
}

// �������ɓǂ݂���WAVE�f�[�^�̍Đ��Ƀ{�����[����ݒ肷��
extern int NS_SetVolumeSoundMem( int VolumePal , int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �Đ�������Ă��Ȃ��ꍇ�� SetNextPlayVolumeSoundMem ���Ă�
//	if( NS_CheckSoundMem( SoundHandle ) == 0 )
//	{
//		return NS_SetNextPlayVolumeSoundMem( VolumePal, SoundHandle ) ;
//	}

	VolumePal -= 10000 ;
//	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	// �{�����[�����Z�b�g����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
//		Sound->Buffer[ i ]->SetVolume( ( LONG )VolumePal ) ;
		SoundBuffer_SetVolumeAll( &Sound->Buffer[ i ], ( LONG )VolumePal ) ;
	}
//	SoundBuffer_SetVolumeAll( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], ( LONG )VolumePal ) ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->BaseVolume[ i ] = VolumePal ;
	}

	// �I��
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^�̍Đ��Ƀ{�����[����ݒ肷��
extern int NS_ChangeVolumeSoundMem( int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;
	LONG vol ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

//	     if( VolumePal > 255 ) VolumePal = 255 ;
//	else if( VolumePal < 0   ) VolumePal = 0   ;
	if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = D_DSBVOLUME_MIN ;
	}
	else
	{
		if( DSOUND.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < D_DSBVOLUME_MIN ) vol = D_DSBVOLUME_MIN ;
	}

	// �{�����[�����Z�b�g����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
//		Sound->Buffer[ i ]->SetVolume( vol ) ;
		SoundBuffer_SetVolumeAll( &Sound->Buffer[ i ], vol ) ;
	}
//	SoundBuffer_SetVolumeAll( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], vol ) ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->BaseVolume[ i ] = vol ;
	}

	// �I��
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^�̍Đ��̃{�����[�����擾����
extern int NS_GetVolumeSoundMem( int SoundHandle )
{
	return NS_GetChannelVolumeSoundMem( 0, SoundHandle ) ;
}


// �T�E���h�n���h���̎w��̃`�����l���̃{�����[����ݒ肷��( 100����1�f�V�x���P�� 0 �` 10000 )
extern int NS_SetChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	VolumePal -= 10000 ;
//	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	// �{�����[�����Z�b�g����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		SoundBuffer_SetVolume( &Sound->Buffer[ i ], Channel, ( LONG )VolumePal ) ;
	}
	Sound->BaseVolume[ Channel ] = VolumePal ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎w��̃`�����l���̃{�����[����ݒ肷��( 0 �` 255 )
extern int NS_ChangeChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;
	LONG vol ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

//	     if( VolumePal > 255 ) VolumePal = 255 ;
//	else if( VolumePal < 0   ) VolumePal = 0   ;
	if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = D_DSBVOLUME_MIN ;
	}
	else
	{
		if( DSOUND.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < D_DSBVOLUME_MIN ) vol = D_DSBVOLUME_MIN ;
	}

	// �{�����[�����Z�b�g����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
		SoundBuffer_SetVolume( &Sound->Buffer[ i ], Channel, vol ) ;
	}

	Sound->BaseVolume[ Channel ] = vol ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎w��̃`�����l���̃{�����[�����擾����
extern int NS_GetChannelVolumeSoundMem( int Channel, int SoundHandle )
{
	SOUND * Sound ;
	LONG Result ;
	SOUNDBUFFER *SBuffer ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// �g�p���������ꍇ�̓G���[
		if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return -1 ;
	
		// �{�����[�����擾����
		SoundBuffer_GetVolume( SBuffer, Channel, &Result ) ;
	}
	else
	{
		SoundBuffer_GetVolume( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], Channel, &Result ) ;
	}

	// �I��
	return Result + 10000 ; 
}

// �������ɓǂݍ���WAVE�f�[�^�̍Đ����g����ݒ肷��
extern int NS_SetFrequencySoundMem( int FrequencyPal , int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( FrequencyPal == -1 ) FrequencyPal = D_DSBFREQUENCY_ORIGINAL ;

	// ���g�����Z�b�g����
	for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
	{
//		Sound->Buffer[ i ]->SetFrequency( ( DWORD )FrequencyPal ) ;
		SoundBuffer_SetFrequency( &Sound->Buffer[ i ], ( DWORD )FrequencyPal ) ;
	}
//	SoundBuffer_SetFrequency( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], ( DWORD )FrequencyPal ) ;
	Sound->BaseFrequency = FrequencyPal ;

	// �I��
	return 0 ;
}

// �������ɓǂݍ���WAVE�f�[�^�̍Đ����g�����擾����
extern int NS_GetFrequencySoundMem( int SoundHandle )
{
	SOUND * Sound ;
	SOUNDBUFFER *SBuffer ;
	HRESULT hr ;
	DWORD Result ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// ���g����Ԃ�
//	return (int)Sound->BufferFormat.nSamplesPerSec ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( Sound->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		// �g�p���������ꍇ�̓G���[
		if( _GetDirectSoundBuffer( SoundHandle, Sound, &SBuffer, false ) == -1 ) return -1 ;
	
		// �p�����擾����
		hr = SoundBuffer_GetFrequency( SBuffer, &Result ) ;
	}
	else
	{
		hr = SoundBuffer_GetFrequency( &Sound->Buffer[ Sound->Normal.BackPlayBufferNo ], &Result ) ;
	}

	// �I��
	return ( int )Result ; 
}

// �������ɓǂݍ���WAVE�f�[�^�̍Đ����g����ǂݍ��ݒ���̏�Ԃɖ߂�
extern	int NS_ResetFrequencySoundMem( int SoundHandle )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// ���g����������Ԃɖ߂�
	return NS_SetFrequencySoundMem( (int)Sound->BufferFormat.nSamplesPerSec, SoundHandle ) ;
}


// �������ɓǂ݂���WAVE�f�[�^�̍Đ��Ƀp����ݒ肷��
extern	int	NS_SetNextPlayPanSoundMem( int PanPal, int SoundHandle )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( PanPal >  10000 ) PanPal =  10000 ;
	if( PanPal < -10000 ) PanPal = -10000 ;

	Sound->NextPlayPan = PanPal ;
	Sound->ValidNextPlayPan = 1 ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎��̍Đ��ɂ̂ݎg�p����p����ݒ肷��( -255 �` 255 )
extern int NS_ChangeNextPlayPanSoundMem( int PanPal, int SoundHandle )
{
	SOUND * Sound ;
	int pan ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( PanPal >  255 ) PanPal =  255 ;
	if( PanPal < -255 ) PanPal = -255 ;

	if( PanPal == 0 )
	{
		pan = 0 ;
	}
	else
	{
		if( PanPal > 0 )
		{
			if( PanPal == 255 )
			{
				pan = 10000 ;
			}
			else
			{
				if( DSOUND.OldVolumeTypeFlag )
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = -(LONG)( _DTOL( _LOG10( ( 255 - PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan > 10000 )
				{
					pan = 10000 ;
				}
			}
		}
		else
		{
			if( PanPal == -255 )
			{
				pan = -10000 ;
			}
			else
			{
				if( DSOUND.OldVolumeTypeFlag )
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 10.0 * 100.0 ) ) ;
				}
				else
				{
					pan = (LONG)( _DTOL( _LOG10( ( 255 + PanPal ) / 255.0 ) * 50.0 * 100.0 ) ) ;
				}
				if( pan < -10000 )
				{
					pan = -10000 ;
				}
			}
		}
	}

	Sound->NextPlayPan = pan ;
	Sound->ValidNextPlayPan = 1 ;

	// �I��
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^�̍Đ��Ƀ{�����[����ݒ肷��( 100����1�f�V�x���P�� )
extern	int	NS_SetNextPlayVolumeSoundMem( int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	VolumePal -= 10000 ;
	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->NextPlayVolume[ i ] = VolumePal ;
		Sound->ValidNextPlayVolume[ i ] = 1 ;
	}

	// �I��
	return 0 ;
}

// �������ɓǂ݂���WAVE�f�[�^�̍Đ��Ƀ{�����[����ݒ肷��( �p�[�Z���g�w�� )
extern	int	NS_ChangeNextPlayVolumeSoundMem( int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	LONG vol ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	     if( VolumePal > 255 ) VolumePal = 255 ;
	else if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = D_DSBVOLUME_MIN ;
	}
	else
	{
		if( DSOUND.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < D_DSBVOLUME_MIN ) vol = D_DSBVOLUME_MIN ;
	}

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Sound->NextPlayVolume[ i ] = vol ;
		Sound->ValidNextPlayVolume[ i ] = 1 ;
	}

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎��̍Đ��ɂ̂ݎg�p����`�����l���̃{�����[����ݒ肷��( 100����1�f�V�x���P�� 0 �` 10000 )
extern int NS_SetNextPlayChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	VolumePal -= 10000 ;
	if( VolumePal >  0 ) VolumePal =  0 ;
	if( VolumePal < -10000 ) VolumePal = -10000 ;

	Sound->NextPlayVolume[ Channel ] = VolumePal ;
	Sound->ValidNextPlayVolume[ Channel ] = 1 ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎��̍Đ��ɂ̂ݎg�p����`�����l���̃{�����[����ݒ肷��( 0 �` 255 )
extern int NS_ChangeNextPlayChannelVolumeSoundMem( int Channel, int VolumePal, int SoundHandle )
{
	SOUND * Sound ;
	LONG vol ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	     if( VolumePal > 255 ) VolumePal = 255 ;
	else if( VolumePal < 0   ) VolumePal = 0   ;

	if( VolumePal == 0 )
	{
		vol = D_DSBVOLUME_MIN ;
	}
	else
	{
		if( DSOUND.OldVolumeTypeFlag )
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 10.0 * 100.0 ) ) ;
		}
		else
		{
			vol = (LONG)( _DTOL( _LOG10( VolumePal / 255.0 ) * 50.0 * 100.0 ) ) ;
		}
		if( vol < D_DSBVOLUME_MIN ) vol = D_DSBVOLUME_MIN ;
	}

	Sound->NextPlayVolume[ Channel ] = vol ;
	Sound->ValidNextPlayVolume[ Channel ] = 1 ;

	// �I��
	return 0 ;
}

// �������ɓǂݍ���WAVE�f�[�^�̍Đ����g����ݒ肷��
extern	int	NS_SetNextPlayFrequencySoundMem( int FrequencyPal, int SoundHandle )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	if( FrequencyPal == -1 ) FrequencyPal = D_DSBFREQUENCY_ORIGINAL ;

	Sound->NextPlayFrequency = FrequencyPal ;
	Sound->ValidNextPlayFrequency = 1 ;

	// �I��
	return 0 ;
}


// �T�E���h�n���h���̍Đ��ʒu���T���v���P�ʂŕύX����(�Đ����~�܂��Ă��鎞�̂ݗL��)
extern int NS_SetCurrentPositionSoundMem( int SamplePosition, int SoundHandle )
{
	SOUND * sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �ύX
	return NS_SetSoundCurrentPosition( sd->BufferFormat.nBlockAlign * SamplePosition, SoundHandle ) ;
}

// �T�E���h�n���h���̍Đ��ʒu���T���v���P�ʂŎ擾����
extern int NS_GetCurrentPositionSoundMem( int SoundHandle )
{
	SOUND * sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �擾
	return NS_GetSoundCurrentPosition( SoundHandle ) / sd->BufferFormat.nBlockAlign ;
}

// �T�E���h�n���h���̍Đ��ʒu���o�C�g�P�ʂŕύX����(�Đ����~�܂��Ă��鎞�̂ݗL��)
extern int NS_SetSoundCurrentPosition( int Byte, int SoundHandle )
{
	SOUND * sd ;
//	int i ;
//	DWORD State ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_SetStreamSoundCurrentPosition( Byte, SoundHandle ) ;
	}

	// �Đ��ʒu��ύX����
//	sd->Buffer[ i ]->SetCurrentPosition( Byte ) ;
//	SoundBuffer_SetCurrentPosition( &sd->Buffer[ i ], Byte ) ;
	SoundBuffer_SetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], ( DWORD )Byte ) ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̍Đ��ʒu���o�C�g�P�ʂŎ擾����
extern int NS_GetSoundCurrentPosition( int SoundHandle )
{
	SOUND * sd ;
	DWORD pos ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_GetStreamSoundCurrentPosition( SoundHandle ) ;
	}

	// �Đ����̃o�b�t�@�̍Đ��ʒu��Ԃ�
//	sd->Buffer[ Sound->Normal.BackPlayBufferNo ]->GetCurrentPosition( &pos, NULL ) ;
	SoundBuffer_GetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], &pos, NULL ) ;

	return (int)pos ;
}

// �T�E���h�n���h���̍Đ��ʒu���~���b�P�ʂŐݒ肷��(���k�`���̏ꍇ�͐������ݒ肳��Ȃ��ꍇ������)
extern int NS_SetSoundCurrentTime( int Time, int SoundHandle )
{
	SOUND * sd ;
	int /*i,*/ time ;
//	DWORD State ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_SetStreamSoundCurrentTime( Time, SoundHandle ) ;
	}

	// �Đ��ʒu��ύX����
//	time = _DTOL( ( (double)sd->BufferFormat.nSamplesPerSec * Time / 1000 ) * sd->BufferFormat.nBlockAlign ) ;
//	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	time = MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, Time ) * sd->BufferFormat.nBlockAlign ;

//	sd->Buffer[ i ]->SetCurrentPosition( time ) ;
//	SoundBuffer_SetCurrentPosition( &sd->Buffer[ i ], time ) ;
	SoundBuffer_SetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], ( DWORD )time ) ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̉��̑����Ԃ��擾����(�P�ʂ̓T���v��)
extern int NS_GetSoundTotalSample( int SoundHandle )
{
	SOUND * sd ;
	int sample = -1 ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �T�E���h�^�C�v�ɂ���ď����𕪊�
	switch( sd->Type )
	{
	case DX_SOUNDTYPE_STREAMSTYLE : // �X�g���[���^�C�v�̏ꍇ
		sample = sd->Stream.TotalSample ;
		break ;
		
	case DX_SOUNDTYPE_NORMAL :		// �m�[�}���^�C�v�̏ꍇ
		sample = sd->Normal.WaveSize / sd->BufferFormat.nBlockAlign ;
		break ;
	}
	
	// �I��
	return sample ;
}

// �T�E���h�n���h���̉��̑����Ԃ��擾����(�P�ʂ̓~���b)
extern int NS_GetSoundTotalTime( int SoundHandle )
{
	SOUND * sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;
	
	// �~���b�ɕϊ����ĕԂ�
	return SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, NS_GetSoundTotalSample( SoundHandle ) ) ;
}

// �T�E���h�n���h���̍Đ��ʒu���~���b�P�ʂŎ擾����(���k�`���̏ꍇ�͐������l���Ԃ��Ă��Ȃ��ꍇ������)
extern int NS_GetSoundCurrentTime( int SoundHandle )
{
	SOUND * sd ;
	DWORD time ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �T�E���h�^�C�v���X�g���[�����T�E���h�������ꍇ�͂������ɏ������ڂ�
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
	{
		return NS_GetStreamSoundCurrentTime( SoundHandle ) ;
	}

	// �T�E���h�o�b�t�@�O�̍Đ��ʒu���擾����
//	sd->Buffer[ 0 ]->GetCurrentPosition( &time, NULL ) ;
	SoundBuffer_GetCurrentPosition( &sd->Buffer[ sd->Normal.BackPlayBufferNo ], &time, NULL ) ;
//	return _DTOL( (double)time * 1000 / ( sd->BufferFormat.nSamplesPerSec * sd->BufferFormat.nBlockAlign ) ) ;
	return SamplePositionToMilliSecPosition( ( int )sd->BufferFormat.nSamplesPerSec, ( int )( time / sd->BufferFormat.nBlockAlign ) ) ;
}


// �T�E���h�n���h���Ƀ��[�v�ʒu��ݒ肷��
extern int NS_SetLoopPosSoundMem( int LoopTime, int SoundHandle )
{
	SOUND * sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ���[�v�T���v���ʒu���Z�b�g
	NS_SetLoopSamplePosSoundMem( LoopTime == -1 ? -1 : MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopTime ), SoundHandle ) ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���Ƀ��[�v�ʒu��ݒ肷��
extern int NS_SetLoopTimePosSoundMem( int LoopTime, int SoundHandle )
{
	SOUND * sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;
	
	// ���[�v�T���v���ʒu���Z�b�g
	NS_SetLoopSamplePosSoundMem( LoopTime == -1 ? -1 : MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopTime ), SoundHandle ) ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���Ƀ��[�v�ʒu��ݒ肷��
extern int NS_SetLoopSamplePosSoundMem( int LoopSamplePosition, int SoundHandle )
{
	SOUND * sd ;
	STREAMFILEDATA * pl ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		goto ERR ;

	// �w��̈ʒu�������t�@�C���S�̂��������ꍇ�̓G���[
	if( NS_GetSoundTotalSample( SoundHandle ) <= LoopSamplePosition )
		goto ERR ;

	// ���[�v�ʒu�� -1 �������ꍇ�̓��[�v�ʒu�ݒ����������
	if( LoopSamplePosition == -1 )
	{
		pl = &sd->Stream.File[0] ;
		pl->LoopSamplePosition = 0 ;
		pl->LoopNum = 0 ;
		goto END ;
	}

	// �T�E���h�n���h�����X�g���[���Đ��ł͂Ȃ�������X�g���[���Đ��̃n���h���ɕύX����
	if( sd->Type == DX_SOUNDTYPE_NORMAL )
	{
		if( SoundTypeChangeToStream( SoundHandle ) < 0 )
			return -1 ;

		// �G���[����
		if( SOUNDHCHK( SoundHandle, sd ) )
			return -1 ;
	}

	// ���̏��X�g���[���f�[�^���Q�ȏ゠�����疳��
	if( sd->Stream.FileNum > 1 )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\x8c\x4e\x64\x30\xe5\x4e\x0a\x4e\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x4b\x30\x89\x30\x5c\x4f\x10\x62\x55\x30\x8c\x30\x5f\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\xeb\x30\xfc\x30\xd7\x30\xdd\x30\xa4\x30\xf3\x30\xc8\x30\x92\x30\x07\x63\x9a\x5b\x59\x30\x8b\x30\x8b\x4e\x6f\x30\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"��ȏ�̉����t�@�C������쐬���ꂽ�T�E���h�n���h���Ƀ��[�v�|�C���g���w�肷�鎖�͏o���܂���\n" @*/ ) ;
		goto ERR ;
	}

	pl = &sd->Stream.File[0] ;

/*	// �����k wave �� OGG �ȊO�͖���
	if( pl->DataType != DX_SOUNDDATATYPE_MEMNOPRESS &&
#ifndef DX_NON_OGGVORBIS
		pl->OggFlag == FALSE && 
#endif
		( pl->DataType == DX_SOUNDDATATYPE_MEMPRESS ||
			( pl->DataType == DX_SOUNDDATATYPE_FILE && pl->FileData.FormatMatchFlag == FALSE ) ) )
	{
		DXST_ERRORLOG_ADDUTF16LE( L"�����k�v�`�u�d�t�@�C���Ⴕ���͂n�f�f�t�@�C���ȊO�̓��[�v�|�C���g���w�肷�鎖�͏o���܂���\n" ) ;

		
		
		return -1 ;
	}
*/
	// ���[�v�ʒu��ݒ�
	pl->LoopSamplePosition = LoopSamplePosition ;
	SetupSeekPosSoundConvert( &pl->ConvData, LoopSamplePosition ) ;

	// �������[�v�ɐݒ�
	pl->LoopNum = -1 ;

END :

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;

ERR :

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[�I��
	return -1 ;
}

// �T�E���h�n���h���Ƀ��[�v�J�n�ʒu��ݒ肷��
extern int NS_SetLoopStartTimePosSoundMem( int LoopStartTime, int SoundHandle )
{
	SOUND * sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ���[�v�T���v���ʒu���Z�b�g
	NS_SetLoopStartSamplePosSoundMem( LoopStartTime == -1 ? -1 : MilliSecPositionToSamplePosition( ( int )sd->BufferFormat.nSamplesPerSec, LoopStartTime ), SoundHandle ) ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���Ƀ��[�v�J�n�ʒu��ݒ肷��
extern int NS_SetLoopStartSamplePosSoundMem( int LoopStartSamplePosition, int SoundHandle )
{
	SOUND * sd ;
	STREAMFILEDATA * pl ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		goto ERR ;

	// �w��̈ʒu�������t�@�C���S�̂��������ꍇ�̓G���[
	if( NS_GetSoundTotalSample( SoundHandle ) <= LoopStartSamplePosition )
		goto ERR ;

	// ���[�v�ʒu�� -1 �������ꍇ�̓��[�v�ʒu�ݒ����������
	if( LoopStartSamplePosition == -1 )
	{
		pl = &sd->Stream.File[0] ;
		pl->LoopStartSamplePosition = -1 ;
		goto END ;
	}

	// �T�E���h�n���h�����X�g���[���Đ��ł͂Ȃ�������X�g���[���Đ��̃n���h���ɕύX����
	if( sd->Type == DX_SOUNDTYPE_NORMAL )
	{
		if( SoundTypeChangeToStream( SoundHandle ) < 0 )
			return -1 ;

		// �G���[����
		if( SOUNDHCHK( SoundHandle, sd ) )
			return -1 ;
	}

	// ���̏��X�g���[���f�[�^���Q�ȏ゠�����疳��
	if( sd->Stream.FileNum > 1 )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\x8c\x4e\x64\x30\xe5\x4e\x0a\x4e\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x4b\x30\x89\x30\x5c\x4f\x10\x62\x55\x30\x8c\x30\x5f\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6b\x30\xeb\x30\xfc\x30\xd7\x30\x8b\x95\xcb\x59\x4d\x4f\x6e\x7f\x92\x30\x07\x63\x9a\x5b\x59\x30\x8b\x30\x8b\x4e\x6f\x30\xfa\x51\x65\x67\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"��ȏ�̉����t�@�C������쐬���ꂽ�T�E���h�n���h���Ƀ��[�v�J�n�ʒu���w�肷�鎖�͏o���܂���\n" @*/ ) ;
		goto ERR ;
	}

	pl = &sd->Stream.File[0] ;

	// ���[�v�ʒu��ݒ�
	pl->LoopStartSamplePosition = LoopStartSamplePosition ;

	// �]�������ʒu�����Ƀ��[�v�J�n�ʒu���z���Ă��܂��Ă�����X�g���[���T�E���h�f�[�^�̃Z�b�g�A�b�v����蒼��
	if( sd->Stream.FileCompCopyLength > pl->LoopStartSamplePosition )
	{
		RefreshStreamSoundPlayCompLength( SoundHandle ) ;
		NS_SetStreamSoundCurrentPosition( ( int )sd->Stream.CompPlayWaveLength, SoundHandle ) ;
	}

END :
	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;

ERR :
	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �G���[�I��
	return -1 ;
}

// �T�E���h�n���h���̍Đ����I�������玩���I�Ƀn���h�����폜���邩�ǂ�����ݒ肷��
extern int NS_SetPlayFinishDeleteSoundMem( int DeleteFlag, int SoundHandle )
{
	SOUND *sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ���܂łƃt���O�������ꍇ�͉������Ȃ�
	if( sd->PlayFinishDeleteFlag == DeleteFlag )
	{
		return 0 ;
	}

	// �t���O��ۑ�
	sd->PlayFinishDeleteFlag = DeleteFlag ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �t���O�����Ă���ꍇ�̓��X�g�ɒǉ�����A�|�����ꍇ�̓��X�g����O��
	if( DeleteFlag )
	{
		AddHandleList( &DSOUND.PlayFinishDeleteSoundListFirst, &sd->PlayFinishDeleteSoundList, SoundHandle, sd ) ;
	}
	else
	{
		SubHandleList( &sd->PlayFinishDeleteSoundList ) ;
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// ����I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̃��o�[�u�p�����[�^��ݒ肷��
extern int NS_Set3DReverbParamSoundMem( const SOUND3D_REVERB_PARAM *Param, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// �p�����[�^��ۑ�
	sd->PresetReverbParam = -1 ;
	_MEMCPY( &sd->ReverbParam, Param, sizeof( sd->ReverbParam ) ) ;

	// �o�b�t�@�̐������J��Ԃ�
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_SetReverbParam( &sd->Buffer[ i ], &sd->ReverbParam ) ;
	}

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̃��o�[�u�p�����[�^���v���Z�b�g���g�p���Đݒ肷��
extern int NS_Set3DPresetReverbParamSoundMem( int PresetNo /* DX_REVERB_PRESET_DEFAULT �� */ , int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;
	if( PresetNo < 0 || PresetNo >= DX_REVERB_PRESET_NUM )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// �p�����[�^��ۑ�
	sd->PresetReverbParam = PresetNo ;

	// �o�b�t�@�̐������J��Ԃ�
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_SetPresetReverbParam( &sd->Buffer[ i ], PresetNo ) ;
	}

	// �I��
	return 0 ;
}

// �S�Ă̂R�c�T�E���h�̃T�E���h�n���h���Ƀ��o�[�u�p�����[�^��ݒ肷��
extern int NS_Set3DReverbParamSoundMemAll( const SOUND3D_REVERB_PARAM *Param, int PlaySoundOnly )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	if( PlaySoundOnly )
	{
		List = DSOUND.Play3DSoundListFirst.Next ;
	}
	else
	{
		List = DSOUND._3DSoundListFirst.Next ;
	}
	for( ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		// �p�����[�^��ۑ�
		Sound->PresetReverbParam = -1 ;
		_MEMCPY( &Sound->ReverbParam, Param, sizeof( Sound->ReverbParam ) ) ;

		// �o�b�t�@�̐������J��Ԃ�
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			// �p�����[�^�̍X�V
			SoundBuffer_SetReverbParam( &Sound->Buffer[ i ], &Sound->ReverbParam ) ;
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �S�Ă̂R�c�T�E���h�̃T�E���h�n���h���Ƀ��o�[�u�p�����[�^���v���Z�b�g���g�p���Đݒ肷��
extern int NS_Set3DPresetReverbParamSoundMemAll( int PresetNo /* DX_REVERB_PRESET_DEFAULT �� */, int PlaySoundOnly )
{
	HANDLELIST *List ;
	SOUND *Sound ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	if( PresetNo < 0 || PresetNo >= DX_REVERB_PRESET_NUM )
		return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	if( PlaySoundOnly )
	{
		List = DSOUND.Play3DSoundListFirst.Next ;
	}
	else
	{
		List = DSOUND._3DSoundListFirst.Next ;
	}
	for( ; List->Next != NULL ; List = List->Next )
	{
		Sound = ( SOUND * )List->Data ;

		// �p�����[�^��ۑ�
		Sound->PresetReverbParam = PresetNo ;

		// �o�b�t�@�̐������J��Ԃ�
		for( i = 0 ; i < Sound->ValidBufferNum ; i ++ )
		{
			// �p�����[�^�̍X�V
			SoundBuffer_SetPresetReverbParam( &Sound->Buffer[ i ], PresetNo ) ;
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���ɐݒ肳��Ă���R�c�T�E���h�p�̃��o�[�u�p�����[�^���擾����
extern int NS_Get3DReverbParamSoundMem( SOUND3D_REVERB_PARAM *ParamBuffer, int SoundHandle )
{
	SOUND *sd ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// �p�����[�^���R�s�[
	if( ParamBuffer != NULL ) *ParamBuffer = sd->ReverbParam ;

	// ����I��
	return 0 ;
}

// �v���Z�b�g�̂R�c�T�E���h�p�̃��o�[�u�p�����[�^���擾����
extern int NS_Get3DPresetReverbParamSoundMem( SOUND3D_REVERB_PARAM *ParamBuffer, int PresetNo /* DX_REVERB_PRESET_DEFAULT �� */ )
{
	// �G���[����
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	if( DSOUND.EnableXAudioFlag == FALSE )
		return -1 ;

	if( PresetNo < 0 || PresetNo >= DX_REVERB_PRESET_NUM )
		return -1 ;

	// �p�����[�^���R�s�[
	if( ParamBuffer != NULL )
	{
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			D_XAUDIO2FX_REVERB_PARAMETERS2_8 *Param2_8 ;

			Param2_8 = &DSOUND.XAudio2_8ReverbParameters[ PresetNo ] ;

			ParamBuffer->WetDryMix           = Param2_8->WetDryMix ;

			ParamBuffer->ReflectionsDelay    = Param2_8->ReflectionsDelay;
			ParamBuffer->ReverbDelay         = Param2_8->ReverbDelay ;
			ParamBuffer->RearDelay           = Param2_8->RearDelay ;

			ParamBuffer->PositionLeft        = Param2_8->PositionLeft ;
			ParamBuffer->PositionRight       = Param2_8->PositionRight ;
			ParamBuffer->PositionMatrixLeft  = Param2_8->PositionMatrixLeft ;
			ParamBuffer->PositionMatrixRight = Param2_8->PositionMatrixRight ;
			ParamBuffer->EarlyDiffusion      = Param2_8->EarlyDiffusion ;
			ParamBuffer->LateDiffusion       = Param2_8->LateDiffusion ;
			ParamBuffer->LowEQGain           = Param2_8->LowEQGain ;
			ParamBuffer->LowEQCutoff         = Param2_8->LowEQCutoff ;
			ParamBuffer->HighEQGain          = Param2_8->HighEQGain ;
			ParamBuffer->HighEQCutoff        = Param2_8->HighEQCutoff ;

			ParamBuffer->RoomFilterFreq      = Param2_8->RoomFilterFreq ;
			ParamBuffer->RoomFilterMain      = Param2_8->RoomFilterMain ;
			ParamBuffer->RoomFilterHF        = Param2_8->RoomFilterHF ;
			ParamBuffer->ReflectionsGain     = Param2_8->ReflectionsGain ;
			ParamBuffer->ReverbGain          = Param2_8->ReverbGain ;
			ParamBuffer->DecayTime           = Param2_8->DecayTime ;
			ParamBuffer->Density             = Param2_8->Density ;
			ParamBuffer->RoomSize            = Param2_8->RoomSize ;
		}
		else
		{
			D_XAUDIO2FX_REVERB_PARAMETERS *Param ;

			Param = &DSOUND.XAudio2ReverbParameters[ PresetNo ] ;

			ParamBuffer->WetDryMix           = Param->WetDryMix ;

			ParamBuffer->ReflectionsDelay    = Param->ReflectionsDelay;
			ParamBuffer->ReverbDelay         = Param->ReverbDelay ;
			ParamBuffer->RearDelay           = Param->RearDelay ;

			ParamBuffer->PositionLeft        = Param->PositionLeft ;
			ParamBuffer->PositionRight       = Param->PositionRight ;
			ParamBuffer->PositionMatrixLeft  = Param->PositionMatrixLeft ;
			ParamBuffer->PositionMatrixRight = Param->PositionMatrixRight ;
			ParamBuffer->EarlyDiffusion      = Param->EarlyDiffusion ;
			ParamBuffer->LateDiffusion       = Param->LateDiffusion ;
			ParamBuffer->LowEQGain           = Param->LowEQGain ;
			ParamBuffer->LowEQCutoff         = Param->LowEQCutoff ;
			ParamBuffer->HighEQGain          = Param->HighEQGain ;
			ParamBuffer->HighEQCutoff        = Param->HighEQCutoff ;

			ParamBuffer->RoomFilterFreq      = Param->RoomFilterFreq ;
			ParamBuffer->RoomFilterMain      = Param->RoomFilterMain ;
			ParamBuffer->RoomFilterHF        = Param->RoomFilterHF ;
			ParamBuffer->ReflectionsGain     = Param->ReflectionsGain ;
			ParamBuffer->ReverbGain          = Param->ReverbGain ;
			ParamBuffer->DecayTime           = Param->DecayTime ;
			ParamBuffer->Density             = Param->Density ;
			ParamBuffer->RoomSize            = Param->RoomSize ;
		}
	}

	// ����I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̍Đ��ʒu��ݒ肷��
extern int NS_Set3DPositionSoundMem( VECTOR Position, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = �P���[�g���̒P�ʂɕϊ�����
	VectorScale( &Position, &Position, 1.0f / DSOUND._3DSoundOneMetre ) ;

	// �o�b�t�@�̐������J��Ԃ�
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_Set3DPosition( &sd->Buffer[ i ], &Position ) ;
	}
	sd->Base3DPosition = Position ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̉����������鋗����ݒ肷��
extern int NS_Set3DRadiusSoundMem( float Radius, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	Radius /= DSOUND._3DSoundOneMetre ;

	// �o�b�t�@�̐������J��Ԃ�
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_Set3DRadius( &sd->Buffer[ i ], Radius ) ;
	}
	sd->Base3DRadius = Radius ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̉��ʂP�O�O���ŕ������鋗����ݒ肷��
extern int NS_Set3DInnerRadiusSoundMem( float Radius, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	Radius /= DSOUND._3DSoundOneMetre ;

	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_Set3DInnerRadius( &sd->Buffer[ i ], Radius ) ;
	}

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̈ړ����x��ݒ肷��
extern int NS_Set3DVelocitySoundMem( VECTOR Velocity, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = �P���[�g���̒P�ʂɕϊ�����
	VectorScale( &Velocity, &Velocity, 1.0f / DSOUND._3DSoundOneMetre ) ;

	// �o�b�t�@�̐������J��Ԃ�
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_Set3DVelocity( &sd->Buffer[ i ], &Velocity ) ;
	}
	sd->Base3DVelocity = Velocity ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̕��������肷�邽�߂̑O�����W�Ə������ݒ肷��
extern int NS_Set3DFrontPositionSoundMem( VECTOR FrontPosition, VECTOR UpVector, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// �o�b�t�@�̐������J��Ԃ�
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_Set3DFrontPosition( &sd->Buffer[ i ], &FrontPosition, &UpVector ) ;
	}

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̕��������肷�邽�߂̑O�����W��ݒ肷��( �������Y�� )
extern int NS_Set3DFrontPosition_UpVecYSoundMem( VECTOR FrontPosition, int SoundHandle )
{
	VECTOR UpVec = { 0.0f, 1.0f, 0.0f } ;

	return NS_Set3DFrontPositionSoundMem( FrontPosition, UpVec, SoundHandle ) ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̕����̊p�x�͈͂�ݒ肷��
extern int NS_Set3DConeAngleSoundMem( float InnerAngle, float OuterAngle, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	if( OuterAngle < 0.0f )
	{
		OuterAngle = 0.0f ;
	}
	else
	if( OuterAngle > DX_PI_F * 2.0f )
	{
		OuterAngle = DX_PI_F * 2.0f ;
	}

	if( InnerAngle < 0.0f )
	{
		InnerAngle = 0.0f ;
	}
	else
	if( InnerAngle > DX_PI_F * 2.0f )
	{
		InnerAngle = DX_PI_F * 2.0f ;
	}

	if( OuterAngle < InnerAngle )
	{
		float Temp ;
		Temp = InnerAngle ;
		InnerAngle = OuterAngle ;
		OuterAngle = Temp ;
	}

	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_Set3DConeAngle( &sd->Buffer[ i ], InnerAngle, OuterAngle ) ;
	}

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̂R�c�T�E���h�p�̕����̊p�x�͈͂̉��ʔ{����ݒ肷��
extern int NS_Set3DConeVolumeSoundMem( float InnerAngleVolume, float OuterAngleVolume, int SoundHandle )
{
	SOUND *sd ;
	int i ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( sd->Is3DSound == FALSE )
		return -1 ;

	// �o�b�t�@�[�̐������J��Ԃ�
	for( i = 0 ; i < sd->ValidBufferNum ; i ++ )
	{
		// �p�����[�^�̍X�V
		SoundBuffer_Set3DConeVolume( &sd->Buffer[ i ], InnerAngleVolume, OuterAngleVolume ) ;
	}

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎��̍Đ��݂̂Ɏg�p����R�c�T�E���h�p�̍Đ��ʒu��ݒ肷��
extern int NS_SetNextPlay3DPositionSoundMem( VECTOR Position, int SoundHandle )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Sound->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = �P���[�g���̒P�ʂɕϊ�����
	VectorScale( &Position, &Position, 1.0f / DSOUND._3DSoundOneMetre ) ;

	Sound->NextPlay3DPosition = Position ;
	Sound->ValidNextPlay3DPosition = 1 ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎��̍Đ��݂̂Ɏg�p����R�c�T�E���h�p�̉����������鋗����ݒ肷��
extern int NS_SetNextPlay3DRadiusSoundMem( float Radius, int SoundHandle )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Sound->Is3DSound == FALSE )
		return -1 ;

	Radius /= DSOUND._3DSoundOneMetre ;

	Sound->NextPlay3DRadius = Radius ;
	Sound->ValidNextPlay3DRadius = 1 ;

	// �I��
	return 0 ;
}

// �T�E���h�n���h���̎��̍Đ��݂̂Ɏg�p����R�c�T�E���h�p�̈ړ����x��ݒ肷��
extern int NS_SetNextPlay3DVelocitySoundMem( VECTOR Velocity, int SoundHandle )
{
	SOUND * Sound ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, Sound ) )
		return -1 ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Sound->Is3DSound == FALSE )
		return -1 ;

	// 1.0f = �P���[�g���̒P�ʂɕϊ�����
	VectorScale( &Velocity, &Velocity, 1.0f / DSOUND._3DSoundOneMetre ) ;

	Sound->NextPlay3DVelocity = Velocity ;
	Sound->ValidNextPlay3DVelocity = 1 ;

	// �I��
	return 0 ;
}










// �ݒ�֌W�֐�

// �쐬����T�E���h�̃f�[�^�`����ݒ肷��
extern int NS_SetCreateSoundDataType( int SoundDataType )
{
	// �l���͈͊O�̃f�[�^�`�����ǂ������ׂ�
	if( SoundDataType >= DX_SOUNDDATATYPE_MEMNOPRESS && SoundDataType <= DX_SOUNDDATATYPE_FILE )
	{
		// ���� DX_SOUNDDATATYPE_MEMNOPRESS_PLUS �͔�Ή�
		if( SoundDataType == DX_SOUNDDATATYPE_MEMNOPRESS_PLUS )
		{
			DSOUND.CreateSoundDataType = DX_SOUNDDATATYPE_MEMNOPRESS ;
		}
		else
		{
			DSOUND.CreateSoundDataType = SoundDataType ;
		}
	}
	else
	{
		return -1 ; 
	}

	// �I��
	return 0 ;
}

// �쐬����T�E���h�̃f�[�^�`�����擾����( DX_SOUNDDATATYPE_MEMNOPRESS �� )
extern	int NS_GetCreateSoundDataType( void )
{
	return DSOUND.CreateSoundDataType ;
}

// �g�p���Ȃ��T�E���h�f�[�^�ǂݍ��ݏ����̃}�X�N��ݒ肷��
extern	int NS_SetDisableReadSoundFunctionMask( int Mask )
{
	DSOUND.DisableReadSoundFunctionMask = Mask ;

	// �I��
	return 0 ;
}

// �g�p���Ȃ��T�E���h�f�[�^�ǂݍ��ݏ����̃}�X�N���擾����
extern	int	NS_GetDisableReadSoundFunctionMask( void )
{
	return DSOUND.DisableReadSoundFunctionMask ;
}

// �T�E���h�L���v�`����O��Ƃ�����������邩�ǂ�����ݒ肷��
extern	int NS_SetEnableSoundCaptureFlag( int Flag )
{
	// �t���O�������ꍇ�͉������Ȃ�
	if( DSOUND.EnableSoundCaptureFlag == Flag ) return 0 ;

	// �S�ẴT�E���h�n���h�����폜����
	NS_InitSoundMem() ;
	
	// �t���O���Z�b�g����
	DSOUND.EnableSoundCaptureFlag = Flag ;
	
	// �I��
	return 0 ;
}

// �T�E���h�̏������\�t�g�E�G�A�ōs�����ǂ�����ݒ肷��( TRUE:�\�t�g�E�G�A  FALSE:�n�[�h�E�G�A( �f�t�H���g ) )
extern int NS_SetUseSoftwareMixingSoundFlag( int Flag )
{
	// �t���O��ۑ�
	DSOUND.UseSoftwareMixing = Flag ;

	// �I��
	return 0 ;
}

// �T�E���h�̍Đ���XAudio���g�p���邩�ǂ�����ݒ肷��( TRUE:�g�p����  FALSE:�g�p���Ȃ�( �f�t�H���g ) )
extern int NS_SetEnableXAudioFlag( int Flag )
{
	// �������ς݂̏ꍇ�̓G���[
	if( DSOUND.InitializeFlag != FALSE )
		return -1 ;

	// �t���O��ۑ�����
	DSOUND.EnableXAudioFlag = Flag ;
	
	// �I��
	return 0 ;
}

// ChangeVolumeSoundMem, ChangeNextPlayVolumeSoundMem, ChangeMovieVolumeToGraph �̉��ʌv�Z���� Ver3.10c�ȑO�̂��̂��g�p���邩�ǂ�����ݒ肷��( TRUE:Ver3.10c�ȑO�̌v�Z�����g�p  FALSE:3.10d�ȍ~�̌v�Z�����g�p( �f�t�H���g ) )
extern int NS_SetUseOldVolumeCalcFlag( int Flag )
{
	// �t���O��ۑ�����
	DSOUND.OldVolumeTypeFlag = Flag ;
	
	// �I��
	return 0 ;
}

// ���ɍ쐬����T�E���h���R�c�T�E���h�p�ɂ��邩�ǂ�����ݒ肷��( TRUE:�R�c�T�E���h�p�ɂ���  FALSE:�R�c�T�E���h�p�ɂ��Ȃ�( �f�t�H���g ) )
extern int NS_SetCreate3DSoundFlag( int Flag )
{
	// �t���O��ۑ�����
	DSOUND.Create3DSoundFlag = Flag != FALSE ? TRUE : FALSE ;

	// �I��
	return 0 ;
}

// �R�c��Ԃ̂P���[�g���ɓ��鋗����ݒ肷��( �f�t�H���g:1.0f )
extern int NS_Set3DSoundOneMetre( float Distance )
{
	if( DSOUND.DirectSoundObject != NULL ) return -1 ;

	if( Distance <= 0.0f )
	{
		DSOUND._3DSoundOneMetreEnable = FALSE ;
		DSOUND._3DSoundOneMetre = 1.0f ;
	}
	else
	{
		DSOUND._3DSoundOneMetreEnable = TRUE ;
		DSOUND._3DSoundOneMetre = Distance ;
	}

	// �I��
	return 0 ;
}

// �R�c�T�E���h�̃��X�i�[�̈ʒu�ƃ��X�i�[�̑O���ʒu��ݒ肷��( ���X�i�[�̏�����ʒu�͂x���Œ� )
extern int NS_Set3DSoundListenerPosAndFrontPos_UpVecY( VECTOR Position, VECTOR FrontPosition )
{
	VECTOR UpVec = { 0.0f, 1.0f, 0.0f } ;

	return NS_Set3DSoundListenerPosAndFrontPosAndUpVec( Position, FrontPosition, UpVec ) ;
}

// �R�c�T�E���h�̃��X�i�[�̈ʒu�ƃ��X�i�[�̑O���ʒu�ƃ��X�i�[�̏�����ʒu��ݒ肷��
extern int NS_Set3DSoundListenerPosAndFrontPosAndUpVec( VECTOR Position, VECTOR FrontPosition, VECTOR UpVector )
{
	VECTOR SideVec ;
	VECTOR DirVec ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	VectorSub( &DirVec, &FrontPosition, &Position ) ;
	VectorNormalize( &DirVec, &DirVec ) ;

	VectorScale( &Position, &Position, 1.0f / DSOUND._3DSoundOneMetre ) ;

	VectorOuterProduct( &SideVec, &DirVec,  &UpVector ) ;
	VectorOuterProduct( &UpVector, &SideVec, &DirVec ) ;
	VectorNormalize( &UpVector, &UpVector ) ;
	VectorNormalize( &SideVec, &SideVec ) ;

	// �l���قƂ�Ǖω����Ȃ��ꍇ�ꍇ�͉������Ȃ�
	if( _FABS( DSOUND.X3DAudioListenerData.Position.x - Position.x ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.Position.y - Position.y ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.Position.z - Position.z ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.OrientFront.x - DirVec.x ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.OrientFront.y - DirVec.y ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.OrientFront.z - DirVec.z ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.OrientTop.x - UpVector.x ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.OrientTop.y - UpVector.y ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.OrientTop.z - UpVector.z ) < 0.001f )
		return 0 ;

	DSOUND.X3DAudioListenerData.Position.x = Position.x ;
	DSOUND.X3DAudioListenerData.Position.y = Position.y ;
	DSOUND.X3DAudioListenerData.Position.z = Position.z ;

	DSOUND.X3DAudioListenerData.OrientFront.x = DirVec.x ;
	DSOUND.X3DAudioListenerData.OrientFront.y = DirVec.y ;
	DSOUND.X3DAudioListenerData.OrientFront.z = DirVec.z ;

	DSOUND.X3DAudioListenerData.OrientTop.x = UpVector.x ;
	DSOUND.X3DAudioListenerData.OrientTop.y = UpVector.y ;
	DSOUND.X3DAudioListenerData.OrientTop.z = UpVector.z ;

	DSOUND.ListenerInfo.Position       = Position ;
	DSOUND.ListenerInfo.FrontDirection = DirVec ;
	DSOUND.ListenerInfo.UpDirection    = UpVector ;
	DSOUND.ListenerSideDirection       = SideVec ;

	// �Đ����̂R�c�T�E���h�̃p�����[�^���X�V����
	Refresh3DSoundParamAll() ;

	// �I��
	return 0 ;
}

// �R�c�T�E���h�̃��X�i�[�̈ړ����x��ݒ肷��
extern int NS_Set3DSoundListenerVelocity( VECTOR Velocity )
{
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	VectorScale( &Velocity, &Velocity, 1.0f / DSOUND._3DSoundOneMetre ) ;

	// �l���قƂ�Ǖω����Ȃ��ꍇ�ꍇ�͉������Ȃ�
	if( _FABS( DSOUND.X3DAudioListenerData.Velocity.x - Velocity.x ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.Velocity.y - Velocity.y ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerData.Velocity.z - Velocity.z ) < 0.001f )
		return 0 ;

	DSOUND.X3DAudioListenerData.Velocity.x = Velocity.x ;
	DSOUND.X3DAudioListenerData.Velocity.y = Velocity.y ;
	DSOUND.X3DAudioListenerData.Velocity.z = Velocity.z ;

	DSOUND.ListenerInfo.Velocity = Velocity ;


	// �Đ����̂R�c�T�E���h�̃p�����[�^���X�V����
	Refresh3DSoundParamAll() ;

	// �I��
	return 0 ;
}

// �R�c�T�E���h�̃��X�i�[�̉��p�x�͈͂�ݒ肷��
extern int NS_Set3DSoundListenerConeAngle( float InnerAngle, float OuterAngle )
{
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	if( OuterAngle < 0.0f )
	{
		OuterAngle = 0.0f ;
	}
	else
	if( OuterAngle > DX_PI_F * 2.0f )
	{
		OuterAngle = DX_PI_F * 2.0f ;
	}

	if( InnerAngle < 0.0f )
	{
		InnerAngle = 0.0f ;
	}
	else
	if( InnerAngle > DX_PI_F * 2.0f )
	{
		InnerAngle = DX_PI_F * 2.0f ;
	}

	if( OuterAngle < InnerAngle )
	{
		float Temp ;
		Temp = InnerAngle ;
		InnerAngle = OuterAngle ;
		OuterAngle = Temp ;
	}

	// �l���قƂ�Ǖω����Ȃ��ꍇ�ꍇ�͉������Ȃ�
	if( _FABS( DSOUND.X3DAudioListenerConeData.InnerAngle - InnerAngle ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerConeData.OuterAngle - OuterAngle ) < 0.001f )
		return 0 ;

	DSOUND.X3DAudioListenerConeData.InnerAngle = InnerAngle ;
	DSOUND.X3DAudioListenerConeData.OuterAngle = OuterAngle ;

	DSOUND.ListenerInfo.InnerAngle = InnerAngle ;
	DSOUND.ListenerInfo.OuterAngle = OuterAngle ;

	// �Đ����̂R�c�T�E���h�̃p�����[�^���X�V����
	Refresh3DSoundParamAll() ;

	// �I��
	return 0 ;
}

// �R�c�T�E���h�̃��X�i�[�̉��p�x�͈͂̉��ʔ{����ݒ肷��
extern int NS_Set3DSoundListenerConeVolume( float InnerAngleVolume, float OuterAngleVolume )
{
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �l���قƂ�Ǖω����Ȃ��ꍇ�ꍇ�͉������Ȃ�
	if( _FABS( DSOUND.X3DAudioListenerConeData.InnerVolume - InnerAngleVolume ) < 0.001f &&
		_FABS( DSOUND.X3DAudioListenerConeData.OuterVolume - OuterAngleVolume ) < 0.001f )
		return 0 ;

	DSOUND.X3DAudioListenerConeData.InnerVolume = InnerAngleVolume ;
	DSOUND.X3DAudioListenerConeData.OuterVolume = OuterAngleVolume ;

	DSOUND.ListenerInfo.InnerVolume = InnerAngleVolume ;
	DSOUND.ListenerInfo.OuterVolume = OuterAngleVolume ;

	// �Đ����̂R�c�T�E���h�̃p�����[�^���X�V����
	Refresh3DSoundParamAll() ;

	// �I��
	return 0 ;
}










// �⏕�n�֐�

// �T�C�Y�␳
static	int AdjustSoundDataBlock( int Length, SOUND * Sound ) 
{
	return Length / Sound->BufferFormat.nBlockAlign * Sound->BufferFormat.nBlockAlign  ;
}

// ���擾�n�֐�

// �c�w���C�u�������g�p���Ă��� DirectSound �I�u�W�F�N�g���擾����
extern const void *NS_GetDSoundObj( void )
{
	return DSOUND.DirectSoundObject ;
}


// �g�`�f�[�^�p
static WAVEDATA *AllocWaveData( int Size, int UseDoubleSizeBuffer )
{
	WAVEDATA *Data ;
	unsigned int AllocSize ;

	AllocSize = sizeof( WAVEDATA ) + 16 + Size ;
	if( UseDoubleSizeBuffer )
	{
		AllocSize += Size * 2 + 32 ;
	}

	Data = ( WAVEDATA * )DXALLOC( AllocSize ) ;
	if( Data == NULL )
		return NULL ;
	Data->Buffer = ( void * )( ( ( DWORD_PTR )Data + sizeof( WAVEDATA ) + 15 ) / 16 * 16 ) ;
	Data->Bytes = Size ;
	Data->RefCount = 1 ;

	if( UseDoubleSizeBuffer )
	{
		Data->DoubleSizeBuffer = ( void * )( ( ( DWORD_PTR )Data->Buffer + Size + 15 ) / 16 * 16 ) ;
	}
	else
	{
		Data->DoubleSizeBuffer = NULL ;
	}

	return Data ;
}

static int ReleaseWaveData( WAVEDATA *Data )
{
	Data->RefCount -- ;
	if( Data->RefCount > 0 )
		return 0 ;

	if( Data->RefCount < 0 )
		return -1 ;

	DXFREE( Data ) ;

	return 0 ;
}

static WAVEDATA *DuplicateWaveData( WAVEDATA *Data )
{
	Data->RefCount ++ ;
	return Data ;
}


void __stdcall SOUNDBUFFER_CALLBACK::OnVoiceProcessingPassStart( DWORD BytesRequired )
{
	int CompPos = Buffer->CompPos ;
	int Loop = Buffer->Loop ;
	D_XAUDIO2_BUFFER XBuffer = { 0 } ;
	DWORD MoveData ;
	DWORD MaxSendSamples ;
	void *SampleBuffer ;
	int BlockAlign ;

	MaxSendSamples = Buffer->Format.nSamplesPerSec / 60 ;
	SampleBuffer = Buffer->Wave->DoubleSizeBuffer ? Buffer->Wave->DoubleSizeBuffer : Buffer->Wave->Buffer ;
	BlockAlign = Buffer->Format.wBitsPerSample * Buffer->XAudioChannels / 8 ;

	MoveData = 0 ;
	while( MoveData < BytesRequired )
	{
		if( CompPos >= Buffer->SampleNum )
		{
			if( Loop )
			{
				CompPos = 0 ;
				goto COPYDATA ;
			}
			else
			{
				SoundBuffer_Stop( Buffer ) ;
				break ;
			}
		}
		else
		{
			DWORD NowBytes ;
			DWORD AddSamples ;

COPYDATA :
			NowBytes           = ( DWORD )( CompPos * BlockAlign ) ;
			XBuffer.pAudioData = ( BYTE * )SampleBuffer + NowBytes ;
			AddSamples         = ( DWORD )( Buffer->SampleNum - CompPos ) ;
			if( AddSamples > MaxSendSamples )
			{
				AddSamples = MaxSendSamples ;
			}
			XBuffer.AudioBytes = AddSamples * BlockAlign ;
			if( XBuffer.AudioBytes > BytesRequired )
			{
				AddSamples = BytesRequired / BlockAlign ;
				XBuffer.AudioBytes = AddSamples * BlockAlign ;
			}
			CompPos += AddSamples ;
			MoveData += XBuffer.AudioBytes ;

			XBuffer.pContext = ( void * )Buffer ;

			if( DSOUND.XAudio2_8DLL != NULL )
			{
				Buffer->XA2_8SourceVoice->SubmitSourceBuffer( &XBuffer ) ;
			}
			else
			{
				Buffer->XA2SourceVoice->SubmitSourceBuffer( &XBuffer ) ;
			}
		}
	}
	Buffer->CompPos = CompPos ;
}

void __stdcall SOUNDBUFFER_CALLBACK::OnVoiceProcessingPassEnd()
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnStreamEnd()
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnBufferStart( void* /*pBufferContext*/ )
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnBufferEnd( void* pBufferContext )
{
	SOUNDBUFFER *Buffer = ( SOUNDBUFFER * )pBufferContext ;

	Buffer->Pos = Buffer->CompPos ;
}

void __stdcall SOUNDBUFFER_CALLBACK::OnLoopEnd( void* /*pBufferContext*/ )
{
}

void __stdcall SOUNDBUFFER_CALLBACK::OnVoiceError( void* /*pBufferContext*/, HRESULT /*Error*/ )
{
}


//�T�E���h�o�b�t�@�p
static HRESULT SoundBuffer_Initialize( SOUNDBUFFER *Buffer, D_DSBUFFERDESC *Desc, SOUNDBUFFER *Src, int Is3DSound )
{
	int i ;
	HRESULT hr ;
	WAVEFORMATEX waveformat ;
	int UseDoubleSizeBuffer = FALSE ;

	// �������ς݂̏ꍇ�̓G���[
	if( Buffer->Valid ) return -1 ;

	Buffer->Wave = NULL ;
	Buffer->DSBuffer = NULL ;
	Buffer->XA2SourceVoice = NULL ;
	Buffer->XA2_8SourceVoice = NULL ;
	Buffer->XA2SubmixVoice = NULL ;
	Buffer->XA2_8SubmixVoice = NULL ;
	Buffer->XA2ReverbEffect = NULL ;
	Buffer->XA2Callback = NULL ;
	Buffer->StopTimeState = 0 ;
	Buffer->StopTime = 0 ;
	Buffer->Is3DSound = Is3DSound ;

	waveformat = Src != NULL ? Src->Format : *Desc->lpwfxFormat ;

	if( DSOUND.EnableSoundCaptureFlag )
	{
		if( Src != NULL )
		{
			Buffer->Wave = DuplicateWaveData( Src->Wave ) ;
		}
		else
		{
			Buffer->Wave = AllocWaveData( ( int )Desc->dwBufferBytes ) ;
		}
		if( Buffer->Wave == NULL )
			goto ERR ;
	}
	else
	if( DSOUND.EnableXAudioFlag )
	{
		// �R�c�T�E���h���ǂ����ŕ���
		if( Is3DSound != FALSE )
		{
			D_XAUDIO2_EFFECT_DESCRIPTOR effects[ 1 ] ;
			D_XAUDIO2_EFFECT_CHAIN effectChain ;

			// �R�c�T�E���h�̏ꍇ�̓T�u�~�b�N�X�{�C�X���쐬����
			hr = D_XAudio2CreateReverb( &Buffer->XA2ReverbEffect, 0 ) ;
			if( FAILED( hr ) )
				goto ERR ;

			effects[ 0 ].pEffect = Buffer->XA2ReverbEffect ;
			effects[ 0 ].InitialState = TRUE ;
			effects[ 0 ].OutputChannels = waveformat.nChannels ;

			effectChain.EffectCount = 1 ;
			effectChain.pEffectDescriptors = effects ;

			if( DSOUND.XAudio2_8DLL != NULL )
			{
				if( FAILED( DSOUND.XAudio2_8Object->CreateSubmixVoice(
					&Buffer->XA2_8SubmixVoice,
					waveformat.nChannels,
					DSOUND.OutputSmaplesPerSec,
					0,
					0,
					NULL,
					&effectChain ) ) )
					goto ERR ;
				_MEMCPY( &Buffer->XAudio2_8ReverbParameter, &DSOUND.XAudio2_8ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ], sizeof( DSOUND.XAudio2_8ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ] ) ) ;
				Buffer->XA2_8SubmixVoice->SetEffectParameters( 0, &Buffer->XAudio2_8ReverbParameter, sizeof( Buffer->XAudio2_8ReverbParameter ) ) ;
				Buffer->XA2_8SubmixVoice->DisableEffect( 0 ) ;
			}
			else
			{
				if( FAILED( DSOUND.XAudio2Object->CreateSubmixVoice(
					&Buffer->XA2SubmixVoice,
					waveformat.nChannels,
					DSOUND.OutputSmaplesPerSec,
					0,
					0,
					NULL,
					&effectChain ) ) )
					goto ERR ;
				_MEMCPY( &Buffer->XAudio2ReverbParameter, &DSOUND.XAudio2ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ], sizeof( DSOUND.XAudio2ReverbParameters[ D_XAUDIO2FX_PRESET_DEFAULT ] ) ) ;
				Buffer->XA2SubmixVoice->SetEffectParameters( 0, &Buffer->XAudio2ReverbParameter, sizeof( Buffer->XAudio2ReverbParameter ) ) ;
				Buffer->XA2SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		else
		{
			// ���m�����T�E���h�ł��R�c�T�E���h�ł͂Ȃ��ꍇ�̓p���̂��߂ɃX�e���I�ɕύX
			if( waveformat.nChannels == 1 )
			{
				waveformat.nChannels = 2 ;
				waveformat.nBlockAlign = ( WORD )( waveformat.nChannels * waveformat.wBitsPerSample / 8 ) ;
				waveformat.nAvgBytesPerSec = waveformat.nBlockAlign * waveformat.nSamplesPerSec ;
				UseDoubleSizeBuffer = TRUE ;
			}
		}
		Buffer->XAudioChannels = waveformat.nChannels ;

		if( Src != NULL )
		{
			Buffer->Wave = DuplicateWaveData( Src->Wave ) ;
		}
		else
		{
			Buffer->Wave = AllocWaveData( ( int )Desc->dwBufferBytes, UseDoubleSizeBuffer ) ;
		}
		if( Buffer->Wave == NULL )
			goto ERR ;

		Buffer->XA2Callback = new SOUNDBUFFER_CALLBACK ;
		if( Buffer->XA2Callback == NULL )
			goto ERR ;

		Buffer->XA2Callback->Buffer = Buffer ;

		if( DSOUND.XAudio2_8DLL != NULL )
		{
			if( Is3DSound != FALSE )
			{
				D_XAUDIO2_VOICE_SENDS2_8 sendList ;
				D_XAUDIO2_SEND_DESCRIPTOR2_8 sendDescriptors[ 2 ] ;

				sendDescriptors[0].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[0].pOutputVoice = DSOUND.XAudio2_8MasteringVoiceObject ;
				sendDescriptors[1].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[1].pOutputVoice = Buffer->XA2_8SubmixVoice ;
				sendList.SendCount = 2 ;
				sendList.pSends = sendDescriptors ;

				hr = DSOUND.XAudio2_8Object->CreateSourceVoice( &Buffer->XA2_8SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->XA2Callback, &sendList ) ;
			}
			else
			{
				hr = DSOUND.XAudio2_8Object->CreateSourceVoice( &Buffer->XA2_8SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->XA2Callback ) ;
			}
		}
		else
		{
			if( Is3DSound != FALSE )
			{
				D_XAUDIO2_VOICE_SENDS sendList ;
				D_XAUDIO2_SEND_DESCRIPTOR sendDescriptors[ 2 ] ;

				sendDescriptors[0].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[0].pOutputVoice = DSOUND.XAudio2MasteringVoiceObject ;
				sendDescriptors[1].Flags = D_XAUDIO2_SEND_USEFILTER ;
				sendDescriptors[1].pOutputVoice = Buffer->XA2SubmixVoice ;
				sendList.SendCount = 2 ;
				sendList.pSends = sendDescriptors ;

				hr = DSOUND.XAudio2Object->CreateSourceVoice( &Buffer->XA2SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->XA2Callback, &sendList ) ;
			}
			else
			{
				hr = DSOUND.XAudio2Object->CreateSourceVoice( &Buffer->XA2SourceVoice, &waveformat, 0, D_XAUDIO2_DEFAULT_FREQ_RATIO, Buffer->XA2Callback ) ;
			}
		}
		
		if( FAILED( hr ) )
			goto ERR ;

		if( Src != NULL )
		{
			for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
			{
				SoundBuffer_SetVolume( Buffer, i, Src->Volume[ i ] ) ;
			}
			SoundBuffer_SetFrequency( Buffer, ( DWORD )Src->Frequency ) ;
			SoundBuffer_SetPan( Buffer, Src->Pan ) ;
		}
	}
	else
	{
		if( DSOUND.DirectSoundObject->CreateSoundBuffer( Desc, &Buffer->DSBuffer, NULL ) != D_DS_OK )
			goto ERR ;
	}

	Buffer->State = FALSE ;
	Buffer->Pos = 0 ;
	Buffer->CompPos = 0 ;
	Buffer->Loop = FALSE ;

	if( Src != NULL )
	{
		Buffer->Pan = Src->Pan ;
		for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
		{
			Buffer->Volume[ i ] = Src->Volume[ i ] ;
		}
		Buffer->Frequency = Src->Frequency ;
		Buffer->SampleNum = Src->SampleNum ;
		Buffer->Format = Src->Format ;
	}
	else
	{
		Buffer->Pan = 0 ;
		for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
		{
			Buffer->Volume[ i ] = 0 ;
		}
		Buffer->Frequency = -1 ;
		Buffer->SampleNum = ( int )( Desc->dwBufferBytes / Desc->lpwfxFormat->nBlockAlign ) ;
		Buffer->Format    = *Desc->lpwfxFormat ;
	}

	// �R�c�T�E���h�̏ꍇ�͂R�c�T�E���h�p�����[�^��������
	if( Is3DSound != FALSE )
	{
		// �ŏ��ɃZ�b�g�A�b�v���s�����߂ɕύX�����t���O�𗧂Ă�
		Buffer->EmitterDataChangeFlag = TRUE ;

		// �G�~�b�^�[�̊�{�I�ȏ����Z�b�g����
		if( Src != NULL )
		{
			Buffer->X3DAudioEmitterConeData              = Src->X3DAudioEmitterConeData ;
			Buffer->X3DAudioEmitterData                  = Src->X3DAudioEmitterData ;
			Buffer->X3DAudioEmitterData.pCone            = &Buffer->X3DAudioEmitterConeData ;
			Buffer->X3DAudioEmitterData.pChannelAzimuths = Buffer->X3DAudioEmitterChannelAzimuths ;
			Buffer->EmitterInfo                          = Src->EmitterInfo ;
			Buffer->EmitterRadius                        = Src->EmitterRadius ;
		}
		else
		{
			Buffer->X3DAudioEmitterData.pCone = &Buffer->X3DAudioEmitterConeData ;
			Buffer->X3DAudioEmitterData.pCone->InnerAngle = 0.0f ;
			Buffer->X3DAudioEmitterData.pCone->OuterAngle = 0.0f ;
			Buffer->X3DAudioEmitterData.pCone->InnerVolume = 0.0f ;
			Buffer->X3DAudioEmitterData.pCone->OuterVolume = 1.0f ;
			Buffer->X3DAudioEmitterData.pCone->InnerLPF = 0.0f ;
			Buffer->X3DAudioEmitterData.pCone->OuterLPF = 1.0f ;
			Buffer->X3DAudioEmitterData.pCone->InnerReverb = 0.0f ;
			Buffer->X3DAudioEmitterData.pCone->OuterReverb = 1.0f ;

			Buffer->X3DAudioEmitterData.Position.x = 0.0f ;
			Buffer->X3DAudioEmitterData.Position.y = 0.0f ;
			Buffer->X3DAudioEmitterData.Position.z = 0.0f ;
			Buffer->X3DAudioEmitterData.OrientFront.x = 0.0f ;
			Buffer->X3DAudioEmitterData.OrientFront.y = 0.0f ;
			Buffer->X3DAudioEmitterData.OrientFront.z = 1.0f ;
			Buffer->X3DAudioEmitterData.OrientTop.x = 0.0f ;
			Buffer->X3DAudioEmitterData.OrientTop.y = 1.0f ;
			Buffer->X3DAudioEmitterData.OrientTop.z = 0.0f ;
			Buffer->X3DAudioEmitterData.Velocity.x = 0.0f ;
			Buffer->X3DAudioEmitterData.Velocity.y = 0.0f ;
			Buffer->X3DAudioEmitterData.Velocity.z = 0.0f ;
			Buffer->X3DAudioEmitterData.ChannelRadius = 0.1f ;
			Buffer->X3DAudioEmitterData.pChannelAzimuths = Buffer->X3DAudioEmitterChannelAzimuths ;

			Buffer->X3DAudioEmitterData.InnerRadius = 2.0f;
			Buffer->X3DAudioEmitterData.InnerRadiusAngle = DX_PI_F / 4.0f ;

			Buffer->X3DAudioEmitterData.pVolumeCurve = ( D_X3DAUDIO_DISTANCE_CURVE * )&D_X3DAudioDefault_LinearCurve ;
			Buffer->X3DAudioEmitterData.pLFECurve    = ( D_X3DAUDIO_DISTANCE_CURVE * )&D_Emitter_LFE_Curve ;
			Buffer->X3DAudioEmitterData.pLPFDirectCurve = NULL ;
			Buffer->X3DAudioEmitterData.pLPFReverbCurve = NULL ;
			Buffer->X3DAudioEmitterData.pReverbCurve    = ( D_X3DAUDIO_DISTANCE_CURVE * )&D_Emitter_Reverb_Curve ;
			Buffer->X3DAudioEmitterData.CurveDistanceScaler = 14.0f ;
			Buffer->X3DAudioEmitterData.DopplerScaler = 1.0f ;

			Buffer->EmitterInfo.Position = VGet( 0.0f, 0.0f, 0.0f ) ;
			Buffer->EmitterInfo.FrontDirection = VGet( 0.0f, 0.0f, 1.0f ) ;
			Buffer->EmitterInfo.UpDirection = VGet( 0.0f, 1.0f, 0.0f ) ;
			Buffer->EmitterInfo.Velocity = VGet( 0.0f, 0.0f, 0.0f ) ;
			Buffer->EmitterInfo.InnerAngle = Buffer->X3DAudioEmitterConeData.InnerAngle ;
			Buffer->EmitterInfo.OuterAngle = Buffer->X3DAudioEmitterConeData.OuterAngle ;
			Buffer->EmitterInfo.InnerVolume = Buffer->X3DAudioEmitterConeData.InnerVolume ;
			Buffer->EmitterInfo.OuterVolume = Buffer->X3DAudioEmitterConeData.OuterVolume ;
			Buffer->EmitterRadius = Buffer->X3DAudioEmitterData.CurveDistanceScaler ;

			// �`�����l������ۑ�����
			Buffer->X3DAudioEmitterData.ChannelCount = Buffer->Format.nChannels/*D_X3DAUDIO_INPUTCHANNELS*/ ;
			for( i = 0 ; i < Buffer->Format.nChannels ; i ++ )
			{
				Buffer->X3DAudioEmitterChannelAzimuths[ i ] = 0.0f ;
			}
		}
	}
	
	Buffer->Valid = TRUE ;
	
	return D_DS_OK ;

ERR :
	if( DSOUND.EnableXAudioFlag )
	{
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			if( Buffer->XA2_8SourceVoice != NULL )
			{
				Buffer->XA2_8SourceVoice->DestroyVoice() ;
				Buffer->XA2_8SourceVoice = NULL ;
			}

			if( Buffer->XA2_8SubmixVoice != NULL )
			{
				Buffer->XA2_8SubmixVoice->DestroyVoice() ;
				Buffer->XA2_8SubmixVoice = NULL ;
			}
		}
		else
		{
			if( Buffer->XA2SourceVoice != NULL )
			{
				Buffer->XA2SourceVoice->DestroyVoice() ;
				Buffer->XA2SourceVoice = NULL ;
			}

			if( Buffer->XA2SubmixVoice != NULL )
			{
				Buffer->XA2SubmixVoice->DestroyVoice() ;
				Buffer->XA2SubmixVoice = NULL ;
			}
		}

		if( Buffer->XA2Callback != NULL )
		{
			delete Buffer->XA2Callback ;
			Buffer->XA2Callback = NULL ;
		}
	}
	else
	{
		if( Buffer->DSBuffer != NULL )
		{
			Buffer->DSBuffer->Release() ;
			Buffer->DSBuffer = NULL ;
		}
	}

	if( Buffer->Wave != NULL )
	{
		ReleaseWaveData( Buffer->Wave ) ;
		Buffer->Wave = NULL ;
	}

	return -1 ;
}

static HRESULT SoundBuffer_Duplicate( SOUNDBUFFER *Buffer, SOUNDBUFFER *Src, int Is3DSound )
{
	return SoundBuffer_Initialize( Buffer, NULL, Src, Is3DSound ) ;
}


static HRESULT SoundBuffer_Terminate(          SOUNDBUFFER *Buffer )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( DSOUND.EnableXAudioFlag )
	{
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			if( Buffer->XA2_8SourceVoice != NULL )
			{
				Buffer->XA2_8SourceVoice->DestroyVoice() ;
				Buffer->XA2_8SourceVoice = NULL ;
			}

			if( Buffer->XA2_8SubmixVoice != NULL )
			{
				Buffer->XA2_8SubmixVoice->DestroyVoice() ;
				Buffer->XA2_8SubmixVoice = NULL ;
			}
		}
		else
		{
			if( Buffer->XA2SourceVoice != NULL )
			{
				Buffer->XA2SourceVoice->DestroyVoice() ;
				Buffer->XA2SourceVoice = NULL ;
			}

			if( Buffer->XA2SubmixVoice != NULL )
			{
				Buffer->XA2SubmixVoice->DestroyVoice() ;
				Buffer->XA2SubmixVoice = NULL ;
			}
		}

		if( Buffer->XA2Callback != NULL )
		{
			delete Buffer->XA2Callback ;
			Buffer->XA2Callback = NULL ;
		}
	}
	else
	{
		if( Buffer->DSBuffer != NULL )
		{
			Buffer->DSBuffer->Release() ;
			Buffer->DSBuffer = NULL ;
		}
	}

	if( Buffer->Wave != NULL )
	{
		ReleaseWaveData( Buffer->Wave ) ;
		Buffer->Wave = NULL ;
	}
	
	Buffer->Valid = FALSE ;
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_Play(               SOUNDBUFFER *Buffer, int Loop )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	Buffer->Loop = Loop ;
	if( DSOUND.EnableSoundCaptureFlag )
	{
		Buffer->State = TRUE ;
	}
	else
	if( DSOUND.EnableXAudioFlag )
	{
		Buffer->State = TRUE ;
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			Buffer->XA2_8SourceVoice->Start( 0 ) ;
			if( Buffer->XA2_8SubmixVoice )
			{
				Buffer->XA2_8SubmixVoice->EnableEffect( 0 ) ;
			}
		}
		else
		{
			Buffer->XA2SourceVoice->Start( 0 ) ;
			if( Buffer->XA2SubmixVoice )
			{
				Buffer->XA2SubmixVoice->EnableEffect( 0 ) ;
			}
		}
		Buffer->StopTimeState = 1 ;
		Buffer->StopTime = 0 ;
	}
	else
	{
		return Buffer->DSBuffer->Play( 0, 0, ( DWORD )( Loop ? D_DSBPLAY_LOOPING : 0 ) ) ;
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_Stop(               SOUNDBUFFER *Buffer, int EffectStop )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( DSOUND.EnableSoundCaptureFlag )
	{
		Buffer->State = FALSE ;
	}
	else
	if( DSOUND.EnableXAudioFlag )
	{
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			Buffer->XA2_8SourceVoice->Stop( 0 ) ;
			if( Buffer->XA2_8SubmixVoice && EffectStop )
			{
				Buffer->XA2_8SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		else
		{
			Buffer->XA2SourceVoice->Stop( 0 ) ;
			if( Buffer->XA2SubmixVoice && EffectStop )
			{
				Buffer->XA2SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		Buffer->State = FALSE ;
	}
	else
	{
		Buffer->DSBuffer->Stop() ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_GetStatus(          SOUNDBUFFER *Buffer, DWORD *Status )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( DSOUND.EnableSoundCaptureFlag ||
		DSOUND.EnableXAudioFlag )
	{
		if( Status )
		{
			*Status = ( DWORD )( Buffer->State ? D_DSBSTATUS_PLAYING : 0 ) ; 
		}
	}
	else
	{
		if( Buffer->DSBuffer->GetStatus( Status ) != D_DS_OK ) return -1 ;
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_Lock(               SOUNDBUFFER *Buffer, DWORD WritePos , DWORD WriteSize, void **LockPos1, DWORD *LockSize1, void **LockPos2, DWORD *LockSize2 )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( DSOUND.EnableSoundCaptureFlag ||
		DSOUND.EnableXAudioFlag )
	{
		DWORD pos, sample, sample1 ;

		pos    = WritePos  / Buffer->Format.nBlockAlign ;
		sample = WriteSize / Buffer->Format.nBlockAlign ;
		if( pos + sample > ( DWORD )Buffer->SampleNum )
		{
			sample1 = Buffer->SampleNum - pos ;
			*LockPos1  = (BYTE *)Buffer->Wave->Buffer + WritePos ;
			*LockSize1 = sample1 * Buffer->Format.nBlockAlign ;
			*LockPos2  = (BYTE *)Buffer->Wave->Buffer ;
			*LockSize2 = ( sample - sample1 ) * Buffer->Format.nBlockAlign ;
		}
		else
		{
			*LockPos1  = (BYTE *)Buffer->Wave->Buffer + WritePos ;
			*LockSize1 = WriteSize ;
			*LockPos2  = NULL ;
			*LockSize2 = 0 ;
		}
	}
	else
	{
		if( Buffer->DSBuffer->Lock( WritePos, WriteSize, LockPos1, LockSize1, LockPos2, LockSize2, 0 ) != D_DS_OK ) return -1 ;
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_Unlock(             SOUNDBUFFER *Buffer, void *LockPos1, DWORD LockSize1, void *LockPos2, DWORD LockSize2 )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( DSOUND.EnableSoundCaptureFlag )
	{
		// ���ɉ������Ȃ�
	}
	if( DSOUND.EnableXAudioFlag )
	{
		DWORD i ;

		// �_�u���T�C�Y�̏ꍇ�̓X�e���I�f�[�^�ɂ���
		if( Buffer->Wave->DoubleSizeBuffer != NULL )
		{
			switch( Buffer->Format.wBitsPerSample )
			{
			case 8 :
				{
					BYTE *Src8bit ;
					WORD *Dest8bit ;
					DWORD SampleNum ;

					Src8bit = ( BYTE * )LockPos1 ;
					Dest8bit = ( WORD * )Buffer->Wave->DoubleSizeBuffer + ( ( BYTE * )LockPos1 - ( BYTE * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize1 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest8bit[ i ] = ( WORD )( Src8bit[ i ] + ( Src8bit[ i ] << 8 ) ) ;
					}

					Src8bit = ( BYTE * )LockPos2 ;
					Dest8bit = ( WORD * )Buffer->Wave->DoubleSizeBuffer + ( ( BYTE * )LockPos2 - ( BYTE * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize2 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest8bit[ i ] = ( WORD )( Src8bit[ i ] + ( Src8bit[ i ] << 8 ) ) ;
					}
				}
				break ;

			case 16 :
				{
					WORD *Src16bit ;
					DWORD *Dest16bit ;
					DWORD SampleNum ;

					Src16bit = ( WORD * )LockPos1 ;
					Dest16bit = ( DWORD * )Buffer->Wave->DoubleSizeBuffer + ( ( WORD * )LockPos1 - ( WORD * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize1 / 2 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest16bit[ i ] = ( DWORD )( Src16bit[ i ] + ( Src16bit[ i ] << 16 ) ) ;
					}

					Src16bit = ( WORD * )LockPos2 ;
					Dest16bit = ( DWORD * )Buffer->Wave->DoubleSizeBuffer + ( ( WORD * )LockPos2 - ( WORD * )Buffer->Wave->Buffer ) ;
					SampleNum = LockSize2 / 2 ;
					for( i = 0 ; i < SampleNum ; i ++ )
					{
						Dest16bit[ i ] = ( DWORD )( Src16bit[ i ] + ( Src16bit[ i ] << 16 ) ) ;
					}
				}
				break ;
			}
		}
	}
	else
	{
		if( Buffer->DSBuffer->Unlock( LockPos1, LockSize1, LockPos2, LockSize2 ) != D_DS_OK ) return -1 ;
	}
	
	return D_DS_OK ;
}


static HRESULT SoundBuffer_SetFrequency(       SOUNDBUFFER *Buffer, DWORD Frequency )
{
	float SetFreq ;

	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( Frequency == D_DSBFREQUENCY_ORIGINAL ) Buffer->Frequency = -1 ;
	else                                       Buffer->Frequency = ( int )Frequency ;

	if( DSOUND.EnableSoundCaptureFlag )
	{
	}
	else
	if( DSOUND.EnableXAudioFlag )
	{
		if( Frequency == D_DSBFREQUENCY_ORIGINAL )
		{
			SetFreq = 1.0f ;
		}
		else
		{
			SetFreq = ( float )Frequency / Buffer->Format.nSamplesPerSec ;
		}

		if( DSOUND.XAudio2_8DLL != NULL )
		{
			Buffer->XA2_8SourceVoice->SetFrequencyRatio( SetFreq ) ;
		}
		else
		{
			Buffer->XA2SourceVoice->SetFrequencyRatio( SetFreq ) ;
		}
	}
	else
	{
		Buffer->DSBuffer->SetFrequency( Frequency ) ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_GetFrequency(             SOUNDBUFFER *Buffer, LPDWORD Frequency )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( DSOUND.EnableSoundCaptureFlag ||
		DSOUND.EnableXAudioFlag )
	{
		if( Buffer->Frequency < 0 )
		{
			*Frequency = Buffer->Format.nSamplesPerSec ;
		}
		else
		{
			*Frequency = ( DWORD )Buffer->Frequency ;
		}
	}
	else
	{
		return Buffer->DSBuffer->GetFrequency( Frequency ) ;
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_SetPan(             SOUNDBUFFER *Buffer, LONG Pan )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Pan < -10000 )
	{
		Pan = -10000 ;
	}
	else
	if( Pan > 10000 ) 
	{
		Pan = 10000 ;
	}
	Buffer->Pan = Pan ;

	return SoundBuffer_RefreshVolume( Buffer ) ;
}

static HRESULT SoundBuffer_GetPan(             SOUNDBUFFER *Buffer, LPLONG Pan )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Pan != NULL )
	{
		*Pan = Buffer->Pan ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_RefreshVolume( SOUNDBUFFER *Buffer )
{
	int i ;
	int ChannelNum ;

	ChannelNum = Buffer->Format.nChannels > SOUNDBUFFER_MAX_CHANNEL_NUM ? SOUNDBUFFER_MAX_CHANNEL_NUM : Buffer->Format.nChannels ;

	if( Buffer->Valid == FALSE ) return -1 ;

	if( DSOUND.EnableSoundCaptureFlag )
	{
		if( Buffer->Volume[ 0 ] <= -10000 )
		{
			Buffer->CalcVolume = 0 ;
		}
		else
		if( Buffer->Volume[ 0 ] >= 0 )
		{
			Buffer->CalcVolume = 256 ;
		}
		else
		{
			const double Min = 0.0000000001 ;
			const double Max = 1.0 ;
			
			Buffer->CalcVolume = _DTOL( ( _POW( (float)10, Buffer->Volume[ 0 ] / 10.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
		}

		if( Buffer->Pan == -10000 )
		{
			Buffer->CalcPan = -256 ;
		}
		else
		if( Buffer->Pan == 10000 ) 
		{
			Buffer->CalcPan = 256 ;
		}
		else
		if( Buffer->Pan == 0 )
		{
			Buffer->CalcPan = 0 ;
		}
		else
		{
			const double Min = 0.0000000001 ;
			const double Max = 1.0 ;
			int temp ;
			
			temp = _DTOL( ( _POW( (float)10, -( _ABS( Buffer->Pan ) ) / 10.0f / 100.0f ) / ( Max - Min ) ) * 256 ) ;
			Buffer->CalcPan = Buffer->Pan < 0 ? -temp : temp ;
		}
	}
	else
	if( DSOUND.EnableXAudioFlag )
	{
		float ChannelVolume[ 16 ] ;

		for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
		{
			ChannelVolume[ i ] = D_XAudio2DecibelsToAmplitudeRatio( Buffer->Volume[ i ] / 100.0f ) ;
		}

		if( ChannelNum <= 2 )
		{
			if( Buffer->Pan < 0 )
			{
				ChannelVolume[ 1 ] *= D_XAudio2DecibelsToAmplitudeRatio( Buffer->Pan / 100.0f ) ;
			}
			else
			if( Buffer->Pan > 0 )
			{
				ChannelVolume[ 0 ] *= D_XAudio2DecibelsToAmplitudeRatio( -Buffer->Pan / 100.0f ) ;
			}
		}

		if( DSOUND.XAudio2_8DLL != NULL )
		{
			Buffer->XA2_8SourceVoice->SetChannelVolumes( ( DWORD )Buffer->XAudioChannels, ChannelVolume ) ;
		}
		else
		{
			Buffer->XA2SourceVoice->SetChannelVolumes( ( DWORD )Buffer->XAudioChannels, ChannelVolume ) ;
		}
	}
	else
	{
		if( ChannelNum > 2 )
		{
			LONG Volume ;

			Volume = ( LONG )Buffer->Volume[ 0 ] ;
			if( Volume > 0 )
			{
				Volume = 0 ;
			}
			else
			if( Volume < -10000 )
			{
				Volume = -10000 ;
			}
			Buffer->DSBuffer->SetVolume( Volume ) ;
			Buffer->DSBuffer->SetPan( Buffer->Pan ) ;
		}
		else
		{
			LONG CalcVolume[ 2 ] ;
			LONG TempVolume[ 2 ] ;
			FLOAT OrigVolume[ 2 ] ;
			LONG Volume ;
			LONG Pan ;

			if( Buffer->Is3DSound )
			{
				OrigVolume[ 0 ] = D_XAudio2DecibelsToAmplitudeRatio( Buffer->Volume[ 0 ] / 100.0f ) ;
				OrigVolume[ 1 ] = D_XAudio2DecibelsToAmplitudeRatio( Buffer->Volume[ 1 ] / 100.0f ) ;

				if( Buffer->DSound_Calc3DPan < 0.0f )
				{
					OrigVolume[ 0 ] *= 1.0f + Buffer->DSound_Calc3DPan ;
				}
				else
				if( Buffer->DSound_Calc3DPan > 0.0f )
				{
					OrigVolume[ 1 ] *= 1.0f - Buffer->DSound_Calc3DPan ;
				}

				TempVolume[ 0 ] = ( LONG )_DTOL( D_XAudio2AmplitudeRatioToDecibels( OrigVolume[ 0 ] * Buffer->DSound_Calc3DVolume ) * 100.0f ) ;
				TempVolume[ 1 ] = ( LONG )_DTOL( D_XAudio2AmplitudeRatioToDecibels( OrigVolume[ 1 ] * Buffer->DSound_Calc3DVolume ) * 100.0f ) ;
			}
			else
			{
				TempVolume[ 0 ] = Buffer->Volume[ 0 ] ;
				TempVolume[ 1 ] = Buffer->Volume[ 1 ] ;
			}

			if( Buffer->Pan < 0 )
			{
				CalcVolume[ 0 ] = 10000 ;
				CalcVolume[ 1 ] = 10000 + Buffer->Pan ;
			}
			else
			{
				CalcVolume[ 0 ] = 10000 - Buffer->Pan ;
				CalcVolume[ 1 ] = 10000 ;
			}

			if( TempVolume[ 0 ] > 0 )
			{
				TempVolume[ 0 ] = 0 ;
			}
			else
			if( TempVolume[ 0 ] < -10000 )
			{
				TempVolume[ 0 ] = -10000 ;
			}
			if( TempVolume[ 1 ] > 0 )
			{
				TempVolume[ 1 ] = 0 ;
			}
			else
			if( TempVolume[ 1 ] < -10000 )
			{
				TempVolume[ 1 ] = -10000 ;
			}

			CalcVolume[ 0 ] = CalcVolume[ 0 ] * ( TempVolume[ 0 ] + 10000 ) / 10000 ;
			CalcVolume[ 1 ] = CalcVolume[ 1 ] * ( TempVolume[ 1 ] + 10000 ) / 10000 ;

			if( CalcVolume[ 0 ] > CalcVolume[ 1 ] )
			{
				Volume = CalcVolume[ 0 ] - 10000 ;
				Pan =    _FTOL( CalcVolume[ 1 ] * ( 10000.0f / CalcVolume[ 0 ] ) ) - 10000 ;
			}
			else
			if( CalcVolume[ 0 ] < CalcVolume[ 1 ] )
			{
				Volume = CalcVolume[ 1 ] - 10000 ;
				Pan = -( _FTOL( CalcVolume[ 0 ] * ( 10000.0f / CalcVolume[ 1 ] ) ) - 10000 ) ;
			}
			else
			{
				Volume = CalcVolume[ 0 ] - 10000 ;
				Pan = 0 ;
			}

			Buffer->DSBuffer->SetPan( ( LONG )Pan ) ;
			Buffer->DSBuffer->SetVolume( ( LONG )Volume ) ;
		}
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_SetVolumeAll( SOUNDBUFFER *Buffer, LONG Volume )
{
	int i ;

	if( Buffer->Valid == FALSE ) return -1 ;

//	if( Volume > 0 )
//	{
//		Volume = 0 ;
//	}
//	else
//	if( Volume < -10000 )
//	{
//		Volume = -10000 ;
//	}

	for( i = 0 ; i < SOUNDBUFFER_MAX_CHANNEL_NUM ; i ++ )
	{
		Buffer->Volume[ i ] = Volume ;
	}

	return SoundBuffer_RefreshVolume( Buffer ) ;
}

static HRESULT SoundBuffer_SetVolume( SOUNDBUFFER *Buffer, int Channel, LONG Volume )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Channel >= SOUNDBUFFER_MAX_CHANNEL_NUM ) return -1 ;

//	if( Volume > 0 )
//	{
//		Volume = 0 ;
//	}
//	else
//	if( Volume < -10000 )
//	{
//		Volume = -10000 ;
//	}

	Buffer->Volume[ Channel ] = Volume ;

	return SoundBuffer_RefreshVolume( Buffer ) ;
}

static HRESULT SoundBuffer_GetVolume( SOUNDBUFFER *Buffer, int Channel, LPLONG Volume )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( Channel >= SOUNDBUFFER_MAX_CHANNEL_NUM ) return -1 ;

	if( Volume )
	{
		*Volume = Buffer->Volume[ Channel ] ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_GetCurrentPosition( SOUNDBUFFER *Buffer, LPDWORD PlayPos, LPDWORD WritePos )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( DSOUND.EnableSoundCaptureFlag ||
		DSOUND.EnableXAudioFlag )
	{
		if( PlayPos  ) *PlayPos  = (DWORD)Buffer->Pos * Buffer->Format.nBlockAlign ;
		if( WritePos ) *WritePos = (DWORD)Buffer->Pos * Buffer->Format.nBlockAlign ;
	}
	else
	{
		Buffer->DSBuffer->GetCurrentPosition( PlayPos, WritePos ) ;
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_SetCurrentPosition( SOUNDBUFFER *Buffer, DWORD NewPos )
{
	if( Buffer->Valid == FALSE ) return -1 ;
	
	if( DSOUND.EnableSoundCaptureFlag ||
		DSOUND.EnableXAudioFlag )
	{
		Buffer->Pos     = ( int )( NewPos / Buffer->Format.nBlockAlign ) ;
		Buffer->CompPos = Buffer->Pos ;

		if( DSOUND.EnableXAudioFlag )
		{
			if( DSOUND.XAudio2_8DLL != NULL )
			{
				Buffer->XA2_8SourceVoice->FlushSourceBuffers() ;
			}
			else
			{
				Buffer->XA2SourceVoice->FlushSourceBuffers() ;
			}
		}
	}
	else
	{
		Buffer->DSBuffer->SetCurrentPosition( NewPos ) ;
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_CycleProcess( SOUNDBUFFER *Buffer )
{
	int NowCount ;
	int Time ;

	if( DSOUND.EnableSoundCaptureFlag )
	{
		return -1 ;
	}
	else
	if( DSOUND.EnableXAudioFlag )
	{
		if( Buffer->XA2SubmixVoice == NULL )
		{
			return -1 ;
		}

		if( Buffer->StopTimeState == 0 )
		{
			return -1 ;
		}

		if( Buffer->State == TRUE )
		{
			return D_DS_OK ;
		}

		if( Buffer->StopTimeState == 1 )
		{
			Buffer->StopTimeState = 2 ;
			Buffer->StopTime = NS_GetNowCount() ;

			return D_DS_OK ;
		}

		NowCount = NS_GetNowCount() ;

		// �Đ�����~���Ă���R�b��ɃG�t�F�N�g����~����
		if( NowCount < Buffer->StopTime )
		{
			Time = 0x7ffffff - Buffer->StopTime + NowCount ;
		}
		else
		{
			Time = NowCount - Buffer->StopTime ;
		}
		if( Time < 3000 )
		{
			return D_DS_OK ;
		}

		Buffer->StopTimeState = 0 ;

		if( DSOUND.XAudio2_8DLL != NULL )
		{
			if( Buffer->XA2_8SubmixVoice )
			{
				Buffer->XA2_8SubmixVoice->DisableEffect( 0 ) ;
			}
		}
		else
		{
			if( Buffer->XA2SubmixVoice )
			{
				Buffer->XA2SubmixVoice->DisableEffect( 0 ) ;
			}
		}

		return -1 ;
	}
	else
	{
		return -1 ;
	}
}


#define DESTADD	\
{									\
	if( vol != 255 )				\
	{								\
		d1 = ( d1 * vol ) >> 8 ;	\
		d2 = ( d2 * vol ) >> 8 ;	\
	}								\
	if( pan != 0 )					\
	{								\
		if( pan > 0 ) d2 = ( d2 * ( 256 -   pan  ) ) >> 8 ;		\
		else          d1 = ( d1 * ( 256 - (-pan) ) ) >> 8 ;		\
	}								\
									\
		 if( DestBuf[0] + d1 >  32767 ) DestBuf[0] =  32767 ;				\
	else if( DestBuf[0] + d1 < -32768 ) DestBuf[0] = -32768 ;				\
	else                                DestBuf[0] += ( short )d1 ; 		\
									\
		 if( DestBuf[1] + d2 >  32767 ) DestBuf[1] =  32767 ;				\
	else if( DestBuf[1] + d2 < -32768 ) DestBuf[1] = -32768 ;				\
	else                                DestBuf[1] += ( short )d2 ; 		\
}

#define CNV( S )		((int)((S) * 65535 / 255) - 32768)

static HRESULT SoundBuffer_FrameProcess(       SOUNDBUFFER *Buffer, int Sample, short *DestBuf )
{
	if( Buffer->Valid == FALSE ) return -1 ;

	if( DSOUND.EnableSoundCaptureFlag )
	{
		int i, pos, d1, d2, b, ch, rate, bit, vol, pan ;
		short *s ;
		BYTE *sb ;
		
		ch   = Buffer->Format.nChannels ;
		rate = ( int )Buffer->Format.nSamplesPerSec ;
		bit  = Buffer->Format.wBitsPerSample ;
		vol  = Buffer->Volume[ 0 ] ;
		pan  = Buffer->Pan ;
		s    = (short *)( (BYTE *)Buffer->Wave->Buffer + Buffer->Pos * Buffer->Format.nBlockAlign ) ;
		sb   = (BYTE *)s ;

		if( DestBuf != NULL &&
			( rate == 44100 || rate == 22050 ) &&
			( bit  == 16    || bit  == 8     ) &&
			( ch   == 2     || ch   == 1     ) )
		{
			if( rate == 44100 )
			{
				pos = Buffer->Pos ;
				for( i = 0 ; i < Sample ; i ++, DestBuf += 2 )
				{
					if( bit == 16 )
					{
						if( ch == 2 ){      d1 = s[0]; d2 = s[1]; s += 2 ; }
						else         { d2 = d1 = s[0];            s += 1 ; }
					}
					else
					{
						if( ch == 2 ){      d1 = CNV(sb[0]); d2 = CNV(sb[1]); sb += 2 ; }
						else         { d2 = d1 = CNV(sb[0]);                  sb += 1 ; }
					}
					
					DESTADD
					pos ++ ;
					if( pos >= Buffer->SampleNum )
					{
						if( Buffer->Loop == FALSE ) break ;
						
						s = (short *)Buffer->Wave->Buffer ;
						sb = (BYTE *)s ;
						pos = 0 ;
					}
				}
			}
			else
			if( rate == 22050 )
			{
				pos = Buffer->Pos ;
				b = Buffer->Pos & 1;
				for( i = 0 ; i < Sample ; i ++, DestBuf += 2 )
				{
					if( b )
					{
						if( pos + 1 >= Buffer->SampleNum )
						{
							if( Buffer->Loop == FALSE )
							{
								if( bit == 16 )
								{
									if( ch == 2 )
									{
										d1=s[0]+(s[0]-s[-2])/2;
										d2=s[1]+(s[1]-s[-3])/2;
									}
									else
									{
										d2 = d1=s[0]+(s[0]-s[-1])/2;
									}
								}
								else
								{
									if( ch == 2 )
									{
										d1=CNV(sb[0])+(CNV(sb[0])-CNV(sb[-2]))/2;
										d2=CNV(sb[1])+(CNV(sb[1])-CNV(sb[-3]))/2;
									}
									else
									{
										d2 = d1=CNV(sb[0])+(CNV(sb[0])-CNV(sb[-1]))/2;
									}
								}
								break;
							}
							else
							{
								if( bit == 16 )
								{
									if( ch == 2 )
									{
										d1 = s[0] ;
										d2 = s[1] ;
										s = (short *)Buffer->Wave->Buffer ;
										d1=d1+(s[0]-d1)/2;
										d2=d2+(s[1]-d2)/2;
									}
									else
									{
										d1 = s[0] ;
										s = (short *)Buffer->Wave->Buffer ;
										d2 = d1=d1+(s[0]-d1)/2;
									}
								}
								else
								{
									if( ch == 2 )
									{
										d1 = CNV(sb[0]) ;
										d2 = CNV(sb[1]) ;
										sb = (BYTE *)Buffer->Wave->Buffer ;
										d1=d1+(CNV(sb[0])-d1)/2;
										d2=d2+(CNV(sb[1])-d2)/2;
									}
									else
									{
										d1 = CNV(sb[0]) ;
										sb = (BYTE *)Buffer->Wave->Buffer ;
										d2 = d1=d1+(CNV(sb[0])-d1)/2;
									}
								}

								pos = 0 ;
								b = 0;
							}
						}
						else
						{
							if( bit == 16 )
							{
								if( ch == 2 )
								{
									d1=s[0]+(s[2]-s[0])/2;
									d2=s[1]+(s[3]-s[1])/2;
									s += 2 ;
								}
								else
								{
									d2 = d1=s[0]+(s[1]-s[0])/2;
									s += 1 ;
								}
							}
							else
							{
								if( ch == 2 )
								{
									d1=CNV(sb[0]);d1+=(CNV(sb[2])-d1)/2;
									d2=CNV(sb[1]);d2+=(CNV(sb[3])-d2)/2;
									sb += 2 ;
								}
								else
								{
									d1=CNV(sb[0]);d1+=(CNV(sb[1])-d1)/2;
									d2 = d1;
									sb += 1 ;
								}
							}
							pos ++ ;
							b = b ? 0 : 1;
						}
						DESTADD
					}
					else
					{
						b = b ? 0 : 1;
						if( bit == 16 )
						{
							if( ch == 2 ){      d1 = s[0]; d2 = s[1]; }
							else         { d2 = d1 = s[0];            }
						}
						else
						{
							if( ch == 2 ){      d1 = CNV(sb[0]); d2 = CNV(sb[1]); }
							else         { d2 = d1 = CNV(sb[0]);	              }
						}
						DESTADD
					}
				}
				Sample >>= 1;
			}
		}

		if( Buffer->Pos + Sample >= Buffer->SampleNum )
		{
			if( Buffer->Loop == TRUE )
			{
				Buffer->Pos = ( Buffer->Pos + Sample ) - Buffer->SampleNum ;
			}
			else
			{
				Buffer->Pos = Buffer->SampleNum ;
				Buffer->State = FALSE ;
			}
		}
		else
		{
			Buffer->Pos += Sample ;
		}
		Buffer->CompPos = Buffer->Pos ;
	}
	else
	{
	}
	
	return D_DS_OK ;
}

static HRESULT SoundBuffer_Set3DPosition( SOUNDBUFFER *Buffer, VECTOR *Position )
{
	DWORD State ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// �l���قڕω����Ȃ��ꍇ�͉������Ȃ�
	if( _FABS( Buffer->X3DAudioEmitterData.Position.x - Position->x ) < 0.001f &&
		_FABS( Buffer->X3DAudioEmitterData.Position.y - Position->y ) < 0.001f &&
		_FABS( Buffer->X3DAudioEmitterData.Position.z - Position->z ) < 0.001f )
	{
		return D_DS_OK ;
	}

	// �l��ۑ�
	Buffer->X3DAudioEmitterData.Position.x = Position->x ;
	Buffer->X3DAudioEmitterData.Position.y = Position->y ;
	Buffer->X3DAudioEmitterData.Position.z = Position->z ;

	Buffer->EmitterInfo.Position = *Position ;

	// �f�[�^���ύX���ꂽ�t���O�𗧂Ă�
	Buffer->EmitterDataChangeFlag = TRUE ;

	// �Đ����������ꍇ�̓p�����[�^���X�V
	SoundBuffer_GetStatus( Buffer, &State ) ;
	if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_Set3DRadius( SOUNDBUFFER *Buffer, float Radius )
{
	DWORD State ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// �l���قڕω����Ȃ��ꍇ�͉������Ȃ�
	if( _FABS( Buffer->X3DAudioEmitterData.CurveDistanceScaler - Radius ) < 0.001f )
	{
		return D_DS_OK ;
	}

	// �������鋗����ۑ�
	Buffer->X3DAudioEmitterData.CurveDistanceScaler = Radius ;

	Buffer->EmitterRadius = Radius ;

	// �f�[�^���ύX���ꂽ�t���O�𗧂Ă�
	Buffer->EmitterDataChangeFlag = TRUE ;

	// �Đ����������ꍇ�̓p�����[�^���X�V
	SoundBuffer_GetStatus( Buffer, &State ) ;
	if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_Set3DInnerRadius( SOUNDBUFFER *Buffer, float Radius )
{
	DWORD State ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// �l���قڕω����Ȃ��ꍇ�͉������Ȃ�
	if( _FABS( Buffer->X3DAudioEmitterData.InnerRadius - Radius ) < 0.001f )
	{
		return D_DS_OK ;
	}

	Buffer->X3DAudioEmitterData.InnerRadius = Radius ;

	// �f�[�^���ύX���ꂽ�t���O�𗧂Ă�
	Buffer->EmitterDataChangeFlag = TRUE ;

	// �Đ����������ꍇ�̓p�����[�^���X�V
	SoundBuffer_GetStatus( Buffer, &State ) ;
	if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_Set3DVelocity( SOUNDBUFFER *Buffer, VECTOR *Velocity )
{
	DWORD State ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// �l���قƂ�Ǖω����Ȃ��ꍇ�ꍇ�͉������Ȃ�
	if( _FABS( Buffer->X3DAudioEmitterData.Velocity.x - Velocity->x ) < 0.001f &&
		_FABS( Buffer->X3DAudioEmitterData.Velocity.y - Velocity->y ) < 0.001f &&
		_FABS( Buffer->X3DAudioEmitterData.Velocity.z - Velocity->z ) < 0.001f )
	{
		return D_DS_OK ;
	}

	// �l��ۑ�
	Buffer->X3DAudioEmitterData.Velocity.x = Velocity->x ;
	Buffer->X3DAudioEmitterData.Velocity.y = Velocity->y ;
	Buffer->X3DAudioEmitterData.Velocity.z = Velocity->z ;

	Buffer->EmitterInfo.Velocity = *Velocity ;

	// �f�[�^���ύX���ꂽ�t���O�𗧂Ă�
	Buffer->EmitterDataChangeFlag = TRUE ;

	// �Đ����������ꍇ�̓p�����[�^���X�V
	SoundBuffer_GetStatus( Buffer, &State ) ;
	if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_Set3DFrontPosition( SOUNDBUFFER *Buffer, VECTOR *FrontPosition, VECTOR *UpVector )
{
	DWORD State ;
	VECTOR SideVec ;
	VECTOR DirVec ;
	VECTOR Position ;
	VECTOR UpVectorT ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	Position.x = Buffer->X3DAudioEmitterData.Position.x * DSOUND._3DSoundOneMetre ;
	Position.y = Buffer->X3DAudioEmitterData.Position.y * DSOUND._3DSoundOneMetre ;
	Position.z = Buffer->X3DAudioEmitterData.Position.z * DSOUND._3DSoundOneMetre ;

	VectorSub( &DirVec, FrontPosition, &Position ) ;
	VectorNormalize( &DirVec, &DirVec ) ;

	VectorOuterProduct( &SideVec, &DirVec,  UpVector ) ;
	VectorOuterProduct( &UpVectorT, &SideVec, &DirVec ) ;
	VectorNormalize( &UpVectorT, &UpVectorT ) ;

	// �l���قƂ�Ǖω����Ȃ��ꍇ�ꍇ�͉������Ȃ�
	if( _FABS( Buffer->X3DAudioEmitterData.OrientFront.x - DirVec.x ) < 0.0001f &&
		_FABS( Buffer->X3DAudioEmitterData.OrientFront.y - DirVec.y ) < 0.0001f &&
		_FABS( Buffer->X3DAudioEmitterData.OrientFront.z - DirVec.z ) < 0.0001f &&
		_FABS( Buffer->X3DAudioEmitterData.OrientTop.x - UpVectorT.x ) < 0.0001f &&
		_FABS( Buffer->X3DAudioEmitterData.OrientTop.y - UpVectorT.y ) < 0.0001f &&
		_FABS( Buffer->X3DAudioEmitterData.OrientTop.z - UpVectorT.z ) < 0.0001f )
	{
		return D_DS_OK ;
	}

	// �l��ۑ�
	Buffer->X3DAudioEmitterData.OrientFront.x = DirVec.x ;
	Buffer->X3DAudioEmitterData.OrientFront.y = DirVec.y ;
	Buffer->X3DAudioEmitterData.OrientFront.z = DirVec.z ;

	Buffer->X3DAudioEmitterData.OrientTop.x = UpVectorT.x ;
	Buffer->X3DAudioEmitterData.OrientTop.y = UpVectorT.y ;
	Buffer->X3DAudioEmitterData.OrientTop.z = UpVectorT.z ;

	Buffer->EmitterInfo.FrontDirection = DirVec ;
	Buffer->EmitterInfo.UpDirection = UpVectorT ;

	// �f�[�^���ύX���ꂽ�t���O�𗧂Ă�
	Buffer->EmitterDataChangeFlag = TRUE ;

	// �Đ����������ꍇ�̓p�����[�^���X�V
	SoundBuffer_GetStatus( Buffer, &State ) ;
	if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_Set3DConeAngle( SOUNDBUFFER *Buffer, float InnerAngle, float OuterAngle )
{
	DWORD State ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// �l���قڕω����Ȃ��ꍇ�͉������Ȃ�
	if( _FABS( Buffer->X3DAudioEmitterConeData.InnerAngle - InnerAngle ) < 0.001f &&
		_FABS( Buffer->X3DAudioEmitterConeData.OuterAngle - OuterAngle ) < 0.001f )
	{
		return D_DS_OK ;
	}

	Buffer->X3DAudioEmitterConeData.InnerAngle = InnerAngle ;
	Buffer->X3DAudioEmitterConeData.OuterAngle = OuterAngle ;

	Buffer->EmitterInfo.InnerAngle = InnerAngle ;
	Buffer->EmitterInfo.OuterAngle = OuterAngle ;

	// �f�[�^���ύX���ꂽ�t���O�𗧂Ă�
	Buffer->EmitterDataChangeFlag = TRUE ;

	// �Đ����������ꍇ�̓p�����[�^���X�V
	SoundBuffer_GetStatus( Buffer, &State ) ;
	if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return D_DS_OK ;
}

static HRESULT SoundBuffer_Set3DConeVolume( SOUNDBUFFER *Buffer, float InnerAngleVolume, float OuterAngleVolume )
{
	DWORD State ;

	// �R�c�T�E���h�ł͂Ȃ��ꍇ�͉������Ȃ�
	if( Buffer->Is3DSound == FALSE )
		return -1 ;

	// �l���قڕω����Ȃ��ꍇ�͉������Ȃ�
	if( _FABS( Buffer->X3DAudioEmitterConeData.InnerVolume - InnerAngleVolume ) < 0.001f &&
		_FABS( Buffer->X3DAudioEmitterConeData.OuterVolume - OuterAngleVolume ) < 0.001f )
	{
		return D_DS_OK ;
	}

	Buffer->X3DAudioEmitterConeData.InnerVolume = InnerAngleVolume ;
	Buffer->X3DAudioEmitterConeData.OuterVolume = OuterAngleVolume ;

	Buffer->EmitterInfo.InnerVolume = InnerAngleVolume ;
	Buffer->EmitterInfo.OuterVolume = OuterAngleVolume ;

	// �f�[�^���ύX���ꂽ�t���O�𗧂Ă�
	Buffer->EmitterDataChangeFlag = TRUE ;

	// �Đ����������ꍇ�̓p�����[�^���X�V
	SoundBuffer_GetStatus( Buffer, &State ) ;
	if( ( State & D_DSBSTATUS_PLAYING ) != 0 )
	{
		SoundBuffer_Refresh3DSoundParam( Buffer ) ;
	}

	return D_DS_OK ;
}


static HRESULT SoundBuffer_Refresh3DSoundParam( SOUNDBUFFER *Buffer, int AlwaysFlag )
{
	DWORD CalcFlags ;
	D_X3DAUDIO_DSP_SETTINGS	DspSettings ;
	float MatrixCoefficients[ D_X3DAUDIO_INPUTCHANNELS * 32 ] ;
	D_XAUDIO2_FILTER_PARAMETERS FilterParametersDirect ;
	D_XAUDIO2_FILTER_PARAMETERS FilterParametersReverb ;
	float Sin, Cos ;
	int i ;
	int Num ;

	if( Buffer->Is3DSound == FALSE || Buffer->Valid == FALSE ) return -1 ;

	// �K�����s����t���O���|��Ă��āA�f�[�^���ύX���ꂽ�t���O���|��Ă����牽�����Ȃ�
	if( AlwaysFlag == FALSE && Buffer->EmitterDataChangeFlag == FALSE )
	{
		return D_DS_OK ;
	}

	if( DSOUND.EnableSoundCaptureFlag )
	{
	}
	else
	// XAudio2 ���g�p���邩�ǂ����ŏ����𕪊�
	if( DSOUND.EnableXAudioFlag )
	{
		// XAudio2 ���g�p����ꍇ
		CalcFlags =
			D_X3DAUDIO_CALCULATE_MATRIX |
			D_X3DAUDIO_CALCULATE_DOPPLER |
			D_X3DAUDIO_CALCULATE_LPF_DIRECT |
			D_X3DAUDIO_CALCULATE_LPF_REVERB |
			D_X3DAUDIO_CALCULATE_REVERB ;
		if( ( DSOUND.XAudio2OutputChannelMask & D_SPEAKER_LOW_FREQUENCY ) != 0 )
		{
			CalcFlags |= D_X3DAUDIO_CALCULATE_REDIRECT_TO_LFE ;
		}

		_MEMSET( MatrixCoefficients, 0, sizeof( MatrixCoefficients ) ) ;
		_MEMSET( &DspSettings, 0, sizeof( DspSettings ) ) ;
	//	DspSettings.SrcChannelCount = D_X3DAUDIO_INPUTCHANNELS ;
		DspSettings.SrcChannelCount = Buffer->Format.nChannels ;
		DspSettings.DstChannelCount = DSOUND.OutputChannels ;
		DspSettings.pMatrixCoefficients = MatrixCoefficients ;

		DSOUND.X3DAudioCalculateFunc(
			DSOUND.X3DAudioInstance,
			&DSOUND.X3DAudioListenerData,
			&Buffer->X3DAudioEmitterData,
			CalcFlags,
			&DspSettings ) ;

		_SINCOS( DX_PI_F / 6.0f * DspSettings.LPFDirectCoefficient, &Sin, &Cos ) ;
		FilterParametersDirect.Type = D_LowPassFilter ;
		FilterParametersDirect.Frequency = 2.0f * Sin ;
		FilterParametersDirect.OneOverQ = 1.0f ;

		_SINCOS( DX_PI_F / 6.0f * DspSettings.LPFReverbCoefficient, &Sin, &Cos ) ;
		FilterParametersReverb.Type = D_LowPassFilter ;
		FilterParametersReverb.Frequency = 2.0f * Sin ;
		FilterParametersReverb.OneOverQ = 1.0f ;
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			Buffer->XA2_8SourceVoice->SetFrequencyRatio( DspSettings.DopplerFactor ) ;

			Buffer->XA2_8SourceVoice->SetOutputMatrix(
				DSOUND.XAudio2_8MasteringVoiceObject,
				Buffer->Format.nChannels,
				DSOUND.OutputChannels,
				MatrixCoefficients
			) ;

			Num = Buffer->Format.nChannels * Buffer->Format.nChannels ;
			for( i = 0 ; i < Num ; i ++ )
			{
				MatrixCoefficients[ i ] = DspSettings.ReverbLevel ;
			}
			Buffer->XA2_8SourceVoice->SetOutputMatrix(
				Buffer->XA2_8SubmixVoice,
				Buffer->Format.nChannels,
				Buffer->Format.nChannels,
				MatrixCoefficients
			) ;

			Buffer->XA2_8SourceVoice->SetOutputFilterParameters( DSOUND.XAudio2_8MasteringVoiceObject, &FilterParametersDirect ) ;
			Buffer->XA2_8SourceVoice->SetOutputFilterParameters( Buffer->XA2_8SubmixVoice,             &FilterParametersReverb ) ;
		}
		else
		{
			Buffer->XA2SourceVoice->SetFrequencyRatio( DspSettings.DopplerFactor ) ;
			Buffer->XA2SourceVoice->SetOutputMatrix(
				DSOUND.XAudio2MasteringVoiceObject,
				Buffer->Format.nChannels,
				DSOUND.OutputChannels,
				MatrixCoefficients
			) ;

			Num = Buffer->Format.nChannels * Buffer->Format.nChannels ;
			for( i = 0 ; i < Num ; i ++ )
			{
				MatrixCoefficients[ i ] = DspSettings.ReverbLevel ;
			}
			Buffer->XA2SourceVoice->SetOutputMatrix(
				Buffer->XA2SubmixVoice,
				Buffer->Format.nChannels,
				Buffer->Format.nChannels,
				MatrixCoefficients
			) ;

			Buffer->XA2SourceVoice->SetOutputFilterParameters( DSOUND.XAudio2MasteringVoiceObject, &FilterParametersDirect ) ;
			Buffer->XA2SourceVoice->SetOutputFilterParameters( Buffer->XA2SubmixVoice,             &FilterParametersReverb ) ;
		}
	}
	else
	{
		float Distance ;
		float Angle ;
		float DistanceVolumeRatio ;
		float AngleVolumeRatio ;
		// LONG Volume ;
		// LONG Pan ;
		float fVolume ;
		float fPan ;
		VECTOR ListenerToEmitterVec ;
		VECTOR PanVec ;

		// �����ł̌��������v�Z
		ListenerToEmitterVec = VSub( Buffer->EmitterInfo.Position, DSOUND.ListenerInfo.Position ) ;
		Distance = VSize( ListenerToEmitterVec ) ;
		if( Distance > Buffer->EmitterRadius )
		{
			DistanceVolumeRatio = 0.0f ;
		}
		else
		{
			DistanceVolumeRatio = 1.0f - Distance / Buffer->EmitterRadius ;
		}
		ListenerToEmitterVec = VScale( ListenerToEmitterVec, 1.0f / Distance ) ;

		// �p�x�ł̌��������v�Z
		Angle = _ACOS( VDot( DSOUND.ListenerInfo.FrontDirection, ListenerToEmitterVec ) ) * 2.0f ;
		if( Angle < DSOUND.ListenerInfo.InnerAngle )
		{
			AngleVolumeRatio = DSOUND.ListenerInfo.InnerVolume ;
		}
		else
		if( Angle > DSOUND.ListenerInfo.OuterAngle )
		{
			AngleVolumeRatio = DSOUND.ListenerInfo.OuterVolume ;
		}
		else
		{
			AngleVolumeRatio = ( Angle - DSOUND.ListenerInfo.InnerAngle ) / ( DSOUND.ListenerInfo.OuterAngle - DSOUND.ListenerInfo.InnerAngle ) ;
			AngleVolumeRatio = ( DSOUND.ListenerInfo.OuterVolume - DSOUND.ListenerInfo.InnerVolume ) * AngleVolumeRatio + DSOUND.ListenerInfo.InnerVolume ;
		}
		// �v�f�̊|�����킹
		fVolume = DistanceVolumeRatio * AngleVolumeRatio ;
//		Volume = ( LONG )_DTOL( _LOG10( DistanceVolumeRatio * AngleVolumeRatio ) * 20.0f * 100.0f ) ;
//		if( Volume > 0 )
//		{
//			Volume = 0 ;
//		}
//		else 
//		if( Volume < D_DSBVOLUME_MIN )
//		{
//			Volume = D_DSBVOLUME_MIN ;
//		}

		// ���E�o�����X���v�Z
		PanVec.x = VDot( ListenerToEmitterVec, DSOUND.ListenerSideDirection ) ;
		PanVec.y = VDot( ListenerToEmitterVec, DSOUND.ListenerInfo.UpDirection ) ;
		PanVec.z = VDot( ListenerToEmitterVec, DSOUND.ListenerInfo.FrontDirection ) ;
		fPan = PanVec.x < 0.0f ? -PanVec.x : PanVec.x ;
		if( fPan > 0.80f ) fPan = 0.80f ;
//		Pan = ( LONG )_DTOL( _LOG10( 1.0f - fPan ) * 20.0f * 100.0f ) ;
//		if( Pan > 0 )
//		{
//			Pan = 0 ;
//		}
//		else 
//		if( Pan < D_DSBVOLUME_MIN )
//		{
//			Pan = D_DSBVOLUME_MIN ;
//		}
//		if( PanVec.x < 0.0f )
//		{
//			Pan = -Pan ;
//		}
		if( PanVec.x < 0.0f )
		{
			fPan = -fPan ;
		}

		Buffer->DSound_Calc3DVolume = fVolume ;
		Buffer->DSound_Calc3DPan = fPan ;

		SoundBuffer_RefreshVolume( Buffer ) ;
	}

	// �f�[�^���ύX���ꂽ�t���O��|��
	Buffer->EmitterDataChangeFlag = FALSE ;

	// �I��
	return D_DS_OK ;
}

static HRESULT SoundBuffer_SetReverbParam( SOUNDBUFFER *Buffer, SOUND3D_REVERB_PARAM *Param )
{
	if( Buffer->Is3DSound == FALSE || Buffer->Valid == FALSE ) return -1 ;

	if( DSOUND.EnableSoundCaptureFlag )
	{
	}
	else
	// XAudio2 ���g�p����ꍇ�̂ݗL��
	if( DSOUND.EnableXAudioFlag )
	{
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			Buffer->XAudio2_8ReverbParameter.WetDryMix           = Param->WetDryMix ;

			Buffer->XAudio2_8ReverbParameter.ReflectionsDelay    = Param->ReflectionsDelay;
			Buffer->XAudio2_8ReverbParameter.ReverbDelay         = Param->ReverbDelay ;
			Buffer->XAudio2_8ReverbParameter.RearDelay           = Param->RearDelay ;

			Buffer->XAudio2_8ReverbParameter.PositionLeft        = Param->PositionLeft ;
			Buffer->XAudio2_8ReverbParameter.PositionRight       = Param->PositionRight ;
			Buffer->XAudio2_8ReverbParameter.PositionMatrixLeft  = Param->PositionMatrixLeft ;
			Buffer->XAudio2_8ReverbParameter.PositionMatrixRight = Param->PositionMatrixRight ;
			Buffer->XAudio2_8ReverbParameter.EarlyDiffusion      = Param->EarlyDiffusion ;
			Buffer->XAudio2_8ReverbParameter.LateDiffusion       = Param->LateDiffusion ;
			Buffer->XAudio2_8ReverbParameter.LowEQGain           = Param->LowEQGain ;
			Buffer->XAudio2_8ReverbParameter.LowEQCutoff         = Param->LowEQCutoff ;
			Buffer->XAudio2_8ReverbParameter.HighEQGain          = Param->HighEQGain ;
			Buffer->XAudio2_8ReverbParameter.HighEQCutoff        = Param->HighEQCutoff ;

			Buffer->XAudio2_8ReverbParameter.RoomFilterFreq      = Param->RoomFilterFreq ;
			Buffer->XAudio2_8ReverbParameter.RoomFilterMain      = Param->RoomFilterMain ;
			Buffer->XAudio2_8ReverbParameter.RoomFilterHF        = Param->RoomFilterHF ;
			Buffer->XAudio2_8ReverbParameter.ReflectionsGain     = Param->ReflectionsGain ;
			Buffer->XAudio2_8ReverbParameter.ReverbGain          = Param->ReverbGain ;
			Buffer->XAudio2_8ReverbParameter.DecayTime           = Param->DecayTime ;
			Buffer->XAudio2_8ReverbParameter.Density             = Param->Density ;
			Buffer->XAudio2_8ReverbParameter.RoomSize            = Param->RoomSize ;

			Buffer->XAudio2_8ReverbParameter.DisableLateField    = FALSE ;

			Buffer->XA2_8SubmixVoice->SetEffectParameters( 0, &Buffer->XAudio2_8ReverbParameter, sizeof( Buffer->XAudio2_8ReverbParameter ) ) ;
		}
		else
		{
			Buffer->XAudio2ReverbParameter.WetDryMix           = Param->WetDryMix ;

			Buffer->XAudio2ReverbParameter.ReflectionsDelay    = Param->ReflectionsDelay;
			Buffer->XAudio2ReverbParameter.ReverbDelay         = Param->ReverbDelay ;
			Buffer->XAudio2ReverbParameter.RearDelay           = Param->RearDelay ;

			Buffer->XAudio2ReverbParameter.PositionLeft        = Param->PositionLeft ;
			Buffer->XAudio2ReverbParameter.PositionRight       = Param->PositionRight ;
			Buffer->XAudio2ReverbParameter.PositionMatrixLeft  = Param->PositionMatrixLeft ;
			Buffer->XAudio2ReverbParameter.PositionMatrixRight = Param->PositionMatrixRight ;
			Buffer->XAudio2ReverbParameter.EarlyDiffusion      = Param->EarlyDiffusion ;
			Buffer->XAudio2ReverbParameter.LateDiffusion       = Param->LateDiffusion ;
			Buffer->XAudio2ReverbParameter.LowEQGain           = Param->LowEQGain ;
			Buffer->XAudio2ReverbParameter.LowEQCutoff         = Param->LowEQCutoff ;
			Buffer->XAudio2ReverbParameter.HighEQGain          = Param->HighEQGain ;
			Buffer->XAudio2ReverbParameter.HighEQCutoff        = Param->HighEQCutoff ;

			Buffer->XAudio2ReverbParameter.RoomFilterFreq      = Param->RoomFilterFreq ;
			Buffer->XAudio2ReverbParameter.RoomFilterMain      = Param->RoomFilterMain ;
			Buffer->XAudio2ReverbParameter.RoomFilterHF        = Param->RoomFilterHF ;
			Buffer->XAudio2ReverbParameter.ReflectionsGain     = Param->ReflectionsGain ;
			Buffer->XAudio2ReverbParameter.ReverbGain          = Param->ReverbGain ;
			Buffer->XAudio2ReverbParameter.DecayTime           = Param->DecayTime ;
			Buffer->XAudio2ReverbParameter.Density             = Param->Density ;
			Buffer->XAudio2ReverbParameter.RoomSize            = Param->RoomSize ;

			Buffer->XA2SubmixVoice->SetEffectParameters( 0, &Buffer->XAudio2ReverbParameter, sizeof( Buffer->XAudio2ReverbParameter ) ) ;
		}
	}
	else
	{
	}

	// �I��
	return D_DS_OK ;
}

static HRESULT SoundBuffer_SetPresetReverbParam( SOUNDBUFFER *Buffer, int PresetNo )
{
	if( Buffer->Is3DSound == FALSE || Buffer->Valid == FALSE ) return -1 ;

	if( DSOUND.EnableSoundCaptureFlag )
	{
	}
	else
	// XAudio2 ���g�p����ꍇ�̂ݗL��
	if( DSOUND.EnableXAudioFlag )
	{
		if( DSOUND.XAudio2_8DLL != NULL )
		{
			Buffer->XAudio2_8ReverbParameter = DSOUND.XAudio2_8ReverbParameters[ PresetNo ] ;
			Buffer->XA2_8SubmixVoice->SetEffectParameters( 0, &Buffer->XAudio2_8ReverbParameter, sizeof( Buffer->XAudio2_8ReverbParameter ) ) ;
		}
		else
		{
			Buffer->XAudio2ReverbParameter = DSOUND.XAudio2ReverbParameters[ PresetNo ] ;
			Buffer->XA2SubmixVoice->SetEffectParameters( 0, &Buffer->XAudio2ReverbParameter, sizeof( Buffer->XAudio2ReverbParameter ) ) ;
		}
	}
	else
	{
	}

	// �I��
	return D_DS_OK ;
}













// XAudio2�֌W
static HRESULT D_XAudio2CreateVolumeMeter( IUnknown** ppApo, DWORD Flags )
{
	if( DSOUND.XAudio2_8DLL != NULL )
	{
		return DSOUND.CreateAudioVolumeMeterFunc( ppApo ) ;
	}
	else
	{
		return WinAPIData.Win32Func.CoCreateInstanceFunc((Flags & D_XAUDIO2FX_DEBUG) ? CLSID_AUDIOVOLUMEMeter2_7_DEBUG : CLSID_AUDIOVOLUMEMeter2_7, NULL, CLSCTX_INPROC_SERVER, IID_IUNKNOWN, (void**)ppApo ) ;
	}
}

static HRESULT D_XAudio2CreateReverb( IUnknown** ppApo, DWORD Flags )
{
	if( DSOUND.XAudio2_8DLL != NULL )
	{
		return DSOUND.CreateAudioReverbFunc( ppApo ) ;
	}
	else
	{
	    return WinAPIData.Win32Func.CoCreateInstanceFunc( ( Flags & D_XAUDIO2FX_DEBUG) ? CLSID_AUDIOREVERB2_7_DEBUG : CLSID_AUDIOREVERB2_7, NULL, CLSCTX_INPROC_SERVER, IID_IUNKNOWN, (void**)ppApo ) ;
	}
}

static void D_ReverbConvertI3DL2ToNative( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS* pNative )
{
    float reflectionsDelay;
    float reverbDelay;

    // RoomRolloffFactor is ignored

    // These parameters have no equivalent in I3DL2
    pNative->RearDelay = D_XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY; // 5
    pNative->PositionLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionMatrixLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->PositionMatrixRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->RoomSize = D_XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE; // 100
    pNative->LowEQCutoff = 4;
    pNative->HighEQCutoff = 6;

    // The rest of the I3DL2 parameters map to the native property set
    pNative->RoomFilterMain = (float)pI3DL2->Room / 100.0f;
    pNative->RoomFilterHF = (float)pI3DL2->RoomHF / 100.0f;

    if (pI3DL2->DecayHFRatio >= 1.0f)
    {
        int index = _DTOL(-4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->HighEQGain = 8;
        pNative->DecayTime = pI3DL2->DecayTime * pI3DL2->DecayHFRatio;
    }
    else
    {
        int index = _DTOL(4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = 8;
        pNative->HighEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->DecayTime = pI3DL2->DecayTime;
    }

    reflectionsDelay = pI3DL2->ReflectionsDelay * 1000.0f;
    if (reflectionsDelay >= D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY) // 300
    {
        reflectionsDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY - 1);
    }
    else if (reflectionsDelay <= 1)
    {
        reflectionsDelay = 1;
    }
    pNative->ReflectionsDelay = (DWORD)reflectionsDelay;

    reverbDelay = pI3DL2->ReverbDelay * 1000.0f;
    if (reverbDelay >= D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY) // 85
    {
        reverbDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY - 1);
    }
    pNative->ReverbDelay = (BYTE)reverbDelay;

    pNative->ReflectionsGain = pI3DL2->Reflections / 100.0f;
    pNative->ReverbGain = pI3DL2->Reverb / 100.0f;
    pNative->EarlyDiffusion = (BYTE)(15.0f * pI3DL2->Diffusion / 100.0f);
    pNative->LateDiffusion = pNative->EarlyDiffusion;
    pNative->Density = pI3DL2->Density;
    pNative->RoomFilterFreq = pI3DL2->HFReference;

    pNative->WetDryMix = pI3DL2->WetDryMix;
}

static void D_ReverbConvertI3DL2ToNative2_8( const D_XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2, D_XAUDIO2FX_REVERB_PARAMETERS2_8* pNative )
{
    float reflectionsDelay;
    float reverbDelay;

    // RoomRolloffFactor is ignored

    // These parameters have no equivalent in I3DL2
    pNative->RearDelay = D_XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY; // 5
    pNative->PositionLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionMatrixLeft = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->PositionMatrixRight = D_XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->RoomSize = D_XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE; // 100
    pNative->LowEQCutoff = 4;
    pNative->HighEQCutoff = 6;

    // The rest of the I3DL2 parameters map to the native property set
    pNative->RoomFilterMain = (float)pI3DL2->Room / 100.0f;
    pNative->RoomFilterHF = (float)pI3DL2->RoomHF / 100.0f;

    if (pI3DL2->DecayHFRatio >= 1.0f)
    {
        int index = _DTOL(-4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->HighEQGain = 8;
        pNative->DecayTime = pI3DL2->DecayTime * pI3DL2->DecayHFRatio;
    }
    else
    {
        int index = _DTOL(4.0 * _LOG10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = 8;
        pNative->HighEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->DecayTime = pI3DL2->DecayTime;
    }

    reflectionsDelay = pI3DL2->ReflectionsDelay * 1000.0f;
    if (reflectionsDelay >= D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY) // 300
    {
        reflectionsDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY - 1);
    }
    else if (reflectionsDelay <= 1)
    {
        reflectionsDelay = 1;
    }
    pNative->ReflectionsDelay = (DWORD)reflectionsDelay;

    reverbDelay = pI3DL2->ReverbDelay * 1000.0f;
    if (reverbDelay >= D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY) // 85
    {
        reverbDelay = (float)(D_XAUDIO2FX_REVERB_MAX_REVERB_DELAY - 1);
    }
    pNative->ReverbDelay = (BYTE)reverbDelay;

    pNative->ReflectionsGain = pI3DL2->Reflections / 100.0f;
    pNative->ReverbGain = pI3DL2->Reverb / 100.0f;
    pNative->EarlyDiffusion = (BYTE)(15.0f * pI3DL2->Diffusion / 100.0f);
    pNative->LateDiffusion = pNative->EarlyDiffusion;
    pNative->Density = pI3DL2->Density;
    pNative->RoomFilterFreq = pI3DL2->HFReference;

    pNative->WetDryMix = pI3DL2->WetDryMix;
    pNative->DisableLateField = FALSE;
}









#ifndef DX_NON_BEEP

// BEEP���Đ��p����

// �r�[�v�����g���ݒ�֐�
extern int NS_SetBeepFrequency( int Freq )
{
	WAVEFORMATEX wfmtx ;
	D_DSBUFFERDESC dsbdesc ;
	HRESULT hr ;
	LPVOID write1 ;
	DWORD length1 ;
	LPVOID write2 ;
	DWORD length2 ;
	int ChNum , Rate , Byte ;

	// �r�[�v���p�̃o�b�t�@���쐬����
	{
		// �v���C�}���o�b�t�@�̃t�H�[�}�b�g���Z�b�g����
		ChNum = 1 ;
		Rate = 44100 ;
		Byte = 2 ;

		_MEMSET( &wfmtx, 0, sizeof( wfmtx ) ) ;
		wfmtx.cbSize			= 0 ;
		wfmtx.wFormatTag		= WAVE_FORMAT_PCM ;										// PCM�t�H�[�}�b�g
		wfmtx.nChannels			= ChNum ;												// �`�����l���Q���X�e���I
		wfmtx.nSamplesPerSec	= Rate ;												// �Đ����[�g
		wfmtx.wBitsPerSample	= Byte * 8 ;											// �P���ɂ�����f�[�^�r�b�g��
		wfmtx.nBlockAlign		= Byte * wfmtx.nChannels ;								// �P�w���c�ɂ�����f�[�^�o�C�g��
		wfmtx.nAvgBytesPerSec	= wfmtx.nSamplesPerSec * wfmtx.nBlockAlign ;			// �P�b�ɂ�����f�[�^�o�C�g��

		// �o�b�t�@�����X�e�[�^�X�̃Z�b�g
		_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
		dsbdesc.dwSize			= sizeof( dsbdesc ) ;
		dsbdesc.dwFlags			= D_DSBCAPS_GLOBALFOCUS | D_DSBCAPS_STATIC | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | D_DSBCAPS_CTRLFREQUENCY ;
		dsbdesc.dwBufferBytes	= Byte * Rate ;
		dsbdesc.lpwfxFormat		= &wfmtx ;

		hr = DSOUND.DirectSoundObject->CreateSoundBuffer( &dsbdesc , &DSOUND.BeepSoundBuffer , NULL ) ;
		if( hr != D_DS_OK )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xd3\x30\xfc\x30\xd7\x30\x28\x75\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x6e\x30\xd7\x30\xe9\x30\xa4\x30\xde\x30\xea\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�r�[�v�pDirectSound�̃v���C�}���T�E���h�o�b�t�@�̍쐬�Ɏ��s���܂���\n" @*/ ) ;
			return -1 ;
		}
	}

	// �f�[�^���Z�b�g����
	{
		DWORD i , j, ltemp ;
		DWORD k ;
		WORD *p ;
		int TempI ;
		float Sin, Cos, Temp ;

		hr = DSOUND.BeepSoundBuffer->Lock( 0 , dsbdesc.dwBufferBytes, &write1 , &length1 , &write2 , &length2 , 0 ) ;		// �o�b�t�@�̃��b�N
		if( hr != D_DS_OK ) return -1 ;

		j = 0 ;
		p = ( WORD * )write1 ;
		ltemp = length1 >> 1 ;
		Temp  = (float)Rate / Freq ;
		TempI = _FTOL( Temp ) ;
		for( i = 0 ; i < ltemp ; i += 1 , j ++ )
		{
			k = j % TempI ;
			_SINCOS( ( PI_F * 2 ) * k / Temp, &Sin, &Cos ) ;
			*p = _FTOL( Sin * 32764 ) ;
			p += 1 ;
		}

		if( write2 != 0 )
		{
			p = ( WORD * )write2 ;
			ltemp = length2 >> 1 ;
			for( i = 0 ; i < ltemp ; i ++ , j ++ )
			{
				k = j % TempI ;
				_SINCOS( ( PI_F * 2 ) / Temp * k, &Sin, &Cos ) ;
				*p = _FTOL( Sin * 32764 ) ;
				p ++ ;
			}
		}
	}

	DSOUND.BeepSoundBuffer->Unlock( write1 , length1 , write2 , length2 ) ;								// �o�b�t�@�̃��b�N����

	// �I��
	return 0 ;
}

// �r�[�v�����Đ�����
extern int NS_PlayBeep( void )
{
	DWORD State ;
	HRESULT hr ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �Đ����������ꍇ�͂Ȃɂ����Ȃ��ŏI��
	hr = DSOUND.BeepSoundBuffer->GetStatus( &State ) ;
	if( hr != D_DS_OK ) return -1 ;
	if( State & D_DSBSTATUS_PLAYING  ) return -1 ;

	// �Đ�
	DSOUND.BeepSoundBuffer->SetCurrentPosition( 0 ) ;
	hr = DSOUND.BeepSoundBuffer->Play( 0 , 0 , D_DSBPLAY_LOOPING ) ;
	if( hr != D_DS_OK ) return -1 ;

	// �I��
	return 0 ;
}

// �r�[�v�����~�߂�	
extern int NS_StopBeep( void )
{
	HRESULT hr ;
	DWORD state ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �Đ���~
	if( DSOUND.BeepSoundBuffer )
	{
		hr = DSOUND.BeepSoundBuffer->GetStatus( &state ) ;
		if( hr != D_DS_OK ) return -1 ;
		if( state & D_DSBSTATUS_PLAYING  )
		{
			DSOUND.BeepSoundBuffer->Stop() ;
		}
	}

	// �I��
	return 0 ;
}

#endif








// ���b�p�[�֐�

// PlaySoundFile �̋�����
extern int NS_PlaySound( const TCHAR *FileName, int PlayType )
{
	return NS_PlaySoundFile( FileName, PlayType ) ;
}

// PlaySoundFile �̋�����
extern int PlaySound_WCHAR_T( const wchar_t *FileName, int PlayType )
{
	return PlaySoundFile_WCHAR_T( FileName, PlayType ) ;
}

// CheckSoundFile �̋�����
extern int NS_CheckSound( void )
{
	return NS_CheckSoundFile() ;
}

// StopSoundFile �̋�����
extern int NS_StopSound( void )
{
	return NS_StopSoundFile() ;
}

// SetVolumeSound �̋�����
extern int NS_SetVolumeSound( int VolumePal )
{
	return NS_SetVolumeSoundFile( VolumePal ) ;
}

// WAVE�t�@�C�����Đ�����
extern int NS_PlaySoundFile( const TCHAR *FileName , int PlayType )
{
#ifdef UNICODE
	return PlaySoundFile_WCHAR_T(
		FileName, PlayType
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = PlaySoundFile_WCHAR_T(
		UseFileNameBuffer, PlayType
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// WAVE�t�@�C�����Đ�����
extern int PlaySoundFile_WCHAR_T( const wchar_t *FileName , int PlayType )
{
	LOADSOUND_GPARAM GParam ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �ȑO�Đ����������f�[�^���~�߂�
	if( DSOUND.PlayWavSoundHandle != -1 )
	{
		NS_DeleteSoundMem( DSOUND.PlayWavSoundHandle ) ;
	}

	// �T�E���h�f�[�^��ǂݍ���
	InitLoadSoundGParam( &GParam ) ;
	DSOUND.PlayWavSoundHandle = LoadSoundMemBase_UseGParam( &GParam, FileName, 1, -1, FALSE, FALSE ) ;
	if( DSOUND.PlayWavSoundHandle == -1 )
	{
		return -1 ;
	}

	// �T�E���h���Đ�����
	NS_PlaySoundMem( DSOUND.PlayWavSoundHandle , PlayType ) ;

	// �I��
	return 0 ;
}

// WAVE�t�@�C�����Đ��������ׂ�
extern int NS_CheckSoundFile( void )
{
	int Result ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;
	if( DSOUND.PlayWavSoundHandle == -1 ) return 0 ;

	Result = NS_CheckSoundMem( DSOUND.PlayWavSoundHandle ) ;

	return Result ;
}

// WAVE�t�@�C���̍Đ����~�߂�
extern int NS_StopSoundFile( void )
{
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;
	if( DSOUND.PlayWavSoundHandle == -1 ) return 0 ;

	return NS_StopSoundMem( DSOUND.PlayWavSoundHandle ) ;
}

// WAVE�t�@�C���̉��ʂ��Z�b�g����
extern int NS_SetVolumeSoundFile( int VolumePal )
{
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;
	if( DSOUND.PlayWavSoundHandle == -1 ) return 0 ;

	return NS_SetVolumeSoundMem( VolumePal, DSOUND.PlayWavSoundHandle ) ;
}

// �T�E���h�L���v�`���̊J�n
extern	int StartSoundCapture( const char *SaveFilePath )
{
	DWORD size ;
	BYTE temp[NORMALWAVE_HEADERSIZE] ;
	
	// �T�E���h�L���v�`���������ȏꍇ�͉��������I��
	if( DSOUND.EnableSoundCaptureFlag == FALSE ) return -1 ;

	// ���ɃL���v�`�����J�n���Ă���ꍇ�͉��������I��
	if( DSOUND.SoundCaptureFlag == TRUE ) return -1 ;
	DSOUND.SoundCaptureFlag = TRUE;
	
	// �ۑ��p�̃t�@�C�����J��
	DSOUND.SoundCaptureFileHandle = CreateFileA( SaveFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;
	if( DSOUND.SoundCaptureFileHandle == INVALID_HANDLE_VALUE )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xad\x30\xe3\x30\xd7\x30\xc1\x30\xe3\x30\xdd\x4f\x58\x5b\x28\x75\x6e\x30\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x4c\x30\x8b\x95\x51\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x02\x30\x00"/*@ L"�T�E���h�L���v�`���ۑ��p�̃t�@�C�����J���܂���ł����B" @*/ ) ;
		return -1 ;
	}
	
	// �w�b�_�[���̋�f�[�^�������o��
	_MEMSET( temp, 0, sizeof( temp ) ) ;
	WriteFile( DSOUND.SoundCaptureFileHandle, temp, NORMALWAVE_HEADERSIZE, &size, NULL ) ;
	
	// �T�E���h�L���v�`���̃t���O�𗧂Ă�
	DSOUND.SoundCaptureFlag = TRUE ;
	
	// �L���v�`�������T���v�������O�ɂ���
	DSOUND.SoundCaptureSample = 0 ;
	
	// �I��
	return 0 ;
}

// �T�E���h�L���v�`���̎����I����
extern	int SoundCaptureProcess( int CaptureSample )
{
	int i, j, num, k ;
	DWORD size ;
	short *Temp = NULL ;
	SOUND *sound ;

	if( DSOUND.EnableSoundCaptureFlag == FALSE ) return -1 ;
	
	// �L���v�`�����s���ꍇ�̓������̊m��
	if( DSOUND.SoundCaptureFlag == TRUE )
	{
		Temp = (short *)DXALLOC( ( size_t )( CaptureSample * 4 ) ) ;
		_MEMSET( Temp, 0, ( size_t )( CaptureSample * 4 ) ) ;
	}
	
	// �T�E���h�o�b�t�@�̐i�s�������s��
	num = HandleManageArray[ DX_HANDLETYPE_SOUND ].Num ;
	for( i = 0, j = HandleManageArray[ DX_HANDLETYPE_SOUND ].AreaMin ; i < num ; j ++ )
	{
		if( HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ j ] == NULL ) continue ;
		i ++ ;
		
		sound = ( SOUND * )HandleManageArray[ DX_HANDLETYPE_SOUND ].Handle[ j ] ;
		switch( sound->Type )
		{
		case DX_SOUNDTYPE_NORMAL :
			for( k = 0 ; k < MAX_SOUNDBUFFER_NUM ; k ++ )
			{
				if( sound->Buffer[ k ].Valid == FALSE || sound->Buffer[ k ].State == FALSE ) continue ;
				SoundBuffer_FrameProcess( &sound->Buffer[ k ], CaptureSample, Temp ) ;
			}
			break ;
		
		case DX_SOUNDTYPE_STREAMSTYLE :
			if( sound->Buffer[ 0 ].Valid && sound->Buffer[ 0 ].State )
				SoundBuffer_FrameProcess( &sound->Buffer[ 0 ], CaptureSample, Temp ) ;
			break ;
		}
	}
	
	// �L���v�`���p�̃f�[�^�������o��
	if( DSOUND.SoundCaptureFlag == TRUE )
	{
		WriteFile( DSOUND.SoundCaptureFileHandle, Temp, ( DWORD )( CaptureSample * 4 ), &size, NULL ) ;
		DSOUND.SoundCaptureSample += CaptureSample ;
		
		// �������̉��
		DXFREE( Temp ) ;
	}
	
	// �I��
	return 0 ;
}

// �T�E���h�L���v�`���̏I��
extern	int EndSoundCapture( void )
{
	BYTE Header[NORMALWAVE_HEADERSIZE], *p ;
	WAVEFORMATEX *format;
	DWORD size ;

	// �T�E���h�L���v�`�������s���Ă��Ȃ������ꍇ�͉��������I��
	if( DSOUND.SoundCaptureFlag == FALSE ) return -1 ;
	
	// �t�H�[�}�b�g���Z�b�g
	format = (WAVEFORMATEX *)&Header[20]; 
	format->wFormatTag      = WAVE_FORMAT_PCM ;
	format->nChannels       = 2 ;
	format->nSamplesPerSec  = 44100 ;
	format->wBitsPerSample  = 16 ;
	format->nBlockAlign     = ( WORD )( format->wBitsPerSample / 8 * format->nChannels ) ;
	format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign ;
	format->cbSize          = 0 ;
	
	// �w�b�_�������o���ăt�@�C�������
	p = Header;
	_MEMCPY( (char *)p, "RIFF", 4 ) ;												p += 4 ;
	*((DWORD *)p) = ( DWORD )( DSOUND.SoundCaptureSample * format->nBlockAlign + NORMALWAVE_HEADERSIZE - 8 ) ;	p += 4 ;
	_MEMCPY( (char *)p, "WAVE", 4 ) ;												p += 4 ;

	_MEMCPY( (char *)p, "fmt ", 4 ) ;												p += 4 ;
	*((DWORD *)p) = NORMALWAVE_FORMATSIZE ;											p += 4 + NORMALWAVE_FORMATSIZE ;

	_MEMCPY( (char *)p, "data", 4 ) ;												p += 4 ;
	*((DWORD *)p) = ( DWORD )( DSOUND.SoundCaptureSample * format->nBlockAlign ) ;	p += 4 ;
	
	SetFilePointer( DSOUND.SoundCaptureFileHandle, 0, NULL, FILE_BEGIN ) ;
	WriteFile( DSOUND.SoundCaptureFileHandle, Header, NORMALWAVE_HEADERSIZE, &size, NULL ) ;
	CloseHandle( DSOUND.SoundCaptureFileHandle ) ;
	DSOUND.SoundCaptureFileHandle = NULL ;
	
	// �L���v�`���I��
	DSOUND.SoundCaptureFlag = FALSE ;
	
	// �I��
	return 0 ;
}



















// �\�t�g�E�G�A����T�E���h�n�֐�

// �\�t�g�T�E���h�n���h�����Z�b�g�A�b�v����
extern int SetupSoftSoundHandle(
	int SoftSoundHandle,
	int IsPlayer,
	int Channels,
	int BitsPerSample,
	int SamplesPerSec,
	int SampleNum
)
{
	SOFTSOUND * SSound ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �G���[����
	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) )
		return -1 ;

	// �p�����[�^�̏�����
	SSound->IsPlayer = IsPlayer ;
	SSound->BufferFormat.wFormatTag      = WAVE_FORMAT_PCM ;
	SSound->BufferFormat.nChannels       = ( WORD )Channels ;
	SSound->BufferFormat.nSamplesPerSec  = ( DWORD )SamplesPerSec ;
	SSound->BufferFormat.wBitsPerSample  = ( WORD )BitsPerSample ;
	SSound->BufferFormat.nBlockAlign     = ( WORD )( SSound->BufferFormat.wBitsPerSample / 8 * SSound->BufferFormat.nChannels ) ;
	SSound->BufferFormat.nAvgBytesPerSec = SSound->BufferFormat.nSamplesPerSec * SSound->BufferFormat.nBlockAlign ;
	SSound->BufferFormat.cbSize = 0 ;

	// �v���C���[���ǂ����ŏ����𕪊�
	if( IsPlayer )
	{
		D_DSBUFFERDESC dsbdesc ;

		// �v���C���[�̏ꍇ

		// �����O�o�b�t�@�̏�����
		RingBufInitialize( &SSound->Player.StockSample ) ;
		SSound->Player.StockSampleNum = 0 ;

		// �Đ��p�T�E���h�o�b�t�@�̍쐬
		_MEMSET( &dsbdesc, 0, sizeof( dsbdesc ) ) ;
		dsbdesc.dwSize = sizeof( dsbdesc ) ;
		dsbdesc.dwFlags = ( DWORD )( D_DSBCAPS_GLOBALFOCUS | D_DSBCAPS_CTRLPAN | D_DSBCAPS_CTRLVOLUME | D_DSBCAPS_CTRLFREQUENCY | D_DSBCAPS_GETCURRENTPOSITION2 | ( DSOUND.UseSoftwareMixing ? D_DSBCAPS_LOCSOFTWARE : D_DSBCAPS_STATIC ) ) ;
		dsbdesc.dwBufferBytes	= SOUNDSIZE( SSND_PLAYER_STRM_BUFSEC * SSound->BufferFormat.nAvgBytesPerSec / SSND_PLAYER_SEC_DIVNUM, SSound->BufferFormat.nBlockAlign ) ;
		dsbdesc.lpwfxFormat		= &SSound->BufferFormat ;

		if( SoundBuffer_Initialize( &SSound->Player.SoundBuffer, &dsbdesc, NULL, FALSE ) != D_DS_OK )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xbd\x30\xd5\x30\xc8\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd7\x30\xec\x30\xa4\x30\xe4\x30\xfc\x30\x28\x75\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xd0\x30\xc3\x30\xd5\x30\xa1\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�\�t�g�T�E���h�v���C���[�p�T�E���h�o�b�t�@�̍쐬�Ɏ��s���܂���" @*/ ) ;
			return -1 ;
		}
		SSound->Player.SoundBufferSize   = ( int )dsbdesc.dwBufferBytes ;
		SSound->Player.DataSetCompOffset = 0 ;

		SSound->Player.NoneDataSetCompOffset           = -1 ;
		SSound->Player.NoneDataPlayCheckBackPlayOffset = 0 ;
		SSound->Player.NoneDataPlayStartFlag           = FALSE ;

		SSound->Player.IsPlayFlag = FALSE ;

		// ��s�o�b�t�@�T�C�Y���Z�b�g
		SSound->Player.MaxDataSetSize = ( int )SOUNDSIZE( SSND_PLAYER_STRM_SAKICOPYSEC    * SSound->BufferFormat.nAvgBytesPerSec / SSND_PLAYER_SEC_DIVNUM, SSound->BufferFormat.nBlockAlign ) ;
		SSound->Player.MinDataSetSize = ( int )SOUNDSIZE( SSND_PLAYER_STRM_MINSAKICOPYSEC * SSound->BufferFormat.nAvgBytesPerSec / SSND_PLAYER_SEC_DIVNUM, SSound->BufferFormat.nBlockAlign ) ;

		// �N���e�B�J���Z�N�V�����̎擾
		CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		// �n���h�����X�g�ɒǉ�
		AddHandleList( &DSOUND.SoftSoundPlayerListFirst, &SSound->Player.SoftSoundPlayerList, -1, SSound ) ;

		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
	}
	else
	{
		// �T�E���h�f�[�^�̏ꍇ

		// �f�[�^���i�[���郁�����̈���m��
		SSound->Wave.BufferSampleNum = SampleNum ;
		SSound->Wave.Buffer          = DXALLOC( ( size_t )( SampleNum * SSound->BufferFormat.nBlockAlign ) ) ;
		if( SSound->Wave.Buffer == NULL )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xbd\x30\xd5\x30\xc8\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\xe2\x6c\x62\x5f\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x20\x00\x69\x00\x6e\x00\x20\x00\x41\x00\x64\x00\x64\x00\x53\x00\x6f\x00\x66\x00\x74\x00\x53\x00\x6f\x00\x75\x00\x6e\x00\x64\x00\x44\x00\x61\x00\x74\x00\x61\x00\x00"/*@ L"�\�t�g�n���h���̔g�`���i�[���郁�����̈�̊m�ۂɎ��s���܂��� in AddSoftSoundData" @*/ ) ;
			return -1 ;
		}
	}

	// ����I��
	return 0 ;
}

// �\�t�g�T�E���h�n���h���̏�����
extern int InitializeSoftSoundHandle( HANDLEINFO * )
{
	// ���ɉ������Ȃ�
	return 0 ;
}

// �\�t�g�T�E���h�n���h���̌�n��
extern int TerminateSoftSoundHandle( HANDLEINFO *HandleInfo )
{
	SOFTSOUND *SSound = ( SOFTSOUND * )HandleInfo ;

	// �v���C���[���ǂ����ŏ����𕪊�
	if( SSound->IsPlayer == TRUE )
	{
		// �����O�o�b�t�@�̉��
		RingBufTerminate( &SSound->Player.StockSample ) ;

		// �T�E���h�o�b�t�@�[�̉��
		SoundBuffer_Terminate( &SSound->Player.SoundBuffer ) ;

		// �\�t�g�T�E���h�v���C���[���X�g����O��
		SubHandleList( &SSound->Player.SoftSoundPlayerList ) ;
	}
	else
	{
		// �T�E���h�f�[�^���i�[���Ă����������̈���J��
		if( SSound->Wave.Buffer )
		{
			DXFREE( SSound->Wave.Buffer ) ;
			SSound->Wave.Buffer = NULL ;
		}
	}

	// �I��
	return 0 ;
}

// �\�t�g�T�E���h�n���h�����폜���邩�ǂ������`�F�b�N����֐�
extern int DeleteCancelCheckSoftSoundFunction( HANDLEINFO *HandleInfo )
{
	SOFTSOUND *SSound = ( SOFTSOUND * )HandleInfo ;

	return SSound->IsPlayer != FALSE ;
}

// �\�t�g�T�E���h�v���[���[�n���h�����폜���邩�ǂ������`�F�b�N����֐�
extern int DeleteCancelCheckSoftSoundPlayerFunction( HANDLEINFO *HandleInfo )
{
	SOFTSOUND *SSound = ( SOFTSOUND * )HandleInfo ;

	return SSound->IsPlayer == FALSE ;
}


// �\�t�g�E�G�A�ň����g�`�f�[�^�����ׂĉ������
extern int NS_InitSoftSound( void )
{
	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	return AllHandleSub( DX_HANDLETYPE_SOFTSOUND, DeleteCancelCheckSoftSoundFunction );
}

// LoadSoftSoundBase �̎������֐�
static int LoadSoftSoundBase_Static(
	LOADSOUND_GPARAM *GParam,
	int SoftSoundHandle,
	const wchar_t *FileName,
	const void *FileImage,
	int FileImageSize,
	int /*ASyncThread*/
)
{
	SOFTSOUND * SSound ;
	STREAMDATA Stream ;
	SOUNDCONV ConvData ;
	WAVEFORMATEX Format ;
	int SampleNum ;
	void *SrcBuffer = NULL ;
	int SoundSize ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) )
		return -1 ;

	// �t�@�C������ NULL �ł͂Ȃ��ꍇ�̓t�@�C������ǂݍ���
	if( FileName != NULL )
	{
		// �t�@�C�����J��
		Stream.DataPoint = (void *)FOPEN( FileName ) ;
		if( Stream.DataPoint == NULL ) return -1 ;
		Stream.ReadShred = *GetFileStreamDataShredStruct() ;
	}
	else
	// ����ȊO�̏ꍇ�̓���������ǂݍ���
	{
		Stream.DataPoint = MemStreamOpen( ( void *)FileImage, ( unsigned int )FileImageSize ) ;
		Stream.ReadShred = *GetMemStreamDataShredStruct() ;
	}

	// �o�b�l�`���ɕϊ�
	{
		_MEMSET( &ConvData, 0, sizeof( ConvData ) ) ;
		if( SetupSoundConvert( &ConvData, &Stream, GParam->DisableReadSoundFunctionMask
#ifndef DX_NON_OGGVORBIS
								,GParam->OggVorbisBitDepth, GParam->OggVorbisFromTheoraFile
#endif
								) < 0 )
		{
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xbd\x30\xd5\x30\xc8\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x28\x75\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x6e\x30\x30\xff\x23\xff\x2d\xff\x78\x30\x6e\x30\x09\x59\xdb\x63\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�\�t�g�T�E���h�p�̉����t�@�C���̂o�b�l�ւ̕ϊ��Ɏ��s���܂���" @*/ )) ;
			goto ERR ;
		}
		if( SoundConvertFast( &ConvData, &Format, &SrcBuffer, &SoundSize ) < 0 )
		{
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xbd\x30\xd5\x30\xc8\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\x28\x75\x6e\x30\xf3\x97\xf0\x58\xd5\x30\xa1\x30\xa4\x30\xeb\x30\x92\x30\x3c\x68\x0d\x7d\x59\x30\x8b\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�\�t�g�T�E���h�p�̉����t�@�C�����i�[���郁�����̈�̊m�ۂɎ��s���܂���" @*/ )) ;
			goto ERR ;
		}

		TerminateSoundConvert( &ConvData ) ;
	}

	// �t�@�C�������
	if( FileName != NULL )
	{
		FCLOSE( ( DWORD_PTR )Stream.DataPoint ) ;
	}
	else
	{
		MemStreamClose( Stream.DataPoint ) ;
	}
	Stream.DataPoint = NULL ;

	// �T���v���̐����Z�o����
	SampleNum = SoundSize / Format.nBlockAlign ;

	// �n���h���̃Z�b�g�A�b�v
	if( SetupSoftSoundHandle( SoftSoundHandle, FALSE, ( int )Format.nChannels, ( int )Format.wBitsPerSample, ( int )Format.nSamplesPerSec, SampleNum ) < 0 )
		goto ERR ;

	// �T�E���h�f�[�^�̃R�s�[
	_MEMCPY( SSound->Wave.Buffer, SrcBuffer, ( size_t )SoundSize ) ;

	// �T�E���h�f�[�^�̉��
	if( SrcBuffer )
	{
		DXFREE( SrcBuffer ) ;
	}

	// ����I��
	return 0 ;

ERR :
	if( Stream.DataPoint )
	{
		if( FileName != NULL )
		{
			FCLOSE( ( DWORD_PTR )Stream.DataPoint ) ;
		}
		else
		{
			MemStreamClose( Stream.DataPoint ) ;
		}
	}
	TerminateSoundConvert( &ConvData ) ;
	if( SrcBuffer )
	{
		DXFREE( SrcBuffer ) ;
	}

	// �G���[�I��
	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadSoftSoundBase �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadSoftSoundBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	LOADSOUND_GPARAM *GParam ;
	int SoftSoundHandle ;
	const wchar_t *FileName ;
	const void *FileImage ;
	int FileImageSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	GParam = ( LOADSOUND_GPARAM * )GetASyncLoadParamStruct( AParam->Data, &Addr ) ;
	SoftSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	FileImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadSoftSoundBase_Static( GParam, SoftSoundHandle, FileName, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( SoftSoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoftSoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// �\�t�g�E�G�A�ň����g�`�f�[�^���t�@�C���܂��̓�������ɓW�J���ꂽ�t�@�C���C���[�W����쐬����
extern int LoadSoftSoundBase_UseGParam(
	LOADSOUND_GPARAM *GParam,
	const wchar_t *FileName,
	const void *FileImage,
	int FileImageSize,
	int ASyncLoadFlag
)
{
	int SoftSoundHandle ;

	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// �n���h���̍쐬
	SoftSoundHandle = AddHandle( DX_HANDLETYPE_SOFTSOUND, FALSE, -1 ) ;
	if( SoftSoundHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FileName, FullPath ) ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamStruct( NULL, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( NULL, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, ( void * )FileImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FileImageSize ) ;

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadSoftSoundBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamStruct( AParam->Data, &Addr, GParam, sizeof( *GParam ) ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, ( void * )FileImage ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileImageSize ) ;

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( SoftSoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadSoftSoundBase_Static( GParam, SoftSoundHandle, FileName, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// �I��
	return SoftSoundHandle ;

ERR :
	SubHandle( SoftSoundHandle ) ;

	return -1 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^���t�@�C������쐬����
extern	int NS_LoadSoftSound( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadSoftSound_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadSoftSound_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// �\�t�g�E�G�A�ň����g�`�f�[�^���t�@�C������쐬����
extern	int LoadSoftSound_WCHAR_T( const wchar_t *FileName )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return LoadSoftSoundBase_UseGParam( &GParam, FileName, NULL, 0, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^����������ɓW�J���ꂽ�t�@�C���C���[�W����쐬����
extern	int	NS_LoadSoftSoundFromMemImage( const void *FileImageBuffer, int FileImageSize )
{
	LOADSOUND_GPARAM GParam ;

	InitLoadSoundGParam( &GParam ) ;
	return LoadSoftSoundBase_UseGParam( &GParam, NULL, FileImageBuffer, FileImageSize, GetASyncLoadFlag() ) ;
}

// MakeSoftSoundBase �̎������֐�
static int MakeSoftSoundBase_Static(
	int SoftSoundHandle,
	int IsPlayer,
	int Channels,
	int BitsPerSample,
	int SamplesPerSec,
	int SampleNum,
	int UseFormat_SoftSoundHandle,
	int /*ASyncThread*/
)
{
	SOFTSOUND * SSound ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) )
		return -1 ;

	if( UseFormat_SoftSoundHandle >= 0 )
	{
		if( NS_GetSoftSoundFormat( UseFormat_SoftSoundHandle, &Channels, &BitsPerSample, &SamplesPerSec ) < 0 )
			return -1 ;
	}

	if( SetupSoftSoundHandle( SoftSoundHandle, IsPlayer, Channels, BitsPerSample, SamplesPerSec, SampleNum ) < 0 )
		return -1 ;

	// ����I��
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD
// MakeSoftSoundBase �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void MakeSoftSoundBase_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int SoftSoundHandle ;
	int IsPlayer ;
	int Channels ;
	int BitsPerSample ;
	int SamplesPerSec ;
	int SampleNum ;
	int UseFormat_SoftSoundHandle ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	SoftSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	IsPlayer = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	Channels = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	BitsPerSample = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	SamplesPerSec = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	SampleNum = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	UseFormat_SoftSoundHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = MakeSoftSoundBase_Static( SoftSoundHandle, IsPlayer, Channels, BitsPerSample, SamplesPerSec, SampleNum, UseFormat_SoftSoundHandle, TRUE ) ;

	DecASyncLoadCount( SoftSoundHandle ) ;
	if( Result < 0 )
	{
		SubHandle( SoftSoundHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// �\�t�g�E�G�A�Ő��䂷��T�E���h�f�[�^�n���h���̍쐬
extern int MakeSoftSoundBase_UseGParam(
	int IsPlayer,
	int Channels,
	int BitsPerSample,
	int SamplesPerSec,
	int SampleNum,
	int UseFormat_SoftSoundHandle,
	int ASyncLoadFlag
)
{
	int SoftSoundHandle ;

	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// �n���h���̍쐬
	SoftSoundHandle = AddHandle( DX_HANDLETYPE_SOFTSOUND, FALSE, -1 ) ;
	if( SoftSoundHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamInt( NULL, &Addr, IsPlayer ) ;
		AddASyncLoadParamInt( NULL, &Addr, Channels ) ;
		AddASyncLoadParamInt( NULL, &Addr, BitsPerSample ) ;
		AddASyncLoadParamInt( NULL, &Addr, SamplesPerSec ) ;
		AddASyncLoadParamInt( NULL, &Addr, SampleNum ) ;
		AddASyncLoadParamInt( NULL, &Addr, UseFormat_SoftSoundHandle ) ;

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = MakeSoftSoundBase_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SoftSoundHandle ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, IsPlayer ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, Channels ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, BitsPerSample ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SamplesPerSec ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, SampleNum ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, UseFormat_SoftSoundHandle ) ;

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( SoftSoundHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( MakeSoftSoundBase_Static( SoftSoundHandle, IsPlayer, Channels, BitsPerSample, SamplesPerSec, SampleNum, UseFormat_SoftSoundHandle, FALSE ) < 0 )
			goto ERR ;
	}

	// �I��
	return SoftSoundHandle ;

ERR :
	SubHandle( SoftSoundHandle ) ;

	return -1 ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �t�H�[�}�b�g�͈����̃\�t�g�E�G�A�T�E���h�n���h���Ɠ������̂ɂ��� )
extern	int NS_MakeSoftSound( int UseFormat_SoftSoundHandle, int SampleNum )
{
	if( UseFormat_SoftSoundHandle < 0 )
		return -1 ;

	return MakeSoftSoundBase_UseGParam( FALSE, 0, 0, 0, SampleNum, UseFormat_SoftSoundHandle, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:2 �ʎq���r�b�g��:16bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSound2Ch16Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 16, 44100, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:2 �ʎq���r�b�g��:16bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSound2Ch16Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 16, 22050, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:2 �ʎq���r�b�g��:8bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSound2Ch8Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 8, 44100, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:2 �ʎq���r�b�g��:8bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSound2Ch8Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 2, 8, 22050, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:1 �ʎq���r�b�g��:16bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSound1Ch16Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 16, 44100, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:1 �ʎq���r�b�g��:16bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSound1Ch16Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 16, 22050, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:1 �ʎq���r�b�g��:8bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSound1Ch8Bit44KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 8, 44100, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����( �`�����l����:1 �ʎq���r�b�g��:8bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSound1Ch8Bit22KHz( int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, 1, 8, 22050, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň�����̔g�`�f�[�^���쐬����
extern	int NS_MakeSoftSoundCustom( int ChannelNum, int BitsPerSample, int SamplesPerSec, int SampleNum )
{
	return MakeSoftSoundBase_UseGParam( FALSE, ChannelNum, BitsPerSample, SamplesPerSec, SampleNum, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^���������
extern	int NS_DeleteSoftSound( int SoftSoundHandle )
{
	SOFTSOUND *SSound ;

	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) ) return -1 ;

	// �v���C���[���ǂ����̃`�F�b�N
	if( SSound->IsPlayer != FALSE )
	{
		// �������G���[
		return -1 ;
	}

	return SubHandle( SoftSoundHandle ) ;
}

#ifndef DX_NON_SAVEFUNCTION

// �\�t�g�E�G�A�ň����g�`�f�[�^�𖳈��kWav�`���ŕۑ�����
extern int NS_SaveSoftSound( int SoftSoundHandle, const TCHAR *FileName )
{
#ifdef UNICODE
	return SaveSoftSound_WCHAR_T(
		SoftSoundHandle, FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = SaveSoftSound_WCHAR_T(
		SoftSoundHandle, UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�𖳈��kWav�`���ŕۑ�����
extern int SaveSoftSound_WCHAR_T( int SoftSoundHandle, const wchar_t *FileName )
{
	FILE *fp ;
	BYTE Header[NORMALWAVE_HEADERSIZE], *p ;
	WAVEFORMATEX *format;
	SOFTSOUND * SSound ;

	// �G���[����
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	fp = _wfopen( FileName, L"wb" ) ;

	// �t�H�[�}�b�g���Z�b�g
	format = (WAVEFORMATEX *)&Header[20]; 
	format->wFormatTag      = WAVE_FORMAT_PCM ;
	format->nChannels       = SSound->BufferFormat.nChannels ;
	format->nSamplesPerSec  = SSound->BufferFormat.nSamplesPerSec ;
	format->wBitsPerSample  = SSound->BufferFormat.wBitsPerSample ;
	format->nBlockAlign     = ( WORD )( format->wBitsPerSample / 8 * format->nChannels ) ;
	format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign ;
	format->cbSize          = 0 ;
	
	// �w�b�_�������o���ăt�@�C�������
	p = Header;
	_MEMCPY( (char *)p, "RIFF", 4 ) ;																	p += 4 ;
	*((DWORD *)p) = ( DWORD )( SSound->Wave.BufferSampleNum * format->nBlockAlign + NORMALWAVE_HEADERSIZE - 8 ) ;	p += 4 ;
	_MEMCPY( (char *)p, "WAVE", 4 ) ;																	p += 4 ;

	_MEMCPY( (char *)p, "fmt ", 4 ) ;																	p += 4 ;
	*((DWORD *)p) = NORMALWAVE_FORMATSIZE ;																p += 4 + NORMALWAVE_FORMATSIZE ;

	_MEMCPY( (char *)p, "data", 4 ) ;																	p += 4 ;
	*((DWORD *)p) = ( DWORD )( SSound->Wave.BufferSampleNum * format->nBlockAlign ) ;					p += 4 ;
	
	fwrite( Header, NORMALWAVE_HEADERSIZE, 1, fp ) ;
	fwrite( SSound->Wave.Buffer, ( size_t )( SSound->Wave.BufferSampleNum * format->nBlockAlign ), 1, fp ) ;
	fclose( fp ) ;

	// �I��
	return 0 ;
}

#endif // DX_NON_SAVEFUNCTION

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃T���v�������擾����
extern	int NS_GetSoftSoundSampleNum( int SoftSoundHandle )
{
	SOFTSOUND * SSound ;

	// �G���[����
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// �T���v������Ԃ�
	return SSound->Wave.BufferSampleNum ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃t�H�[�}�b�g���擾����
extern	int NS_GetSoftSoundFormat( int SoftSoundHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec )
{
	SOFTSOUND * SSound ;

	// �G���[����
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// �f�[�^���Z�b�g
	if( Channels      ) *Channels      = SSound->BufferFormat.nChannels ;
	if( BitsPerSample ) *BitsPerSample = SSound->BufferFormat.wBitsPerSample ;
	if( SamplesPerSec ) *SamplesPerSec = ( int )SSound->BufferFormat.nSamplesPerSec ;

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃T���v����ǂݎ��
extern	int NS_ReadSoftSoundData( int SoftSoundHandle, int SamplePosition, int *Channel1, int *Channel2 )
{
	SOFTSOUND * SSound ;
	BYTE *Src ;

	// �G���[����
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// �T���v���͈̔͊O�������ꍇ���G���[
	if( SSound->Wave.BufferSampleNum <= SamplePosition || SamplePosition < 0 )
		return -1 ;

	// �T���v���̈ʒu���Z�o
	Src = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * SamplePosition ;

	// �T���v����Ԃ�
	switch( SSound->BufferFormat.wBitsPerSample )
	{
	case 8 :
		if( Channel1 )
		{
			*Channel1 = ( int )Src[ 0 ] ;
		}

		if( Channel2 )
		{
			if( SSound->BufferFormat.nChannels == 1 )
			{
				*Channel2 = 127 ;
			}
			else
			{
				*Channel2 = ( int )Src[ 1 ] ;
			}
		}
		break ;

	case 16 :
		if( Channel1 )
		{
			*Channel1 = ( ( short * )Src )[ 0 ] ;
		}

		if( Channel2 )
		{
			if( SSound->BufferFormat.nChannels == 1 )
			{
				*Channel2 = 0 ;
			}
			else
			{
				*Channel2 = ( ( short * )Src )[ 1 ] ;
			}
		}
		break ;
	}

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃T���v������������
extern	int NS_WriteSoftSoundData( int SoftSoundHandle, int SamplePosition, int Channel1, int Channel2 )
{
	SOFTSOUND * SSound ;
	BYTE *Dest ;

	// �G���[����
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	// �T���v���͈̔͊O�������ꍇ���G���[
	if( SSound->Wave.BufferSampleNum <= SamplePosition || SamplePosition < 0 )
		return -1 ;

	// �T���v���̈ʒu���Z�o
	Dest = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * SamplePosition ;

	// �T���v������������
	switch( SSound->BufferFormat.wBitsPerSample )
	{
	case 8 :
		     if( Channel1 > 255 ) Channel1 = 255 ;
		else if( Channel1 <   0 ) Channel1 =   0 ;
		Dest[ 0 ] = ( BYTE )Channel1 ;

		if( SSound->BufferFormat.nChannels != 1 )
		{
				 if( Channel2 > 255 ) Channel2 = 255 ;
			else if( Channel2 <   0 ) Channel2 =   0 ;
			Dest[ 1 ] = ( BYTE )Channel2 ;
		}
		break ;

	case 16 :
			 if( Channel1 >  32767 ) Channel1 =  32767 ;
		else if( Channel1 < -32768 ) Channel1 = -32768 ;
		( ( short * )Dest )[ 0 ] = ( short )Channel1 ;

		if( SSound->BufferFormat.nChannels != 1 )
		{
				 if( Channel2 >  32767 ) Channel2 =  32767 ;
			else if( Channel2 < -32768 ) Channel2 = -32768 ;
			( ( short * )Dest )[ 1 ] = ( short )Channel2 ;
		}
		break ;
	}

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̔g�`�C���[�W���i�[����Ă��郁�����A�h���X���擾����
extern	void *NS_GetSoftSoundDataImage( int SoftSoundHandle )
{
	SOFTSOUND * SSound ;

	// �G���[����
	if( SSND_MASKHCHK_ASYNC( SoftSoundHandle, SSound ) ) return NULL ;
	if( SSound->IsPlayer == 1 ) return NULL ;

	// �I��
	return SSound->Wave.Buffer ;
}


// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�����ׂĉ������
extern	int NS_InitSoftSoundPlayer( void )
{
	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	return AllHandleSub( DX_HANDLETYPE_SOFTSOUND, DeleteCancelCheckSoftSoundPlayerFunction );
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �t�H�[�}�b�g�͈����̃\�t�g�E�G�A�T�E���h�n���h���Ɠ������̂ɂ��� )
extern int NS_MakeSoftSoundPlayer( int UseFormat_SoftSoundHandle )
{
	if( UseFormat_SoftSoundHandle < 0 )
		return -1 ;

	return MakeSoftSoundBase_UseGParam( TRUE, 0, 0, 0, 0, UseFormat_SoftSoundHandle, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:2 �ʎq���r�b�g��:16bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSoundPlayer2Ch16Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 16, 44100, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:2 �ʎq���r�b�g��:16bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSoundPlayer2Ch16Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 16, 22050, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:2 �ʎq���r�b�g��:8bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSoundPlayer2Ch8Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 8, 44100, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:2 �ʎq���r�b�g��:8bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSoundPlayer2Ch8Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 2, 8, 22050, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:1 �ʎq���r�b�g��:16bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSoundPlayer1Ch16Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 16, 44100, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:1 �ʎq���r�b�g��:16bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSoundPlayer1Ch16Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 16, 22050, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:1 �ʎq���r�b�g��:8bit �T���v�����O���g��:44KHz )
extern	int NS_MakeSoftSoundPlayer1Ch8Bit44KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 8, 44100, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����( �`�����l����:1 �ʎq���r�b�g��:8bit �T���v�����O���g��:22KHz )
extern	int NS_MakeSoftSoundPlayer1Ch8Bit22KHz( void )
{
	return MakeSoftSoundBase_UseGParam( TRUE, 1, 8, 22050, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���쐬����
extern	int NS_MakeSoftSoundPlayerCustom( int ChannelNum, int BitsPerSample, int SamplesPerSec )
{
	return MakeSoftSoundBase_UseGParam( TRUE, ChannelNum, BitsPerSample, SamplesPerSec, 0, -1, GetASyncLoadFlag() ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[���폜����
extern	int NS_DeleteSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND *SSound ;

	if( SSND_MASKHCHK_ASYNC( SSoundPlayerHandle, SSound ) ) return -1 ;

	// �v���C���[���ǂ����̃`�F�b�N
	if( SSound->IsPlayer != TRUE )
	{
		// �������G���[
		return -1 ;
	}

	return SubHandle( SSoundPlayerHandle ) ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�ɔg�`�f�[�^��ǉ�����( �t�H�[�}�b�g�������ł͂Ȃ��ꍇ�̓G���[ )
extern	int NS_AddDataSoftSoundPlayer( int SSoundPlayerHandle, int SoftSoundHandle, int AddSamplePosition, int AddSampleNum )
{
	SOFTSOUND *SPlayer, *SSound ;
	void *Src ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �G���[����
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == 1 ) return -1 ;

	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// �t�H�[�}�b�g���������G���[
	if( SSound->BufferFormat.nChannels != SPlayer->BufferFormat.nChannels ||
		SSound->BufferFormat.wBitsPerSample != SPlayer->BufferFormat.wBitsPerSample ||
		SSound->BufferFormat.nSamplesPerSec != SPlayer->BufferFormat.nSamplesPerSec )
		return -1 ;

	// �T���v���ʒu�̎w�肪�Ԉ���Ă�����G���[
	if( AddSampleNum == 0 || SSound->Wave.BufferSampleNum > AddSamplePosition + AddSampleNum )
		return -1 ; 

	// �]���ʒu�̃A�h���X���Z�o
	Src = ( BYTE * )SSound->Wave.Buffer + SSound->BufferFormat.nBlockAlign * AddSamplePosition ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �����O�o�b�t�@�Ƀf�[�^��ǉ�
	if( RingBufDataAdd( &SPlayer->Player.StockSample, Src, AddSampleNum * SSound->BufferFormat.nBlockAlign ) < 0 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
		return -1 ;
	}

	// �ǉ��������T���v�����𑝂₷
	SPlayer->Player.StockSampleNum += AddSampleNum ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�Ƀv���C���[���Ή������t�H�[�}�b�g�̐��g�`�f�[�^��ǉ�����
extern	int	NS_AddDirectDataSoftSoundPlayer( int SSoundPlayerHandle, const void *SoundData, int AddSampleNum )
{
	SOFTSOUND * SPlayer ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �����O�o�b�t�@�Ƀf�[�^��ǉ�
	if( RingBufDataAdd( &SPlayer->Player.StockSample, SoundData, AddSampleNum * SPlayer->BufferFormat.nBlockAlign ) < 0 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
		return -1 ;
	}

	// �ǉ��������T���v�����𑝂₷
	SPlayer->Player.StockSampleNum += AddSampleNum ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�ɔg�`�f�[�^����ǉ�����
extern int NS_AddOneDataSoftSoundPlayer( int SSoundPlayerHandle, int Channel1, int Channel2 )
{
	SOFTSOUND * SPlayer ;
	BYTE Dest[ 16 ] ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �f�[�^�̏���
	switch( SPlayer->BufferFormat.wBitsPerSample )
	{
	case 8 :
		     if( Channel1 > 255 ) Channel1 = 255 ;
		else if( Channel1 <   0 ) Channel1 =   0 ;
		Dest[ 0 ] = ( BYTE )Channel1 ;

		if( SPlayer->BufferFormat.nChannels != 1 )
		{
				 if( Channel2 > 255 ) Channel2 = 255 ;
			else if( Channel2 <   0 ) Channel2 =   0 ;
			Dest[ 1 ] = ( BYTE )Channel2 ;
		}
		break ;

	case 16 :
			 if( Channel1 >  32767 ) Channel1 =  32767 ;
		else if( Channel1 < -32768 ) Channel1 = -32768 ;
		( ( short * )Dest )[ 0 ] = ( short )Channel1 ;

		if( SPlayer->BufferFormat.nChannels != 1 )
		{
				 if( Channel2 >  32767 ) Channel2 =  32767 ;
			else if( Channel2 < -32768 ) Channel2 = -32768 ;
			( ( short * )Dest )[ 1 ] = ( short )Channel2 ;
		}
		break ;
	}

	// �����O�o�b�t�@�Ƀf�[�^��ǉ�
	if( RingBufDataAdd( &SPlayer->Player.StockSample, Dest, SPlayer->BufferFormat.nBlockAlign ) < 0 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;
		return -1 ;
	}

	// �T���v����������₷
	SPlayer->Player.StockSampleNum ++ ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}


// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�ɍĐ��p�T�E���h�o�b�t�@�ɓ]�����Ă��Ȃ��g�`�f�[�^�������A�Đ��p�T�E���h�o�b�t�@�ɂ������f�[�^�ȊO�������ǂ������擾����( TRUE:�����f�[�^�ȊO����  FALSE:�L���f�[�^������ )
extern	int	NS_CheckSoftSoundPlayerNoneData( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;
	int Result = -1 ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
		goto END ;

	// �X�V�������s��
	_SoftSoundPlayerProcess( SPlayer ) ;

	// �����f�[�^�Đ������ǂ����̃t���O���Z�b�g
	Result = SPlayer->Player.NoneDataPlayStartFlag ;

	// ���f�[�^�̃X�g�b�N������ꍇ�͖����f�[�^�Đ����ł͂Ȃ�
	if( SPlayer->Player.StockSampleNum != 0 )
		Result = FALSE ;

END :
	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// ��Ԃ�Ԃ�
	return Result ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�ɒǉ������g�`�f�[�^�ł܂��Đ��p�T�E���h�o�b�t�@�ɓ]������Ă��Ȃ��g�`�f�[�^�̃T���v�������擾����
extern	int	NS_GetStockDataLengthSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
		return -1 ;

	// �����O�o�b�t�@�ɂ��関�]�����̃T���v����Ԃ�
	return SPlayer->Player.StockSampleNum ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�������f�[�^�t�H�[�}�b�g���擾����
extern	int	NS_GetSoftSoundPlayerFormat( int SSoundPlayerHandle, int *Channels, int *BitsPerSample, int *SamplesPerSec )
{
	SOFTSOUND * SPlayer ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) ) return -1 ;
	if( SPlayer->IsPlayer == 0 ) return -1 ;

	// �f�[�^���Z�b�g
	if( Channels      ) *Channels      = SPlayer->BufferFormat.nChannels ;
	if( BitsPerSample ) *BitsPerSample = SPlayer->BufferFormat.wBitsPerSample ;
	if( SamplesPerSec ) *SamplesPerSec = ( int )SPlayer->BufferFormat.nSamplesPerSec ;

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�̍Đ��������J�n����
extern	int NS_StartSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		return -1 ;
	}

	// ���ɍĐ�����Ă����牽�����Ȃ�
	if( SPlayer->Player.IsPlayFlag )
		goto END ;

	// �Đ���Ԃɂ���
	SPlayer->Player.IsPlayFlag = TRUE ;

	// �Đ��p�f�[�^���T�E���h�o�b�t�@�ɃZ�b�g����
	_SoftSoundPlayerProcess( SPlayer ) ;

	// �T�E���h�o�b�t�@�̍Đ��J�n
	SoundBuffer_Play( &SPlayer->Player.SoundBuffer, TRUE ) ;

END :

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�̍Đ��������J�n����Ă��邩�擾����( TRUE:�J�n���Ă���  FALSE:��~���Ă��� )
extern	int	NS_CheckStartSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
		return -1 ;

	// �Đ����t���O��Ԃ�
	return SPlayer->Player.IsPlayFlag ;
}


// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�̍Đ��������~����
extern	int NS_StopSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		return -1 ;
	}

	// �Đ�����Ă��Ȃ������牽�����Ȃ�
	if( SPlayer->Player.IsPlayFlag == FALSE )
		goto END ;

	// �Đ����~�߂�
	SoundBuffer_Stop( &SPlayer->Player.SoundBuffer, TRUE ) ;

	// �Đ����t���O��|��
	SPlayer->Player.IsPlayFlag = FALSE ;

END :

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^�̃v���C���[�̏�Ԃ�������Ԃɖ߂�( �ǉ����ꂽ�g�`�f�[�^�͍폜����A�Đ���Ԃ������ꍇ�͒�~���� )
extern	int NS_ResetSoftSoundPlayer( int SSoundPlayerHandle )
{
	SOFTSOUND * SPlayer ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �G���[����
	if( SSND_MASKHCHK( SSoundPlayerHandle, SPlayer ) || SPlayer->IsPlayer == 0 )
	{
		// �N���e�B�J���Z�N�V�����̉��
		CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

		return -1 ;
	}

	// �Đ����~�߂�
	SoundBuffer_Stop( &SPlayer->Player.SoundBuffer, TRUE ) ;

	// �Đ��ʒu��擪�ɖ߂�
	SoundBuffer_SetCurrentPosition( &SPlayer->Player.SoundBuffer, 0 ) ;

	// �����O�o�b�t�@���ď���������
	RingBufTerminate( &SPlayer->Player.StockSample ) ;
	RingBufInitialize( &SPlayer->Player.StockSample ) ;
	SPlayer->Player.StockSampleNum = 0 ;

	// ���̑���������������
	SPlayer->Player.DataSetCompOffset = 0 ;
	SPlayer->Player.NoneDataSetCompOffset = 0 ;
	SPlayer->Player.NoneDataPlayCheckBackPlayOffset = 0 ;
	SPlayer->Player.NoneDataPlayStartFlag = FALSE ;
	SPlayer->Player.IsPlayFlag = FALSE ;

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �S�Ẵ\�t�g�E�G�A�T�E���h�̈ꎞ��~��Ԃ�ύX����
extern int PauseSoftSoundAll( int PauseFlag )
{
	HANDLELIST *List ;
	SOFTSOUND * SPlayer ;
	DWORD State ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// ��~���邩�J�n���邩�ŏ����𕪊�
	if( PauseFlag )
	{
		// ��~����ꍇ

		for( List = DSOUND.SoftSoundPlayerListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			SPlayer = ( SOFTSOUND * )List->Data ;

			if( SPlayer->Player.SoundBuffer.Valid == FALSE || SPlayer->Player.SoundBuffer.DSBuffer == NULL )
				continue ;

			// ���ɏ�ԕۑ��ς݂̏ꍇ�͉������Ȃ�
			if( SPlayer->Player.SoundBufferPlayStateBackupFlagValid )
				continue ;

			// �T�E���h�o�b�t�@�̍Đ���Ԃ�ۑ�
			if( SoundBuffer_GetStatus( &SPlayer->Player.SoundBuffer, &State ) == D_DS_OK )
			{
				SPlayer->Player.SoundBufferPlayStateBackupFlagValid = TRUE ;

				if( State & D_DSBSTATUS_PLAYING )
				{
					SPlayer->Player.SoundBufferPlayStateBackupFlag = TRUE ;

					// �Đ�����Ă�����Đ����~�߂�
					SoundBuffer_Stop( &SPlayer->Player.SoundBuffer, TRUE ) ;
				}
				else
				{
					SPlayer->Player.SoundBufferPlayStateBackupFlag = FALSE ;
				}
			}
			else
			{
				SPlayer->Player.SoundBufferPlayStateBackupFlagValid = FALSE ;
			}
		}
	}
	else
	{
		// �ĊJ����ꍇ

		for( List = DSOUND.SoftSoundPlayerListFirst.Next ; List->Next != NULL ; List = List->Next )
		{
			SPlayer = ( SOFTSOUND * )List->Data ;

			// �Đ����ł͂Ȃ��ꍇ�̂ݏ���
			if( SPlayer->Player.IsPlayFlag )
			{
				if( SPlayer->Player.SoundBuffer.Valid == FALSE || SPlayer->Player.SoundBuffer.DSBuffer == NULL ) continue ;

				// �T�E���h�o�b�t�@�̍Đ���Ԃ��L���ŁA���Đ����Ă����ꍇ�͍Đ����ĊJ����
				if( SPlayer->Player.SoundBufferPlayStateBackupFlagValid &&
					SPlayer->Player.SoundBufferPlayStateBackupFlag )
				{
					SoundBuffer_Play( &SPlayer->Player.SoundBuffer, SPlayer->Player.SoundBuffer.Loop ) ;
				}
			}

			SPlayer->Player.SoundBufferPlayStateBackupFlagValid = FALSE ;
		}
	}

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�Ő��䂷��S�ẴT�E���h�f�[�^�v���C���[�̒���������s��
extern	int ST_SoftSoundPlayerProcessAll( void )
{
	HANDLELIST *List ;
	SOFTSOUND * SPlayer ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	for( List = DSOUND.SoftSoundPlayerListFirst.Next ; List->Next != NULL ; List = List->Next )
	{
		SPlayer = ( SOFTSOUND * )List->Data ;

		// �Đ����ł͂Ȃ��ꍇ�͉�����������
		if( SPlayer->Player.IsPlayFlag == FALSE ) continue ;

		_SoftSoundPlayerProcess( SPlayer ) ;
	}

	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�Ő��䂷��T�E���h�f�[�^�v���C���[�̒������
static int _SoftSoundPlayerProcess( SOFTSOUND * SPlayer )
{
	int WriteSize, NoneWriteSize, WriteStartPos ;
	DWORD PlayPos, WritePos ;
	HRESULT hr ;
	int Result = -1 ;
	DWORD MoveSize, MoveTempSize ;
	DWORD C, S, P, N ;
	SOUNDBUFFERLOCKDATA LockData ;

	if( DSOUND.InitializeFlag == FALSE ) return -1 ;

	// �Đ���Ԃł͂Ȃ��ꍇ�͉������Ȃ�
	if( SPlayer->Player.IsPlayFlag == FALSE ) return 0 ;

	// �N���e�B�J���Z�N�V�����̎擾
	CRITICALSECTION_LOCK( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �Đ��ʒu�̎擾
	SoundBuffer_GetCurrentPosition( &SPlayer->Player.SoundBuffer, &PlayPos, &WritePos ) ;

	// �����f�[�^�Đ��`�F�b�N
	if( SPlayer->Player.NoneDataSetCompOffset != -1 && SPlayer->Player.NoneDataPlayStartFlag == FALSE )
	{
		P = ( DWORD )SPlayer->Player.NoneDataPlayCheckBackPlayOffset ;
		N = PlayPos ;
		if( ( N > P && ( P <= ( DWORD )SPlayer->Player.NoneDataSetCompOffset && N >= ( DWORD )SPlayer->Player.NoneDataSetCompOffset ) ) ||
			( N < P && ( P <= ( DWORD )SPlayer->Player.NoneDataSetCompOffset || N >= ( DWORD )SPlayer->Player.NoneDataSetCompOffset ) ) )
		{
			SPlayer->Player.NoneDataPlayStartFlag = TRUE ;
		}
		else
		{
			SPlayer->Player.NoneDataPlayCheckBackPlayOffset = ( int )N ;
		}
	}

	// �]������K�v�����邩�ǂ����𒲂ׂ�
	WriteStartPos = ( int )SPlayer->Player.DataSetCompOffset - ( int )SPlayer->Player.MinDataSetSize ;
	if( WriteStartPos < 0 )
	{
		WriteStartPos += SPlayer->Player.SoundBufferSize ;
	}
	C = ( DWORD )SPlayer->Player.DataSetCompOffset ;
	S = ( DWORD )WriteStartPos ;
	if( ( S > C && ( S > PlayPos && C < PlayPos ) ) ||
		( S < C && ( S > PlayPos || C < PlayPos ) ) )
	{
		Result = 0 ;
		goto END ;
	}

	// �]������T�C�Y���Z�o����
	WriteSize = 0 ;
	if( S > C )
	{
		if( S < PlayPos )
		{
			WriteSize = ( int )( PlayPos - S ) ;
		}
		else
		{
			WriteSize = ( int )( ( SPlayer->Player.SoundBufferSize - S ) + PlayPos ) ;
		}
	}
	else
	{
		WriteSize = ( int )( PlayPos - S ) ;
	}
	WriteSize += SPlayer->Player.MinDataSetSize ;
	WriteSize /= SPlayer->BufferFormat.nBlockAlign ;
	if( WriteSize < 0 )
	{
		Result = 0 ;
		goto END ;
	}

	// �T�E���h�o�b�t�@�ɓ]������L���f�[�^�̃T�C�Y�Ɩ����f�[�^�̃T�C�Y���Z�o
	NoneWriteSize = 0 ;
	if( WriteSize > SPlayer->Player.StockSampleNum )
	{
		if( SPlayer->Player.StockSampleNum <= 0 )
		{
			NoneWriteSize = WriteSize ;
			WriteSize = 0 ;
		}
		else
		{
			WriteSize = SPlayer->Player.StockSampleNum ;
		}
	}

	// �T�E���h�o�b�t�@�����b�N����
	MoveSize = ( DWORD )( ( NoneWriteSize + WriteSize ) * SPlayer->BufferFormat.nBlockAlign ) ;
	hr = SoundBuffer_Lock(
		&SPlayer->Player.SoundBuffer, ( DWORD )SPlayer->Player.DataSetCompOffset,
		MoveSize,
		( void ** )&LockData.WriteP,  &LockData.Length,
		( void ** )&LockData.WriteP2, &LockData.Length2 ) ;
	if( hr != D_DS_OK )
		goto END ;

	// �����f�[�^���Z�b�g����ꍇ�͖����f�[�^�̃Z�b�g���J�n�����I�t�Z�b�g��ۑ�����
	if( NoneWriteSize != 0 && SPlayer->Player.NoneDataSetCompOffset == -1 )
	{
		SPlayer->Player.NoneDataSetCompOffset = SPlayer->Player.DataSetCompOffset + WriteSize * SPlayer->BufferFormat.nBlockAlign ;
		if( SPlayer->Player.NoneDataSetCompOffset > SPlayer->Player.SoundBufferSize )
		{
			SPlayer->Player.NoneDataSetCompOffset -= SPlayer->Player.SoundBufferSize ;
		}
		SPlayer->Player.NoneDataPlayCheckBackPlayOffset = ( int )PlayPos ;
		SPlayer->Player.NoneDataPlayStartFlag           = FALSE ;
	}

	// �����f�[�^�Đ����ɗL���f�[�^��]������ꍇ�͖����f�[�^�Đ��t���O��|��
	if( SPlayer->Player.NoneDataPlayStartFlag && WriteSize != 0 )
	{
		SPlayer->Player.NoneDataPlayStartFlag = FALSE ;
		SPlayer->Player.NoneDataPlayCheckBackPlayOffset = 0 ;
		SPlayer->Player.NoneDataSetCompOffset = -1 ;
	}

	// �c�T���v���������炷
	SPlayer->Player.StockSampleNum -= WriteSize ;

	// ���b�N�����Z�b�g
	LockData.StartOffst = ( DWORD )SPlayer->Player.DataSetCompOffset ;
	LockData.Offset     = 0 ;
	LockData.Offset2    = 0 ;
	LockData.Valid      = LockData.Length ;
	LockData.Valid2     = LockData.Length2 ;

	// �L���f�[�^��]��
	if( LockData.Valid != 0 && WriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( WriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid )
			MoveTempSize = LockData.Valid ;

		RingBufDataGet( &SPlayer->Player.StockSample, LockData.WriteP + LockData.Offset, ( int )MoveTempSize, FALSE ) ;
		LockData.Offset += MoveTempSize ;
		LockData.Valid -= MoveTempSize ;
		WriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}
	if( LockData.Valid2 != 0 && WriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( WriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid2 )
			MoveTempSize = LockData.Valid2 ;

		RingBufDataGet( &SPlayer->Player.StockSample, LockData.WriteP2 + LockData.Offset2, ( int )MoveTempSize, FALSE ) ;
		LockData.Offset2 += MoveTempSize ;
		LockData.Valid2 -= MoveTempSize ;
		WriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}

	// �����f�[�^��]��
	if( LockData.Valid != 0 && NoneWriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( NoneWriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid )
			MoveTempSize = LockData.Valid ;

		switch( SPlayer->BufferFormat.wBitsPerSample )
		{
		case 8  : _MEMSET( LockData.WriteP + LockData.Offset, 127, MoveTempSize ) ; break ;
		case 16 : _MEMSET( LockData.WriteP + LockData.Offset,   0, MoveTempSize ) ; break ;
		}

		LockData.Offset += MoveTempSize ;
		LockData.Valid -= MoveTempSize ;
		NoneWriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}
	if( LockData.Valid2 != 0 && NoneWriteSize != 0 )
	{
		MoveTempSize = ( DWORD )( NoneWriteSize * SPlayer->BufferFormat.nBlockAlign ) ;
		if( MoveTempSize > LockData.Valid2 )
			MoveTempSize = LockData.Valid2 ;

		switch( SPlayer->BufferFormat.wBitsPerSample )
		{
		case 8  : _MEMSET( LockData.WriteP2 + LockData.Offset2, 127, MoveTempSize ) ; break ;
		case 16 : _MEMSET( LockData.WriteP2 + LockData.Offset2,   0, MoveTempSize ) ; break ;
		}

		LockData.Offset2 += MoveTempSize ;
		LockData.Valid2 -= MoveTempSize ;
		NoneWriteSize -= MoveTempSize / SPlayer->BufferFormat.nBlockAlign ;
	}

	// �T�E���h�o�b�t�@�̃��b�N����������
	SoundBuffer_Unlock(
		&SPlayer->Player.SoundBuffer,
		LockData.WriteP,  LockData.Length,
		LockData.WriteP2, LockData.Length2 ) ;

	// �f�[�^�Z�b�g�����������ʒu��ύX����
	SPlayer->Player.DataSetCompOffset += MoveSize ;
	if( SPlayer->Player.DataSetCompOffset >= SPlayer->Player.SoundBufferSize )
		SPlayer->Player.DataSetCompOffset -= SPlayer->Player.SoundBufferSize ;

	Result = 0 ;
END :

	// �N���e�B�J���Z�N�V�����̉��
	CriticalSection_Unlock( &HandleManageArray[ DX_HANDLETYPE_SOFTSOUND ].CriticalSection ) ;

	// �I��
	return Result ;
}





































// �l�h�c�h����֐�

// �l�h�c�h�n���h����������������֐�
extern int InitializeMidiHandle( HANDLEINFO *HandleInfo )
{
	MIDIHANDLEDATA *MusicData = ( MIDIHANDLEDATA * )HandleInfo ;

	// ���ʂ��Z�b�g
	MusicData->Volume = 255 ;

	// �I��
	return 0 ;
}

// �l�h�c�h�n���h���̌�n�����s���֐�
extern int TerminateMidiHandle( HANDLEINFO *HandleInfo )
{
	MIDIHANDLEDATA *MusicData = ( MIDIHANDLEDATA * )HandleInfo ;

	// ���̍Đ����~�߂�
	if( NS_CheckMusicMem( HandleInfo->Handle ) == TRUE )
		NS_StopMusicMem( HandleInfo->Handle ) ;

	// DirectMusicSegment8 �I�u�W�F�N�g�̉��
	if( MusicData->DirectMusicSegmentObject != NULL )
	{
		// ���F�f�[�^�̉��
		MusicData->DirectMusicSegmentObject->Unload( DSOUND.DirectMusicPerformanceObject ) ;

		// �I�u�W�F�N�g�̉��
		DSOUND.DirectMusicLoaderObject->ReleaseObjectByUnknown( MusicData->DirectMusicSegmentObject ) ;
		MusicData->DirectMusicSegmentObject->Release() ;
		MusicData->DirectMusicSegmentObject = NULL ;
	}

	// �l�h�c�h�f�[�^�̉��
	if( MusicData->DataImage != NULL )
	{
		_MEMSET( MusicData->DataImage, 0, ( size_t )MusicData->DataSize );
		DXFREE( MusicData->DataImage ) ;
		MusicData->DataImage = NULL ;
	}

	// �I��
	return 0 ;
}

// �V�����l�h�c�h�n���h�����擾����
extern int NS_AddMusicData( void )
{
	int NewHandle ;

	NewHandle = AddHandle( DX_HANDLETYPE_MUSIC, FALSE, -1 ) ;

	return NewHandle ;
}

// �l�h�c�h�n���h�����폜����
extern int NS_DeleteMusicMem( int MusicHandle )
{
	return SubHandle( MusicHandle ) ;
}

// LoadMusicMemByMemImage �̎������֐�
static int LoadMusicMemByMemImage_Static(
	int MusicHandle,
	const void *FileImage,
	int FileImageSize,
	int ASyncThread
)
{
	MIDIHANDLEDATA *MusicData ;

	if( ASyncThread )
	{
		if( MIDI_MASKHCHK_ASYNC( MusicHandle, MusicData ) )
			return -1 ;
	}
	else
	{
		if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
			return -1 ;
	}

	// �C���[�W�̃R�s�[���쐬
	MusicData->DataImage = DXALLOC( ( size_t )FileImageSize ) ;
	if( MusicData->DataImage == NULL )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\xdf\x30\xe5\x30\xfc\x30\xb8\x30\xc3\x30\xaf\x30\xc7\x30\xfc\x30\xbf\x30\x92\x30\x00\x4e\x42\x66\x84\x76\x6b\x30\xdd\x4f\x58\x5b\x57\x30\x66\x30\x4a\x30\x4f\x30\xe1\x30\xe2\x30\xea\x30\x18\x98\xdf\x57\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�~���[�W�b�N�f�[�^���ꎞ�I�ɕۑ����Ă����������̈�̊m�ۂɎ��s���܂���\n" @*/ ) ;
		return -1 ;
	}
	_MEMCPY( MusicData->DataImage, FileImage, ( size_t )FileImageSize ) ;
	MusicData->DataSize = FileImageSize ;

	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		break ;

	case DX_MIDIMODE_DM :
		// DirectMusic ���g�p����ꍇ�̓��������� DirectMusicSegment8 ���쐬����
		{
			D_DMUS_OBJECTDESC ObjDesc ;

			_MEMSET( &ObjDesc, 0, sizeof( ObjDesc ) ) ;
			ObjDesc.dwSize = sizeof( D_DMUS_OBJECTDESC ) ;
			ObjDesc.dwValidData = D_DMUS_OBJ_MEMORY | D_DMUS_OBJ_CLASS ;
			ObjDesc.guidClass = CLSID_DIRECTMUSICSEGMENT ;
			ObjDesc.pbMemData = (BYTE *)MusicData->DataImage ;
			ObjDesc.llMemLength = MusicData->DataSize ;

			DSOUND.DirectMusicLoaderObject->ClearCache( IID_IDIRECTMUSICSEGMENT8 );
//			if( DSOUND.DirectMusicLoaderObject->GetObject( &ObjDesc, IID_IDirectMusicSegment8, ( void ** )&MusicData->DirectMusicSegmentObject ) != S_OK )
			if( DSOUND.DirectMusicLoaderObject->GetObject( &ObjDesc, IID_IDIRECTMUSICSEGMENT8, ( void ** )&MusicData->DirectMusicSegmentObject ) != S_OK )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\xdf\x30\xe5\x30\xfc\x30\xb8\x30\xc3\x30\xaf\x30\xc7\x30\xfc\x30\xbf\x30\x4b\x30\x89\x30\x20\x00\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x4d\x00\x75\x00\x73\x00\x69\x00\x63\x00\x53\x00\x65\x00\x67\x00\x6d\x00\x65\x00\x6e\x00\x74\x00\x38\x00\x20\x00\x92\x30\xd6\x53\x97\x5f\x59\x30\x8b\x30\xe6\x51\x06\x74\x4c\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�~���[�W�b�N�f�[�^���� DirectMusicSegment8 ���擾���鏈�������s���܂���\n" @*/ ) ;
				return -1 ;
			}

			// �f�[�^�`���� MIDI �ɃZ�b�g
			MusicData->DirectMusicSegmentObject->SetParam( GUID_STANDARDMIDIFILE, 0xFFFFFFFF, 0, 0, NULL);

			// ���F�f�[�^�̃_�E�����[�h
			MusicData->DirectMusicSegmentObject->Download( DSOUND.DirectMusicPerformanceObject ) ;
		}
		break ;
	}

	// �Đ����t���O��|��
	MusicData->PlayFlag = FALSE ;

	// �I��
	return 0 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadMusicMemByMemImage �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadMusicMemByMemImage_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MusicHandle ;
	const void *FileImage ;
	int FileImageSize ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MusicHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileImage = GetASyncLoadParamVoidP( AParam->Data, &Addr ) ;
	FileImageSize = GetASyncLoadParamInt( AParam->Data, &Addr ) ;

	Result = LoadMusicMemByMemImage_Static( MusicHandle, FileImage, FileImageSize, TRUE ) ;

	DecASyncLoadCount( MusicHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MusicHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadMusicMemByMemImage �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int LoadMusicMemByMemImage_UseGParam(
	const void *FileImage,
	int FileImageSize,
	int ASyncLoadFlag
)
{
	int MusicHandle ;

	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// �n���h���̍쐬
	MusicHandle = AddHandle( DX_HANDLETYPE_MUSIC, FALSE, -1 ) ;
	if( MusicHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, MusicHandle ) ;
		AddASyncLoadParamConstVoidP( NULL, &Addr, FileImage ) ;
		AddASyncLoadParamInt( NULL, &Addr, FileImageSize ) ;

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadMusicMemByMemImage_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MusicHandle ) ;
		AddASyncLoadParamConstVoidP( AParam->Data, &Addr, FileImage ) ;
		AddASyncLoadParamInt( AParam->Data, &Addr, FileImageSize ) ;

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( MusicHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadMusicMemByMemImage_Static( MusicHandle, FileImage, FileImageSize, FALSE ) < 0 )
			goto ERR ;
	}

	// �I��
	return MusicHandle ;

ERR :
	SubHandle( MusicHandle ) ;

	return -1 ;
}

// ��������ɓW�J���ꂽ�l�h�c�h�t�@�C����ǂݍ���
extern int NS_LoadMusicMemByMemImage( const void *FileImageBuffer, int FileImageSize )
{
	return LoadMusicMemByMemImage_UseGParam( FileImageBuffer, FileImageSize, GetASyncLoadFlag() ) ;
}

// LoadMusicMem �̎������֐�
static int LoadMusicMem_Static(
	int MusicHandle,
	const wchar_t *FileName,
	int ASyncThread
)
{
	MIDIHANDLEDATA * MusicData ;
	DWORD_PTR fp = 0 ;
	void *Buffer = NULL ;
	size_t FileSize ;
	int Ret ;

	if( ASyncThread )
	{
		if( MIDI_MASKHCHK_ASYNC( MusicHandle, MusicData ) )
			return -1 ;
	}
	else
	{
		if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
			return -1 ;
	}

	// �t�@�C���̓ǂݍ���
	{
		fp = FOPEN( FileName ) ;
		if( fp == 0 )
		{
			DXST_ERRORLOGFMT_ADDW(( L"Music File Open Error : %s", FileName )) ;
			goto ERR ;
		}
		FSEEK( fp, 0L, SEEK_END ) ;
		FileSize = ( size_t )FTELL( fp ) ;
		FSEEK( fp, 0L, SEEK_SET ) ;
		Buffer = DXALLOC( FileSize ) ;
		if( Buffer == NULL )
		{
			DXST_ERRORLOGFMT_ADDW(( L"Music File Memory Alloc Error : %s", FileName )) ;
			goto ERR ;
		}
		FREAD( Buffer, FileSize, 1, fp ) ;
		FCLOSE( fp ) ;
		fp = 0 ;
	}

	// �n���h���̃Z�b�g�A�b�v
	Ret = LoadMusicMemByMemImage_Static( MusicHandle, Buffer, ( int )FileSize, ASyncThread ) ;

	// �������̉��
	DXFREE( Buffer ) ;

	// ���ʂ�Ԃ�
	return Ret ;

ERR :
	if( fp != 0 ) FCLOSE( fp ) ;
	if( Buffer != NULL ) DXFREE( Buffer ) ;

	return -1 ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadMusicMem �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadMusicMem_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MusicHandle ;
	const wchar_t *FileName ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MusicHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	FileName = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadMusicMem_Static( MusicHandle, FileName, TRUE ) ;

	DecASyncLoadCount( MusicHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MusicHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadMusicMem �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int LoadMusicMem_UseGParam(
	const wchar_t *FileName,
	int ASyncLoadFlag
)
{
	int MusicHandle ;

	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// �n���h���̍쐬
	MusicHandle = AddHandle( DX_HANDLETYPE_MUSIC, FALSE, -1 ) ;
	if( MusicHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;
		wchar_t FullPath[ 1024 ] ;

		ConvertFullPathW_( FileName, FullPath ) ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, MusicHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, FullPath ) ;

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadMusicMem_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MusicHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, FullPath ) ;

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( MusicHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadMusicMem_Static( MusicHandle, FileName, FALSE ) < 0 )
			goto ERR ;
	}

	// �I��
	return MusicHandle ;

ERR :
	SubHandle( MusicHandle ) ;

	return -1 ;
}

// �l�h�c�h�t�@�C����ǂݍ���
extern int NS_LoadMusicMem( const TCHAR *FileName )
{
#ifdef UNICODE
	return LoadMusicMem_WCHAR_T(
		FileName
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = LoadMusicMem_WCHAR_T(
		UseFileNameBuffer
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}

// �l�h�c�h�t�@�C����ǂݍ���
extern int LoadMusicMem_WCHAR_T( const wchar_t *FileName )
{
	return LoadMusicMem_UseGParam( FileName, GetASyncLoadFlag() ) ;
}

// LoadMusicMemByResource �̎������֐�
static int LoadMusicMemByResource_Static(
	int MusicHandle,
	const wchar_t *ResourceName,
	const wchar_t *ResourceType,
	int ASyncThread
)
{
	MIDIHANDLEDATA * MusicData ;
	void *Image ;
	int ImageSize ;

	if( ASyncThread )
	{
		if( MIDI_MASKHCHK_ASYNC( MusicHandle, MusicData ) )
			return -1 ;
	}
	else
	{
		if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
			return -1 ;
	}

	// ���\�[�X�̏����擾
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 )
	{
		return -1 ;
	}

	// �n���h���̃Z�b�g�A�b�v
	return LoadMusicMemByMemImage_Static( MusicHandle, Image, ImageSize, ASyncThread ) ;
}

#ifndef DX_NON_ASYNCLOAD
// LoadMusicMemByResource �̔񓯊��ǂݍ��݃X���b�h����Ă΂��֐�
static void LoadMusicMemByResource_ASync( ASYNCLOADDATA_COMMON *AParam )
{
	int MusicHandle ;
	const wchar_t *ResourceName ;
	const wchar_t *ResourceType ;
	int Addr ;
	int Result ;

	Addr = 0 ;
	MusicHandle = GetASyncLoadParamInt( AParam->Data, &Addr ) ;
	ResourceName = GetASyncLoadParamString( AParam->Data, &Addr ) ;
	ResourceType = GetASyncLoadParamString( AParam->Data, &Addr ) ;

	Result = LoadMusicMemByResource_Static( MusicHandle, ResourceName, ResourceType, TRUE ) ;

	DecASyncLoadCount( MusicHandle ) ;
	if( Result < 0 )
	{
		SubHandle( MusicHandle ) ;
	}
}
#endif // DX_NON_ASYNCLOAD

// LoadMusicMemByResource �̃O���[�o���ϐ��ɃA�N�Z�X���Ȃ��o�[�W����
extern int LoadMusicMemByResource_UseGParam(
	const wchar_t *ResourceName,
	const wchar_t *ResourceType,
	int ASyncLoadFlag
)
{
	int MusicHandle ;

	if( DSOUND.InitializeFlag == FALSE )
		return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// �n���h���̍쐬
	MusicHandle = AddHandle( DX_HANDLETYPE_MUSIC, FALSE, -1 ) ;
	if( MusicHandle == -1 ) return -1 ;

#ifndef DX_NON_ASYNCLOAD
	if( ASyncLoadFlag )
	{
		ASYNCLOADDATA_COMMON *AParam = NULL ;
		int Addr ;

		// �p�����[�^�ɕK�v�ȃ������̃T�C�Y���Z�o
		Addr = 0 ;
		AddASyncLoadParamInt( NULL, &Addr, MusicHandle ) ;
		AddASyncLoadParamString( NULL, &Addr, ResourceName ) ;
		AddASyncLoadParamString( NULL, &Addr, ResourceType ) ;

		// �������̊m��
		AParam = AllocASyncLoadDataMemory( Addr ) ;
		if( AParam == NULL )
			goto ERR ;

		// �����ɕK�v�ȏ����Z�b�g
		AParam->ProcessFunction = LoadMusicMemByResource_ASync ;
		Addr = 0 ;
		AddASyncLoadParamInt( AParam->Data, &Addr, MusicHandle ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, ResourceName ) ;
		AddASyncLoadParamString( AParam->Data, &Addr, ResourceType ) ;

		// �f�[�^��ǉ�
		if( AddASyncLoadData( AParam ) < 0 )
		{
			DXFREE( AParam ) ;
			AParam = NULL ;
			goto ERR ;
		}

		// �񓯊��ǂݍ��݃J�E���g���C���N�������g
		IncASyncLoadCount( MusicHandle, AParam->Index ) ;
	}
	else
#endif // DX_NON_ASYNCLOAD
	{
		if( LoadMusicMemByResource_Static( MusicHandle, ResourceName, ResourceType, FALSE ) < 0 )
			goto ERR ;
	}

	// �I��
	return MusicHandle ;

ERR :
	SubHandle( MusicHandle ) ;

	return -1 ;
}

// ���\�[�X��̂l�h�c�h�t�@�C����ǂݍ���
extern int NS_LoadMusicMemByResource( const TCHAR *ResourceName, const TCHAR *ResourceType )
{
#ifdef UNICODE
	return LoadMusicMemByResource_WCHAR_T(
		ResourceName, ResourceType
	) ;
#else
	int Result = -1 ;

	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )

	Result = LoadMusicMemByResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer
	) ;

ERR :

	TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_END( ResourceType )

	return Result ;
#endif
}

// ���\�[�X��̂l�h�c�h�t�@�C����ǂݍ���
extern int LoadMusicMemByResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType )
{
	return LoadMusicMemByResource_UseGParam( ResourceName, ResourceType, GetASyncLoadFlag() ) ;
}

// �ǂݍ��񂾂l�h�c�h�f�[�^�̉��t���J�n����
extern int NS_PlayMusicMem( int MusicHandle, int PlayType )
{
	MIDIHANDLEDATA * MusicData ;
	int Result ;
	MCI_OPEN_PARMSW		mciOpenParms;
	MCI_PLAY_PARMS		mciPlayParms;
	MCI_STATUS_PARMS	mciStatusParms;
	HANDLE FileHandle ;
	DWORD WriteSize ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// �G���[����
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// ���t���~����
	NS_StopMusicMem( MIDI.PlayHandle ) ;

	Result = 0 ;
	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		// �ȑO�̃e���|�����t�@�C�����c���Ă��邩������Ȃ��̂ŁA�ꉞ�폜
		DeleteFileW( MIDI.FileName ) ;

		// �e���|�����t�@�C�����J��
		FileHandle = CreateTemporaryFile( MIDI.FileName ) ;
		if( FileHandle == NULL ) return -1 ;

		// �e���|�����t�@�C���Ƀf�[�^����������
		WriteFile( FileHandle, MusicData->DataImage, ( DWORD )MusicData->DataSize, &WriteSize, NULL ) ;
		CloseHandle( FileHandle ) ;

		// �I�[�v���X�e�[�^�X�Z�b�g
		mciOpenParms.lpstrElementName = MIDI.FileName ;
		mciOpenParms.lpstrDeviceType  = L"sequencer" ;

		// �l�b�h�̃I�[�v��
		Result = ( int )WinAPIData.Win32Func.mciSendCommandFunc( 0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, ( DWORD_PTR )( LPVOID )&mciOpenParms ) ;
		if( Result != 0 )
		{
			LOADSOUND_GPARAM GParam ;
			int OldHandle ;

//			DXST_ERRORLOG_ADDUTF16LE( L"�l�b�h�̃I�[�v���Ɏ��s���܂���\n" ) ;

			// �G���[���N������ꎞ�t�@�C�����폜
			DeleteFileW( MIDI.FileName ) ;

			// ���t�Ɏ��s�����畁�ʂ̃T�E���h�t�@�C���̉\��������
			OldHandle = MIDI.DefaultHandle ;
			InitLoadSoundGParam( &GParam ) ;
			GParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
			MIDI.DefaultHandle = LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, MusicData->DataImage, MusicData->DataSize, 1, -1, 0 ) ;
			if( MIDI.DefaultHandle == -1 )
			{
				// ����ł����s������t�@�C�����Ȃ��Ƃ�������
				MIDI.DefaultHandle = 0;
				return -1 ;
			}

			// ���ʐݒ�
			NS_ChangeVolumeSoundMem( MusicData->Volume, MIDI.DefaultHandle ) ;

			// ���܂ł̃n���h���͍폜
			if( OldHandle != 0 )
			{
				if( MIDI.DefaultHandleToSoundHandleFlag == TRUE )	NS_DeleteSoundMem( OldHandle ) ;
				else												NS_DeleteMusicMem( OldHandle ) ;
				OldHandle = 0 ;
			}
			MIDI.DefaultHandleToSoundHandleFlag = TRUE ;

			// �Đ��J�n
			NS_PlaySoundMem( MIDI.DefaultHandle, PlayType ) ;
			return 0 ;
		}

		// �l�h�c�h�̂h�c��ۑ�
		MIDI.MidiID = mciOpenParms.wDeviceID ;

		// �l�h�c�h�}�b�p�[������
		mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT ;
		if( WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID , MCI_STATUS , MCI_STATUS_ITEM , ( DWORD_PTR )( LPVOID )&mciStatusParms ) )
		{
			WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID , MCI_CLOSE , 0 , 0 ) ;
			DXST_ERRORLOG_ADDUTF16LE( "\x2d\xff\x29\xff\x24\xff\x29\xff\x14\x6f\x4f\x59\x8b\x95\xcb\x59\xe6\x51\x06\x74\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x77\x8d\x4d\x30\x7e\x30\x57\x30\x5f\x30\x11\xff\x0a\x00\x00"/*@ L"�l�h�c�h���t�J�n�����ŃG���[���N���܂����P\n" @*/ ) ;
			goto MCI_ERROR ;
		}
		if( LOWORD( mciStatusParms.dwReturn ) != LOWORD( MIDI_MAPPER ) )
		{
			WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID , MCI_CLOSE , 0 , 0 ) ;
			DXST_ERRORLOG_ADDUTF16LE( "\x2d\xff\x29\xff\x24\xff\x29\xff\x14\x6f\x4f\x59\x8b\x95\xcb\x59\xe6\x51\x06\x74\x67\x30\xa8\x30\xe9\x30\xfc\x30\x4c\x30\x77\x8d\x4d\x30\x7e\x30\x57\x30\x5f\x30\x12\xff\x0a\x00\x00"/*@ L"�l�h�c�h���t�J�n�����ŃG���[���N���܂����Q\n" @*/ ) ;
			goto MCI_ERROR ;
		}

		// �R�[���o�b�N�Ώۂ����C���E�C���h�E�ɐݒ肵�ĉ��t�J�n
		mciPlayParms.dwCallback = ( DWORD_PTR )NS_GetMainWindowHandle() ;
		if( WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID , MCI_PLAY , MCI_NOTIFY , ( DWORD_PTR )( LPVOID )&mciPlayParms ) )
		{
			WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID , MCI_CLOSE , 0 , 0 ) ;
			DXST_ERRORLOG_ADDUTF16LE( "\x2d\xff\x29\xff\x24\xff\x29\xff\x14\x6f\x4f\x59\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�l�h�c�h���t�Ɏ��s���܂���\n" @*/ ) ;
			goto MCI_ERROR ;
		}

		// �e���|�����t�@�C������Đ����Ă��邱�Ƃ������t���O�𗧂Ă�
		MIDI.MemImagePlayFlag = TRUE ;
		break ;

	case DX_MIDIMODE_DM :
		{
			D_REFERENCE_TIME RTime ;
			HRESULT hr ;

			// ���[�v�ݒ�
			MusicData->DirectMusicSegmentObject->SetRepeats( PlayType == DX_PLAYTYPE_LOOP ? D_DMUS_SEG_REPEAT_INFINITE : 0 ) ;

			// ���t�J�n
			hr = DSOUND.DirectMusicPerformanceObject->PlaySegmentEx( 
				MusicData->DirectMusicSegmentObject,	// ���t����Z�O�����g�B
				NULL,									// �\���O�Ɏg�p����p�����[�^�B��������Ă��Ȃ��B
				NULL,									// �g�����W�V�����Ɋւ���p�����[�^�B
				D_DMUS_SEGF_REFTIME,					// �t���O�B
				0,										// �J�n�^�C���B0�͒����ɊJ�n�B
				NULL,									// �Z�O�����g��Ԃ��󂯎��|�C���^�B
				NULL,									// ��~����I�u�W�F�N�g�B
				NULL									// �f�t�H���g�łȂ��ꍇ�̓I�[�f�B�I�p�X�B
			) ;

			// ���t�J�n���Ԃ̕ۑ�
			DSOUND.DirectMusicPerformanceObject->GetTime( &RTime, NULL ) ;
			MusicData->StartTime = _DTOL( (double)RTime / 10000.0 ) ;
		}
		// �e���|�����t�@�C������Đ����Ă��邱�Ƃ������t���O��|��
		MIDI.MemImagePlayFlag = FALSE ;
		break ;
	}

	MIDI.PlayFlag			 = TRUE ;								// ���t�t���O�𗧂Ă�
	MIDI.PlayHandle			 = MusicHandle ;						// ���t���Ă���n���h���̍X�V
	MIDI.LoopFlag			 = PlayType == DX_PLAYTYPE_LOOP ;		// ���[�v�t���O���Z�b�g����

	MusicData->PlayFlag		 = TRUE ;								// ��Ԃ��Đ����ɂ���
	MusicData->PlayStartFlag = FALSE ;								// ���t���J�n���ꂽ���t���O��|��

	// ���t�I���܂ő҂w��̏ꍇ�͂����ő҂�
	if( PlayType == DX_PLAYTYPE_NORMAL )
	{
		// �l�b�h�Đ��̏ꍇ�͉��t���J�n�����܂ő҂�
//		if( DSOUND.SoundMode == DX_MIDIMODE_MCI )
//			while( NS_ProcessMessage() == 0 && CheckMusicMCI() == FALSE ){}

		// �Đ��I���܂ő҂�
		while( NS_ProcessMessage() == 0 && NS_CheckMusicMem( MusicHandle ) == TRUE ){}

		// �e���|�����t�@�C������Đ�����Ă����ꍇ�͍폜����
		if( MIDI.MemImagePlayFlag == TRUE )
		{
			DeleteFileW( MIDI.FileName ) ;
		}
		MIDI.MemImagePlayFlag = FALSE ;
	}

	// �I��
	return 0 ;

MCI_ERROR:
	// �G���[���N������ꎞ�t�@�C�����폜���ďI��
	DeleteFileW( MIDI.FileName ) ;

	// �G���[�I��
	return -1 ;
}

// �l�h�c�h�f�[�^�̉��t���~����
extern int NS_StopMusicMem( int MusicHandle )
{
	int i ;
	MIDIHANDLEDATA * MusicData ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// �T�E���h�n���h���̍Đ��̏ꍇ�̓T�E���h���~�߂�
	if( MIDI.DefaultHandle != 0 && MIDI.DefaultHandleToSoundHandleFlag == TRUE )
	{
		NS_StopSoundMem( MIDI.DefaultHandle ) ;
		NS_DeleteSoundMem( MIDI.DefaultHandle ) ;

		MIDI.DefaultHandle = 0 ;
		return 0 ;
	}

	// ���[�v�t���O��|��
	MIDI.LoopFlag = FALSE ;

	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		// ���t�I���֐����Ă�
		MidiCallBackProcess() ;

		// �E�G�C�g
		for( i = 0 ; i <= 4 ; i++ ) NS_ProcessMessage() ;
		break ;

	case DX_MIDIMODE_DM :
		// ���t���~����
		DSOUND.DirectMusicPerformanceObject->StopEx( MusicData->DirectMusicSegmentObject, 0, 0 ) ;
		break ;
	}

	// �e���|�����t�@�C������Đ�����Ă����ꍇ�͍폜����
	if( MIDI.MemImagePlayFlag == TRUE )
	{
		DeleteFileW( MIDI.FileName ) ;
		MIDI.MemImagePlayFlag = FALSE ;
	}

	// ��Ԃ��~���ɂ���
	MusicData->PlayFlag = FALSE ;

	// ���t���J�n���ꂽ���t���O��|��
	MusicData->PlayStartFlag = FALSE ;

	// �I��
	return 0 ;
}

// �l�h�c�h�f�[�^�����t�����ǂ������擾����( TRUE:���t��  FALSE:��~�� )
extern int NS_CheckMusicMem( int MusicHandle )
{
	MIDIHANDLEDATA * MusicData ;
	int Result = -1 ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// �T�E���h�n���h���̍Đ��̏ꍇ�̓T�E���h�̍Đ���Ԃ�Ԃ�
	if( MIDI.DefaultHandle != 0 && MIDI.DefaultHandleToSoundHandleFlag == TRUE )
	{
		return NS_CheckSoundMem( MIDI.DefaultHandle ) ;
	}

	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		Result = MIDI.PlayFlag ;
		break ;

	case DX_MIDIMODE_DM :
		Result = MusicData->PlayFlag ;
	}

	return Result ;
}

// �l�h�c�h�f�[�^�̍Đ����ʂ��Z�b�g����
extern int NS_SetVolumeMusicMem( int Volume, int MusicHandle )
{
	MIDIHANDLEDATA * MusicData ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	// ���ʂ��Z�b�g
	MusicData->Volume = Volume ;

	// �T�E���h�n���h���̍Đ��̏ꍇ�̓T�E���h�̉��ʂ�ύX����
	if( MIDI.DefaultHandle != 0 && MIDI.DefaultHandleToSoundHandleFlag == TRUE )
	{
		return NS_ChangeVolumeSoundMem( Volume, MIDI.DefaultHandle );
	}

	return NS_SetVolumeMusic( Volume )  ;
}

// �l�h�c�h�f�[�^�n���h�������ׂč폜����
extern int NS_InitMusicMem( void )
{
	return AllHandleSub( DX_HANDLETYPE_MUSIC ) ;
}

// �l�h�c�h�f�[�^�̎����I����
extern int NS_ProcessMusicMem( void )
{
	MIDIHANDLEDATA * MusicData ;
	int i, play ;

	if( DSOUND.DirectSoundObject == NULL )
		return -1 ;

	if( HandleManageArray[ DX_HANDLETYPE_MUSIC ].InitializeFlag == FALSE )
		return -1 ;

	for( i = HandleManageArray[ DX_HANDLETYPE_MUSIC ].AreaMin ; i <= HandleManageArray[ DX_HANDLETYPE_MUSIC ].AreaMax ; i ++ )
	{
		MusicData = ( MIDIHANDLEDATA * )HandleManageArray[ DX_HANDLETYPE_MUSIC ].Handle[ i ] ;
		if( MusicData == NULL ) continue ;

		if( MusicData->PlayFlag == FALSE ) continue ;

		switch( DSOUND.SoundMode )
		{
		case DX_MIDIMODE_MCI :
			break ;

		case DX_MIDIMODE_DM :
			play = DSOUND.DirectMusicPerformanceObject->IsPlaying( MusicData->DirectMusicSegmentObject , NULL ) != S_FALSE ;
			if( MusicData->PlayStartFlag == FALSE )
			{
				// �܂����t���n�܂��Ă��Ȃ������ꍇ�͏�Ԃ����t���ɂȂ������Ƃ�
				// ���߂ĉ��t���Ƃ������ƂɂȂ�
				if( play == TRUE ) MusicData->PlayStartFlag = TRUE ;
			}
			else
			{
				// ���t���n�܂������Ƃ͌��݂̏�Ԃ����̂܂ܔ��f�����
				MusicData->PlayFlag = play ;
			}
			break ;
		}
	}

	// �I��
	return 0;
}

// �l�h�c�h�f�[�^�̌��݂̍Đ��ʒu���擾����
extern int NS_GetMusicMemPosition( int MusicHandle )
{
	MIDIHANDLEDATA * MusicData ;
	int Result = -1 ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( MIDI_MASKHCHK( MusicHandle, MusicData ) )
		return -1 ;

	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		Result = NS_GetMusicPosition() ;
		break ;

	case DX_MIDIMODE_DM :
		{
			D_REFERENCE_TIME RTime ;
			D_MUSIC_TIME Time ;
			D_IDirectMusicSegmentState *State ;

			DSOUND.DirectMusicPerformanceObject->GetTime( &RTime, &Time ) ;
			if( DSOUND.DirectMusicPerformanceObject->GetSegmentState( &State, Time ) != S_OK )
				return -1 ;

			State->GetSeek( &Time ) ;
			State->Release() ;

			// ���Ԃ�Ԃ�
			Result = _DTOL( (double)RTime / 10000.0 ) - MusicData->StartTime ;
		}
		break ;
	}

	return Result ;
}



// �l�h�c�h�t�@�C�����Đ�����
extern int NS_PlayMusic( const TCHAR *FileName , int PlayType )
{
#ifdef UNICODE
	return PlayMusic_WCHAR_T(
		FileName, PlayType
	) ;
#else
	int Result ;

	TCHAR_TO_WCHAR_T_STRING_ONE_BEGIN( FileName, return -1 )

	Result = PlayMusic_WCHAR_T(
		UseFileNameBuffer, PlayType
	) ;

	TCHAR_TO_WCHAR_T_STRING_END( FileName )

	return Result ;
#endif
}


// �l�h�c�h�t�@�C�����Đ�����
extern int PlayMusic_WCHAR_T( const wchar_t *FileName , int PlayType )
{
	// �������t����������~�߂�
	if( MIDI.DefaultHandle != 0 )
	{
		if( MIDI.DefaultHandleToSoundHandleFlag == TRUE )	NS_DeleteSoundMem( MIDI.DefaultHandle ) ;
		else												NS_DeleteMusicMem( MIDI.DefaultHandle ) ;
		MIDI.DefaultHandle = 0 ;
	}

	// �ǂݍ���
	MIDI.DefaultHandle = LoadMusicMem_UseGParam( FileName, FALSE ) ;
	if( MIDI.DefaultHandle == -1 )
	{
		MIDI.DefaultHandle = 0;
		return -1 ;
	}
	MIDI.DefaultHandleToSoundHandleFlag = FALSE ;

	// ���t
	if( NS_PlayMusicMem( MIDI.DefaultHandle, PlayType ) == -1 )
	{
		LOADSOUND_GPARAM GParam ;

		// ���t�Ɏ��s�����畁�ʂ̃T�E���h�t�@�C���̉\��������
		NS_DeleteMusicMem( MIDI.DefaultHandle ) ;

		InitLoadSoundGParam( &GParam ) ;
		GParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMPRESS ;
		MIDI.DefaultHandle = LoadSoundMemBase_UseGParam( &GParam, FileName, 1, -1, FALSE, FALSE ) ;
		if( MIDI.DefaultHandle == -1 )
		{
			// ����ł����s������t�@�C�����Ȃ��Ƃ�������
			MIDI.DefaultHandle = 0;
			return -1 ;
		}

		// �Đ��J�n
		NS_PlaySoundMem( MIDI.DefaultHandle, PlayType ) ;
		MIDI.DefaultHandleToSoundHandleFlag = TRUE ;
	}

	return 0;
}





// ��������ɓW�J����Ă���l�h�c�h�t�@�C�������t����
extern int NS_PlayMusicByMemImage( const void *FileImageBuffer, int FileImageSize, int PlayType )
{
	// �������t����������~�߂�
	if( MIDI.DefaultHandle != 0 )
	{
		if( MIDI.DefaultHandleToSoundHandleFlag == TRUE )	NS_DeleteSoundMem( MIDI.DefaultHandle ) ;
		else												NS_DeleteMusicMem( MIDI.DefaultHandle ) ;
		MIDI.DefaultHandle = 0 ;
	}

	// �ǂݍ���
	MIDI.DefaultHandle = LoadMusicMemByMemImage_UseGParam( FileImageBuffer, FileImageSize, FALSE ) ;
	if( MIDI.DefaultHandle == -1 )
	{
		LOADSOUND_GPARAM GParam ;

		// �ǂݍ��݂Ɏ��s�����特���Ƃ��čĐ�����
		InitLoadSoundGParam( &GParam ) ;
		MIDI.DefaultHandle = LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, FileImageBuffer, FileImageSize, 1, -1, FALSE, FALSE ) ;
		if( MIDI.DefaultHandle == -1 )
		{
			// ����ł����s������f�[�^�����Ă���Ƃ�������
			MIDI.DefaultHandle = 0;
			return -1 ;
		}
		MIDI.DefaultHandleToSoundHandleFlag = TRUE ;
	}
	else
	{
		MIDI.DefaultHandleToSoundHandleFlag = FALSE ;
	}

	// ���t
	NS_PlayMusicMem( MIDI.DefaultHandle, PlayType ) ;

	return 0;
}

// ���\�[�X����l�h�c�h�t�@�C����ǂݍ���ŉ��t����
extern int NS_PlayMusicByResource( const TCHAR *ResourceName, const TCHAR *ResourceType, int PlayType )
{
#ifdef UNICODE
	return PlayMusicByResource_WCHAR_T(
		ResourceName, ResourceType, PlayType
	) ;
#else
	int Result = -1 ;

	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_BEGIN( ResourceType )

	TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceName, goto ERR )
	TCHAR_TO_WCHAR_T_STRING_SETUP( ResourceType, goto ERR )

	Result = PlayMusicByResource_WCHAR_T(
		UseResourceNameBuffer, UseResourceTypeBuffer, PlayType
	) ;

ERR :

	TCHAR_TO_WCHAR_T_STRING_END( ResourceName )
	TCHAR_TO_WCHAR_T_STRING_END( ResourceType )

	return Result ;
#endif
}

// ���\�[�X����l�h�c�h�t�@�C����ǂݍ���ŉ��t����
extern int PlayMusicByResource_WCHAR_T( const wchar_t *ResourceName, const wchar_t *ResourceType, int PlayType )
{
	void *Image ;
	int ImageSize ;

	// ���\�[�X�̏����擾
	if( GetResourceInfo_WCHAR_T( ResourceName, ResourceType, &Image, &ImageSize ) < 0 )
	{
		return -1 ;
	}

	// ���t�J�n
	return NS_PlayMusicByMemImage( Image, ImageSize, PlayType ) ;
}

// �l�h�c�h�t�@�C���̉��t��~
extern int NS_StopMusic( void )
{
	if( MIDI.DefaultHandle == 0 ) return 0 ;

	if( MIDI.DefaultHandleToSoundHandleFlag == TRUE )
	{
		NS_StopSoundMem( MIDI.DefaultHandle ) ;
		NS_DeleteSoundMem( MIDI.DefaultHandle ) ;
	}
	else
	{
		NS_StopMusicMem( MIDI.DefaultHandle ) ;
		NS_DeleteMusicMem( MIDI.DefaultHandle ) ;
	}

	MIDI.DefaultHandle = 0 ;

	return 0 ;
}



// �l�h�c�h�t�@�C�������t�����ۂ������擾����
extern int NS_CheckMusic( void )
{
	if( MIDI.DefaultHandle == 0 ) return 0 ;

	if( MIDI.DefaultHandleToSoundHandleFlag == TRUE )	return NS_CheckSoundMem( MIDI.DefaultHandle ) ;
	else												return NS_CheckMusicMem( MIDI.DefaultHandle ) ;
}

// �l�h�c�h���t�I�����Ă΂��R�[���o�b�N�֐�
extern int MidiCallBackProcess( void )
{
	MCI_PLAY_PARMS		mciPlayParms;

	// ���[�v�w�肪����ꍇ�Ăщ��t���J�n����
	if( MIDI.LoopFlag == TRUE )
	{
		mciPlayParms.dwCallback = (DWORD_PTR) NS_GetMainWindowHandle() ;
		mciPlayParms.dwFrom		= 0 ;
		WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID , MCI_PLAY, MCI_NOTIFY | MCI_FROM, (DWORD_PTR)(LPVOID) &mciPlayParms );

		MIDI.PlayFlag = TRUE ;
	}
	else 
	{
		// ���t���������ꍇ�͎~�߂�
		if( MIDI.PlayFlag == TRUE )
		{
			WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID , MCI_CLOSE, 0, 0 );
			MIDI.PlayFlag = FALSE;
		}
	}

	return 0 ;
}


// �l�h�c�h�̍Đ��`�����Z�b�g����
extern int NS_SelectMidiMode( int Mode )
{
	// �������[�h�����܂łƓ����ꍇ�͂Ȃɂ������I��
	if( Mode == DSOUND.SoundMode ) return 0 ;

	// �Đ����[�h���Z�b�g
	DSOUND.SoundMode = Mode ;

	if( DSOUND.DirectSoundObject == NULL ) return 0 ;

	// �I�u�W�F�N�g�����
	TerminateDirectSound() ;

	// ���Đ��n������
	return InitializeDirectSound() ;
}

// �l�h�c�h�̍Đ����ʂ��Z�b�g����
extern int NS_SetVolumeMusic( int Volume )
{
	long V ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	if( MIDI.DefaultHandleToSoundHandleFlag == FALSE )
	{
		switch( DSOUND.SoundMode )
		{
		case DX_MIDIMODE_MCI :
			// �l�b�h�̏ꍇ�͉��ʂ̕ύX�͏o���Ȃ�
			break ;

		case DX_MIDIMODE_DM :
			// ���ʂ̃Z�b�g
			V = _DTOL( ( double )( DSOUND.MaxVolume - DM_MIN_VOLUME ) / 256 * Volume ) + DM_MIN_VOLUME ;
			if( Volume == 255 ) V = DSOUND.MaxVolume ;
			DSOUND.DirectMusicPerformanceObject->SetGlobalParam( GUID_PERFMASTERVOLUME , &V , sizeof( long ) ) ;
			
			break ;
		}
	}
	else
	{
		NS_ChangeVolumeSoundMem( Volume, MIDI.DefaultHandle );
	}

	// �I��
	return 0 ;
}

// �l�h�c�h�̌��݂̍Đ��ʒu���擾����
extern int NS_GetMusicPosition( void )
{
	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �Đ����ł͂Ȃ������牽�����Ȃ�
	if( NS_CheckMusic() == FALSE ) return -1 ;

	switch( DSOUND.SoundMode )
	{
	case DX_MIDIMODE_MCI :
		{
			MCI_SET_PARMS mciSetParms ;
			MCI_STATUS_PARMS mciStatusParms ;

			// �擾���鎞�Ԃ̒P�ʂ��~���b�P�ʂɂ���
			_MEMSET( &mciSetParms, 0, sizeof( mciSetParms ) ) ;
			mciSetParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS ;
			if( WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&mciSetParms ) != 0 )
				return -1 ;

			// ���Ԃ��擾����
			_MEMSET( &mciStatusParms, 0, sizeof( mciStatusParms ) ) ;
			mciStatusParms.dwItem = MCI_STATUS_POSITION ;
			if( WinAPIData.Win32Func.mciSendCommandFunc( MIDI.MidiID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatusParms ) != 0 )
				return -1 ;

			// ���Ԃ�Ԃ�
			return ( int )mciStatusParms.dwReturn ;
		}
		break ;

	case DX_MIDIMODE_DM :
		{
			D_REFERENCE_TIME RTime ;
			D_MUSIC_TIME /*StartTime,*/ Time ;
			D_IDirectMusicSegmentState *State ;

			DSOUND.DirectMusicPerformanceObject->GetTime( &RTime, &Time ) ;
			if( DSOUND.DirectMusicPerformanceObject->GetSegmentState( &State, Time ) != S_OK )
				return -1 ;

			State->GetSeek( &Time ) ;
			State->Release() ;

//			DSOUND.DirectMusicPerformanceObject->MusicToReferenceTime( Time, &RTime ) ;

			// ���Ԃ�Ԃ�
//			return (int)Time ;
			return _DTOL( (double)RTime / 10000.0 ) - MIDI.StartTime ;
		}
		break ;
	}

	// �I��
	return 0 ;
}
	





// �⏕�n

// �����t�H�[�}�b�g�Ɣg�`�C���[�W����v�`�u�d�t�@�C���C���[�W���쐬����
extern	int CreateWaveFileImage( 	void **DestBufferP, int *DestBufferSizeP,
									const WAVEFORMATEX *Format, int FormatSize,
									const void *WaveData, int WaveDataSize )
{
	int BufferSize ;
	void *Buffer ;
	BYTE *p ;
	
	// �o�b�t�@���m��
	BufferSize = FormatSize + WaveDataSize
					+ 12/*"RIFF" + �t�@�C���T�C�Y + "WAVE"*/
					+ 8 * 2/*"fmt "�`�����N + "data"�`�����N*/ ;
	Buffer = DXALLOC( ( size_t )BufferSize ) ;
	if( Buffer == NULL )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\xee\x4e\x57\x00\x41\x00\x56\x00\x45\x00\xd5\x30\xa1\x30\xa4\x30\xeb\x30\xa4\x30\xe1\x30\xfc\x30\xb8\x30\x28\x75\x6e\x30\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"��WAVE�t�@�C���C���[�W�p�̃������m�ۂɎ��s���܂���\n" @*/ ) ;
		return -1 ;
	}
	
	// �f�[�^�̃Z�b�g
	p = (BYTE *)Buffer ;
	_MEMCPY( (char *)p, "RIFF", 4 ) ;					p += 4 ;
	*((DWORD *)p) = ( DWORD )( BufferSize - 8 ) ;		p += 4 ;
	_MEMCPY( (char *)p, "WAVE", 4 ) ;					p += 4 ;

	_MEMCPY( (char *)p, "fmt ", 4 ) ;					p += 4 ;
	*((DWORD *)p) = ( DWORD )FormatSize ;				p += 4 ;
	_MEMCPY( p, Format, ( size_t )FormatSize ) ;		p += FormatSize ;

	_MEMCPY( (char *)p, "data", 4 ) ;					p += 4 ;
	*((DWORD *)p) = ( DWORD )WaveDataSize ;				p += 4 ;
	_MEMCPY( p, WaveData, ( size_t )WaveDataSize ) ;	p += WaveDataSize ;

	// ����ۑ�
	*DestBufferP = Buffer ;
	*DestBufferSizeP = BufferSize ;

	// �I��
	return 0 ;
}

// �t�@�C�����ۂ܂郁�����ɓǂݍ���
static	int FileFullRead( const wchar_t *FileName, void **BufferP, int *SizeP )
{
	DWORD_PTR fp = 0 ;
	size_t Size ;
	void *Buffer ;
	
	fp = FOPEN( FileName ) ;
	if( fp == 0 )
	{
		DXST_ERRORLOGFMT_ADDW(( L"File Open Error : %s", FileName )) ;
		goto ERR ;
	}

	FSEEK( fp, 0, SEEK_END ) ;
	Size = ( size_t )FTELL( fp ) ;
	FSEEK( fp, 0, SEEK_SET ) ;
	
	Buffer = DXALLOC( Size ) ;
	if( Buffer == NULL )
	{
		DXST_ERRORLOGFMT_ADDW(( L"File Load : Memory Alloc Error : %s", FileName )) ;
		goto ERR ;
	}
	
	FREAD( Buffer, Size, 1, fp ) ;
	FCLOSE( fp ) ;
	
	*BufferP = Buffer ;
	*SizeP = ( int )Size ;
	
	// �I��
	return 0 ;
	
ERR :
	if( fp != 0 ) FCLOSE( fp ) ;
	
	return -1 ;
}

// �X�g���[���f�[�^���ۂ܂郁�����ɓǂݍ���
extern	int StreamFullRead( STREAMDATA *Stream, void **BufferP, int *SizeP )
{
	size_t Size ;
	void *Buffer ;
	
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_END ) ;
	Size = ( size_t )Stream->ReadShred.Tell( Stream->DataPoint ) ;
	Stream->ReadShred.Seek( Stream->DataPoint, 0, SEEK_SET ) ;
	
	Buffer = DXALLOC( Size ) ;
	if( Buffer == NULL )
	{
		return DXST_ERRORLOGFMT_ADDUTF16LE(( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x6b\x30\xc5\x5f\x81\x89\x6a\x30\xe1\x30\xe2\x30\xea\x30\x6e\x30\xba\x78\xdd\x4f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�X�g���[���̓ǂݍ��݂ɕK�v�ȃ������̊m�ۂɎ��s���܂���" @*/ )) ;
	}

	Stream->ReadShred.Read( Buffer, Size, 1, Stream->DataPoint ) ;
	
	*BufferP = Buffer ;
	*SizeP = ( int )Size ;
	
	// �I��
	return 0 ;
}

// �\�t�g�E�G�A�ň����g�`�f�[�^����T�E���h�n���h�����쐬����
extern int NS_LoadSoundMemFromSoftSound( int SoftSoundHandle, int BufferNum )
{
	SOFTSOUND * SSound ;
	int NewHandle ;
	void *WaveImage ;
	int WaveSize ;
	LOADSOUND_GPARAM GParam ;

	// �G���[����
	if( SSND_MASKHCHK( SoftSoundHandle, SSound ) ) return -1 ;
	if( SSound->IsPlayer == TRUE ) return -1 ;

	// �v�`�u�d�t�@�C�����ł����グ��
	if( CreateWaveFileImage( &WaveImage, &WaveSize,
						 &SSound->BufferFormat, sizeof( WAVEFORMATEX ),
						 SSound->Wave.Buffer, SSound->Wave.BufferSampleNum * SSound->BufferFormat.nBlockAlign ) < 0 )
		 return -1 ;

	// �n���h���̍쐬
	InitLoadSoundGParam( &GParam ) ;
	if( GParam.CreateSoundDataType == DX_SOUNDDATATYPE_FILE )
		GParam.CreateSoundDataType = DX_SOUNDDATATYPE_MEMNOPRESS ;
	NewHandle = LoadSoundMemByMemImageBase_UseGParam( &GParam, TRUE, -1, WaveImage, WaveSize, BufferNum, -1, FALSE, FALSE ) ;

	// �������̉��
	DXFREE( WaveImage ) ;
	WaveImage = NULL ;

	// �I��
	return NewHandle ;
}

// �T�E���h�n���h���̃^�C�v���X�g���[���n���h���ł͂Ȃ��ꍇ�ɃX�g���[���n���h���ɕύX����
static int SoundTypeChangeToStream( int SoundHandle )
{
	SOUND * sd ;
	void *WaveImage ;
	int WaveSize ;
	int Time, Volume, Frequency, Pan ;

	if( DSOUND.DirectSoundObject == NULL ) return -1 ;

	// �G���[����
	if( SOUNDHCHK( SoundHandle, sd ) )
		return -1 ;

	// ���ɃX�g���[���n���h���������牽�������ɏI��
	if( sd->Type == DX_SOUNDTYPE_STREAMSTYLE )
		return 0 ;

	// �v�`�u�d�t�@�C�����ł����グ��
	if( CreateWaveFileImage( &WaveImage, &WaveSize,
						 &sd->BufferFormat, sizeof( WAVEFORMATEX ),
						 sd->Normal.WaveData, sd->Normal.WaveSize ) < 0 )
		return -1 ;

	// �����n���h���̐ݒ���擾���Ă���
	Volume = NS_GetVolumeSoundMem( SoundHandle ) ;
	Time = NS_GetSoundCurrentPosition( SoundHandle ) ;
	Frequency = NS_GetFrequencySoundMem( SoundHandle ) ;
	Pan = NS_GetPanSoundMem( SoundHandle ) ;

	// �����n���h���̍폜
	{
		NS_StopSoundMem( SoundHandle ) ;					// �Đ����������Ƃ��̂��Ƃ��l���Ď~�߂Ă���
		NS_DeleteSoundMem( SoundHandle ) ;					// �n���h�����폜
	}

	// �X�g���[���Đ��`���̃n���h���Ƃ��č�蒼��
	{
		int f, res ;
		STREAMDATA Stream ;
		
		Stream.DataPoint = MemStreamOpen( WaveImage, ( unsigned int )WaveSize ) ;
		Stream.ReadShred = *GetMemStreamDataShredStruct() ;

		SoundHandle = AddHandle( DX_HANDLETYPE_SOUND, FALSE, SoundHandle ) ;
		res = NS_AddStreamSoundMem( &Stream, 0, SoundHandle, DX_SOUNDDATATYPE_MEMNOPRESS, &f ) ;
		if( res == -1 )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xb9\x30\xc8\x30\xea\x30\xfc\x30\xe0\x30\xb5\x30\xa6\x30\xf3\x30\xc9\x30\xcf\x30\xf3\x30\xc9\x30\xeb\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�X�g���[���T�E���h�n���h���̍쐬�Ɏ��s���܂���\n" @*/ ) ;
			return -1 ;
		}

		Stream.ReadShred.Close( Stream.DataPoint ) ;
	}
	
	// �������̉��
	DXFREE( WaveImage ) ;

	// ���̃n���h���̐ݒ���Z�b�g����
	NS_SetVolumeSoundMem( Volume, SoundHandle ) ;
	NS_SetSoundCurrentPosition( Time, SoundHandle ) ;
	NS_SetFrequencySoundMem( Frequency, SoundHandle ) ;
	NS_SetPanSoundMem( Pan, SoundHandle ) ;

	// ����I��
	return 0 ;
}

// �~���b�P�ʂ̐��l���T���v���P�ʂ̐��l�ɕϊ�����
static	int MilliSecPositionToSamplePosition( int SamplesPerSec, int MilliSecTime )
{
	LONGLONG TempValue1, TempValue2 ;
	unsigned int Temp[ 4 ] ;

	TempValue1 = SamplesPerSec ;
	TempValue2 = MilliSecTime ;
	_MUL128_1( ( DWORD * )&TempValue1, ( DWORD * )&TempValue2, ( DWORD * )Temp ) ;
	TempValue2 = 1000 ;
	_DIV128_1( ( DWORD * )Temp, ( DWORD * )&TempValue2, ( DWORD * )&TempValue1 ) ;
	return ( int )TempValue1 ;
}

// �T���v���P�ʂ̐��l���~���b�P�ʂ̐��l�ɕϊ�����
static	int SamplePositionToMilliSecPosition( int SamplesPerSec, int SampleTime )
{
	LONGLONG TempValue1, TempValue2 ;
	unsigned int Temp[ 4 ] ;

	TempValue1 = SampleTime ;
	TempValue2 = 1000 ;
	_MUL128_1( ( DWORD * )&TempValue1, ( DWORD * )&TempValue2, ( DWORD * )Temp ) ;
	TempValue2 = SamplesPerSec ;
	_DIV128_1( ( DWORD * )Temp, ( DWORD * )&TempValue2, ( DWORD * )&TempValue1 ) ;
	return ( int )TempValue1 ;
}

#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // DX_NON_SOUND







