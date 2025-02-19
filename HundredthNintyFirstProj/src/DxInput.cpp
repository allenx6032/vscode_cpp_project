// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		�c�����������h������������v���O����
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

// �c�w���C�u�����쐬���p��`
#define __DX_MAKE

#include "DxInput.h"

#ifndef DX_NON_INPUT

// �C���N���[�h ------------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxUseCLib.h"
#include "DxMemory.h"
#include "DxSystem.h"
#include "DxLog.h"
#include "DxWindow.h"
#include "DxWinAPI.h"
#include "DxGuid.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` -------------------------------------
#ifdef __BCC
	#ifdef sprintf
	#undef sprintf
	#endif
	
	#ifdef vsprintf
	#undef vsprintf
	#endif
#endif

// �c�����������w���C�u���������n�j���̎��ʎq
#define DXLIB_OK  D_DI_OK

#define DEADZONE							(DWORD)( 0.35 * 65536 )
#define DEADZONE_DIRECTINPUT( ZONE )		(DWORD)( 10000 * (ZONE) / 65536)
#define DEADZONE_WINMM( ZONE )				(DWORD)(0x8000 * (ZONE) / 65536)
#define DEADZONE_XINPUT( ZONE )				(SHORT)( 32767 * (ZONE) / 65536)
#define DEADZONE_XINPUT_TRIGGER( ZONE )		(SHORT)(   255 * (ZONE) / 65536)
#define RANGE								(1000)
#define VALIDRANGE_WINMM( ZONE )			(0x8000 - DEADZONE_WINMM(ZONE))
#define VALIDRANGE_XINPUT( ZONE )			( 32767 - DEADZONE_XINPUT(ZONE))
#define VALIDRANGE_XINPUT_TRIGGER( ZONE )	(   255 - DEADZONE_XINPUT_TRIGGER(ZONE))

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS			(0xBB)
#endif

#ifndef VK_OEM_COMMA
#define VK_OEM_COMMA		(0xBC)
#endif

#ifndef VK_OEM_MINUS
#define VK_OEM_MINUS		(0xBD)
#endif

#ifndef VK_OEM_PERIOD
#define VK_OEM_PERIOD		(0xBE)
#endif

#ifndef VK_OEM_1
#define VK_OEM_1			(0xBA)
#endif

#ifndef VK_OEM_2
#define VK_OEM_2			(0xBF)
#endif

#ifndef VK_OEM_3
#define VK_OEM_3			(0xC0)
#endif

#ifndef VK_OEM_4
#define VK_OEM_4			(0xDB)
#endif

#ifndef VK_OEM_5
#define VK_OEM_5			(0xDC)
#endif

#ifndef VK_OEM_6
#define VK_OEM_6			(0xDD)
#endif

#ifndef VK_OEM_7
#define VK_OEM_7			(0xDE)
#endif

#ifndef VK_OEM_102
#define VK_OEM_102			(0xE2)
#endif

#ifndef VK_OEM_COPY
#define VK_OEM_COPY			(0xF2)
#endif

#ifndef VK_OEM_AUTO
#define VK_OEM_AUTO			(0xF3)
#endif

#ifndef VK_OEM_ENLW
#define VK_OEM_ENLW			(0xF4)
#endif

#ifndef WHEEL_DELTA
#define WHEEL_DELTA			(120)
#endif

const static unsigned short __KeyMap[][3 /* 0:Windows���z�L�[�R�[�h  1:DirectInput�L�[�R�[�h  2:DirectInput�g�p���Ɏg�p���邩�ۂ� */ ] =
{
	'A',			D_DIK_A,			0,	// �`�L�[
	'B',			D_DIK_B,			0,	// �a�L�[
	'C',			D_DIK_C,			0,	// �b�L�[
	'D',			D_DIK_D,			0,	// �c�L�[
	'E',			D_DIK_E,			0,	// �d�L�[
	'F',			D_DIK_F,			0,	// �e�L�[
	'G',			D_DIK_G,			0,	// �f�L�[
	'H',			D_DIK_H,			0,	// �g�L�[
	'I',			D_DIK_I,			0,	// �h�L�[
	'J',			D_DIK_J,			0,	// �i�L�[
	'K',			D_DIK_K,			0,	// �j�L�[
	'L',			D_DIK_L,			0,	// �k�L�[
	'M',			D_DIK_M,			0,	// �l�L�[
	'N',			D_DIK_N,			0,	// �m�L�[
	'O',			D_DIK_O,			0,	// �n�L�[
	'P',			D_DIK_P,			0,	// �o�L�[
	'Q',			D_DIK_Q,			0,	// �p�L�[
	'R',			D_DIK_R,			0,	// �q�L�[
	'S',			D_DIK_S,			0,	// �r�L�[
	'T',			D_DIK_T,			0,	// �s�L�[
	'U',			D_DIK_U,			0,	// �t�L�[
	'V',			D_DIK_V,			0,	// �u�L�[
	'W',			D_DIK_W,			0,	// �v�L�[
	'X',			D_DIK_X,			0,	// �w�L�[
	'Y',			D_DIK_Y,			0,	// �x�L�[
	'Z',			D_DIK_Z,			0,	// �y�L�[
	'0',			D_DIK_0,			0,	// �O�L�[
	'1',			D_DIK_1,			0,	// �P�L�[
	'2',			D_DIK_2,			0,	// �Q�L�[
	'3',			D_DIK_3,			0,	// �R�L�[
	'4',			D_DIK_4,			0,	// �S�L�[
	'5',			D_DIK_5,			0,	// �T�L�[
	'6',			D_DIK_6,			0,	// �U�L�[
	'7',			D_DIK_7,			0,	// �V�L�[
	'8',			D_DIK_8,			0,	// �W�L�[
	'9',			D_DIK_9,			0,	// �X�L�[

	VK_LEFT,		D_DIK_LEFT,			1,	// ���L�[
	VK_UP,			D_DIK_UP,			1,	// ��L�[
	VK_RIGHT,		D_DIK_RIGHT,		1,	// �E�L�[
	VK_DOWN,		D_DIK_DOWN,			1,	// ���L�[

	VK_OEM_PLUS,	D_DIK_SEMICOLON,	0,	// �G�L�[
	VK_OEM_1,		D_DIK_COLON,		0,	// �F�L�[
	VK_OEM_4,		D_DIK_LBRACKET,		0,	// �m�L�[
	VK_OEM_6,		D_DIK_RBRACKET,		0,	// �n�L�[
	VK_OEM_3,		D_DIK_AT,			0,	// ���L�[
	VK_OEM_102,		D_DIK_BACKSLASH,	0,	// �_�L�[
	VK_OEM_COMMA,	D_DIK_COMMA,		0,	// �C�L�[
	VK_OEM_7,		D_DIK_PREVTRACK,	0,	// �O���[

	VK_OEM_MINUS,	D_DIK_MINUS,		0,	// �|�L�[
	VK_OEM_5,		D_DIK_YEN,			0,	// ���L�[
	VK_OEM_PERIOD,	D_DIK_PERIOD,		0,	// �D�L�[
	VK_OEM_2,		D_DIK_SLASH,		0,	// �^�L�[

	VK_DELETE,		D_DIK_DELETE,		0,	// �f���[�g�L�[

	VK_ESCAPE,		D_DIK_ESCAPE,		0,	// �G�X�P�[�v�L�[
	VK_SPACE,		D_DIK_SPACE,		0,	// �X�y�[�X�L�[

	VK_BACK, 		D_DIK_BACK,			0,	// �o�b�N�X�y�[�X�L�[
	VK_TAB, 		D_DIK_TAB,			0,	// �^�u�L�[
	VK_RETURN,		D_DIK_RETURN,		0,	// �G���^�[�L�[

	VK_NUMLOCK,		D_DIK_NUMLOCK,		1,	// �e���L�[NumLock�L�[
	VK_NUMPAD0,		D_DIK_NUMPAD0,		1,	// �e���L�[�O
	VK_NUMPAD1,		D_DIK_NUMPAD1,		1,	// �e���L�[�P
	VK_NUMPAD2,		D_DIK_NUMPAD2,		1,	// �e���L�[�Q
	VK_NUMPAD3,		D_DIK_NUMPAD3,		1,	// �e���L�[�R
	VK_NUMPAD4,		D_DIK_NUMPAD4,		1,	// �e���L�[�S
	VK_NUMPAD5,		D_DIK_NUMPAD5,		1,	// �e���L�[�T
	VK_NUMPAD6,		D_DIK_NUMPAD6,		1,	// �e���L�[�U
	VK_NUMPAD7,		D_DIK_NUMPAD7,		1,	// �e���L�[�V
	VK_NUMPAD8,		D_DIK_NUMPAD8,		1,	// �e���L�[�W
	VK_NUMPAD9,		D_DIK_NUMPAD9,		1,	// �e���L�[�X
	VK_MULTIPLY,	D_DIK_MULTIPLY,		1,	// �e���L�[���L�[
	VK_ADD,			D_DIK_ADD,			1,	// �e���L�[�{�L�[
	VK_SUBTRACT,	D_DIK_SUBTRACT,		1,	// �e���L�[�|�L�[
	VK_DECIMAL,		D_DIK_DECIMAL,		1,	// �e���L�[�D�L�[
	VK_DIVIDE,		D_DIK_DIVIDE,		1,	// �e���L�[�^�L�[

	VK_RETURN,		D_DIK_NUMPADENTER,	1,	// �e���L�[�̃G���^�[�L�[
//	VK_NUMPADENTER,	D_DIK_NUMPADENTER,	1,	// �e���L�[�̃G���^�[�L�[

	VK_LSHIFT,		D_DIK_LSHIFT,		1,	// ���V�t�g�L�[
	VK_RSHIFT,		D_DIK_RSHIFT,		1,	// �E�V�t�g�L�[
	VK_LCONTROL,	D_DIK_LCONTROL,		1,	// ���R���g���[���L�[
	VK_RCONTROL,	D_DIK_RCONTROL,		1,	// �E�R���g���[���L�[
	VK_PRIOR,		D_DIK_PGUP,			1,	// �o�������t�o�L�[
	VK_NEXT,		D_DIK_PGDN,			1,	// �o�������c�������L�[
	VK_END,			D_DIK_END,			1,	// �G���h�L�[
	VK_HOME,		D_DIK_HOME,			1,	// �z�[���L�[
	VK_INSERT,		D_DIK_INSERT,		1,	// �C���T�[�g�L�[

	VK_LMENU,		D_DIK_LALT,			1,	// ���`�k�s�L�[
	VK_RMENU,		D_DIK_RALT,			1,	// �E�`�k�s�L�[
	VK_SCROLL,		D_DIK_SCROLL,		1,	// ScrollLock�L�[
	VK_APPS,		D_DIK_APPS,			1,	// �A�v���P�[�V�������j���[�L�[
	VK_CAPITAL,		D_DIK_CAPSLOCK,		1,	// CaspLock�L�[
	VK_SNAPSHOT,	D_DIK_SYSRQ,		1,	// PrintScreen�L�[
	VK_PAUSE,		D_DIK_PAUSE,		1,	// PauseBreak�L�[
	VK_LWIN,		D_DIK_LWIN,			1,	// ���v�����L�[
	VK_RWIN,		D_DIK_RWIN,			1,	// �E�v�����L�[

	VK_OEM_ENLW,	D_DIK_KANJI,		1,	// ���p�^�S�p�L�[
	VK_CONVERT,		D_DIK_CONVERT,		1,	// �ϊ��L�[
	VK_NONCONVERT,	D_DIK_NOCONVERT,	1,	// ���ϊ��L�[
	VK_OEM_COPY,	D_DIK_KANA,			0,	// �J�i�L�[

	VK_F1,			D_DIK_F1,			1,	// �e�P�L�[
	VK_F2,			D_DIK_F2,			1,	// �e�Q�L�[
	VK_F3,			D_DIK_F3,			1,	// �e�R�L�[
	VK_F4,			D_DIK_F4,			1,	// �e�S�L�[
	VK_F5,			D_DIK_F5,			1,	// �e�T�L�[
	VK_F6,			D_DIK_F6,			1,	// �e�U�L�[
	VK_F7,			D_DIK_F7,			1,	// �e�V�L�[
	VK_F8,			D_DIK_F8,			1,	// �e�W�L�[
	VK_F9,			D_DIK_F9,			1,	// �e�X�L�[
	VK_F10,			D_DIK_F10,			1,	// �e�P�O�L�[
	VK_F11,			D_DIK_F11,			1,	// �e�P�P�L�[
	VK_F12,			D_DIK_F12,			1,	// �e�P�Q�L�[

	0xffff,			0xffff,
} ;

#define XINPUT_TO_DIRECTINPUT_BUTTONNUM		(10)
WORD XInputButtonToDirectInputButtonNo[ XINPUT_TO_DIRECTINPUT_BUTTONNUM ] =
{
	D_XINPUT_GAMEPAD_A,
	D_XINPUT_GAMEPAD_B,
	D_XINPUT_GAMEPAD_X,
	D_XINPUT_GAMEPAD_Y,
	D_XINPUT_GAMEPAD_LEFT_SHOULDER,
	D_XINPUT_GAMEPAD_RIGHT_SHOULDER,
	D_XINPUT_GAMEPAD_BACK,
	D_XINPUT_GAMEPAD_START,
	D_XINPUT_GAMEPAD_LEFT_THUMB,
	D_XINPUT_GAMEPAD_RIGHT_THUMB
} ;

// �\���̒�` --------------------------------------------------------------------

// �萔��` ----------------------------------------------------------------------

//#include "DxInputDef.h"
// �L�[�{�[�h�f�o�C�X�̃f�[�^�t�H�[�}�b�g��` =============

// �f�t�h�c
extern GUID GUIDDIKEYBOARD ;

// �f�o�C�X�I�u�W�F�N�g�f�[�^
extern D_DIOBJECTDATAFORMAT C_ODFDIKEYBOARD[256] ;

// �f�o�C�X�f�[�^�t�H�[�}�b�g
extern D_DIDATAFORMAT C_DFDIKEYBOARD ;


// �W���C�X�e�B�b�N�f�o�C�X�̃f�[�^�t�H�[�}�b�g��` =======

// �f�t�h�c
extern GUID GUIDDIJOYSTICK[8] ;

// �f�o�C�X�I�u�W�F�N�g�f�[�^
extern D_DIOBJECTDATAFORMAT C_ODFDIJOYSTICK[44] ;

// �f�o�C�X�f�[�^�t�H�[�}�b�g
extern D_DIDATAFORMAT C_DFDIJOYSTICK ;
extern D_DIDATAFORMAT C_DFDIJOYSTICK2 ;
extern D_DIDATAFORMAT C_DFDIMOUSE2 ;
extern D_DIDATAFORMAT C_DFDIMOUSE ;


// �������ϐ��錾 --------------------------------------------------------------

// �c�����������h���������Ǘ��p�f�[�^
DINPUTDATA DInputData ;

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

// �W���C�p�b�h�񋓗p�R�[���o�b�N�֐�
		BOOL FAR PASCAL EnumJoypadProc( const D_DIDEVICEINSTANCEW *pdinst , LPVOID pvRef ) ;

static	void F10Process( void ) ;												// F10 ����
static	int UpdateJoypadInputState( int padno ) ;								// �p�b�h�̓��͏�Ԃ̍X�V

// �v���O���� --------------------------------------------------------------------

// �c�����������h���������̏������A�I���֐�

