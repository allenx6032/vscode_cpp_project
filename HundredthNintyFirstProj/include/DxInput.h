// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		���͏��v���O�����w�b�_�t�@�C��
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXINPUT_H__
#define __DXINPUT_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_INPUT

// �C���N���[�h ------------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxDirectX.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` --------------------------------------------------------------------

// ���[�^�[�̔ԍ���`
#define DINPUTPAD_MOTOR_LEFT		0
#define DINPUTPAD_MOTOR_RIGHT		1
#define DINPUTPAD_MOTOR_NUM			2

// �\���̒�` --------------------------------------------------------------------

// �p�b�h�̐U���̏��
struct DINPUTPAD_EFFECT
{
	D_IDirectInputEffect	*DIEffect ;							// �p�b�h�f�o�C�X�G�t�F�N�g�I�u�W�F�N�g
	int						PlayFlag ;							// �G�t�F�N�g���Đ������A�t���O
	int						Power ;								// �G�t�F�N�g�̋���
	int						Time ;								// �G�t�F�N�g�̒���
	int						CompTime ;							// �G�t�F�N�g���J�n���Ă���o�߂�������
	int						BackTime ;							// �G�t�F�N�g�̑O�񒲂ׂ�����
	int						PlayState ;							// DirectInputEffect �̍Đ����(�\�t�g����A�N�e�B�u�ɂȂ�΍Đ���~�����)
	int						SetPower ;							// DirectInputEffect �̐ݒ�p���[
} ;

// �p�b�h�̏��
struct DINPUTPAD
{
	int						XInputDeviceNo ;					// XInput�ŃA�N�Z�X����f�o�C�X�̏ꍇ�͂O�ȏ�̒l������( DirectInput �ň����ꍇ�� -1 )
	D_XINPUT_STATE			XInputState ;						// XInput�ł̃p�b�h�̏��

	D_IDirectInputDevice7	*Device ;							// �p�b�h�f�o�C�X�I�u�W�F�N�g
//	D_IDirectInputEffect	*LeftEffect ;						// �p�b�h�f�o�C�X�G�t�F�N�g�I�u�W�F�N�g
//	D_IDirectInputEffect	*RightEffect ;						// �p�b�h�f�o�C�X�G�t�F�N�g�I�u�W�F�N�g
	HANDLE					Event ;								// �p�b�h�̃C�x���g�n���h��
	D_DIJOYSTATE			State ;								// ���
	int						RightStickFlag ;					// �E�X�e�B�b�N�����邩�ǂ���
	int						GetTime ;							// �擾����
//	int						EffectNotCommonFlag ;				// ���E�̃��[�^�[�Ńp�����[�^�����L���Ȃ����ǂ����̃t���O
//	DINPUTPAD_EFFECT		EffectCommon ;						// ���E�̃��[�^�[�Ńp�����[�^�����L����ꍇ�̏��
	DINPUTPAD_EFFECT		Effect[ DINPUTPAD_MOTOR_NUM ] ;		// ���[�^�[�̏��
//	int						EffectPlayFlag ;					// �G�t�F�N�g���Đ������A�t���O
//	int						EffectPower ;						// �G�t�F�N�g�̋���
//	int						EffectTime ;						// �G�t�F�N�g�̒���
//	int						EffectCompTime ;					// �G�t�F�N�g���J�n���Ă���o�߂�������
//	int						EffectBackTime ;					// �G�t�F�N�g�̑O�񒲂ׂ�����
//	int						EffectPlayStateLeft ;				// DirectInputEffect ��ł́A������̍Đ����(�\�t�g����A�N�e�B�u�ɂȂ�΍Đ���~�����)
//	int						EffectPlayStateRight ;				// DirectInputEffect ��ł́A������̍Đ����(�\�t�g����A�N�e�B�u�ɂȂ�΍Đ���~�����)
	wchar_t					InstanceName[MAX_PATH];				// �f�o�C�X�̓o�^��
	wchar_t					ProductName[MAX_PATH];				// �f�o�C�X�̐��i�o�^��
	DWORD					DeadZone ;							// �����]�[��( 65536 = 100% )
} ;

// �c�����������h������������p�f�[�^�\���̌^
struct DINPUTDATA
{
	HMODULE					XInputDLL ;							// �w�h���������̂c�k�k

	DWORD					( WINAPI * XInputGetStateFunc )( DWORD dwUserIndex, D_XINPUT_STATE* pState ) ;
	DWORD					( WINAPI * XInputSetStateFunc )( DWORD dwUserIndex, D_XINPUT_VIBRATION* pVibration ) ;

	int						NoUseXInputFlag ;					// �w�h�����������g�p���Ȃ����ǂ����̃t���O
	int						NoUseDirectInputFlag ;				// �c�����������h�����������g�p���Ȃ����ǂ����̃t���O
	int						NoUseVibrationFlag ;				// �c�����������h���������̐U���@�\���g�p���Ȃ����ǂ����̃t���O
	int						UseDirectInputFlag ;				// �c�����������h�����������g�p���邩�ǂ����̃t���O( NoUseDirectInputFlag ��⏕������� )
	int						UseDirectInput8Flag ;				// �c�����������h���������W���g�p���邩�ǂ����̃t���O
	D_IDirectInput7			*DirectInputObject ;				// �c�����������h���������I�u�W�F�N�g

