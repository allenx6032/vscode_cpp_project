// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		入力情報プログラムヘッダファイル
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXINPUT_H__
#define __DXINPUT_H__

#include "DxCompileConfig.h"

#ifndef DX_NON_INPUT

// インクルード ------------------------------------------------------------------
#include "DxLib.h"
#include "DxStatic.h"
#include "DxDirectX.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// モーターの番号定義
#define DINPUTPAD_MOTOR_LEFT		0
#define DINPUTPAD_MOTOR_RIGHT		1
#define DINPUTPAD_MOTOR_NUM			2

// 構造体定義 --------------------------------------------------------------------

// パッドの振動の情報
struct DINPUTPAD_EFFECT
{
	D_IDirectInputEffect	*DIEffect ;							// パッドデバイスエフェクトオブジェクト
	int						PlayFlag ;							// エフェクトが再生中か、フラグ
	int						Power ;								// エフェクトの強さ
	int						Time ;								// エフェクトの長さ
	int						CompTime ;							// エフェクトを開始してから経過した時間
	int						BackTime ;							// エフェクトの前回調べた時刻
	int						PlayState ;							// DirectInputEffect の再生状態(ソフトが非アクティブになれば再生停止する為)
	int						SetPower ;							// DirectInputEffect の設定パワー
} ;

// パッドの情報
struct DINPUTPAD
{
	int						XInputDeviceNo ;					// XInputでアクセスするデバイスの場合は０以上の値が入る( DirectInput で扱う場合は -1 )
	D_XINPUT_STATE			XInputState ;						// XInputでのパッドの状態

	D_IDirectInputDevice7	*Device ;							// パッドデバイスオブジェクト
//	D_IDirectInputEffect	*LeftEffect ;						// パッドデバイスエフェクトオブジェクト
//	D_IDirectInputEffect	*RightEffect ;						// パッドデバイスエフェクトオブジェクト
	HANDLE					Event ;								// パッドのイベントハンドル
	D_DIJOYSTATE			State ;								// 状態
	int						RightStickFlag ;					// 右スティックがあるかどうか
	int						GetTime ;							// 取得時間
//	int						EffectNotCommonFlag ;				// 左右のモーターでパラメータを共有しないかどうかのフラグ
//	DINPUTPAD_EFFECT		EffectCommon ;						// 左右のモーターでパラメータを共有する場合の情報
	DINPUTPAD_EFFECT		Effect[ DINPUTPAD_MOTOR_NUM ] ;		// モーターの情報
//	int						EffectPlayFlag ;					// エフェクトが再生中か、フラグ
//	int						EffectPower ;						// エフェクトの強さ
//	int						EffectTime ;						// エフェクトの長さ
//	int						EffectCompTime ;					// エフェクトを開始してから経過した時間
//	int						EffectBackTime ;					// エフェクトの前回調べた時刻
//	int						EffectPlayStateLeft ;				// DirectInputEffect 上での、事実上の再生状態(ソフトが非アクティブになれば再生停止する為)
//	int						EffectPlayStateRight ;				// DirectInputEffect 上での、事実上の再生状態(ソフトが非アクティブになれば再生停止する為)
	wchar_t					InstanceName[MAX_PATH];				// デバイスの登録名
	wchar_t					ProductName[MAX_PATH];				// デバイスの製品登録名
	DWORD					DeadZone ;							// 無効ゾーン( 65536 = 100% )
} ;

// ＤｉｒｅｃｔＩｎｐｕｔ制御用データ構造体型
struct DINPUTDATA
{
	HMODULE					XInputDLL ;							// ＸＩｎｐｕｔのＤＬＬ

	DWORD					( WINAPI * XInputGetStateFunc )( DWORD dwUserIndex, D_XINPUT_STATE* pState ) ;
	DWORD					( WINAPI * XInputSetStateFunc )( DWORD dwUserIndex, D_XINPUT_VIBRATION* pVibration ) ;

	int						NoUseXInputFlag ;					// ＸＩｎｐｕｔを使用しないかどうかのフラグ
	int						NoUseDirectInputFlag ;				// ＤｉｒｅｃｔＩｎｐｕｔを使用しないかどうかのフラグ
	int						NoUseVibrationFlag ;				// ＤｉｒｅｃｔＩｎｐｕｔの振動機能を使用しないかどうかのフラグ
	int						UseDirectInputFlag ;				// ＤｉｒｅｃｔＩｎｐｕｔを使用するかどうかのフラグ( NoUseDirectInputFlag を補助するもの )
	int						UseDirectInput8Flag ;				// ＤｉｒｅｃｔＩｎｐｕｔ８を使用するかどうかのフラグ
	D_IDirectInput7			*DirectInputObject ;				// ＤｉｒｅｃｔＩｎｐｕｔオブジェクト