// �c�����������h��������������������
extern int InitializeDirectInput( void )
{
	HRESULT hr ;
	int NoUseXInputFlag ;
	int NoUseDirectInputFlag ;
	int UseDirectInputFlag ;
	int KeyToJoypadInputInitializeFlag ;
	int NoUseVibrationFlag ;
	int i, j;
	int KeyToJoypadInput[ MAX_JOYPAD_NUM ][ 32 ][ 4 ] ;
	static int NowInitialize = FALSE ;

	// ����������
	if( DInputData.DirectInputObject != NULL || NowInitialize == TRUE ) return 0 ;
	NowInitialize = TRUE ;

	// �[��������
	NoUseXInputFlag								= DInputData.NoUseXInputFlag ;
	NoUseDirectInputFlag						= DInputData.NoUseDirectInputFlag ;
	UseDirectInputFlag							= DInputData.UseDirectInputFlag ;
	KeyToJoypadInputInitializeFlag				= DInputData.KeyToJoypadInputInitializeFlag ;
	NoUseVibrationFlag							= DInputData.NoUseVibrationFlag ;
	_MEMCPY( KeyToJoypadInput, DInputData.KeyToJoypadInput, sizeof( DInputData.KeyToJoypadInput ) ) ;
	_MEMSET( &DInputData, 0, sizeof( DInputData ) ) ; 
	DInputData.NoUseXInputFlag					= NoUseXInputFlag ;
	DInputData.NoUseDirectInputFlag				= NoUseDirectInputFlag ;
	DInputData.UseDirectInputFlag				= UseDirectInputFlag ;
	DInputData.KeyToJoypadInputInitializeFlag	= KeyToJoypadInputInitializeFlag ;
	DInputData.NoUseVibrationFlag				= NoUseVibrationFlag ;
	_MEMCPY( DInputData.KeyToJoypadInput, KeyToJoypadInput, sizeof( DInputData.KeyToJoypadInput ) ) ;

	// �c�����������h�������� ���g�p���邩�ǂ����ŏ����𕪊�
START:
	if( DInputData.NoUseDirectInputFlag == FALSE )
	{
		int i ;
		const wchar_t *XInputDllFileName[] = 
		{
			L"xinput1_4.dll",
			L"xinput1_3.dll",
			L"xinput9_1_0.dll",
			NULL
		} ;

		// DirectInput ���g�p����ꍇ

		DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\xa2\x95\xc2\x4f\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x0a\x00\x00"/*@ L"DirectInput�֌W����������\n" @*/ ) ;
		DXST_ERRORLOG_TABADD ;

		// �w�h�����������g�p���Ȃ��t���O�������Ă��Ȃ�������w�h���������̂c�k�k��ǂݍ���
		if( DInputData.NoUseXInputFlag == FALSE )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\x58\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\x44\x00\x4c\x00\x4c\x00\x20\x00\x6e\x30\xad\x8a\x7f\x30\xbc\x8f\x7f\x30\x2d\x4e\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"XInput DLL �̓ǂݍ��ݒ�... " @*/ ) ;
			for( i = 0 ; XInputDllFileName[ i ] != NULL ; i ++ )
			{
				DInputData.XInputDLL = LoadLibraryW( XInputDllFileName[ i ] ) ;
				if( DInputData.XInputDLL != NULL )
					break ;
			}

			if( DInputData.XInputDLL == NULL )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x31\x59\x57\x65\x01\x30\x58\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x6f\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x0a\x00\x00"/*@ L"���s�AXInput�͎g�p���܂���\n" @*/ ) ;
			}
			else
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����\n" @*/ ) ;

				DInputData.XInputGetStateFunc = ( DWORD ( WINAPI * )( DWORD, D_XINPUT_STATE*     ) )GetProcAddress( DInputData.XInputDLL, "XInputGetState" ) ;
				DInputData.XInputSetStateFunc = ( DWORD ( WINAPI * )( DWORD, D_XINPUT_VIBRATION* ) )GetProcAddress( DInputData.XInputDLL, "XInputSetState" ) ;
			}
		}

		// �c�����������h���������I�u�W�F�N�g���쐬����
		DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x37\x00\x20\x00\x6e\x30\xd6\x53\x97\x5f\x2d\x4e\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"DirectInput7 �̎擾��... " @*/ ) ;

		hr = WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTINPUT , NULL, CLSCTX_INPROC_SERVER, IID_IDIRECTINPUT7, ( LPVOID * )&DInputData.DirectInputObject ) ;
		if( !FAILED( hr ) )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����\n" @*/ ) ;
			DXST_ERRORLOG_TABADD ;
			DXST_ERRORLOG_ADDUTF16LE( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"������������������... " @*/ ) ;
			hr = DInputData.DirectInputObject->Initialize( WinAPIData.Win32Func.GetModuleHandleWFunc( NULL ), 0x700 ) ;
			if( FAILED( hr ) ) 
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x02\x30\x00"/*@ L"�������Ɏ��s�B" @*/ ) ;
				DInputData.DirectInputObject->Release() ;
				DInputData.DirectInputObject = NULL ;
				goto ER1;
			}
			DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����������\n" @*/ ) ;
			DXST_ERRORLOG_TABSUB ;
		}
		else
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x02\x30\x0a\x00\x00"/*@ L"�擾�Ɏ��s�B\n" @*/ ) ;
			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xa8\x30\xe9\x30\xfc\x30\xb3\x30\xfc\x30\xc9\x30\x20\x00\x30\x00\x78\x00\x25\x00\x78\x00\x0a\x00\x00"/*@ L"�G���[�R�[�h 0x%x\n" @*/, hr ));
ER1:
			DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x38\x00\x20\x00\x6e\x30\xd6\x53\x97\x5f\x92\x30\x66\x8a\x7f\x30\x7e\x30\x59\x30\x2e\x00\x2e\x00\x2e\x00\x00"/*@ L"DirectInput8 �̎擾�����݂܂�..." @*/ ) ;
			hr = WinAPIData.Win32Func.CoCreateInstanceFunc( CLSID_DIRECTINPUT8 , NULL, CLSCTX_INPROC_SERVER, IID_IDIRECTINPUT8, ( LPVOID * )&DInputData.DirectInputObject ) ;
			if( !FAILED( hr ) )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����\n" @*/ ) ;
				DXST_ERRORLOG_TABADD ;
				DXST_ERRORLOG_ADDUTF16LE( "\x15\x5f\x4d\x30\x9a\x7d\x4d\x30\x1d\x52\x1f\x67\x16\x53\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"������������������... " @*/ ) ;
				hr = DInputData.DirectInputObject->Initialize( WinAPIData.Win32Func.GetModuleHandleWFunc( NULL ) , 0x800 ) ;
				if( FAILED( hr ) ) 
				{
					DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x6b\x30\x31\x59\x57\x65\x02\x30\x00"/*@ L"�������Ɏ��s�B" @*/ ) ;
					DInputData.DirectInputObject->Release() ;
					DInputData.DirectInputObject = NULL ;
					goto ER2;
				}
				DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����������\n" @*/ ) ;
				DInputData.UseDirectInput8Flag = TRUE;
				DXST_ERRORLOG_TABSUB ;
			}
			else
			{
				DXST_ERRORLOG_ADDUTF16LE( "\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x02\x30\x00"/*@ L"�擾�Ɏ��s�B" @*/ ) ;
ER2:
				DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\x92\x30\x7f\x4f\x28\x75\x57\x30\x7e\x30\x5b\x30\x93\x30\x02\x30\x0a\x00\x00"/*@ L"DirectInput ���g�p���܂���B\n" @*/ ) ;
				DXST_ERRORLOG_TABSUB ;

				if( DInputData.DirectInputObject ) DInputData.DirectInputObject->Release() ;
				DInputData.DirectInputObject = NULL;
				DInputData.NoUseDirectInputFlag = TRUE;
//				return -1 ;
				goto START;
			}
		}

		// ���C���E�C���h�E���A�N�e�B�u�ɂ���
		// BringWindowToTop( NS_GetMainWindowHandle() ) ;

		// �A�N�e�B�u�ɂȂ�܂ő҂�
		NS_ProcessMessage() ;

		// �W���C�p�b�h�f�o�C�X������������
		SetupJoypad();

		// �}�E�X�f�o�C�X������������
		DInputData.MouseDeviceObject = NULL ;
		{
			// �}�E�X�f�o�C�X���쐬����
			DXST_ERRORLOG_ADDUTF16LE( "\xde\x30\xa6\x30\xb9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"�}�E�X�f�o�C�X�̏�����... " @*/ ) ; 
			if( DInputData.UseDirectInput8Flag == TRUE )
			{
				hr = DInputData.DirectInputObject->CreateDevice( GUID_SYSMOUSE, ( D_IDirectInputDevice ** )&DInputData.MouseDeviceObject , NULL ) ;
			}
			else
			{
				hr = DInputData.DirectInputObject->CreateDeviceEx( GUID_SYSMOUSE, IID_IDIRECTINPUTDEVICE7, ( void ** )&DInputData.MouseDeviceObject , NULL ) ;
			}
			if( hr != D_DI_OK )
			{
				DInputData.MouseDeviceObject = NULL ;
				goto MOUSEDEVICEINITEND ;
//				DInputData.DirectInputObject->Release() ;
//				DInputData.DirectInputObject = NULL ;

//				NowInitialize = FALSE ;
//				return DxLib_Error( L"�}�E�X�f�o�C�X�̎擾�Ɏ��s���܂���" ) ;
			}

			// �}�E�X�f�o�C�X�̃f�[�^�`����ݒ肷��
	//		hr = DInputData.MouseDeviceObject->SetDataFormat( &c_dfDIMouse2 ) ;
			hr = DInputData.MouseDeviceObject->SetDataFormat( &C_DFDIMOUSE2 ) ;
			if( hr != D_DI_OK )
			{
				DInputData.MouseDeviceObject->Release() ;
				DInputData.MouseDeviceObject = NULL ;
				goto MOUSEDEVICEINITEND ;

//				DInputData.DirectInputObject->Release() ;
//				DInputData.DirectInputObject = NULL ;

//				NowInitialize = FALSE ;
//				return DxLib_Error( L"�}�E�X�f�o�C�X�̃f�[�^�`���̐ݒ�Ɏ��s���܂���" ) ;
			}

			// �}�E�X�̋������x����ݒ肷��
	//		hr = DInputData.MouseDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_EXCLUSIVE | D_DISCL_FOREGROUND ) ;
			hr = DInputData.MouseDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ;
	//		hr = DInputData.MouseDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_FOREGROUND ) ;
 			if( hr != D_DI_OK ) 
			{
				DInputData.MouseDeviceObject->Release() ;
				DInputData.MouseDeviceObject = NULL ;
				goto MOUSEDEVICEINITEND ;

//				DInputData.DirectInputObject->Release() ;
//				DInputData.DirectInputObject = NULL ;

//				NowInitialize = FALSE ;
//				return DxLib_Error( L"�}�E�X�f�o�C�X�̋������x���̐ݒ�Ɏ��s���܂���" ) ;
			}

			DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����������\n" @*/ ) ;

MOUSEDEVICEINITEND:
			if( DInputData.MouseDeviceObject == NULL )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\xde\x30\xa6\x30\xb9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�}�E�X�f�o�C�X�̎擾�Ɏ��s���܂���\n" @*/ ) ; 
			}

			DInputData.KeyInputGetTime = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		}

		// �L�[�{�[�h�f�o�C�X������������
		DInputData.KeyboardDeviceObject = NULL ;
		if( DInputData.KeyboardNotDirectInputFlag == FALSE )
		{
			HANDLE Event ;

			// �L�[�{�[�h�f�o�C�X���쐬����
			DXST_ERRORLOG_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x20\x00\x00"/*@ L"�L�[�{�[�h�f�o�C�X�̏�����... " @*/ ) ; 
			if( DInputData.UseDirectInput8Flag == TRUE )
			{
				hr = DInputData.DirectInputObject->CreateDevice( GUID_SYSKEYBOARD, ( D_IDirectInputDevice ** )&DInputData.KeyboardDeviceObject , NULL ) ;
			}
			else
			{
				hr = DInputData.DirectInputObject->CreateDeviceEx( GUID_SYSKEYBOARD, IID_IDIRECTINPUTDEVICE7, ( void ** )&DInputData.KeyboardDeviceObject , NULL ) ;
			}
			if( hr != D_DI_OK )
			{
				if( DInputData.MouseDeviceObject )
				{
					DInputData.MouseDeviceObject->Release() ;
					DInputData.MouseDeviceObject = NULL ;
				}

				DInputData.DirectInputObject->Release() ;
				DInputData.DirectInputObject = NULL ;

				NowInitialize = FALSE ;
				return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�L�[�{�[�h�f�o�C�X�̎擾�Ɏ��s���܂���" @*/ ) ;
			}

			// �L�[�{�[�h�f�o�C�X�̃f�[�^�`����ݒ肷��
	//		hr = DInputData.KeyboardDeviceObject->SetDataFormat( &c_dfDIKeyboard ) ;
			hr = DInputData.KeyboardDeviceObject->SetDataFormat( &C_DFDIKEYBOARD ) ;
			if( hr != D_DI_OK )
			{
				if( DInputData.MouseDeviceObject )
				{
					DInputData.MouseDeviceObject->Release() ;
					DInputData.MouseDeviceObject = NULL ;
				}

				DInputData.KeyboardDeviceObject->Release() ;
				DInputData.KeyboardDeviceObject = NULL ;

				DInputData.DirectInputObject->Release() ;
				DInputData.DirectInputObject = NULL ;

				NowInitialize = FALSE ;
				return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xc7\x30\xfc\x30\xbf\x30\x62\x5f\x0f\x5f\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�L�[�{�[�h�f�o�C�X�̃f�[�^�`���̐ݒ�Ɏ��s���܂���" @*/ ) ;
			}

			// �L�[�{�[�h�̋������x����ݒ肷��
			DInputData.KeyExclusiveCooperativeLevelFlag = FALSE ;
	//		hr = DInputData.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_EXCLUSIVE | D_DISCL_FOREGROUND ) ;
			hr = DInputData.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ;
	//		hr = DInputData.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_NONEXCLUSIVE | D_DISCL_FOREGROUND ) ;
 			if( hr != D_DI_OK ) 
			{
				if( DInputData.MouseDeviceObject )
				{
					DInputData.MouseDeviceObject->Release() ;
					DInputData.MouseDeviceObject = NULL ;
				}

				DInputData.KeyboardDeviceObject->Release() ;
				DInputData.KeyboardDeviceObject = NULL ;

				DInputData.DirectInputObject->Release() ;
				DInputData.DirectInputObject = NULL ;

				NowInitialize = FALSE ;
				return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x54\x53\xbf\x8a\xec\x30\xd9\x30\xeb\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�L�[�{�[�h�f�o�C�X�̋������x���̐ݒ�Ɏ��s���܂���" @*/ ) ;
			}

			// �o�b�t�@��ݒ肵�Ă݂�
			{
				D_DIPROPDWORD DIProp ;

				DIProp.diph.dwSize = sizeof( DIProp ) ;
				DIProp.diph.dwHeaderSize = sizeof( DIProp.diph ) ;
				DIProp.diph.dwObj = 0 ;
				DIProp.diph.dwHow = D_DIPH_DEVICE ;
				DIProp.dwData = 100 ;
				if( DInputData.KeyboardDeviceObject->SetProperty( D_DIPROP_BUFFERSIZE, &DIProp.diph ) != D_DI_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\x6e\x30\xd7\x30\xed\x30\xd1\x30\xc6\x30\xa3\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�L�[�{�[�h�̃v���p�e�B�ݒ�Ɏ��s���܂���\n" @*/ ) ;
				}
			}

			// �ω��ʒm�p�̃C�x���g���쐬�E�A�^�b�`����
			Event = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
			hr = DInputData.KeyboardDeviceObject->SetEventNotification( Event ) ;
			if( hr != D_DI_POLLEDDEVICE && hr != D_DI_OK )
			{
				DXST_ERRORLOG_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xa4\x30\xd9\x30\xf3\x30\xc8\x30\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x4c\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�L�[�{�[�h�C�x���g�̃Z�b�g�A�b�v�����s���܂���\n" @*/ ) ;
				NowInitialize = FALSE ;
				return -1 ;
			}
			DInputData.KeyEvent = Event ;

			// �L�[�{�[�h�̃f�o�C�X���擾����
			hr = DInputData.KeyboardDeviceObject->Acquire() ;
			if( hr != D_DI_OK )
			{
				hr = DInputData.KeyboardDeviceObject->Acquire() ;
				if( hr != D_DI_OK )
				{
					if( DInputData.MouseDeviceObject )
					{
						DInputData.MouseDeviceObject->Release() ;
						DInputData.MouseDeviceObject = NULL ;
					}

					DInputData.KeyboardDeviceObject->Release() ;
					DInputData.KeyboardDeviceObject = NULL ;

					DInputData.DirectInputObject->Release() ;
					DInputData.DirectInputObject = NULL ;

					NowInitialize = FALSE ;
					return DxLib_ErrorUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�L�[�{�[�h�f�o�C�X�̎擾�Ɏ��s���܂���" @*/ ) ;
				}
			}

			DXST_ERRORLOG_ADDUTF16LE( "\x1d\x52\x1f\x67\x16\x53\x10\x62\x9f\x52\x0a\x00\x00"/*@ L"����������\n" @*/ ) ;

			DInputData.KeyInputGetTime = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		}
		

		DXST_ERRORLOG_TABSUB ;
		DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\xa2\x95\x23\x90\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"DirectInput �֘A�̏������͐���ɏI�����܂���\n" @*/ ) ;
	}
	else
	{
		// DirectInput ���g�p���Ȃ��ꍇ
		int i ;
		JOYCAPS joycaps ;
		DXST_ERRORLOG_TABADD ;

		// �p�b�h�̐��𒲂ׂ�
		for( i = 0 ; i < MAX_JOYPAD_NUM ; i ++ )
		{
//			_MEMSET( &joyex, 0, sizeof( joyex ) ) ;
//			joyex.dwSize  = sizeof( joyex ) ;
//			joyex.dwFlags = JOY_RETURNALL ;
//			if( WinAPIData.Win32Func.joyGetPosExFunc( i, &joyex ) != JOYERR_NOERROR ) break ;
			_MEMSET( &joycaps, 0, sizeof( joycaps ) ) ;
			hr = ( HRESULT )WinAPIData.Win32Func.joyGetDevCapsFunc( ( UINT )i, &joycaps, sizeof( joycaps ) ) ;
			if( hr != JOYERR_NOERROR ) break ;
			DInputData.Joypad[ i ].RightStickFlag = ( joycaps.wCaps & JOYCAPS_HASZ ) && ( joycaps.wCaps & JOYCAPS_HASR ) ;
			DInputData.Joypad[ i ].DeadZone = DEADZONE ;
		}
		DInputData.JoyPadNum = i ;

		DXST_ERRORLOGFMT_ADDUTF16LE(( "\xd1\x30\xc3\x30\xc9\x30\x6e\x30\x70\x65\x6f\x30\x20\x00\x25\x00\x64\x00\x20\x00\x0b\x50\x67\x30\x59\x30\x0a\x00\x00"/*@ L"�p�b�h�̐��� %d �ł�\n" @*/, i ));

		DXST_ERRORLOG_TABSUB ;
		DInputData.DirectInputObject = (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ;
		DXST_ERRORLOG_ADDUTF16LE( "\x65\x51\x9b\x52\xa2\x95\x23\x90\x6e\x30\x1d\x52\x1f\x67\x16\x53\x92\x30\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"���͊֘A�̏����������܂���\n" @*/ ) ;
	}

	// �L�[�{�[�h�ƃW���C�p�b�h�̓��͂̃f�t�H���g�̑Ή��\��ݒ肷��
	if( DInputData.KeyToJoypadInputInitializeFlag == FALSE )
	{
		DInputData.KeyToJoypadInputInitializeFlag = TRUE ;

		for( i = 0 ; i < MAX_JOYPAD_NUM ; i ++ )
		{
			for ( j = 0 ; j < 32 ; j ++ )
			{
				DInputData.KeyToJoypadInput[ i ][ j ][ 0 ] = -1 ;
				DInputData.KeyToJoypadInput[ i ][ j ][ 1 ] = -1 ;
			}
		}
		DInputData.KeyToJoypadInput[ 0 ][  0 ][ 0 ] = D_DIK_NUMPAD2;
		DInputData.KeyToJoypadInput[ 0 ][  0 ][ 1 ] = D_DIK_DOWN;
		DInputData.KeyToJoypadInput[ 0 ][  1 ][ 0 ] = D_DIK_NUMPAD4;
		DInputData.KeyToJoypadInput[ 0 ][  1 ][ 1 ] = D_DIK_LEFT;
		DInputData.KeyToJoypadInput[ 0 ][  2 ][ 0 ] = D_DIK_NUMPAD6;
		DInputData.KeyToJoypadInput[ 0 ][  2 ][ 1 ] = D_DIK_RIGHT;
		DInputData.KeyToJoypadInput[ 0 ][  3 ][ 0 ] = D_DIK_NUMPAD8;
		DInputData.KeyToJoypadInput[ 0 ][  3 ][ 1 ] = D_DIK_UP;
		DInputData.KeyToJoypadInput[ 0 ][  4 ][ 0 ] = D_DIK_Z;
		DInputData.KeyToJoypadInput[ 0 ][  5 ][ 0 ] = D_DIK_X;
		DInputData.KeyToJoypadInput[ 0 ][  6 ][ 0 ] = D_DIK_C;
		DInputData.KeyToJoypadInput[ 0 ][  7 ][ 0 ] = D_DIK_A;
		DInputData.KeyToJoypadInput[ 0 ][  8 ][ 0 ] = D_DIK_S;
		DInputData.KeyToJoypadInput[ 0 ][  9 ][ 0 ] = D_DIK_D;
		DInputData.KeyToJoypadInput[ 0 ][ 10 ][ 0 ] = D_DIK_Q;
		DInputData.KeyToJoypadInput[ 0 ][ 11 ][ 0 ] = D_DIK_W;
		DInputData.KeyToJoypadInput[ 0 ][ 12 ][ 0 ] = D_DIK_ESCAPE;
		DInputData.KeyToJoypadInput[ 0 ][ 13 ][ 0 ] = D_DIK_SPACE;
	}

	// �I��
	NowInitialize = FALSE ;
	return 0 ;
}

// �c�����������h���������̎g�p���I������
extern int TerminateDirectInput( void )
{
	// ����������
	if( DInputData.DirectInputObject == NULL ) return 0 ;

	// �c�����������h�������� ���g�p���Ă������ǂ����ŏ����𕪊�
	if( DInputData.DirectInputObject != (D_IDirectInput7 *)(DWORD_PTR)0xffffffff )
	{
		// DirectInput ���g�p�����ꍇ
		DXST_ERRORLOG_ADDUTF16LE( "\x44\x00\x69\x00\x72\x00\x65\x00\x63\x00\x74\x00\x49\x00\x6e\x00\x70\x00\x75\x00\x74\x00\x20\x00\xa2\x95\x23\x90\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x8c\x5b\x86\x4e\x0a\x00\x00"/*@ L"DirectInput �֘A�̏I������... ����\n" @*/ ) ;

		// �W���C�p�b�h�f�o�C�X�̉��
		TerminateJoypad() ;

		// �L�[�{�[�h�f�o�C�X�̉��
		if( DInputData.KeyboardDeviceObject != NULL )
		{
			DInputData.KeyboardDeviceObject->SetEventNotification( NULL ) ;
			DInputData.KeyboardDeviceObject->Unacquire() ;
			DInputData.KeyboardDeviceObject->Release() ;
			DInputData.KeyboardDeviceObject = NULL ;
			CloseHandle( DInputData.KeyEvent ) ;
		}

		// �}�E�X�f�o�C�X�̉��
		if( DInputData.MouseDeviceObject != NULL )
		{
			DInputData.MouseDeviceObject->Unacquire() ;
			DInputData.MouseDeviceObject->Release() ;
			DInputData.MouseDeviceObject = NULL ;
		}

		// �c�����������h���������I�u�W�F�N�g�̉��
		DInputData.DirectInputObject->Release() ;
		DInputData.DirectInputObject = NULL ;

		// �w�h���������̌�n������
		if( DInputData.XInputDLL )
		{
			FreeLibrary( DInputData.XInputDLL ) ;
			DInputData.XInputDLL = NULL ;
			DInputData.XInputGetStateFunc = NULL ;
			DInputData.XInputSetStateFunc = NULL ;
		}
	}
	else
	{
		// DirectInput ���g�p���Ȃ������ꍇ
		DXST_ERRORLOG_ADDUTF16LE( "\x65\x51\x9b\x52\xa2\x95\x23\x90\x6e\x30\x42\x7d\x86\x4e\xe6\x51\x06\x74\x2e\x00\x2e\x00\x2e\x00\x20\x00\x8c\x5b\x86\x4e\x0a\x00\x00"/*@ L"���͊֘A�̏I������... ����\n" @*/ ) ;

		DInputData.DirectInputObject = NULL ;
	}

	// �I��
	return 0 ;
}

// �W���C�p�b�h�̃Z�b�g�A�b�v���s��
extern int SetupJoypad( void )
{
	// ����������
	if( DInputData.DirectInputObject == NULL || DInputData.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ) return -1 ;

	// ���ɏ���������Ă����ꍇ���l���Č�n�����ŏ��ɍs��
	TerminateJoypad() ;

	// �W���C�p�b�h�f�o�C�X������������
	DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x2e\x00\x2e\x00\x2e\x00\x20\x00\x0a\x00\x00"/*@ L"�W���C�p�b�h�̏�����... \n" @*/ ) ;
	DInputData.JoyPadNum = 0 ;

	// XInput �Ŏ擾�ł���p�b�h���Ɍ��o
	if( DInputData.XInputDLL != NULL )
	{
		DWORD i ;
		DWORD j ;
		DINPUTPAD *pad ;
		D_XINPUT_STATE state ;

		pad = &DInputData.Joypad[ DInputData.JoyPadNum ] ;
		for( i = 0 ; i < 4 ; i ++ )
		{
			if( DInputData.XInputGetStateFunc( i, &state ) != ERROR_SUCCESS )
				continue ;

			DXST_ERRORLOGFMT_ADDUTF16LE(( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xca\x30\xf3\x30\xd0\x30\xfc\x30\x20\x00\x25\x00\x64\x00\x20\x00\x6f\x30\x20\x00\x58\x00\x62\x00\x6f\x00\x78\x00\x33\x00\x36\x00\x30\x00\x20\x00\x43\x00\x6f\x00\x6e\x00\x74\x00\x72\x00\x6f\x00\x6c\x00\x6c\x00\x65\x00\x72\x00\x20\x00\x67\x30\x59\x30\x00"/*@ L"�W���C�p�b�h�i���o�[ %d �� Xbox360 Controller �ł�" @*/, DInputData.JoyPadNum )) ;

			// XInput �ł̃f�o�C�X�ԍ���ۑ�
			pad->XInputDeviceNo = ( int )i ;

			// �����]�[���̃Z�b�g
			pad->DeadZone = DEADZONE ;

			// �U���֌W�̐ݒ�̏��������s��
			for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
			{
				pad->Effect[ j ].DIEffect   = NULL ;
				pad->Effect[ j ].PlayFlag	= FALSE ;
				pad->Effect[ j ].Power		= 0 ;
				pad->Effect[ j ].PlayState  = FALSE ;
				pad->Effect[ j ].SetPower   = 0 ;
			}
//			pad->EffectNotCommonFlag    = FALSE ;
//			pad->EffectCommon.PlayFlag	= FALSE ;
//			pad->EffectCommon.Power		= D_DI_FFNOMINALMAX ;
//			pad->Effect[ DINPUTPAD_MOTOR_LEFT ].PlayFlag	= FALSE ;
//			pad->Effect[ DINPUTPAD_MOTOR_LEFT ].Power		= D_DI_FFNOMINALMAX ;
//			pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].PlayFlag	= FALSE ;
//			pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].Power		= D_DI_FFNOMINALMAX ;
//			pad->EffectPlayStateLeft	= FALSE ;
//			pad->EffectPlayStateRight	= FALSE ;

			// �X�e�[�^�X��������
			_MEMSET( &pad->State, 0, sizeof( D_DIJOYSTATE ) ) ;
			_MEMSET( &pad->XInputState, 0, sizeof( pad->XInputState ) ) ;

			// ���O��ݒ�
			_WCSCPY( pad->InstanceName, L"Controller (XBOX 360 For Windows)" );
			_WCSCPY( pad->ProductName,  L"Controller (XBOX 360 For Windows)" );

			pad ++ ;
			DInputData.JoyPadNum ++ ;
		}
	}

	DInputData.DirectInputObject->EnumDevices( D_DIDEVTYPE_JOYSTICK, EnumJoypadProc, NULL, D_DIEDFL_ATTACHEDONLY ) ;
	DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\x6e\x30\x1d\x52\x1f\x67\x16\x53\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�̏������͐���ɏI�����܂���\n" @*/ ) ;

	// �I��
	return 0;
}

// �W���C�p�b�h�̌�n�����s��
extern int TerminateJoypad( void )
{
	int i ;
	int j ;
	DINPUTPAD *pad ;

	// ����������
	if( DInputData.DirectInputObject == NULL || DInputData.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ) return -1 ;

	// �W���C�p�b�h�f�o�C�X�̉��
	pad = DInputData.Joypad ;
	for( i = 0 ; i < DInputData.JoyPadNum ; i ++ , pad ++ )
	{
		if( pad->XInputDeviceNo < 0 )
		{
			pad->Device->Unacquire() ;
			pad->Device->SetEventNotification( NULL ) ;

			for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
			{
				if( pad->Effect[ j ].DIEffect != NULL )
				{
					pad->Effect[ j ].DIEffect->Stop() ;
					pad->Effect[ j ].DIEffect->Unload() ;
					pad->Effect[ j ].DIEffect = NULL ;
				}
			}

			pad->Device->Release() ;
			pad->Device = NULL ;

			CloseHandle( pad->Event ) ;
		}
		
		_MEMSET( pad, 0, sizeof( *pad ) ) ;
	}

	DInputData.JoyPadNum = 0 ;

	// �I��
	return 0;
}

BOOL CALLBACK EffectEnumCallBack( const D_DIEFFECTINFOA * /*Info*/, void * /*Data*/ )
{
	return D_DIENUM_CONTINUE ;
}

// �W���C�p�b�h�񋓗p�R�[���o�b�N�֐�
BOOL FAR PASCAL EnumJoypadProc( const D_DIDEVICEINSTANCEW *pdinst , LPVOID /*pvRef*/ )
{
	HRESULT hr ;
	D_IDirectInputDevice7 *Joypad ;
	D_DIPROPRANGE diprg ;
	D_DIPROPDWORD dipdw ;
	HANDLE Event ;
	DINPUTPAD *pad ;
	int i ;

	pad = &DInputData.Joypad[ DInputData.JoyPadNum ] ;

	// �W���C�X�e�B�b�N�f�o�C�X�̍쐬
	pad->XInputDeviceNo = -1 ;
	if( DInputData.UseDirectInput8Flag == TRUE )
	{
		hr = DInputData.DirectInputObject->CreateDevice( pdinst->guidInstance, ( D_IDirectInputDevice ** )&Joypad , NULL ) ;
	}
	else
	{
		hr = DInputData.DirectInputObject->CreateDeviceEx( pdinst->guidInstance, IID_IDIRECTINPUTDEVICE7, ( void ** )&Joypad , NULL ) ;
	}
	if( hr != D_DI_OK )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\x65\x51\x9b\x52\xc5\x88\x6e\x7f\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"���͑��u�̎擾�Ɏ��s���܂���\n" @*/ ) ;
		return D_DIENUM_CONTINUE ;
	}

	// ���\��
	{
		D_DIDEVICEINSTANCEW State ;

		_MEMSET( &State, 0, sizeof( State ) ) ;
		State.dwSize = sizeof( State ) ;
		Joypad->GetDeviceInfo( &State ) ;

		// Xbox360 �R���g���[���̏ꍇ�ŁAXInput�Ō��o����Ă�����e��
		if( _WCSSTR( State.tszInstanceName, L"XBOX 360 For Windows" ) != NULL )
		{
			int i ;

			for( i = 0 ; i < DInputData.JoyPadNum ; i ++ )
			{
				if( DInputData.Joypad[ i ].XInputDeviceNo >= 0 )
					break ;
			}
			if( i != DInputData.JoyPadNum )
			{
				Joypad->Release() ;
				Joypad = NULL ;
				return D_DIENUM_CONTINUE ;
			}
		}

		DXST_ERRORLOG_ADDUTF16LE( "\x65\x51\x9b\x52\xc5\x88\x6e\x7f\x92\x30\x8b\x89\x64\x30\x51\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"���͑��u�������܂���\n" @*/ ) ;

		_WCSCPY( pad->InstanceName, State.tszInstanceName );
		_WCSCPY( pad->ProductName,  State.tszProductName  );

		DXST_ERRORLOG_TABADD ;
		DXST_ERRORLOGFMT_ADDW(( L"Device Instance Name : %s", State.tszInstanceName )) ;

		DXST_ERRORLOGFMT_ADDW(( L"Device Product Name  : %s", State.tszProductName )) ;
		DXST_ERRORLOG_TABSUB ;
	}
	
	// �W���C�X�p�b�h�̃f�[�^�`����ݒ�
//	hr = Joypad->SetDataFormat( &c_dfDIJoystick ) ;
	hr = Joypad->SetDataFormat( &C_DFDIJOYSTICK ) ;
	if( hr != D_DI_OK )
	{
		Joypad->Release() ;
		DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x67\x30\x6f\x30\x42\x30\x8a\x30\x7e\x30\x5b\x30\x93\x30\x67\x30\x57\x30\x5f\x30\x01\x30\xc8\x53\x6f\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�f�o�C�X�ł͂���܂���ł����A���͐ݒ�Ɏ��s���܂���\n" @*/ ) ;
		return D_DIENUM_CONTINUE ;
	}

	// �W���C�p�b�h�̋������x����ݒ肷��
//	hr = Joypad->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
	hr = Joypad->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_FOREGROUND | D_DISCL_EXCLUSIVE ) ;
	if( hr != D_DI_OK )
	{
//		Joypad->Release() ;
		DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x54\x53\xbf\x8a\xec\x30\xd9\x30\xeb\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�f�o�C�X�̋������x���̐ݒ�Ɏ��s���܂���\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// �w�͈̔͂�ݒ�
	diprg.diph.dwSize		= sizeof( diprg ) ;
	diprg.diph.dwHeaderSize	= sizeof( diprg.diph ) ;
	diprg.diph.dwObj		= D_DIJOFS_X ;
	diprg.diph.dwHow		= D_DIPH_BYOFFSET ;
	diprg.lMin				= -RANGE ;
	diprg.lMax				= +RANGE ;
	hr = Joypad->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joypad->Release() ;
		DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x38\xff\xf8\x8e\xa2\x95\xc2\x4f\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�f�o�C�X�̂w���֌W�̐ݒ�Ɏ��s���܂���\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// �x�͈̔͂�ݒ�
	diprg.diph.dwObj		= D_DIJOFS_Y ;
	hr = Joypad->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joypad->Release() ;
		DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x39\xff\xf8\x8e\xa2\x95\xc2\x4f\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�f�o�C�X�̂x���֌W�̐ݒ�Ɏ��s���܂���\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// �y�͈̔͂�ݒ�
	diprg.diph.dwObj		= D_DIJOFS_Z ;
	Joypad->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;

	// �q�w�q�x�q�y�͈̔͂�ݒ�
	diprg.diph.dwObj		= D_DIJOFS_RX ;
	Joypad->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	diprg.diph.dwObj		= D_DIJOFS_RY ;
	Joypad->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;
	diprg.diph.dwObj		= D_DIJOFS_RZ ;
	Joypad->SetProperty( D_DIPROP_RANGE , &diprg.diph ) ;

	// �����]�[���̃Z�b�g
	pad->DeadZone = DEADZONE ;

	// �w�̖����]�[����ݒ�
	dipdw.diph.dwSize		= sizeof( dipdw ) ;
	dipdw.diph.dwHeaderSize	= sizeof( dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_X ;
	dipdw.diph.dwHow		= D_DIPH_BYOFFSET ;
	dipdw.dwData			= DEADZONE_DIRECTINPUT( pad->DeadZone ) ;
	hr = Joypad->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joypad->Release() ;
		DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x38\xff\xf8\x8e\x6e\x30\x21\x71\xb9\x52\xbe\x30\xfc\x30\xf3\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�f�o�C�X�̂w���̖����]�[���̐ݒ�Ɏ��s���܂���\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// �x�̖����]�[����ݒ�
	dipdw.diph.dwObj		= D_DIJOFS_Y ;
	hr = Joypad->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	if( hr != D_DI_OK )
	{
//		Joypad->Release() ;
		DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\x39\xff\xf8\x8e\x6e\x30\x21\x71\xb9\x52\xbe\x30\xfc\x30\xf3\x30\x6e\x30\x2d\x8a\x9a\x5b\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�f�o�C�X�̂x���̖����]�[���̐ݒ�Ɏ��s���܂���\n" @*/ ) ;
//		return D_DIENUM_CONTINUE ;
	}

	// �y�̖����]�[����ݒ�
	dipdw.diph.dwObj		= D_DIJOFS_Z ;
	Joypad->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// �q�w�q�x�q�y�̖����]�[����ݒ�
	dipdw.diph.dwObj		= D_DIJOFS_RX ;
	Joypad->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_RY ;
	Joypad->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_RZ ;
	Joypad->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// �t�H�[�X�t�F�[�h�o�b�N�I�u�W�F�N�g�̍쐬(PS�p�b�h�z��)
	{
		DWORD dwAxes[2] = { D_DIJOFS_X, D_DIJOFS_Y } ;
		LONG lDirection[2] = { 0, 0 } ;

//		Joypad->EnumEffects( EffectEnumCallBack, NULL, D_DIEFT_ALL ) ;

		// �E�����[�^�[�p(�茳�̃p�b�h�ł͉��̂� GUID_SINE �G�t�F�N�g�͉E���[�^�[�ɂȂ��Ă�)
		{
			D_DIPERIODIC diPeriodic ;	// �^�C�v�ŗL�p�����[�^
			D_DIENVELOPE diEnvelope ;	// �G���x���[�v
			D_DIEFFECT diEffect ;		// �ėp�p�����[�^

			_MEMSET( &diEffect, 0, sizeof( diEffect ) ) ;
			_MEMSET( &diEnvelope, 0, sizeof( diEnvelope ) ) ;
			_MEMSET( &diPeriodic, 0, sizeof( diPeriodic ) ) ;

			// ���Ƀ^�C�v�ŗL�p�����[�^������������B���̗�̂悤�Ȓl��ݒ肷��ƁA1/20 �b�����̑S�͎����I�G�t�F�N�g�𐶐����邱�ƂɂȂ�B

			diPeriodic.dwMagnitude	= D_DI_FFNOMINALMAX ;
			diPeriodic.lOffset		= 0;
			diPeriodic.dwPhase		= 0;
			diPeriodic.dwPeriod		= (DWORD) (1.5 * D_DI_SECONDS);
			// �`�F�[���\�[�̃��[�^�[���n�����悤�Ƃ��āA�Z���ԊP�̂悤�ȑ������o���A����ɂ��ꂪ�~��ł����Ƃ����G�t�F�N�g�𓾂邽�߂ɁA�G���x���[�v�ɔ��b�Ԃ̃A�^�b�N���Ԃ� 1 �b�Ԃ̃t�F�[�h���Ԃ�ݒ肷��B�������āA�Z���Ԃ̈ێ����Ԓl��������B

			diEnvelope.dwSize			= sizeof(D_DIENVELOPE);
			diEnvelope.dwAttackLevel	= 0;
			diEnvelope.dwAttackTime		= (DWORD) (0.5 * D_DI_SECONDS);
			diEnvelope.dwFadeLevel		= 0;
			diEnvelope.dwFadeTime		= (DWORD) (1.0 * D_DI_SECONDS);
			// ���ɁA��{�I�G�t�F�N�g�p�����[�^��ݒ肷��B�����̃p�����[�^�ɂ́A�����ƃf�o�C�X �I�u�W�F�N�g (�{�^���Ǝ�) �̎��ʕ��@�����肷��t���O�A�G�t�F�N�g�̃T���v�������ƃQ�C���A����т����قǏ����������̃f�[�^�ւ̃|�C���^���܂܂��B����ɁA�W���C�X�e�B�b�N�̔��˃{�^���ɃG�t�F�N�g������t���āA�{�^���������΃G�t�F�N�g�������I�ɍĐ�����悤�ɂ���B

			diEffect.dwSize						= sizeof(D_DIEFFECT);
			diEffect.dwFlags					= D_DIEFF_POLAR | D_DIEFF_OBJECTOFFSETS;
			diEffect.dwDuration					= INFINITE ;
 
			diEffect.dwSamplePeriod				= 0;               // �f�t�H���g�l
			diEffect.dwGain						= D_DI_FFNOMINALMAX;         // �X�P�[�����O�Ȃ�
			diEffect.dwTriggerButton			= D_DIEB_NOTRIGGER ; // D_DIJOFS_BUTTON( 0 );
			diEffect.dwTriggerRepeatInterval	= 0;
			diEffect.cAxes						= 2;
			diEffect.rgdwAxes					= dwAxes;
			diEffect.rglDirection				= &lDirection[0];
			diEffect.lpEnvelope					= &diEnvelope;
			diEffect.cbTypeSpecificParams		= sizeof(diPeriodic);
			diEffect.lpvTypeSpecificParams		= &diPeriodic;
			// �Z�b�g�A�b�v�͂����܂ŁB����ƃG�t�F�N�g�𐶐��ł���B

			pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].DIEffect = NULL ;
			if( DInputData.NoUseVibrationFlag == FALSE )
			{
				Joypad->CreateEffect(
								 GUID_SINE,     // �񋓂���� GUID
								 &diEffect,      // �f�[�^�̏ꏊ
								 &pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].DIEffect,  // �C���^�[�t�F�C�X �|�C���^��u���ꏊ
								 NULL ) ;          // �W�����Ȃ�
			}
		}

		// �������[�^�[�ݒ�(�茳�̃p�b�h�ł͉��̂� GUID_CONSTANTFORCE �G�t�F�N�g�͉E���[�^�[�ɂȂ��Ă�)
		{
			DWORD    rgdwAxes[2] = { D_DIJOFS_X, D_DIJOFS_Y };
			LONG     rglDirection[2] = { 0, 0 };
			D_DICONSTANTFORCE cf = { D_DI_FFNOMINALMAX };

			D_DIEFFECT eff;
			_MEMSET( &eff, 0, sizeof(eff) );
			eff.dwSize                  = sizeof(D_DIEFFECT);
			eff.dwFlags                 = D_DIEFF_CARTESIAN | D_DIEFF_OBJECTOFFSETS;
			eff.dwDuration              = INFINITE;
			eff.dwSamplePeriod          = 0;
			eff.dwGain                  = D_DI_FFNOMINALMAX;
			eff.dwTriggerButton         = D_DIEB_NOTRIGGER;
			eff.dwTriggerRepeatInterval = 0;
			eff.cAxes                   = 2;
			eff.rgdwAxes                = rgdwAxes;
			eff.rglDirection            = rglDirection;
			eff.lpEnvelope              = 0;
			eff.cbTypeSpecificParams    = sizeof(D_DICONSTANTFORCE);
			eff.lpvTypeSpecificParams   = &cf;
			eff.dwStartDelay            = 0;

			pad->Effect[ DINPUTPAD_MOTOR_LEFT ].DIEffect = NULL ;
			if( DInputData.NoUseVibrationFlag == FALSE )
			{
				hr = Joypad->CreateEffect(
								 GUID_CONSTANTFORCE,	// �񋓂���� GUID
								 &eff,					// �f�[�^�̏ꏊ
								 &pad->Effect[ DINPUTPAD_MOTOR_LEFT ].DIEffect,  // �C���^�[�t�F�C�X �|�C���^��u���ꏊ
								 NULL					// �W�����Ȃ�
				) ;
				if( hr != D_DI_OK )
				{
					DXST_ERRORLOG_ADDUTF16LE(( "\x68\x54\x1f\x67\x84\x76\xa8\x30\xd5\x30\xa7\x30\xaf\x30\xc8\x30\x6e\x30\x5c\x4f\x10\x62\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x02\x30\x0a\x00\x00"/*@ L"�����I�G�t�F�N�g�̍쐬�Ɏ��s���܂����B\n" @*/ )) ;
				}
			}
		}

		// �U���֌W�̐ݒ�̏��������s��
		for( i = 0 ; i < DINPUTPAD_MOTOR_NUM ; i ++ )
		{
			pad->Effect[ i ].PlayFlag  = FALSE ;
			pad->Effect[ i ].Power     = D_DI_FFNOMINALMAX ;
			pad->Effect[ i ].PlayState = FALSE ;
			pad->Effect[ i ].SetPower  = 0 ;
		}
//		pad->EffectNotCommonFlag    = FALSE ;
//		pad->EffectCommon.PlayFlag	= FALSE ;
//		pad->EffectCommon.Power		= D_DI_FFNOMINALMAX ;
//		pad->Effect[ DINPUTPAD_MOTOR_LEFT ].PlayFlag	= FALSE ;
//		pad->Effect[ DINPUTPAD_MOTOR_LEFT ].Power		= D_DI_FFNOMINALMAX ;
//		pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].PlayFlag	= FALSE ;
//		pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].Power		= D_DI_FFNOMINALMAX ;
//		pad->EffectPlayStateLeft	= FALSE ;
//		pad->EffectPlayStateRight	= FALSE ;
	}

	// �ω��ʒm�p�̃C�x���g���쐬�E�A�^�b�`����
	Event = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
	hr = Joypad->SetEventNotification( Event ) ;
	if( hr != D_DI_POLLEDDEVICE && hr != D_DI_OK )
	{
		DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\xa4\x30\xd9\x30\xf3\x30\xc8\x30\x6e\x30\xbb\x30\xc3\x30\xc8\x30\xa2\x30\xc3\x30\xd7\x30\x4c\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�C�x���g�̃Z�b�g�A�b�v�����s���܂���\n" @*/ ) ;
		return D_DIENUM_CONTINUE ;
	}

	// �f�o�C�X���擾����
	Joypad->Acquire() ;

	// �C�x���g�n���h����ۑ�
	pad->Event = Event ;

	// �W���C�p�b�h�f�o�C�X�̃A�h���X��ۑ�
	pad->Device = Joypad ;

	// �X�e�[�^�X��������
	_MEMSET( &pad->State, 0, sizeof( D_DIJOYSTATE ) ) ;

	// �W���C�p�b�h�̐��𑝂₷
	DInputData.JoyPadNum ++ ;

	DXST_ERRORLOG_ADDUTF16LE( "\xb8\x30\xe7\x30\xa4\x30\xd1\x30\xc3\x30\xc9\x30\x6e\x30\xfd\x8f\xa0\x52\x6f\x30\x63\x6b\x38\x5e\x6b\x30\x42\x7d\x86\x4e\x57\x30\x7e\x30\x57\x30\x5f\x30\x0a\x00\x00"/*@ L"�W���C�p�b�h�̒ǉ��͐���ɏI�����܂���\n" @*/ ) ;

	// �I��
	return ( DInputData.JoyPadNum != MAX_JOYPAD_NUM ) ? D_DIENUM_CONTINUE : D_DIENUM_STOP ;
}










// ���͏�Ԏ擾�֐�

// �L�[�{�[�h�̏�Ԏ擾
extern int NS_CheckHitKey( int KeyCode )
{
	int Result ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL )
	{
		if( NS_GetWindowCloseFlag() == FALSE )
			return InitializeDirectInput() ;
		else
			return 0;
	}

/*
	if( KeyCode == KEY_INPUT_F10 )
	{
		int Result ;
		
		F10Process() ;
		Result = DInputData.F10Flag ;

		
		
		return Result ;
	}
*/

	// �L�[�{�[�h�̓��͏�Ԃ��X�V
	UpdateKeyboardInputState() ;

	// �w��̃L�[�̏�Ԃ�Ԃ�
	Result = ( DInputData.KeyInputBuf[ KeyCode ] & 0x80 ) != 0 ;
	

	
	

	return Result ;
}

// �S�L�[�̉�����Ԃ��擾
extern int NS_CheckHitKeyAll( int CheckType )
{
	int i ;
	unsigned char *KeyData ;

	if( WinData.MainWindow == NULL )
		return -1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	// �L�[�{�[�h�̃`�F�b�N
	if( CheckType & DX_CHECKINPUT_KEY )
	{
		// �L�[�{�[�h�̓��͏�Ԃ��X�V
		UpdateKeyboardInputState() ;

		// �ǂꂩ��ł��L�[��������Ă��邩����
		KeyData = DInputData.KeyInputBuf ;
		for( i = 0 ; i < 256 ; i ++ , KeyData ++ )
		{
			if( ( *KeyData & 0x80 ) && i != D_DIK_KANJI && i != D_DIK_CAPITAL && i != D_DIK_KANA )
				return i ;
		}
		F10Process() ;
		if( DInputData.F10Flag )
		{
			return KEY_INPUT_F10 ;
		}
	}

	// �W���C�p�b�h�̃`�F�b�N
	if( CheckType & DX_CHECKINPUT_PAD )
	{
		for( i = 0 ; i < DInputData.JoyPadNum ; i ++ )
		{
			if( NS_GetJoypadInputState( i + 1 ) != 0 )
				return -1 ;
		}
	}

	// �}�E�X�{�^���̃`�F�b�N
	if( CheckType & DX_CHECKINPUT_MOUSE )
	{
		int x, y ;
		RECT Rect ;

		NS_GetMousePoint( &x, &y ) ;
		NS_GetWindowCRect( &Rect ) ;
		if( x >= 0                      && y >= 0                      && NS_GetWindowActiveFlag() &&
			x <= Rect.right - Rect.left && y <= Rect.bottom - Rect.top && NS_GetMouseInput() != 0 )
			return -1 ;
	}

	// �I��
	return 0 ;
}

// �}�E�X�̏��擾�ɂc�����������h�����������g�p���Ă��邩�ǂ������擾����( �߂�l  TRUE:DirectInput���g�p���Ă���  FALSE:DirectInput���g�p���Ă��Ȃ� )
extern	int CheckUseDirectInputMouse( void )
{
	if( DInputData.NoUseDirectInputFlag == TRUE || DInputData.MouseDeviceObject == NULL || ( DInputData.UseDirectInputFlag == FALSE && WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA ) )
	{
		return FALSE ;
	}

	return TRUE ;
}

// �c�����������h�����������g�p�����}�E�X�z�C�[���̈ړ��l���擾����
extern	int GetDirectInputMouseMoveZ( int CounterReset )
{
	int Result ;

	NS_GetMouseInput() ;

	Result = DInputData.MouseMoveZ / WHEEL_DELTA ;
	if( CounterReset )
	{
		DInputData.MouseMoveZ -= Result * WHEEL_DELTA ;
	}

	return Result ;
}

// �c�����������h�����������g�p�����}�E�X�z�C�[���̈ړ��l���擾����( float�� )
extern	float GetDirectInputMouseMoveZF( int CounterReset )
{
	float Result ;

	NS_GetMouseInput() ;

	Result = ( float )DInputData.MouseMoveZ / WHEEL_DELTA ;
	if( CounterReset )
	{
		DInputData.MouseMoveZ = 0 ;
	}

	return Result ;
}

// �}�E�X�̃{�^���̏�Ԃ𓾂� 
extern int NS_GetMouseInput( void )
{
	D_DIMOUSESTATE2 input ;
	HRESULT hr ;
	int res ;

	// DirectInput ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( CheckUseDirectInputMouse() == FALSE )
	{
		// DirectInput ���g�p���Ȃ��ꍇ
		UpdateKeyboardInputState() ;
		if( DInputData.MouseInputBufValidFlag == TRUE )
		{
			res = 0 ;
			if( DInputData.MouseInputBuf[ 0 ] ) res |= MOUSE_INPUT_LEFT ;
			if( DInputData.MouseInputBuf[ 1 ] ) res |= MOUSE_INPUT_RIGHT ;
			if( DInputData.MouseInputBuf[ 2 ] ) res |= MOUSE_INPUT_MIDDLE ;
		}
		else
		{
			unsigned char KeyBuffer[256] ;

			GetKeyboardState( KeyBuffer ) ;
			res = 0 ;
			if( KeyBuffer[ VK_LBUTTON ] & 0x80 ) res |= MOUSE_INPUT_LEFT ;
			if( KeyBuffer[ VK_RBUTTON ] & 0x80 ) res |= MOUSE_INPUT_RIGHT ;
			if( KeyBuffer[ VK_MBUTTON ] & 0x80 ) res |= MOUSE_INPUT_MIDDLE ;
		}
	}
	else
	{
		// DirectInput ���g�p����ꍇ

		hr = DInputData.MouseDeviceObject->GetDeviceState( sizeof( D_DIMOUSESTATE2 ), (LPVOID)&input ) ;
		if( hr != D_DI_OK )
		{
			if( DInputData.MouseDeviceObject->Acquire() == D_DI_OK )
			{
				hr = DInputData.MouseDeviceObject->GetDeviceState( sizeof( D_DIMOUSESTATE2 ) , (LPVOID)&input ) ;
				if( hr != D_DI_OK ) return 0 ;
			}
			else return 0 ;
		}

		res = 0 ;
		if( input.rgbButtons[0] & 0x80 ) res |= MOUSE_INPUT_1 ;
		if( input.rgbButtons[1] & 0x80 ) res |= MOUSE_INPUT_2 ;
		if( input.rgbButtons[2] & 0x80 ) res |= MOUSE_INPUT_3 ;
		if( input.rgbButtons[3] & 0x80 ) res |= MOUSE_INPUT_4 ;
		if( input.rgbButtons[4] & 0x80 ) res |= MOUSE_INPUT_5 ;
		if( input.rgbButtons[5] & 0x80 ) res |= MOUSE_INPUT_6 ;
		if( input.rgbButtons[6] & 0x80 ) res |= MOUSE_INPUT_7 ;
		if( input.rgbButtons[7] & 0x80 ) res |= MOUSE_INPUT_8 ;

		DInputData.MouseMoveZ += ( int )input.lZ ;
	}

	return res ;
}

// ���ׂẴL�[�̉�����Ԃ��擾����
extern int NS_GetHitKeyStateAll( char *KeyStateBuf )
{
	int i ;
//	char *Buf = (char *)KeyStateBuf ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	// �L�[�{�[�h�̓��͏�Ԃ��X�V
	UpdateKeyboardInputState() ;

	// �L�[�̓��͒l��ϊ�
	for( i = 0 ; i < 256 ; i ++ , KeyStateBuf ++ )
	{
		*KeyStateBuf = ( BYTE )( DInputData.KeyInputBuf[i] ) >> 7 ;
	}
	F10Process() ;
//	Buf[KEY_INPUT_F10] = (char)DInputData.F10Flag ;
//	Buf[KEY_INPUT_F12] = (char)DInputData.F12Flag ;

	// �I��
	return 0 ;
}

// �L�[�{�[�h�̋������x����r�����x���ɂ��邩�ǂ����̃t���O���Z�b�g����
extern int NS_SetKeyExclusiveCooperativeLevelFlag( int Flag )
{
	int hr ;

	// ����������Ă��Ȃ������牽�������I��
	if( DInputData.KeyboardDeviceObject == NULL )
		return 0 ;

	// ��Ԃ������������牽�������I��
	if( DInputData.KeyExclusiveCooperativeLevelFlag == Flag )
		return 0 ;

	// ��x�A�N�Z�X�����������
	DInputData.KeyboardDeviceObject->Unacquire() ;

	// �������x�����Z�b�g
	DInputData.KeyboardDeviceObject->SetCooperativeLevel( NS_GetMainWindowHandle(),
			( DWORD )( Flag == TRUE ? ( D_DISCL_EXCLUSIVE | D_DISCL_FOREGROUND ) : ( D_DISCL_NONEXCLUSIVE | D_DISCL_BACKGROUND ) ) ) ;

	// �A�N�Z�X�����擾����
	hr = DInputData.KeyboardDeviceObject->Acquire() ;
	if( hr != D_DI_OK )
	{
		hr = DInputData.KeyboardDeviceObject->Acquire() ;
		if( hr != D_DI_OK )
		{
			DXST_ERRORLOG_ADDUTF16LE( "\xad\x30\xfc\x30\xdc\x30\xfc\x30\xc9\x30\xc7\x30\xd0\x30\xa4\x30\xb9\x30\x6e\x30\xd6\x53\x97\x5f\x6b\x30\x31\x59\x57\x65\x57\x30\x7e\x30\x57\x30\x5f\x30\x00"/*@ L"�L�[�{�[�h�f�o�C�X�̎擾�Ɏ��s���܂���" @*/ ) ;
			return -1 ;
		}
	}

	// �t���O��ۑ�����
	DInputData.KeyExclusiveCooperativeLevelFlag = Flag ;

	// �I��
	return 0 ;
}




// �W���C�p�b�h���ڑ�����Ă��鐔���擾����
extern int NS_GetJoypadNum( void )
{
	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	return DInputData.JoyPadNum ;
}

// �W���C�o�b�h�̓��͏�Ԏ擾
extern int NS_GetJoypadInputState( int InputType )
{
	int BackData = 0 ;
	DINPUTPAD *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	int iX , iY ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	if( ( JoypadNum < -1 || JoypadNum >= DInputData.JoyPadNum ) && 
		( DInputData.JoyPadNum != 0 && ( InputType & DX_INPUT_KEY ) == 0 ) )
	{
		return 0 ;
	}

	if( JoypadNum != -1 && DInputData.JoyPadNum != 0 && JoypadNum < DInputData.JoyPadNum )
	{
		pad = &DInputData.Joypad[ JoypadNum ] ;

		// �p�b�h�̏����擾����
		{
			// ���̍X�V
			UpdateJoypadInputState( JoypadNum ) ;

			// ���͏�Ԃ�ۑ�
			iX = pad->State.lX ;
			iY = pad->State.lY ;

			// �J�[�\���{�^������
			     if( iX < 0 ) BackData |= PAD_INPUT_LEFT ;			// �����͔���
			else if( iX > 0 ) BackData |= PAD_INPUT_RIGHT ;			// �����͔���

			     if( iY < 0 ) BackData |= PAD_INPUT_UP ;			// �����͔���
			else if( iY > 0 ) BackData |= PAD_INPUT_DOWN ;			// �����͔���

			// �{�^�����͔���
			{
				int i ;

				// ���͏�Ԃ�ۑ�
				for( i = 0 ; i < 24 ; i ++ )
				{
					BackData |= ( pad->State.rgbButtons[ i ] & 0x80 ) != 0 ? 1 << ( i + 4 ) : 0 ;
				}
			}
		}
	}

	// �L�[���͎w����������ꍇ�̓L�[�̓��͏�Ԃ����f������
	if( InputType & DX_INPUT_KEY )
	{
		BYTE *KeyBuf ;
		int *Map ;
		int i, j ;
		unsigned int pad ;

		// �L�[�{�[�h�̓��͏�Ԃ��X�V
		UpdateKeyboardInputState() ;

		KeyBuf = DInputData.KeyInputBuf ;
		Map = DInputData.KeyToJoypadInput[ JoypadNum < 0 ? 0 : JoypadNum ][ 0 ] ;

		pad = 1 ;
		for( i = 0 ; i < 32 ; i ++, pad <<= 1, Map += 4 )
		{
			for( j = 0 ; j < 4 ; j ++ )
			{
				if( Map[ j ] > 0 && ( KeyBuf[ Map[ j ] ] & 0x80 ) )
				{
					BackData |= ( int )pad ;
					break ; 
				}
			}
		}
/*
		if( KeyBuf[ D_DIK_NUMPAD2 ] & 0x80 || KeyBuf[ D_DIK_DOWN  ] & 0x80 ) BackData |= PAD_INPUT_DOWN  ;		// �Q�L�[���L�[�`�F�b�N
		if( KeyBuf[ D_DIK_NUMPAD4 ] & 0x80 || KeyBuf[ D_DIK_LEFT  ] & 0x80 ) BackData |= PAD_INPUT_LEFT  ;		// �S�L�[���`�F�b�N
		if( KeyBuf[ D_DIK_NUMPAD6 ] & 0x80 || KeyBuf[ D_DIK_RIGHT ] & 0x80 ) BackData |= PAD_INPUT_RIGHT ;		// �U�L�[���L�[�`�F�b�N
		if( KeyBuf[ D_DIK_NUMPAD8 ] & 0x80 || KeyBuf[ D_DIK_UP    ] & 0x80 ) BackData |= PAD_INPUT_UP    ;		// �W�L�[���L�[�`�F�b�N

		if( KeyBuf[ D_DIK_SPACE  ] & 0x80 ) BackData |= PAD_INPUT_M ;											// �X�y�[�X�`�F�b�N
		if( KeyBuf[ D_DIK_ESCAPE ] & 0x80 ) BackData |= PAD_INPUT_START ;										// �G�X�P�[�v�`�F�b�N
		if( KeyBuf[ D_DIK_Z ] & 0x80 ) BackData |= PAD_INPUT_A ;												// �y�L�[�`�F�b�N
		if( KeyBuf[ D_DIK_X ] & 0x80 ) BackData |= PAD_INPUT_B ;												// �w�L�[�`�F�b�N
		if( KeyBuf[ D_DIK_C ] & 0x80 ) BackData |= PAD_INPUT_C ;												// �b�L�[�`�F�b�N
		if( KeyBuf[ D_DIK_A ] & 0x80 ) BackData |= PAD_INPUT_X ;												// �`�L�[�`�F�b�N
		if( KeyBuf[ D_DIK_S ] & 0x80 ) BackData |= PAD_INPUT_Y ;												// �r�L�[�`�F�b�N
		if( KeyBuf[ D_DIK_D ] & 0x80 ) BackData |= PAD_INPUT_Z ;												// �c�L�[�`�F�b�N
		if( KeyBuf[ D_DIK_Q ] & 0x80 ) BackData |= PAD_INPUT_L ;												// �p�L�[�`�F�b�N
		if( KeyBuf[ D_DIK_W ] & 0x80 ) BackData |= PAD_INPUT_R ;												// �v�L�[�`�F�b�N
*/
	}

	// �I��
	return BackData ;
}

// �W���C�p�b�h�̃A�i���O�I�ȃX�e�B�b�N���͏��𓾂�
extern int NS_GetJoypadAnalogInput( int *XBuf , int *YBuf , int InputType )
{
	int BackData = 0 ;
	DINPUTPAD *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;
	
	if( XBuf != NULL ) *XBuf = 0 ;
	if( YBuf != NULL ) *YBuf = 0 ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	if( ( JoypadNum < -1 || JoypadNum >= DInputData.JoyPadNum ) && 
		( DInputData.JoyPadNum != 0 && ( InputType & DX_INPUT_KEY ) == 0 ) )
	{
		return 0 ;
	}

	if( JoypadNum != -1 && DInputData.JoyPadNum != 0 )
	{
		pad = &DInputData.Joypad[ JoypadNum ] ;

		// ���͏�Ԃ̍X�V
		UpdateJoypadInputState( JoypadNum ) ;

		// ���͏�Ԃ�ۑ�
		if( XBuf ) *XBuf = pad->State.lX ;
		if( YBuf ) *YBuf = pad->State.lY ;
	}

	// �L�[���͎w����������ꍇ�̓L�[�̓��͏�Ԃ����f������
	if( InputType & DX_INPUT_KEY )
	{
		BYTE *KeyBuf ;

		// �L�[�{�[�h�̓��͏�Ԃ��X�V
		UpdateKeyboardInputState() ;

		KeyBuf = DInputData.KeyInputBuf ;
		if( XBuf )
		{
			if( KeyBuf[ D_DIK_NUMPAD4 ] & 0x80 || KeyBuf[ D_DIK_LEFT  ] & 0x80 ) *XBuf = -RANGE  ;		// �S�L�[���`�F�b�N
			if( KeyBuf[ D_DIK_NUMPAD6 ] & 0x80 || KeyBuf[ D_DIK_RIGHT ] & 0x80 ) *XBuf = RANGE	;		// �U�L�[���L�[�`�F�b�N
		}
		if( YBuf )
		{
			if( KeyBuf[ D_DIK_NUMPAD2 ] & 0x80 || KeyBuf[ D_DIK_DOWN  ] & 0x80 ) *YBuf = RANGE	;		// �Q�L�[���L�[�`�F�b�N
			if( KeyBuf[ D_DIK_NUMPAD8 ] & 0x80 || KeyBuf[ D_DIK_UP    ] & 0x80 ) *YBuf = -RANGE  ;		// �W�L�[���L�[�`�F�b�N
		}
	}

	// �I��
	return BackData ;
}

// �W���C�p�b�h�̃A�i���O�I�ȃX�e�B�b�N���͏��𓾂�(�E�X�e�B�b�N�p)
extern int NS_GetJoypadAnalogInputRight( int *XBuf, int *YBuf, int InputType )
{
	int BackData = 0 ;
	DINPUTPAD *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;
	
	if( XBuf != NULL ) *XBuf = 0 ;
	if( YBuf != NULL ) *YBuf = 0 ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	if( JoypadNum < -1 || JoypadNum >= DInputData.JoyPadNum )
		return 0 ;

	if( JoypadNum != -1 )
	{
		pad = &DInputData.Joypad[ JoypadNum ] ;

		// ���͏�Ԃ̍X�V
		UpdateJoypadInputState( JoypadNum ) ;

		// ���͏�Ԃ�ۑ�
		if( pad->XInputDeviceNo >= 0 )
		{
			if( XBuf ) *XBuf = pad->State.lRx ;
			if( YBuf ) *YBuf = pad->State.lRy ;
		}
		else
		{
			if( XBuf ) *XBuf = pad->State.lZ ;
			if( YBuf ) *YBuf = pad->State.lRz ;
		}
	}

	// �I��
	return BackData ;
}

// �W���C�p�b�h���� DirectInput ���瓾���鐶�̃f�[�^���擾����( DX_CHECKINPUT_KEY �� DX_INPUT_KEY_PAD1 �������ɓn���ƃG���[ )
extern	int	NS_GetJoypadDirectInputState( int InputType, DINPUT_JOYSTATE *DInputState )
{
	DINPUTPAD *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum )
	{
		_MEMSET( DInputState, 0, sizeof( DINPUT_JOYSTATE ) ) ;
		DInputState->POV[ 0 ] = 0xffffffff ;
		DInputState->POV[ 1 ] = 0xffffffff ;
		DInputState->POV[ 2 ] = 0xffffffff ;
		DInputState->POV[ 3 ] = 0xffffffff ;
		return -1 ;
	}

	pad = &DInputData.Joypad[ JoypadNum ] ;

	// ���͏�Ԃ̍X�V
	UpdateJoypadInputState( JoypadNum ) ;

	// ���͏�Ԃ�������
	if( DInputState )
	{
		DInputState->X = pad->State.lX ;
		DInputState->Y = pad->State.lY ;
		DInputState->Z = pad->State.lZ ;
		DInputState->Rx = pad->State.lRx ;
		DInputState->Ry = pad->State.lRy ;
		DInputState->Rz = pad->State.lRz ;
		DInputState->Slider[ 0 ] = pad->State.rglSlider[ 0 ] ;
		DInputState->Slider[ 1 ] = pad->State.rglSlider[ 1 ] ;
		DInputState->POV[ 0 ] = pad->State.rgdwPOV[ 0 ] ;
		DInputState->POV[ 1 ] = pad->State.rgdwPOV[ 1 ] ;
		DInputState->POV[ 2 ] = pad->State.rgdwPOV[ 2 ] ;
		DInputState->POV[ 3 ] = pad->State.rgdwPOV[ 3 ] ;
		DInputState->Buttons[ 0 ] = pad->State.rgbButtons[ 0 ] ;
		DInputState->Buttons[ 1 ] = pad->State.rgbButtons[ 1 ] ;
		DInputState->Buttons[ 2 ] = pad->State.rgbButtons[ 2 ] ;
		DInputState->Buttons[ 3 ] = pad->State.rgbButtons[ 3 ] ;
		DInputState->Buttons[ 4 ] = pad->State.rgbButtons[ 4 ] ;
		DInputState->Buttons[ 5 ] = pad->State.rgbButtons[ 5 ] ;
		DInputState->Buttons[ 6 ] = pad->State.rgbButtons[ 6 ] ;
		DInputState->Buttons[ 7 ] = pad->State.rgbButtons[ 7 ] ;
		DInputState->Buttons[ 8 ] = pad->State.rgbButtons[ 8 ] ;
		DInputState->Buttons[ 9 ] = pad->State.rgbButtons[ 9 ] ;
		DInputState->Buttons[ 10 ] = pad->State.rgbButtons[ 10 ] ;
		DInputState->Buttons[ 11 ] = pad->State.rgbButtons[ 11 ] ;
		DInputState->Buttons[ 12 ] = pad->State.rgbButtons[ 12 ] ;
		DInputState->Buttons[ 13 ] = pad->State.rgbButtons[ 13 ] ;
		DInputState->Buttons[ 14 ] = pad->State.rgbButtons[ 14 ] ;
		DInputState->Buttons[ 15 ] = pad->State.rgbButtons[ 15 ] ;
		DInputState->Buttons[ 16 ] = pad->State.rgbButtons[ 16 ] ;
		DInputState->Buttons[ 17 ] = pad->State.rgbButtons[ 17 ] ;
		DInputState->Buttons[ 18 ] = pad->State.rgbButtons[ 18 ] ;
		DInputState->Buttons[ 19 ] = pad->State.rgbButtons[ 19 ] ;
		DInputState->Buttons[ 20 ] = pad->State.rgbButtons[ 20 ] ;
		DInputState->Buttons[ 21 ] = pad->State.rgbButtons[ 21 ] ;
		DInputState->Buttons[ 22 ] = pad->State.rgbButtons[ 22 ] ;
		DInputState->Buttons[ 23 ] = pad->State.rgbButtons[ 23 ] ;
		DInputState->Buttons[ 24 ] = pad->State.rgbButtons[ 24 ] ;
		DInputState->Buttons[ 25 ] = pad->State.rgbButtons[ 25 ] ;
		DInputState->Buttons[ 26 ] = pad->State.rgbButtons[ 26 ] ;
		DInputState->Buttons[ 27 ] = pad->State.rgbButtons[ 27 ] ;
		DInputState->Buttons[ 28 ] = pad->State.rgbButtons[ 28 ] ;
		DInputState->Buttons[ 29 ] = pad->State.rgbButtons[ 29 ] ;
		DInputState->Buttons[ 30 ] = pad->State.rgbButtons[ 30 ] ;
		DInputState->Buttons[ 31 ] = pad->State.rgbButtons[ 31 ] ;
	}

	// ����I��
	return 0 ;
}

// �w��̓��̓f�o�C�X�� XInput �ɑΉ����Ă��邩�ǂ������擾����
// ( �߂�l  TRUE:XInput�Ή��̓��̓f�o�C�X  FALSE:XInput��Ή��̓��̓f�o�C�X   -1:�G���[ )
// ( DX_INPUT_KEY �� DX_INPUT_KEY_PAD1 �ȂǁA�L�[�{�[�h�����ރ^�C�v�� InputType �ɓn���ƃG���[�ƂȂ� -1 ��Ԃ� )
extern int NS_CheckJoypadXInput( int InputType )
{
	DINPUTPAD *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum )
		return -1 ;

	pad = &DInputData.Joypad[ JoypadNum ] ;

	return pad->XInputDeviceNo < 0 ? FALSE : TRUE ;
}

// XInput ���瓾������̓f�o�C�X( Xbox360�R���g���[���� )�̐��̃f�[�^���擾����( XInput��Ή��̃p�b�h�̏ꍇ�̓G���[�ƂȂ� -1 ��Ԃ��ADX_INPUT_KEY �� DX_INPUT_KEY_PAD1 �ȂǁA�L�[�{�[�h�����ރ^�C�v�� InputType �ɓn���ƃG���[�ƂȂ� -1 ��Ԃ� )
extern int NS_GetJoypadXInputState(	int InputType, XINPUT_STATE *XInputState )
{
	DINPUTPAD *pad ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum )
	{
		_MEMSET( XInputState, 0, sizeof( XINPUT_STATE ) ) ;
		return -1 ;
	}

	pad = &DInputData.Joypad[ JoypadNum ] ;

	if( pad->XInputDeviceNo < 0 )
	{
		_MEMSET( XInputState, 0, sizeof( XINPUT_STATE ) ) ;
		return -1 ;
	}

	// ���͏�Ԃ̍X�V
	UpdateJoypadInputState( JoypadNum ) ;

	// ���͏�Ԃ�������
	if( XInputState )
	{
		XInputState->Buttons[ 0  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 0 ) & 1 ) ;
		XInputState->Buttons[ 1  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 1 ) & 1 ) ;
		XInputState->Buttons[ 2  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 2 ) & 1 ) ;
		XInputState->Buttons[ 3  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 3 ) & 1 ) ;
		XInputState->Buttons[ 4  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 4 ) & 1 ) ;
		XInputState->Buttons[ 5  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 5 ) & 1 ) ;
		XInputState->Buttons[ 6  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 6 ) & 1 ) ;
		XInputState->Buttons[ 7  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 7 ) & 1 ) ;
		XInputState->Buttons[ 8  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 8 ) & 1 ) ;
		XInputState->Buttons[ 9  ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 9 ) & 1 ) ;
		XInputState->Buttons[ 10 ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 10 ) & 1 ) ;
		XInputState->Buttons[ 11 ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 11 ) & 1 ) ;
		XInputState->Buttons[ 12 ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 12 ) & 1 ) ;
		XInputState->Buttons[ 13 ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 13 ) & 1 ) ;
		XInputState->Buttons[ 14 ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 14 ) & 1 ) ;
		XInputState->Buttons[ 15 ] = ( unsigned char )( ( pad->XInputState.Gamepad.wButtons >> 15 ) & 1 ) ;
		XInputState->LeftTrigger   = pad->XInputState.Gamepad.bLeftTrigger ;
		XInputState->RightTrigger  = pad->XInputState.Gamepad.bRightTrigger ;
		XInputState->ThumbLX       = pad->XInputState.Gamepad.sThumbLX ;
		XInputState->ThumbLY       = pad->XInputState.Gamepad.sThumbLY ;
		XInputState->ThumbRX       = pad->XInputState.Gamepad.sThumbRX ;
		XInputState->ThumbRY       = pad->XInputState.Gamepad.sThumbRY ;
	}

	// ����I��
	return 0 ;
}