	D_IDirectInputDevice7	*MouseDeviceObject ;				// �}�E�X�f�o�C�X�I�u�W�F�N�g
	D_IDirectInputDevice7	*KeyboardDeviceObject ;				// �L�[�{�[�h�f�o�C�X�I�u�W�F�N�g
	HANDLE					KeyEvent ;							// �L�[�{�[�h�̃C�x���g�n���h��
	int						KeyInputGetTime ;					// ��O�ɏ�Ԃ��擾��������
	unsigned char			KeyInputBuf[256] ;					// �L�[�{�[�h�̓��͏��
	int						KeyExclusiveCooperativeLevelFlag ;	// �L�[�{�[�h�̋������x�����r�����x���ɂȂ��Ă��邩�ǂ����t���O
	int						KeyToJoypadInputInitializeFlag ;	// KeyToJoypadInput ����x�ł��������������ǂ����̃t���O
	int						KeyToJoypadInput[ MAX_JOYPAD_NUM ][ 32 ][ 4 ] ;	// �W���C�p�b�h�̓��͂ɑΉ������L�[�{�[�h�̓���
	int						MouseInputBufValidFlag ;			// MouseInputBuf ���L�����ǂ���( TRUE:�L��  FALSE:���� )
	BYTE					MouseInputBuf[ 4 ] ;				// �}�E�X�̓��͏��( 0:LEFT 1:RIGHT 2:MIDDLE )
	int						MouseMoveZ ;						// �}�E�X�z�C�[���̈ړ��l

	int						JoyPadNum ;							// �W���C�p�b�h�̐�
	DINPUTPAD				Joypad[ MAX_JOYPAD_NUM ] ;			// �p�b�h�f�o�C�X���

	int						KeyboardNotDirectInputFlag ;		// �L�[�{�[�h�f�o�C�X�̃A�N�Z�X�� DirectInput ���g�p���Ȃ����A�t���O
	int						EndFlag ;
	int						F10Flag ;							// F10 �������ꂽ���t���O
	int						F10InputTime ;						// F10 �������ꂽ����
	int						F12Flag ;							// F12 �������ꂽ���t���O
	int						F12InputTime ;						// F12 �������ꂽ����
} ;

// �e�[�u��-----------------------------------------------------------------------

// �������ϐ��錾 --------------------------------------------------------------

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

// �c�����������h���������̏������A�I���֐�
extern	int InitializeDirectInput( void ) ;											// �c�����������h�������������������� 
extern	int TerminateDirectInput( void ) ;											// �c�����������h���������̎g�p���I������

extern	int SetF10Input( void ) ;													// F10 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern	int ResetF10Input( void ) ;													// F10 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern	int SetF12Input( void ) ;													// F12 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern	int ResetF12Input( void ) ;													// F12 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern	int RefreshInputDeviceAcquireState( void ) ;								// �c�����������h���������f�o�C�X�̎擾��Ԃ��X�V����
extern	int JoypadEffectProcess( void ) ;											// �p�b�h�G�t�F�N�g�Ɋւ���t���[���������s��
extern	int RefreshEffectPlayState( void ) ;										// �p�b�h�G�t�F�N�g�̍Đ���Ԃ��X�V����(�\�t�g����A�N�e�B�u�ɂȂ邱�Ƃ����邽��)
extern	int UpdateKeyboardInputState( int UseProcessMessage = TRUE ) ;				// ���͏�Ԃ̍X�V
extern	int SetupJoypad( void ) ;													// �W���C�p�b�h�̃Z�b�g�A�b�v���s��
extern	int TerminateJoypad( void ) ;												// �W���C�p�b�h�̌�n�����s��
extern	int CheckUseDirectInputMouse( void ) ;										// �}�E�X�̏��擾�ɂc�����������h�����������g�p���Ă��邩�ǂ������擾����( �߂�l  TRUE:DirectInput���g�p���Ă���  FALSE:DirectInput���g�p���Ă��Ȃ� )
extern	int GetDirectInputMouseMoveZ( int CounterReset = TRUE ) ;					// �c�����������h�����������g�p�����}�E�X�z�C�[���̈ړ��l���擾����
extern	float GetDirectInputMouseMoveZF( int CounterReset = TRUE ) ;				// �c�����������h�����������g�p�����}�E�X�z�C�[���̈ړ��l���擾����( float�� )

// wchar_t�Ŋ֐�
extern int GetJoypadName_WCHAR_T( int InputType, wchar_t *InstanceNameBuffer, wchar_t *ProductNameBuffer ) ;


#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // DX_NON_INPUT

#endif // __DXINPUT_H__