	D_IDirectInputDevice7	*MouseDeviceObject ;				// マウスデバイスオブジェクト
	D_IDirectInputDevice7	*KeyboardDeviceObject ;				// キーボードデバイスオブジェクト
	HANDLE					KeyEvent ;							// キーボードのイベントハンドル
	int						KeyInputGetTime ;					// 一つ前に状態を取得した時間
	unsigned char			KeyInputBuf[256] ;					// キーボードの入力状態
	int						KeyExclusiveCooperativeLevelFlag ;	// キーボードの協調レベルが排他レベルになっているかどうかフラグ
	int						KeyToJoypadInputInitializeFlag ;	// KeyToJoypadInput を一度でも初期化したかどうかのフラグ
	int						KeyToJoypadInput[ MAX_JOYPAD_NUM ][ 32 ][ 4 ] ;	// ジョイパッドの入力に対応したキーボードの入力
	int						MouseInputBufValidFlag ;			// MouseInputBuf が有効かどうか( TRUE:有効  FALSE:無効 )
	BYTE					MouseInputBuf[ 4 ] ;				// マウスの入力状態( 0:LEFT 1:RIGHT 2:MIDDLE )
	int						MouseMoveZ ;						// マウスホイールの移動値

	int						JoyPadNum ;							// ジョイパッドの数
	DINPUTPAD				Joypad[ MAX_JOYPAD_NUM ] ;			// パッドデバイス情報

	int						KeyboardNotDirectInputFlag ;		// キーボードデバイスのアクセスに DirectInput を使用しないか、フラグ
	int						EndFlag ;
	int						F10Flag ;							// F10 が押されたかフラグ
	int						F10InputTime ;						// F10 が押された時間
	int						F12Flag ;							// F12 が押されたかフラグ
	int						F12InputTime ;						// F12 が押された時間
} ;

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

// 関数プロトタイプ宣言-----------------------------------------------------------

// ＤｉｒｅｃｔＩｎｐｕｔの初期化、終了関数
extern	int InitializeDirectInput( void ) ;											// ＤｉｒｅｃｔＩｎｐｕｔを初期化する 
extern	int TerminateDirectInput( void ) ;											// ＤｉｒｅｃｔＩｎｐｕｔの使用を終了する

extern	int SetF10Input( void ) ;													// F10 が押されたことにするフラグを立てる
extern	int ResetF10Input( void ) ;													// F10 が離されたことにするフラグを立てる
extern	int SetF12Input( void ) ;													// F12 が押されたことにするフラグを立てる
extern	int ResetF12Input( void ) ;													// F12 が離されたことにするフラグを立てる
extern	int RefreshInputDeviceAcquireState( void ) ;								// ＤｉｒｅｃｔＩｎｐｕｔデバイスの取得状態を更新する
extern	int JoypadEffectProcess( void ) ;											// パッドエフェクトに関するフレーム処理を行う
extern	int RefreshEffectPlayState( void ) ;										// パッドエフェクトの再生状態を更新する(ソフトが非アクティブになることもあるため)
extern	int UpdateKeyboardInputState( int UseProcessMessage = TRUE ) ;				// 入力状態の更新
extern	int SetupJoypad( void ) ;													// ジョイパッドのセットアップを行う
extern	int TerminateJoypad( void ) ;												// ジョイパッドの後始末を行う
extern	int CheckUseDirectInputMouse( void ) ;										// マウスの情報取得にＤｉｒｅｃｔＩｎｐｕｔを使用しているかどうかを取得する( 戻り値  TRUE:DirectInputを使用している  FALSE:DirectInputを使用していない )
extern	int GetDirectInputMouseMoveZ( int CounterReset = TRUE ) ;					// ＤｉｒｅｃｔＩｎｐｕｔを使用したマウスホイールの移動値を取得する
extern	float GetDirectInputMouseMoveZF( int CounterReset = TRUE ) ;				// ＤｉｒｅｃｔＩｎｐｕｔを使用したマウスホイールの移動値を取得する( float版 )

// wchar_t版関数
extern int GetJoypadName_WCHAR_T( int InputType, wchar_t *InstanceNameBuffer, wchar_t *ProductNameBuffer ) ;


#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // DX_NON_INPUT

#endif // __DXINPUT_H__