// �L�[�{�[�h�̃o�b�t�@����f�[�^���擾���鏈��(�o�b�t�@����ɂ��邾�������ǁE�E�E)
extern int NS_KeyboradBufferProcess( void )
{
	D_DIDEVICEOBJECTDATA_DX7 data7[5] ;
	D_DIDEVICEOBJECTDATA_DX8 data8[5] ;
	DWORD num ;

	if( DInputData.KeyboardDeviceObject == NULL )
		return 0 ;

	num = 5 ;
	while( num == 5 )
	{
		if( DInputData.UseDirectInput8Flag == FALSE )
		{
			if( DInputData.KeyboardDeviceObject->GetDeviceData( sizeof( D_DIDEVICEOBJECTDATA_DX7 ), data7, &num, 0 ) != D_DI_OK ) return 0 ;
		}
		else
		{
			if( DInputData.KeyboardDeviceObject->GetDeviceData( sizeof( D_DIDEVICEOBJECTDATA_DX8 ), data8, &num, 0 ) != D_DI_OK ) return 0 ;
		}
//		if( num != 0 ) DXST_ERRORLOGFMT_ADDW(( L"�����ꂽ�������ꂽ�������L�[:0x%x  0x%x \n", data[0].dwOfs, data[0].dwData & 0xff )) ;
	}
	
	return 0 ;
}

// �W���C�p�b�h�̂f�tI�c�𓾂�
extern int NS_GetJoypadGUID( int PadIndex, GUID *GuidBuffer )
{
	D_DIDEVICEINSTANCEW inst ;

	if( DInputData.JoyPadNum <= PadIndex )
		return -1 ;

	if( DInputData.Joypad[ PadIndex ].Device == NULL ) return -1 ;

	_MEMSET( &inst, 0, sizeof( inst ) ) ;
	inst.dwSize = sizeof( inst ) ;
	DInputData.Joypad[ PadIndex ].Device->GetDeviceInfo( &inst ) ;
	*GuidBuffer = inst.guidInstance ;

	return 0 ;
}

// �c�w���C�u�����̃L�[�R�[�h���� Windows �̉��z�L�[�R�[�h���擾����
extern int NS_ConvertKeyCodeToVirtualKey( int KeyCode )
{
	int i ;

	for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
	{
		if( KeyCode == __KeyMap[i][1] ) break ;
	}

	return __KeyMap[i][0] ;
}

//  Windows �̉��z�L�[�R�[�h( VK_LEFT �Ȃ� ) �ɑΉ�����c�w���C�u�����̃L�[�R�[�h( KEY_INPUT_A �Ȃ� )���擾����( VirtualKey:�ϊ�������Windows�̉��z�L�[�R�[�h�@�߂�l�F�c�w���C�u�����̃L�[�R�[�h )
extern int NS_ConvertVirtualKeyToKeyCode( int VirtualKey )
{
	int i ;

	for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
	{
		if( VirtualKey == __KeyMap[i][0] ) break ;
	}

	return __KeyMap[i][1] ;
}

// �W���C�p�b�h�̓��͂ɑΉ������L�[�{�[�h�̓��͂�ݒ肷��
extern	int NS_SetJoypadInputToKeyInput( int InputType, int PadInput, int KeyInput1, int KeyInput2 , int KeyInput3 , int KeyInput4 )
{
	unsigned int Bit , i ;

	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;

	if( JoypadNum < 0 || JoypadNum >= MAX_JOYPAD_NUM )
		return 0 ;

	Bit = (unsigned int)PadInput ;
	for( i = 0 ; i < 32 ; i ++ )
	{
		if( Bit & ( 1 << i ) )
		{
			DInputData.KeyToJoypadInput[ JoypadNum ][ i ][ 0 ] = KeyInput1 ;
			DInputData.KeyToJoypadInput[ JoypadNum ][ i ][ 1 ] = KeyInput2 ;
			DInputData.KeyToJoypadInput[ JoypadNum ][ i ][ 2 ] = KeyInput3 ;
			DInputData.KeyToJoypadInput[ JoypadNum ][ i ][ 3 ] = KeyInput4 ;
		}
	}

	// ����I��
	return 0 ;
}

// �W���C�p�b�h�̖����]�[���̐ݒ���s��
extern	int NS_SetJoypadDeadZone( int InputType, double Zone )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	DINPUTPAD *pad = &DInputData.Joypad[ JoypadNum ] ;
	D_DIPROPDWORD dipdw ;
	DWORD ZoneI ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;
	
	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	// �G���[�`�F�b�N
	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum || pad->Device == NULL  )
		return 0 ;

	// ���܂łƓ����]�[���̏ꍇ�͉��������I��
	ZoneI = ( DWORD )_DTOL( Zone * 65536 ) ;
	if( pad->DeadZone == ZoneI )
		return 0 ;

	// �]�[����ۑ�
	pad->DeadZone = ZoneI ;

	// �w�̖����]�[����ݒ�
	dipdw.diph.dwSize		= sizeof( dipdw ) ;
	dipdw.diph.dwHeaderSize	= sizeof( dipdw.diph ) ;
	dipdw.diph.dwObj		= D_DIJOFS_X ;
	dipdw.diph.dwHow		= D_DIPH_BYOFFSET ;
	dipdw.dwData			= 10000 * pad->DeadZone / 65536 ;
	pad->Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// �x�̖����]�[����ݒ�
	dipdw.diph.dwObj		= D_DIJOFS_Y ;
	pad->Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// �y�̖����]�[����ݒ�
	dipdw.diph.dwObj		= D_DIJOFS_Z ;
	pad->Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// �y��]�̖����]�[����ݒ�
	dipdw.diph.dwObj		= D_DIJOFS_RZ ;
	pad->Device->SetProperty( D_DIPROP_DEADZONE , &dipdw.diph ) ;

	// ����I��
	return 0 ;
}

// �W���C�p�b�h�̐U�����J�n����
extern	int NS_StartJoypadVibration( int InputType, int Power, int Time, int EffectIndex )
{
//	D_DICONSTANTFORCE cf ;
//	D_DIEFFECT        eff;
	int               SetPower ;
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	DINPUTPAD *pad = &DInputData.Joypad[ JoypadNum ] ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	// �G���[�`�F�b�N
	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum )
	{
		return 0 ;
	}

	// EffectIndex ���}�C�i�X�̏ꍇ�͏����𕪊�
	if( EffectIndex < 0 )
	{
		if( Power < 0 )
		{
			NS_StartJoypadVibration( InputType, pad->Effect[ DINPUTPAD_MOTOR_LEFT ].Power, Time, DINPUTPAD_MOTOR_LEFT ) ;
		}
		else
		{
			SetPower = Power * 2 ;
			if( SetPower > D_DI_FFNOMINALMAX / 10 )
			{
				SetPower = D_DI_FFNOMINALMAX / 10 ;
			}
			NS_StartJoypadVibration( InputType, SetPower, Time, DINPUTPAD_MOTOR_LEFT ) ;
		}

		if( Power < 0 )
		{
			NS_StartJoypadVibration( InputType, pad->Effect[ DINPUTPAD_MOTOR_RIGHT ].Power, Time, DINPUTPAD_MOTOR_RIGHT ) ;
		}
		else
		{
			SetPower = ( Power - ( D_DI_FFNOMINALMAX / 10 / 2 ) ) * 2 ;
			if( SetPower <= 0 )
			{
				SetPower = 0 ;
			}
			else
			if( SetPower > D_DI_FFNOMINALMAX / 10 )
			{
				SetPower = D_DI_FFNOMINALMAX / 10 ;
			}
			NS_StartJoypadVibration( InputType, SetPower, Time, DINPUTPAD_MOTOR_RIGHT ) ;
		}

		// �I��
		return 0 ;
	}

	if( EffectIndex >= DINPUTPAD_MOTOR_NUM )
	{
		return 0 ;
	}

	if( pad->XInputDeviceNo < 0 && ( pad->Device == NULL || pad->Effect[ EffectIndex ].DIEffect == NULL ) )
	{
		return 0 ;
	}

	// ���̑傫���� DirectInput �̌^�ɍ��킹��
	if( Power < 0 )
	{
		Power = pad->Effect[ EffectIndex ].Power ;
	}
	else
	{
		Power *= 10 ;
		if( Power >  D_DI_FFNOMINALMAX )
		{
			Power =  D_DI_FFNOMINALMAX ;
		}
/*
		if( Power >  D_DI_FFNOMINALMAX )
		{
			Power =  D_DI_FFNOMINALMAX ;
		}
		else
		if( Power < -D_DI_FFNOMINALMAX )
		{
			Power = -D_DI_FFNOMINALMAX ;
		}
*/
	}

	// �h��̑傫�����O�̏ꍇ�͐U�����X�g�b�v����
	if( Power == 0 )
	{
		NS_StopJoypadVibration( InputType, EffectIndex ) ;
	}
	else
	{
		// �Đ��J�n�����Ȃǂ��Z�b�g
		pad->Effect[ EffectIndex ].BackTime	= NS_GetNowCount() ;
		pad->Effect[ EffectIndex ].Time		= Time ;
		pad->Effect[ EffectIndex ].CompTime	= 0 ;

		// ���ɍĐ����ł��̑傫���������ꍇ�͉������Ȃ�
		if( pad->Effect[ EffectIndex ].PlayFlag == TRUE && pad->Effect[ EffectIndex ].Power == Power )
		{
			return 0 ;
		}

/*		// ���̋�����ύX����
		if( pad->XInputDeviceNo < 0 )
		{
			cf.lMagnitude = Power ;

			_MEMSET( &eff, 0, sizeof(eff) ) ;
			eff.dwSize                = sizeof( D_DIEFFECT ) ;
			eff.dwFlags               = D_DIEFF_CARTESIAN | D_DIEFF_OBJECTOFFSETS ;
			eff.lpEnvelope            = 0 ;
			eff.cbTypeSpecificParams  = sizeof( D_DICONSTANTFORCE) ;
			eff.lpvTypeSpecificParams = &cf ;
			eff.dwStartDelay          = 0 ;

			if( pad->Effect[ EffectIndex ].PlayState == TRUE )
			{
				pad->Effect[ EffectIndex ].DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS | D_DIEP_START ) ;
			}
			else
			{
				pad->Effect[ EffectIndex ].DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS ) ;
			}
		}
*/
		// �Đ��ݒ��ۑ�
		pad->Effect[ EffectIndex ].PlayFlag	= TRUE ;
		pad->Effect[ EffectIndex ].Power	= Power ;

		// �Đ���Ԃ̐ݒ�𔽉f
		RefreshEffectPlayState() ;
	}

	// �I��
	return 0 ;
}

// �W���C�p�b�h�̐U�����~����
extern	int NS_StopJoypadVibration( int InputType, int EffectIndex )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	DINPUTPAD *pad = &DInputData.Joypad[ JoypadNum ] ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;
	
	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	// �G���[�`�F�b�N
	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum )
	{
		return 0 ;
	}

	// EffectIndex ���}�C�i�X�̏ꍇ�͏����𕪊�
	if( EffectIndex < 0 )
	{
		// ���E���[�^�[�̐U�����~�߂�
		NS_StopJoypadVibration( InputType, DINPUTPAD_MOTOR_LEFT ) ;
		NS_StopJoypadVibration( InputType, DINPUTPAD_MOTOR_RIGHT ) ;

		// �I��
		return 0 ;
	}

	if( EffectIndex >= DINPUTPAD_MOTOR_NUM )
	{
		return 0 ;
	}

	if( pad->XInputDeviceNo < 0 && ( pad->Device == NULL || pad->Effect[ EffectIndex ].DIEffect == NULL ) )
	{
		return 0 ;
	}

	// ���ɍĐ�����~���Ă����牽�����Ȃ�
	if( pad->Effect[ EffectIndex ].PlayFlag == FALSE )
	{
		return 0 ;
	}

	// �Đ��t���O��|��
	pad->Effect[ EffectIndex ].PlayFlag = FALSE ;

	// �U���̋�����������
	pad->Effect[ EffectIndex ].Power = D_DI_FFNOMINALMAX ;

	// �Đ���Ԃ̐ݒ�𔽉f
	RefreshEffectPlayState() ;

	// �I��
	return 0 ;
}

// �W���C�p�b�h�̂o�n�u���͂̏�Ԃ𓾂�( �P�ʂ͊p�x�̂P�O�O�{  ���S�ʒu�ɂ���ꍇ�� -1 ���Ԃ� )
extern int NS_GetJoypadPOVState( int InputType, int POVNumber )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	DINPUTPAD *pad = &DInputData.Joypad[ JoypadNum ] ;
	DWORD pov;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;
	
	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	// �G���[�`�F�b�N
	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum || pad->Device == NULL || POVNumber >= 4 )
		return -1 ;

	// ���̍X�V
	UpdateJoypadInputState( JoypadNum ) ;
	pov = pad->State.rgdwPOV[POVNumber];

	// ���S�`�F�b�N
	if( ( pov & 0xffff ) == 0xffff ) return -1;

	// ���炩�̊p�x������ꍇ�͂��̂܂ܕԂ�
	return (int)pov;
}

// �W���C�p�b�h�̃f�o�C�X�o�^���Ɛ��i�o�^�����擾����
extern int NS_GetJoypadName( int InputType, TCHAR *InstanceNameBuffer, TCHAR *ProductNameBuffer )
{
#ifdef UNICODE
	return GetJoypadName_WCHAR_T( InputType, InstanceNameBuffer, ProductNameBuffer ) ;
#else
	int Result ;
	wchar_t TempInstanceNameBuffer[ 256 ] ;
	wchar_t TempProductNameBuffer[ 256 ] ;

	Result = GetJoypadName_WCHAR_T( InputType, TempInstanceNameBuffer, TempProductNameBuffer ) ;

	if( InstanceNameBuffer )
	{
		ConvString( ( const char * )TempInstanceNameBuffer, WCHAR_T_CODEPAGE, InstanceNameBuffer, _TCODEPAGE ) ;
	}

	if( ProductNameBuffer )
	{
		ConvString( ( const char * )TempProductNameBuffer, WCHAR_T_CODEPAGE, ProductNameBuffer, _TCODEPAGE ) ;
	}

	return Result ;
#endif
}

// �W���C�p�b�h�̃f�o�C�X�o�^���Ɛ��i�o�^�����擾����
extern int GetJoypadName_WCHAR_T( int InputType, wchar_t *InstanceNameBuffer, wchar_t *ProductNameBuffer )
{
	int JoypadNum = ( InputType & ~DX_INPUT_KEY ) - 1 ;
	DINPUTPAD *pad = &DInputData.Joypad[ JoypadNum ] ;

	if( WinData.ActiveFlag == FALSE )
		DxActiveWait() ;
	
	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return InitializeDirectInput() ;

	// �G���[�`�F�b�N
	if( JoypadNum < 0 || JoypadNum >= DInputData.JoyPadNum || ( pad->Device == NULL && pad->XInputDeviceNo < 0 ) )
		return -1 ;

	// ���O���R�s�[
	if( InstanceNameBuffer != NULL ) _WCSCPY( InstanceNameBuffer, pad->InstanceName ) ;
	if( ProductNameBuffer  != NULL ) _WCSCPY( ProductNameBuffer,  pad->ProductName  ) ;

	// �I��
	return 0;
}

// �W���C�p�b�h�̍ăZ�b�g�A�b�v���s��( �V���ɐڑ����ꂽ�W���C�p�b�h������ꍇ�Ɍ��o����� )
extern int NS_ReSetupJoypad( void )
{
	// �W���C�p�b�h�̍ăZ�b�g�A�b�v���s��
	return SetupJoypad() ;
}

// F10 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern int SetF10Input( void )
{
	DInputData.F10Flag = TRUE ;
//	DInputData.F10InputTime = NS_GetNowCount() ;

	// �I��
	return 0 ;
}

// F10 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern int ResetF10Input( void )
{
	DInputData.F10Flag = FALSE ;

	// �I��
	return 0 ;
}

// F12 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern int SetF12Input( void )
{
	DInputData.F12Flag = TRUE ;
//	DInputData.F12InputTime = NS_GetNowCount() ;

	// �I��
	return 0 ;
}

// F12 �������ꂽ���Ƃɂ���t���O�𗧂Ă�
extern int ResetF12Input( void )
{
	DInputData.F12Flag = FALSE ;

	// �I��
	return 0 ;
}


// F10 ����
static void F10Process( void )
{
/*
	if( DInputData.F10Flag == TRUE )
	{
		if( NS_GetNowCount() - DInputData.F10InputTime > 64 )
		{
			DInputData.F10Flag = FALSE ;
		}
	}
*/
}

// �L�[�{�[�h�̓��͏�Ԃ̍X�V
extern int UpdateKeyboardInputState( int UseProcessMessage )
{
	static bool InFunction = false ;
	unsigned char KeyBuffer[256] ;
	int Time, hr, i ;
	int Result = 0 ;

	// �����ċA�h�~
	if( InFunction == true ) return 0 ;
	InFunction = true ;

	// �L�[�{�[�h���͏����� DirectInput ���g�p���邩�ǂ����ŏ����𕪊�
	if( DInputData.NoUseDirectInputFlag == TRUE || DInputData.KeyboardNotDirectInputFlag == TRUE || DInputData.KeyboardDeviceObject == NULL || ( DInputData.UseDirectInputFlag == FALSE && WinData.WindowsVersion >= DX_WINDOWSVERSION_VISTA ) )
	{
		// ���C���X���b�h�ȊO����Ă΂ꂽ�ꍇ�͉��������ɏI��
		if( GetCurrentThreadId() != WinData.MainThreadID ) goto ENDFUNCTION ;

		// �܂��O��̎擾���� 1msec ���o�߂��Ă��Ȃ��ꍇ�͈ȑO�̏�Ԃ�Ԃ�
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( DInputData.KeyInputGetTime == Time ) goto ENDFUNCTION ;
		DInputData.KeyInputGetTime = Time ;

		// �L�[���͂̃��b�Z�[�W�����Ă���\��������̂ŁA���b�Z�[�W�������s��
		if( UseProcessMessage != FALSE && NS_ProcessMessage() != 0 ) goto ENDFUNCTION ;

		// �W���`�o�h�œ��͏�Ԃ��擾����
		GetKeyboardState( KeyBuffer ) ;
		_MEMSET( &DInputData.KeyInputBuf, 0, sizeof( DInputData.KeyInputBuf ) ) ;
		for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
			DInputData.KeyInputBuf[__KeyMap[i][1]] = KeyBuffer[__KeyMap[i][0]] ;

		DInputData.MouseInputBuf[ 0 ] = ( BYTE )( ( KeyBuffer[ VK_LBUTTON ] & 0x80 ) ? 1 : 0 ) ;
		DInputData.MouseInputBuf[ 1 ] = ( BYTE )( ( KeyBuffer[ VK_RBUTTON ] & 0x80 ) ? 1 : 0 ) ;
		DInputData.MouseInputBuf[ 2 ] = ( BYTE )( ( KeyBuffer[ VK_MBUTTON ] & 0x80 ) ? 1 : 0 ) ;
		DInputData.MouseInputBufValidFlag = TRUE ;
	}
	else
	{
		// �܂��O��̎擾���� 1msec ���o�߂��Ă��Ȃ��ꍇ�͈ȑO�̏�Ԃ�Ԃ�
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( DInputData.KeyInputGetTime == Time && WaitForSingleObject( DInputData.KeyEvent, 0 ) != WAIT_OBJECT_0 ) goto ENDFUNCTION ;

		// �L�[���͂̃��b�Z�[�W�����Ă���\��������̂ŁA���b�Z�[�W�������s��
		if( UseProcessMessage != FALSE && NS_ProcessMessage() != 0 ) goto ENDFUNCTION ;

		// ����������
		if( DInputData.DirectInputObject == NULL ) goto ENDFUNCTION ;

		// �X�e�[�^�X�擾�A���X�g���Ă�����Ď擾
		hr = DInputData.KeyboardDeviceObject->GetDeviceState( sizeof( DInputData.KeyInputBuf ) , ( LPVOID )DInputData.KeyInputBuf ) ;
//		if( hr == DIERR_INPUTLOST )
		if( hr != D_DI_OK )
		{
			if( DInputData.KeyboardDeviceObject->Acquire() == D_DI_OK )
			{
				hr = DInputData.KeyboardDeviceObject->GetDeviceState( sizeof( DInputData.KeyInputBuf ) , ( LPVOID )DInputData.KeyInputBuf ) ;
				if( hr != D_DI_OK ) goto ENDFUNCTION ;
			}
			else goto ENDFUNCTION ;
		}

		DInputData.KeyInputGetTime = Time ;
		
		// �C�x���g�����Z�b�g
		if( WaitForSingleObject( DInputData.KeyEvent, 0 ) == WAIT_OBJECT_0 ) ResetEvent( DInputData.KeyEvent ) ;

		// �E�V�t�g�L�[��W���`�o�h������
		GetKeyboardState( KeyBuffer ) ;
		DInputData.KeyInputBuf[ KEY_INPUT_RSHIFT ] = KeyBuffer[ VK_RSHIFT ] ;

		// ******* �c�����������h�����������M�p�ł��Ȃ��̂ŁA�ꕔ�̃L�[�������W���`�o�h�ŃL�[��Ԃ��擾 ******* //
/*		if( DInputData.KeyExclusiveCooperativeLevelFlag == FALSE )
		{
			//���C���X���b�h�ȊO�������炱�̏������s��Ȃ�
			if( WinData.MainThreadID == GetCurrentThreadId() )
			{
				// �W���`�o�h�œ��͏�Ԃ��擾����
				GetKeyboardState( KeyBuffer ) ;
				for( i = 0 ; __KeyMap[i][0] != 0xffff ; i ++ )
					if( __KeyMap[i][2] != 1 )
						DInputData.KeyInputBuf[__KeyMap[i][1]] = KeyBuffer[__KeyMap[i][0]] ;
			}
		}
*/	}

	Result = 1 ;

ENDFUNCTION:
	InFunction = false ;

	return Result ;
}

// �p�b�h�̓��͏�Ԃ̍X�V
static int UpdateJoypadInputState( int padno )
{
	DINPUTPAD *pad = &DInputData.Joypad[padno] ;
	D_DIJOYSTATE InputState ;
	JOYINFOEX joyex ;
	HRESULT hr ;
	int Time, i ;

	if( padno >= MAX_JOYPAD_NUM ) return -1 ;

	// DirectInput ���g�p���Ă��邩�ǂ����ŏ����𕪊�
	if( DInputData.NoUseDirectInputFlag == TRUE )
	{
		// DirectInput ���g�p���Ȃ��ꍇ

		// �O��̎擾���� 1msec ���o�߂��Ă��Ȃ��ꍇ�͈ȑO�̏�Ԃ�Ԃ�
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( pad->GetTime == Time ) return 0 ;
		pad->GetTime = Time ;

		// ���͏�Ԃ��擾����
		_MEMSET( &joyex, 0, sizeof( joyex ) ) ;
		joyex.dwSize  = sizeof( JOYINFOEX ) ;
		joyex.dwFlags = JOY_RETURNALL ;
		if( WinAPIData.Win32Func.joyGetPosExFunc( ( UINT )padno, &joyex ) != JOYERR_NOERROR )
		{
			// �擾�ł��Ȃ������ꍇ�͐ڑ�����Ă��Ȃ��Ƃ������ƂȂ̂ŏ����[���Ŗ��߂Ă���
			_MEMSET( &pad->State, 0, sizeof( pad->State ) ) ;
		}
		else
		{
			// �}���`���f�B�A�`�o�h�p�̓��͒l�� DirectInput �p�ɕϊ�����

			switch( joyex.dwXpos )
			{
			case 0x7fff :	pad->State.lX =      0 ;	break ;
			case 0x0000 :	pad->State.lX = -RANGE ;	break ;
			case 0xffff :	pad->State.lX =  RANGE ;	break ;
			default :
				     if( joyex.dwXpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lX = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwXpos ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else if( joyex.dwXpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lX = ( LONG )(          ( ( (LONG)joyex.dwXpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else                                                               pad->State.lX = 0 ;
				break ;
			}

			switch( joyex.dwYpos )
			{
			case 0x7fff :	pad->State.lY =      0 ;	break ;
			case 0x0000 :	pad->State.lY = -RANGE ;	break ;
			case 0xffff :	pad->State.lY =  RANGE ;	break ;
			default :
				     if( joyex.dwYpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lY = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwYpos ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else if( joyex.dwYpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lY = ( LONG )(          ( ( (LONG)joyex.dwYpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
				else                                                               pad->State.lY = 0 ;
				break ;
			}

			if( pad->RightStickFlag )
			{
				switch( joyex.dwZpos )
				{
				case 0x7fff :	pad->State.lZ =      0 ;	break ;
				case 0x0000 :	pad->State.lZ = -RANGE ;	break ;
				case 0xffff :	pad->State.lZ =  RANGE ;	break ;
				default :
						 if( joyex.dwZpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lZ = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwZpos ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else if( joyex.dwZpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lZ = ( LONG )(          ( ( (LONG)joyex.dwZpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else                                                               pad->State.lZ = 0 ;
					break ;
				}

				switch( joyex.dwRpos )
				{
				case 0x7fff :	pad->State.lRz =      0 ;	break ;
				case 0x0000 :	pad->State.lRz = -RANGE ;	break ;
				case 0xffff :	pad->State.lRz =  RANGE ;	break ;
				default :
						 if( joyex.dwRpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lRz = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwRpos ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else if( joyex.dwRpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lRz = ( LONG )(          ( ( (LONG)joyex.dwRpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else                                                               pad->State.lRz = 0 ;
					break ;
				}

				switch( joyex.dwUpos )
				{
				case 0x7fff :	pad->State.lRx =      0 ;	break ;
				case 0x0000 :	pad->State.lRx = -RANGE ;	break ;
				case 0xffff :	pad->State.lRx =  RANGE ;	break ;
				default :
						 if( joyex.dwUpos < 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lRx = ( LONG )( -( LONG )( ( ( 0x8000 - DEADZONE_WINMM( pad->DeadZone ) ) - (LONG)joyex.dwUpos ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else if( joyex.dwUpos > 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) pad->State.lRx = ( LONG )(          ( ( (LONG)joyex.dwUpos - ( 0x8000 + DEADZONE_WINMM( pad->DeadZone ) ) ) * RANGE / VALIDRANGE_WINMM( pad->DeadZone ) ) ) ;
					else                                                               pad->State.lRx = 0 ;
					break ;
				}
			}
			else
			{
				pad->State.lZ = 0 ;
				pad->State.lRx = 0 ;
				pad->State.lRz = 0 ;
			}

			for( i = 0 ; i < 24 ; i ++ )
			{
				if( joyex.dwButtons & ( 1 << i ) ) pad->State.rgbButtons[ i ] = 0x80 ;
				else                               pad->State.rgbButtons[ i ] = 0x00 ;
			}
		}
	}
	else
	if( pad->XInputDeviceNo < 0 )
	{
		// DirectInput ���g�p����ꍇ

		// �O��̎擾���� 1msec ���o�߂��Ă��Ȃ��ꍇ�͈ȑO�̏�Ԃ�Ԃ�
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( pad->GetTime == Time &&
			WaitForSingleObject( pad->Event, 0 ) != WAIT_OBJECT_0 ) return 0 ;
		pad->GetTime = Time ;

		// ���̓f�[�^�̃|�[�����O
		pad->Device->Poll() ;

		if( ( hr = pad->Device->GetDeviceState( sizeof( D_DIJOYSTATE ) , &InputState ) ) != D_DI_OK )
		{
			if( NS_ProcessMessage() != 0 ) return -1 ;
			if( DInputData.DirectInputObject == NULL ) return -1;

			if( pad->Device->Acquire() == D_DI_OK )
			{
				if( NS_ProcessMessage() ) return -1 ;
				if( DInputData.DirectInputObject == NULL ) return -1;

				if( pad->Device->GetDeviceState( sizeof( D_DIJOYSTATE ) , &InputState ) == D_DI_OK )
				{
					hr = D_DI_OK;
				}
			}
		}

		if( hr == D_DI_OK )
		{
			pad->State = InputState ;
		}

		// �C�x���g�̃��Z�b�g
		if( WaitForSingleObject( pad->Event, 0 ) == WAIT_OBJECT_0 ) ResetEvent( pad->Event ) ;
	}
	else
	{
		// XInput ���g�p����ꍇ

		// �O��̎擾���� 1msec ���o�߂��Ă��Ȃ��ꍇ�͈ȑO�̏�Ԃ�Ԃ�
		Time = ( int )WinAPIData.Win32Func.timeGetTimeFunc() ;
		if( pad->GetTime == Time ) return 0 ;
		pad->GetTime = Time ;

		if( DInputData.XInputGetStateFunc == NULL || DInputData.XInputGetStateFunc( ( DWORD )pad->XInputDeviceNo, &pad->XInputState ) != ERROR_SUCCESS )
		{
			// �擾�ł��Ȃ������ꍇ�͐ڑ�����Ă��Ȃ��Ƃ������ƂȂ̂ŏ����[���Ŗ��߂Ă���
			_MEMSET( &pad->XInputState, 0, sizeof( pad->XInputState ) ) ;
		}
		else
		{
			WORD Buttons ;

			// XInput�p�̓��͒l�� DirectInput �p�ɕϊ�����

			switch( pad->XInputState.Gamepad.sThumbLX )
			{
			case  32767 : pad->State.lX =  RANGE ; break ;
			case -32768 : pad->State.lX = -RANGE ; break ;
			default :
				     if( pad->XInputState.Gamepad.sThumbLX < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lX = -( ( -pad->XInputState.Gamepad.sThumbLX - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( pad->XInputState.Gamepad.sThumbLX >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lX =  ( (  pad->XInputState.Gamepad.sThumbLX - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                             pad->State.lX = 0 ;
				break ;
			}

			switch( pad->XInputState.Gamepad.sThumbLY )
			{
			case  32767 : pad->State.lY = -RANGE ; break ;
			case -32768 : pad->State.lY =  RANGE ; break ;
			default :
				     if( pad->XInputState.Gamepad.sThumbLY < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lY =  ( ( -pad->XInputState.Gamepad.sThumbLY - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( pad->XInputState.Gamepad.sThumbLY >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lY = -( (  pad->XInputState.Gamepad.sThumbLY - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                             pad->State.lY = 0 ;
				break ;
			}

			switch( pad->XInputState.Gamepad.sThumbRX )
			{
			case  32767 : pad->State.lRx =  RANGE ; break ;
			case -32768 : pad->State.lRx = -RANGE ; break ;
			default :
				     if( pad->XInputState.Gamepad.sThumbRX < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lRx = -( ( -pad->XInputState.Gamepad.sThumbRX - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( pad->XInputState.Gamepad.sThumbRX >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lRx =  ( (  pad->XInputState.Gamepad.sThumbRX - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                             pad->State.lRx = 0 ;
				break ;
			}

			switch( pad->XInputState.Gamepad.sThumbRY )
			{
			case  32767 : pad->State.lRy = -RANGE ; break ;
			case -32768 : pad->State.lRy =  RANGE ; break ;
			default :
				     if( pad->XInputState.Gamepad.sThumbRY < -DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lRy =  ( ( -pad->XInputState.Gamepad.sThumbRY - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else if( pad->XInputState.Gamepad.sThumbRY >  DEADZONE_XINPUT( pad->DeadZone ) ) pad->State.lRy = -( (  pad->XInputState.Gamepad.sThumbRY - DEADZONE_XINPUT( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT( pad->DeadZone ) ) ;
				else                                                                             pad->State.lRy = 0 ;
				break ;
			}

			Buttons = pad->XInputState.Gamepad.wButtons ;
			for( i = 0 ; i < XINPUT_TO_DIRECTINPUT_BUTTONNUM ; i ++ )
			{
				pad->State.rgbButtons[ i ] = ( BYTE )( ( Buttons & XInputButtonToDirectInputButtonNo[ i ] ) ? 0x80 : 0x00 ) ;
			}

			     if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) == 0 ) pad->State.rgdwPOV[ 0 ] =     0 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 ) pad->State.rgdwPOV[ 0 ] =  4500 ;
			else if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) == 0 ) pad->State.rgdwPOV[ 0 ] =  9000 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) != 0 ) pad->State.rgdwPOV[ 0 ] = 13500 ;
			else if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_RIGHT ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) == 0 ) pad->State.rgdwPOV[ 0 ] = 18000 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) != 0 ) pad->State.rgdwPOV[ 0 ] = 22500 ;
			else if( ( Buttons & D_XINPUT_GAMEPAD_DPAD_DOWN  ) == 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) == 0 ) pad->State.rgdwPOV[ 0 ] = 27000 ;
			else if(                                                   ( Buttons & D_XINPUT_GAMEPAD_DPAD_LEFT  ) != 0 && ( Buttons & D_XINPUT_GAMEPAD_DPAD_UP    ) != 0 ) pad->State.rgdwPOV[ 0 ] = 31500 ;
			else                                                                                                                                                          pad->State.rgdwPOV[ 0 ] = 0xffffffff ;
			pad->State.rgdwPOV[ 1 ] = 0xffffffff ;
			pad->State.rgdwPOV[ 2 ] = 0xffffffff ;
			pad->State.rgdwPOV[ 3 ] = 0xffffffff ;

			pad->State.lZ = 0 ;
			if( pad->XInputState.Gamepad.bLeftTrigger  > DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) )
			{
				pad->State.lZ += ( pad->XInputState.Gamepad.bLeftTrigger  - DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT_TRIGGER( pad->DeadZone ) ;
			}
			if( pad->XInputState.Gamepad.bRightTrigger > DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) )
			{
				pad->State.lZ -= ( pad->XInputState.Gamepad.bRightTrigger - DEADZONE_XINPUT_TRIGGER( pad->DeadZone ) ) * RANGE / VALIDRANGE_XINPUT_TRIGGER( pad->DeadZone ) ;
			}
		}
	}

	// �I��
	return 0 ;
}

// �L�[�{�[�h�̓��͏����� DirectInput ���g��Ȃ����A�t���O���Z�b�g����
extern int NS_SetKeyboardNotDirectInputFlag( int Flag )
{
	DInputData.KeyboardNotDirectInputFlag = Flag ;

	// �I��
	return 0 ;
}

// ���͏����� DirectInput ���g�p���邩�ǂ����̃t���O���Z�b�g����
extern int NS_SetUseDirectInputFlag( int Flag )
{
	DInputData.NoUseDirectInputFlag = !Flag ;
	DInputData.UseDirectInputFlag = Flag ;

	// �I��
	return 0 ;
}

// Xbox360�R���g���[���̓��͏����� XInput ���g�p���邩�ǂ�����ݒ肷��( TRUE:XInput ���g�p����( �f�t�H���g )�@�@FALSE:XInput ���g�p���Ȃ� )
extern int NS_SetUseXInputFlag(	int Flag )
{
	DInputData.NoUseXInputFlag = !Flag ;

	// �I��
	return 0 ;
}

// �W���C�p�b�h�̐U���@�\���g�p���邩�ǂ����̃t���O���Z�b�g����
extern int NS_SetUseJoypadVibrationFlag( int Flag )
{
	DInputData.NoUseVibrationFlag = !Flag ;

	// �I��
	return 0 ;
}


// �c�����������h���������f�o�C�X�̎擾��Ԃ��X�V����
extern int RefreshInputDeviceAcquireState( void )
{
	int i, num ;
	DINPUTPAD *Pad ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return 0 ;

	// DirectInput ���g�p���Ă��Ȃ��ꍇ�͉��������I��
	if( DInputData.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ) return 0 ;

	// �p�b�h�f�o�C�X�̍Ď擾
	num = DInputData.JoyPadNum ;
	Pad = DInputData.Joypad ;
	for( i = 0 ; i < num ; i ++, Pad ++ )
	{
		if( Pad->Device != NULL )
		{
			Pad->Device->Unacquire() ;

			// �\�t�g�̃A�N�e�B�u��Ԃɂ���ċ������x����ύX
			if( NS_GetActiveFlag() == TRUE )
			{
				Pad->Device->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_FOREGROUND | D_DISCL_EXCLUSIVE ) ;
			}
			else
			{
				Pad->Device->SetCooperativeLevel( NS_GetMainWindowHandle(), D_DISCL_BACKGROUND | D_DISCL_NONEXCLUSIVE ) ;
			}

			Pad->Device->Acquire() ;
		}
	}

	// �L�[�{�[�h�f�o�C�X�̍Ď擾
	if( DInputData.KeyboardDeviceObject != NULL )
	{
		DInputData.KeyboardDeviceObject->Unacquire() ;
		DInputData.KeyboardDeviceObject->Acquire() ;
	}

	// �I��
	return 0 ;
}

// �p�b�h�̃G�t�F�N�g�����Ɋւ���t���[���������s��
extern int JoypadEffectProcess( void )
{
	int        i ;
	int        j ;
	int        num ;
	int        time ;
	DINPUTPAD *Pad ;

	// ����������
	if( DInputData.DirectInputObject == NULL && NS_GetWindowCloseFlag() == FALSE )
		return 0 ;

	// �c�����������h�����������g�p���Ă��Ȃ��ꍇ�̓G�t�F�N�g�����͏o���Ȃ��̂ŏI��
	if( DInputData.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ) return 0 ;

	// �p�b�h�f�o�C�X�̍Ď擾
	num = DInputData.JoyPadNum ;
	Pad = DInputData.Joypad ;
	time = NS_GetNowCount() ;
	for( i = 0 ; i < num ; i ++, Pad ++ )
	{
		if( Pad->XInputDeviceNo < 0 && Pad->Device == NULL )
		{
			continue ;
		}

		for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
		{
			if( Pad->Effect[ j ].PlayFlag == FALSE ) continue ;

			// �����Đ��̏ꍇ�͓��ɉ������Ȃ�
			if( Pad->Effect[ j ].Time < 0 ) continue ;

			// �O�񂩂玞�����ς���Ă��Ȃ������牽�����Ȃ�
			if( Pad->Effect[ j ].BackTime == time ) continue ;

			// �o�ߎ��Ԃ̉��Z
			if( time < Pad->Effect[ j ].BackTime )
			{
				Pad->Effect[ j ].BackTime = time ;
			}
			Pad->Effect[ j ].CompTime += time - Pad->Effect[ j ].BackTime ;

			// ����̎�����ۑ�
			Pad->Effect[ j ].BackTime = time ;

			// �Đ���~�̃`�F�b�N
			if( Pad->Effect[ j ].CompTime >= Pad->Effect[ j ].Time )
			{
				NS_StopJoypadVibration( i + 1, j ) ;
			}
		}
	}

	// �Đ���Ԃ̍X�V
	RefreshEffectPlayState() ;

	// �I��
	return 0 ;
}

// �p�b�h�G�t�F�N�g�̍Đ���Ԃ��X�V����(�\�t�g����A�N�e�B�u�ɂȂ邱�Ƃ����邽��)
extern int RefreshEffectPlayState( void )
{
	int                 i ;
	int                 j ;
	int                 num ;
	int                 time ;
	int                 PlayState ;
	int                 SetPower ;
	int                 Flag ;
	DINPUTPAD          *Pad ;
	D_DICONSTANTFORCE   cf ;
	D_DIEFFECT          eff;
	D_XINPUT_VIBRATION  VibState ;

	// ����������
	if( DInputData.DirectInputObject == NULL /* && NS_GetWindowCloseFlag() == FALSE */ )
		return 0 ;

	// �c�����������h�����������g�p���Ă��Ȃ��ꍇ�̓G�t�F�N�g�����͏o���Ȃ��̂ŏI��
	if( DInputData.DirectInputObject == (D_IDirectInput7 *)(DWORD_PTR)0xffffffff ) return 0 ;

	// �p�b�h�f�o�C�X�̍Ď擾
	num  = DInputData.JoyPadNum ;
	Pad  = DInputData.Joypad ;
	time = NS_GetNowCount() ;

	// �G�t�F�N�g�\���̂̏�����
	_MEMSET( &eff, 0, sizeof( eff ) ) ;
	eff.dwSize                = sizeof( D_DIEFFECT ) ;
	eff.dwFlags               = D_DIEFF_CARTESIAN | D_DIEFF_OBJECTOFFSETS ;
	eff.lpEnvelope            = 0 ;
	eff.cbTypeSpecificParams  = sizeof( D_DICONSTANTFORCE ) ;
	eff.lpvTypeSpecificParams = &cf ;
	eff.dwStartDelay          = 0 ;

	// �\�t�g�̃A�N�e�B�u��Ԃɂ���ď����𕪊�
	if( NS_GetActiveFlag() == TRUE )
	{
		// �\�t�g���A�N�e�B�u�ȏꍇ
		for( i = 0 ; i < num ; i ++, Pad ++ )
		{
			// XInput ���ǂ����ŏ����𕪊�
			if( Pad->XInputDeviceNo >= 0 )
			{
				// XInput �̏ꍇ
				VibState.wLeftMotorSpeed  = 0 ;
				VibState.wRightMotorSpeed = 0 ;
				Flag = 0 ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].PlayFlag )
					{
						SetPower = Pad->Effect[ j ].Power ;
					}
					else
					{
						SetPower = 0 ;
					}

					switch( j )
					{
					case DINPUTPAD_MOTOR_LEFT :
						VibState.wLeftMotorSpeed  = ( WORD )( SetPower * 65535 / D_DI_FFNOMINALMAX ) ;
						break ;

					case DINPUTPAD_MOTOR_RIGHT :
						VibState.wRightMotorSpeed = ( WORD )( SetPower * 65535 / D_DI_FFNOMINALMAX ) ;
						break ;
					}

					if( Pad->Effect[ j ].SetPower != SetPower )
					{
						Flag = 1 ;
						Pad->Effect[ j ].SetPower = SetPower ;
					}

					Pad->Effect[ j ].PlayState = Pad->Effect[ j ].PlayFlag ;
				}

				if( DInputData.XInputSetStateFunc != NULL && Flag == 1 )
				{
					DInputData.XInputSetStateFunc( ( DWORD )Pad->XInputDeviceNo, &VibState ) ;
				}
			}
			else
			{
				if( Pad->Device == NULL ) continue ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].DIEffect == NULL )
					{
						continue ;
					}

					if( Pad->Effect[ j ].PlayFlag )
					{
						SetPower = Pad->Effect[ j ].Power ;
					}
					else
					{
						SetPower = 0 ;
					}

					PlayState = SetPower > 0 ? TRUE : FALSE ;

					if( Pad->Effect[ j ].SetPower  != SetPower ||
						Pad->Effect[ j ].PlayState != PlayState )
					{
						cf.lMagnitude = SetPower ;

						if( PlayState == TRUE )
						{
							if( Pad->Effect[ j ].PlayState == TRUE )
							{
								if( Pad->Effect[ j ].SetPower != SetPower )
								{
									Pad->Effect[ j ].DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS | D_DIEP_START ) ;
								}
							}
							else
							if( Pad->Effect[ j ].PlayState == FALSE )
							{
								if( Pad->Effect[ j ].SetPower != SetPower )
								{
									Pad->Effect[ j ].DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS ) ;
								}
								Pad->Effect[ j ].DIEffect->Start( INFINITE, 0 ) ;
							}
						}
						else
						{
							if( Pad->Effect[ j ].PlayState )
							{
								Pad->Effect[ j ].DIEffect->Stop() ;
							}

							if( Pad->Effect[ j ].SetPower != SetPower )
							{
								Pad->Effect[ j ].DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS ) ;
							}
						}

						Pad->Effect[ j ].SetPower  = SetPower ;
						Pad->Effect[ j ].PlayState = Pad->Effect[ j ].PlayFlag ;
					}
				}
			}
		}
	}
	else
	{
		// �\�t�g����A�N�e�B�u�ȏꍇ
		for( i = 0 ; i < num ; i ++, Pad ++ )
		{
			if( Pad->XInputDeviceNo >= 0 )
			{
				VibState.wLeftMotorSpeed  = 0 ;
				VibState.wRightMotorSpeed = 0 ;
				Flag = 0 ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].SetPower != 0 )
					{
						Flag = 1 ;
						Pad->Effect[ j ].SetPower = 0 ;
					}

					Pad->Effect[ j ].PlayState = FALSE ;
				}

				if( DInputData.XInputSetStateFunc != NULL && Flag == 1 )
				{
					DInputData.XInputSetStateFunc( ( DWORD )Pad->XInputDeviceNo, &VibState ) ;
				}
			}
			else
			{
				if( Pad->Device == NULL ) continue ;

				for( j = 0 ; j < DINPUTPAD_MOTOR_NUM ; j ++ )
				{
					if( Pad->Effect[ j ].DIEffect == NULL )
					{
						continue ;
					}

					if( Pad->Effect[ j ].SetPower  != 0 ||
						Pad->Effect[ j ].PlayState != FALSE )
					{
						cf.lMagnitude = 0 ;

						if( Pad->Effect[ j ].PlayState )
						{
							Pad->Effect[ j ].DIEffect->Stop() ;
						}

						if( Pad->Effect[ j ].SetPower != 0 )
						{
							Pad->Effect[ j ].DIEffect->SetParameters( &eff, D_DIEP_TYPESPECIFICPARAMS ) ;
						}

						Pad->Effect[ j ].SetPower  = 0 ;
						Pad->Effect[ j ].PlayState = FALSE ;
					}
				}
			}
		}
	}

	// �I��
	return 0 ;
}

// �L�[�{�[�h�f�o�C�X�̃f�[�^�t�H�[�}�b�g��` -----------------------

// �f�t�h�c
GUID GUIDDIKEYBOARD =
{
//	Data1		Data2	Data3	Data4
	0x55728220,	0xd33c,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00", 
} ;

// �f�o�C�X�I�u�W�F�N�g�f�[�^
D_DIOBJECTDATAFORMAT C_ODFDIKEYBOARD[256] =
{
//		pguid				dwOfs	dwType 		dwFlags
	{ 	&GUIDDIKEYBOARD, 	0x00,	0x8000000c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x01,	0x8000010c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x02,	0x8000020c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x03,	0x8000030c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x04,	0x8000040c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x05,	0x8000050c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x06,	0x8000060c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x07,	0x8000070c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x08,	0x8000080c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x09,	0x8000090c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0a,	0x80000a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0b,	0x80000b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0c,	0x80000c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0d,	0x80000d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0e,	0x80000e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x0f,	0x80000f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x10,	0x8000100c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x11,	0x8000110c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x12,	0x8000120c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x13,	0x8000130c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x14,	0x8000140c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x15,	0x8000150c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x16,	0x8000160c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x17,	0x8000170c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x18,	0x8000180c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x19,	0x8000190c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1a,	0x80001a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1b,	0x80001b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1c,	0x80001c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1d,	0x80001d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1e,	0x80001e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x1f,	0x80001f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x20,	0x8000200c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x21,	0x8000210c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x22,	0x8000220c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x23,	0x8000230c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x24,	0x8000240c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x25,	0x8000250c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x26,	0x8000260c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x27,	0x8000270c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x28,	0x8000280c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x29,	0x8000290c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2a,	0x80002a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2b,	0x80002b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2c,	0x80002c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2d,	0x80002d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2e,	0x80002e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x2f,	0x80002f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x30,	0x8000300c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x31,	0x8000310c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x32,	0x8000320c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x33,	0x8000330c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x34,	0x8000340c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x35,	0x8000350c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x36,	0x8000360c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x37,	0x8000370c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x38,	0x8000380c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x39,	0x8000390c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3a,	0x80003a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3b,	0x80003b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3c,	0x80003c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3d,	0x80003d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3e,	0x80003e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x3f,	0x80003f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x40,	0x8000400c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x41,	0x8000410c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x42,	0x8000420c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x43,	0x8000430c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x44,	0x8000440c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x45,	0x8000450c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x46,	0x8000460c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x47,	0x8000470c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x48,	0x8000480c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x49,	0x8000490c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4a,	0x80004a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4b,	0x80004b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4c,	0x80004c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4d,	0x80004d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4e,	0x80004e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x4f,	0x80004f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x50,	0x8000500c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x51,	0x8000510c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x52,	0x8000520c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x53,	0x8000530c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x54,	0x8000540c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x55,	0x8000550c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x56,	0x8000560c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x57,	0x8000570c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x58,	0x8000580c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x59,	0x8000590c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5a,	0x80005a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5b,	0x80005b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5c,	0x80005c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5d,	0x80005d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5e,	0x80005e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x5f,	0x80005f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x60,	0x8000600c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x61,	0x8000610c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x62,	0x8000620c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x63,	0x8000630c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x64,	0x8000640c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x65,	0x8000650c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x66,	0x8000660c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x67,	0x8000670c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x68,	0x8000680c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x69,	0x8000690c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6a,	0x80006a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6b,	0x80006b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6c,	0x80006c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6d,	0x80006d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6e,	0x80006e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x6f,	0x80006f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x70,	0x8000700c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x71,	0x8000710c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x72,	0x8000720c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x73,	0x8000730c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x74,	0x8000740c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x75,	0x8000750c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x76,	0x8000760c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x77,	0x8000770c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x78,	0x8000780c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x79,	0x8000790c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7a,	0x80007a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7b,	0x80007b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7c,	0x80007c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7d,	0x80007d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7e,	0x80007e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x7f,	0x80007f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x80,	0x8000800c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x81,	0x8000810c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x82,	0x8000820c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x83,	0x8000830c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x84,	0x8000840c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x85,	0x8000850c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x86,	0x8000860c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x87,	0x8000870c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x88,	0x8000880c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x89,	0x8000890c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8a,	0x80008a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8b,	0x80008b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8c,	0x80008c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8d,	0x80008d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8e,	0x80008e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x8f,	0x80008f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0x90,	0x8000900c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x91,	0x8000910c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x92,	0x8000920c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x93,	0x8000930c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x94,	0x8000940c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x95,	0x8000950c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x96,	0x8000960c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x97,	0x8000970c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x98,	0x8000980c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x99,	0x8000990c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9a,	0x80009a0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9b,	0x80009b0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9c,	0x80009c0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9d,	0x80009d0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9e,	0x80009e0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0x9f,	0x80009f0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xa0,	0x8000a00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa1,	0x8000a10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa2,	0x8000a20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa3,	0x8000a30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa4,	0x8000a40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa5,	0x8000a50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa6,	0x8000a60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa7,	0x8000a70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa8,	0x8000a80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xa9,	0x8000a90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xaa,	0x8000aa0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xab,	0x8000ab0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xac,	0x8000ac0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xad,	0x8000ad0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xae,	0x8000ae0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xaf,	0x8000af0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xb0,	0x8000b00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb1,	0x8000b10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb2,	0x8000b20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb3,	0x8000b30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb4,	0x8000b40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb5,	0x8000b50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb6,	0x8000b60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb7,	0x8000b70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb8,	0x8000b80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xb9,	0x8000b90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xba,	0x8000ba0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbb,	0x8000bb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbc,	0x8000bc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbd,	0x8000bd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbe,	0x8000be0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xbf,	0x8000bf0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xc0,	0x8000c00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc1,	0x8000c10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc2,	0x8000c20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc3,	0x8000c30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc4,	0x8000c40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc5,	0x8000c50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc6,	0x8000c60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc7,	0x8000c70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc8,	0x8000c80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xc9,	0x8000c90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xca,	0x8000ca0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcb,	0x8000cb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcc,	0x8000cc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcd,	0x8000cd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xce,	0x8000ce0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xcf,	0x8000cf0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xd0,	0x8000d00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd1,	0x8000d10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd2,	0x8000d20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd3,	0x8000d30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd4,	0x8000d40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd5,	0x8000d50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd6,	0x8000d60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd7,	0x8000d70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd8,	0x8000d80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xd9,	0x8000d90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xda,	0x8000da0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdb,	0x8000db0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdc,	0x8000dc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdd,	0x8000dd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xde,	0x8000de0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xdf,	0x8000df0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xe0,	0x8000e00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe1,	0x8000e10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe2,	0x8000e20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe3,	0x8000e30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe4,	0x8000e40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe5,	0x8000e50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe6,	0x8000e60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe7,	0x8000e70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe8,	0x8000e80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xe9,	0x8000e90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xea,	0x8000ea0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xeb,	0x8000eb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xec,	0x8000ec0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xed,	0x8000ed0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xee,	0x8000ee0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xef,	0x8000ef0c,	0x0 },

	{ 	&GUIDDIKEYBOARD, 	0xf0,	0x8000f00c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf1,	0x8000f10c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf2,	0x8000f20c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf3,	0x8000f30c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf4,	0x8000f40c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf5,	0x8000f50c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf6,	0x8000f60c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf7,	0x8000f70c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf8,	0x8000f80c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xf9,	0x8000f90c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfa,	0x8000fa0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfb,	0x8000fb0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfc,	0x8000fc0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfd,	0x8000fd0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xfe,	0x8000fe0c,	0x0 },
	{ 	&GUIDDIKEYBOARD, 	0xff,	0x8000ff0c,	0x0 },
} ;

// �f�o�C�X�f�[�^�t�H�[�}�b�g
D_DIDATAFORMAT C_DFDIKEYBOARD =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	0x02,	// dwFlags
	256,	// dwDataSize
	256,	// dwNumObjs
	C_ODFDIKEYBOARD, // rgodf
} ;


// �W���C�X�e�B�b�N�f�o�C�X�̃f�[�^�t�H�[�}�b�g��` -----------------

// �f�t�h�c
GUID GUID_X_AXIS	= { 0xa36d02e0, 0xc9f3, 0x11cf, "\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_Y_AXIS	= { 0xa36d02e1,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_Z_AXIS	= { 0xa36d02e2,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_RX_AXIS	= { 0xa36d02f4,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_RY_AXIS	= { 0xa36d02f5,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_RZ_AXIS	= { 0xa36d02e3,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_SLIDER	= { 0xa36d02e4,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };
GUID GUID_POV		= { 0xa36d02f2,	0xc9f3,	0x11cf,	"\xbf\xc7\x44\x45\x53\x54\x00" };

// �W���C�X�e�B�b�N�P�f�o�C�X�I�u�W�F�N�g�f�[�^
D_DIOBJECTDATAFORMAT C_ODFDIJOYSTICK[44] =
{
//	pguid				dwOfs	dwType		dwFlags
	&GUID_X_AXIS,		0x00,	0x80ffff03,	0x100,
	&GUID_Y_AXIS,		0x04,	0x80ffff03,	0x100,
	&GUID_Z_AXIS,		0x08,	0x80ffff03,	0x100,
	&GUID_RX_AXIS,		0x0c,	0x80ffff03,	0x100,
	&GUID_RY_AXIS,		0x10,	0x80ffff03,	0x100,
	&GUID_RZ_AXIS,		0x14,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x100,
	&GUID_POV,			0x20,	0x80ffff10,	0x000,
	&GUID_POV,			0x24,	0x80ffff10,	0x000,
	&GUID_POV,			0x28,	0x80ffff10,	0x000,
	&GUID_POV,			0x2c,	0x80ffff10,	0x000,
	NULL,				0x30,	0x80ffff0c,	0x000,
	NULL,				0x31,	0x80ffff0c,	0x000,
	NULL,				0x32,	0x80ffff0c,	0x000,
	NULL,				0x33,	0x80ffff0c,	0x000,
	NULL,				0x34,	0x80ffff0c,	0x000,
	NULL,				0x35,	0x80ffff0c,	0x000,
	NULL,				0x36,	0x80ffff0c,	0x000,
	NULL,				0x37,	0x80ffff0c,	0x000,
	NULL,				0x38,	0x80ffff0c,	0x000,
	NULL,				0x39,	0x80ffff0c,	0x000,
	NULL,				0x3a,	0x80ffff0c,	0x000,
	NULL,				0x3b,	0x80ffff0c,	0x000,
	NULL,				0x3c,	0x80ffff0c,	0x000,
	NULL,				0x3d,	0x80ffff0c,	0x000,
	NULL,				0x3e,	0x80ffff0c,	0x000,
	NULL,				0x3f,	0x80ffff0c,	0x000,
	NULL,				0x40,	0x80ffff0c,	0x000,
	NULL,				0x41,	0x80ffff0c,	0x000,
	NULL,				0x42,	0x80ffff0c,	0x000,
	NULL,				0x43,	0x80ffff0c,	0x000,
	NULL,				0x44,	0x80ffff0c,	0x000,
	NULL,				0x45,	0x80ffff0c,	0x000,
	NULL,				0x46,	0x80ffff0c,	0x000,
	NULL,				0x47,	0x80ffff0c,	0x000,
	NULL,				0x48,	0x80ffff0c,	0x000,
	NULL,				0x49,	0x80ffff0c,	0x000,
	NULL,				0x4a,	0x80ffff0c,	0x000,
	NULL,				0x4b,	0x80ffff0c,	0x000,
	NULL,				0x4c,	0x80ffff0c,	0x000,
	NULL,				0x4d,	0x80ffff0c,	0x000,
	NULL,				0x4e,	0x80ffff0c,	0x000,
	NULL,				0x4f,	0x80ffff0c,	0x000,
} ;

// �W���C�X�e�B�b�N�P�f�o�C�X�f�[�^�t�H�[�}�b�g
D_DIDATAFORMAT C_DFDIJOYSTICK =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	0x01,	// dwFlags
	80,		// dwDataSize
	44,		// dwNumObjs
	C_ODFDIJOYSTICK, // rgodf
} ;

// �W���C�X�e�B�b�N�Q�f�o�C�X�I�u�W�F�N�g�f�[�^
D_DIOBJECTDATAFORMAT C_ODFDIJOYSTICK2[164] =
{
//	pguid				dwOfs	dwType		dwFlags
	&GUID_X_AXIS,		0x00,	0x80ffff03,	0x100,
	&GUID_Y_AXIS,		0x04,	0x80ffff03,	0x100,
	&GUID_Z_AXIS,		0x08,	0x80ffff03,	0x100,
	&GUID_RX_AXIS,		0x0c,	0x80ffff03,	0x100,
	&GUID_RY_AXIS,		0x10,	0x80ffff03,	0x100,
	&GUID_RZ_AXIS,		0x14,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x100,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x100,
	&GUID_POV,			0x20,	0x80ffff10,	0x000,
	&GUID_POV,			0x24,	0x80ffff10,	0x000,
	&GUID_POV,			0x28,	0x80ffff10,	0x000,
	&GUID_POV,			0x2c,	0x80ffff10,	0x000,
	NULL,				0x30,	0x80ffff0c,	0x000,
	NULL,				0x31,	0x80ffff0c,	0x000,
	NULL,				0x32,	0x80ffff0c,	0x000,
	NULL,				0x33,	0x80ffff0c,	0x000,
	NULL,				0x34,	0x80ffff0c,	0x000,
	NULL,				0x35,	0x80ffff0c,	0x000,
	NULL,				0x36,	0x80ffff0c,	0x000,
	NULL,				0x37,	0x80ffff0c,	0x000,
	NULL,				0x38,	0x80ffff0c,	0x000,
	NULL,				0x39,	0x80ffff0c,	0x000,
	NULL,				0x3a,	0x80ffff0c,	0x000,
	NULL,				0x3b,	0x80ffff0c,	0x000,
	NULL,				0x3c,	0x80ffff0c,	0x000,
	NULL,				0x3d,	0x80ffff0c,	0x000,
	NULL,				0x3e,	0x80ffff0c,	0x000,
	NULL,				0x3f,	0x80ffff0c,	0x000,
	NULL,				0x40,	0x80ffff0c,	0x000,
	NULL,				0x41,	0x80ffff0c,	0x000,
	NULL,				0x42,	0x80ffff0c,	0x000,
	NULL,				0x43,	0x80ffff0c,	0x000,
	NULL,				0x44,	0x80ffff0c,	0x000,
	NULL,				0x45,	0x80ffff0c,	0x000,
	NULL,				0x46,	0x80ffff0c,	0x000,
	NULL,				0x47,	0x80ffff0c,	0x000,
	NULL,				0x48,	0x80ffff0c,	0x000,
	NULL,				0x49,	0x80ffff0c,	0x000,
	NULL,				0x4a,	0x80ffff0c,	0x000,
	NULL,				0x4b,	0x80ffff0c,	0x000,
	NULL,				0x4c,	0x80ffff0c,	0x000,
	NULL,				0x4d,	0x80ffff0c,	0x000,
	NULL,				0x4e,	0x80ffff0c,	0x000,
	NULL,				0x4f,	0x80ffff0c,	0x000,
	NULL,				0x50,	0x80ffff0c,	0x000,
	NULL,				0x51,	0x80ffff0c,	0x000,
	NULL,				0x52,	0x80ffff0c,	0x000,
	NULL,				0x53,	0x80ffff0c,	0x000,
	NULL,				0x54,	0x80ffff0c,	0x000,
	NULL,				0x55,	0x80ffff0c,	0x000,
	NULL,				0x56,	0x80ffff0c,	0x000,
	NULL,				0x57,	0x80ffff0c,	0x000,
	NULL,				0x58,	0x80ffff0c,	0x000,
	NULL,				0x59,	0x80ffff0c,	0x000,
	NULL,				0x5a,	0x80ffff0c,	0x000,
	NULL,				0x5b,	0x80ffff0c,	0x000,
	NULL,				0x5c,	0x80ffff0c,	0x000,
	NULL,				0x5d,	0x80ffff0c,	0x000,
	NULL,				0x5e,	0x80ffff0c,	0x000,
	NULL,				0x5f,	0x80ffff0c,	0x000,
	NULL,				0x60,	0x80ffff0c,	0x000,
	NULL,				0x61,	0x80ffff0c,	0x000,
	NULL,				0x62,	0x80ffff0c,	0x000,
	NULL,				0x63,	0x80ffff0c,	0x000,
	NULL,				0x64,	0x80ffff0c,	0x000,
	NULL,				0x65,	0x80ffff0c,	0x000,
	NULL,				0x66,	0x80ffff0c,	0x000,
	NULL,				0x67,	0x80ffff0c,	0x000,
	NULL,				0x68,	0x80ffff0c,	0x000,
	NULL,				0x69,	0x80ffff0c,	0x000,
	NULL,				0x6a,	0x80ffff0c,	0x000,
	NULL,				0x6b,	0x80ffff0c,	0x000,
	NULL,				0x6c,	0x80ffff0c,	0x000,
	NULL,				0x6d,	0x80ffff0c,	0x000,
	NULL,				0x6e,	0x80ffff0c,	0x000,
	NULL,				0x6f,	0x80ffff0c,	0x000,
	NULL,				0x70,	0x80ffff0c,	0x000,
	NULL,				0x71,	0x80ffff0c,	0x000,
	NULL,				0x72,	0x80ffff0c,	0x000,
	NULL,				0x73,	0x80ffff0c,	0x000,
	NULL,				0x74,	0x80ffff0c,	0x000,
	NULL,				0x75,	0x80ffff0c,	0x000,
	NULL,				0x76,	0x80ffff0c,	0x000,
	NULL,				0x77,	0x80ffff0c,	0x000,
	NULL,				0x78,	0x80ffff0c,	0x000,
	NULL,				0x79,	0x80ffff0c,	0x000,
	NULL,				0x7a,	0x80ffff0c,	0x000,
	NULL,				0x7b,	0x80ffff0c,	0x000,
	NULL,				0x7c,	0x80ffff0c,	0x000,
	NULL,				0x7d,	0x80ffff0c,	0x000,
	NULL,				0x7e,	0x80ffff0c,	0x000,
	NULL,				0x7f,	0x80ffff0c,	0x000,
	NULL,				0x80,	0x80ffff0c,	0x000,
	NULL,				0x81,	0x80ffff0c,	0x000,
	NULL,				0x82,	0x80ffff0c,	0x000,
	NULL,				0x83,	0x80ffff0c,	0x000,
	NULL,				0x84,	0x80ffff0c,	0x000,
	NULL,				0x85,	0x80ffff0c,	0x000,
	NULL,				0x86,	0x80ffff0c,	0x000,
	NULL,				0x87,	0x80ffff0c,	0x000,
	NULL,				0x88,	0x80ffff0c,	0x000,
	NULL,				0x89,	0x80ffff0c,	0x000,
	NULL,				0x8a,	0x80ffff0c,	0x000,
	NULL,				0x8b,	0x80ffff0c,	0x000,
	NULL,				0x8c,	0x80ffff0c,	0x000,
	NULL,				0x8d,	0x80ffff0c,	0x000,
	NULL,				0x8e,	0x80ffff0c,	0x000,
	NULL,				0x8f,	0x80ffff0c,	0x000,
	NULL,				0x90,	0x80ffff0c,	0x000,
	NULL,				0x91,	0x80ffff0c,	0x000,
	NULL,				0x92,	0x80ffff0c,	0x000,
	NULL,				0x93,	0x80ffff0c,	0x000,
	NULL,				0x94,	0x80ffff0c,	0x000,
	NULL,				0x95,	0x80ffff0c,	0x000,
	NULL,				0x96,	0x80ffff0c,	0x000,
	NULL,				0x97,	0x80ffff0c,	0x000,
	NULL,				0x98,	0x80ffff0c,	0x000,
	NULL,				0x99,	0x80ffff0c,	0x000,
	NULL,				0x9a,	0x80ffff0c,	0x000,
	NULL,				0x9b,	0x80ffff0c,	0x000,
	NULL,				0x9c,	0x80ffff0c,	0x000,
	NULL,				0x9d,	0x80ffff0c,	0x000,
	NULL,				0x9e,	0x80ffff0c,	0x000,
	NULL,				0x9f,	0x80ffff0c,	0x000,
	NULL,				0xa0,	0x80ffff0c,	0x000,
	NULL,				0xa1,	0x80ffff0c,	0x000,
	NULL,				0xa2,	0x80ffff0c,	0x000,
	NULL,				0xa3,	0x80ffff0c,	0x000,
	NULL,				0xa4,	0x80ffff0c,	0x000,
	NULL,				0xa5,	0x80ffff0c,	0x000,
	NULL,				0xa6,	0x80ffff0c,	0x000,
	NULL,				0xa7,	0x80ffff0c,	0x000,
	NULL,				0xa8,	0x80ffff0c,	0x000,
	NULL,				0xa9,	0x80ffff0c,	0x000,
	NULL,				0xaa,	0x80ffff0c,	0x000,
	NULL,				0xab,	0x80ffff0c,	0x000,
	NULL,				0xac,	0x80ffff0c,	0x000,
	NULL,				0xad,	0x80ffff0c,	0x000,
	NULL,				0xae,	0x80ffff0c,	0x000,
	NULL,				0xaf,	0x80ffff0c,	0x000,
	&GUID_X_AXIS,		0xb0,	0x80ffff03,	0x200,
	&GUID_Y_AXIS,		0xb4,	0x80ffff03,	0x200,
	&GUID_Z_AXIS,		0xb8,	0x80ffff03,	0x200,
	&GUID_RX_AXIS,		0xbc,	0x80ffff03,	0x200,
	&GUID_RY_AXIS,		0xc0,	0x80ffff03,	0x200,
	&GUID_RZ_AXIS,		0xc4,	0x80ffff03,	0x200,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x200,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x200,
	&GUID_X_AXIS,		0xd0,	0x80ffff03,	0x300,
	&GUID_Y_AXIS,		0xd4,	0x80ffff03,	0x300,
	&GUID_Z_AXIS,		0xd8,	0x80ffff03,	0x300,
	&GUID_RX_AXIS,		0xdc,	0x80ffff03,	0x300,
	&GUID_RY_AXIS,		0xe0,	0x80ffff03,	0x300,
	&GUID_RZ_AXIS,		0xe4,	0x80ffff03,	0x300,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x300,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x300,
	&GUID_X_AXIS,		0xf0,	0x80ffff03,	0x400,
	&GUID_Y_AXIS,		0xf4,	0x80ffff03,	0x400,
	&GUID_Z_AXIS,		0xf8,	0x80ffff03,	0x400,
	&GUID_RX_AXIS,		0xfc,	0x80ffff03,	0x400,
	&GUID_RY_AXIS,		0x00,	0x80ffff03,	0x400,
	&GUID_RZ_AXIS,		0x04,	0x80ffff03,	0x400,
	&GUID_SLIDER,		0x18,	0x80ffff03,	0x400,
	&GUID_SLIDER,		0x1c,	0x80ffff03,	0x400,
} ;

// �W���C�X�e�B�b�N�Q�f�o�C�X�f�[�^�t�H�[�}�b�g
D_DIDATAFORMAT C_DFDIJOYSTICK2 =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	0x01,	// dwFlags
	272,	// dwDataSize
	164,	// dwNumObjs
	C_ODFDIJOYSTICK2, // rgodf
} ;


GUID GUIDDIMOUSE2[3] =
{
	0xa36d02e0, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e1, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e2, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
} ;

D_DIOBJECTDATAFORMAT C_ODFDIMOUSE2[11] =
{
	&GUIDDIMOUSE2[0],	0x00, 0x00ffff03, 0x000,
	&GUIDDIMOUSE2[1],	0x04, 0x00ffff03, 0x000,
	&GUIDDIMOUSE2[2],	0x08, 0x80ffff03, 0x000,
	NULL,				0x0c, 0x00ffff0c, 0x000,
	NULL,				0x0d, 0x00ffff0c, 0x000,
	NULL,				0x0e, 0x80ffff0c, 0x000,
	NULL, 				0x0f, 0x80ffff0c, 0x000,
	NULL, 				0x10, 0x80ffff0c, 0x000,
	NULL, 				0x11, 0x80ffff0c, 0x000,
	NULL, 				0x12, 0x80ffff0c, 0x000,
	NULL, 				0x13, 0x80ffff0c, 0x000,
} ;

D_DIDATAFORMAT C_DFDIMOUSE2 =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	2,		// dwFlags
	20,		// dwDataSize
	11,		// dwNumObjs
	C_ODFDIMOUSE2,
} ;


GUID GUIDDIMOUSE[3] =
{
	0xa36d02e0, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e1, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
	0xa36d02e2, 0xc9f3, 0x11cf, 0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00, 
} ;

D_DIOBJECTDATAFORMAT C_ODFDIMOUSE[7] =
{
	&GUIDDIMOUSE[0],	0x00, 0x00ffff03, 0x000,
	&GUIDDIMOUSE[1],	0x04, 0x00ffff03, 0x000,
	&GUIDDIMOUSE[2],	0x08, 0x80ffff03, 0x000,
	NULL,				0x0c, 0x00ffff0c, 0x000,
	NULL,				0x0d, 0x00ffff0c, 0x000,
	NULL,				0x0e, 0x80ffff0c, 0x000,
	NULL, 				0x0f, 0x80ffff0c, 0x000,
} ;

D_DIDATAFORMAT C_DFDIMOUSE =
{
#ifdef _WIN64
	32, 	// dwSize
	24,		// dwObjSize
#else
	24, 	// dwSize
	16,		// dwObjSize
#endif
	2,		// dwFlags
	16,		// dwDataSize
	7,		// dwNumObjs
	C_ODFDIMOUSE,
} ;

#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // DX_NON_INPUT
