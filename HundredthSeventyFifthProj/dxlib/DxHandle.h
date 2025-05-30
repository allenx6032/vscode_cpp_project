// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		ハンドル管理プログラムヘッダファイル
// 
// 				Ver 3.24b
// 
// -------------------------------------------------------------------------------

#ifndef DX_HANDLE_H
#define DX_HANDLE_H

// インクルード ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxThread.h"

#ifndef DX_NON_ASYNCLOAD
	#include "DxASyncLoad.h"
#endif // DX_NON_ASYNCLOAD

#ifndef DX_NON_NAMESPACE

namespace DxLib
{

#endif // DX_NON_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// ハンドルの内訳
#define DX_HANDLEINDEX_MASK							(0x0003ffff)		// ハンドル配列インデックスマスク
#define DX_HANDLEINDEX_MAX							(0x40000)			// ハンドル配列インデックスの最大数

#define DX_HANDLECHECK_MASK							(0x03fc0000)		// ハンドルの正当性チェック用マスク
#define DX_HANDLECHECK_ADDRESS						(18)				// ハンドルの正当性チェック用マスクの開始アドレス
#define DX_HANDLECHECK_MAX							(256)				// ハンドルの正当性チェック値の最大数

#define DX_HANDLETYPE_MASK							(0x7c000000)		// ハンドルタイプマスク
#define DX_HANDLETYPE_ADDRESS						(26)				// ハンドルタイプマスクの開始アドレス
#define DX_HANDLETYPE_MAX							(32)				// ハンドルタイプの最大数

#define DX_HANDLEERROR_MASK							(0x80000000)		// エラーチェックマスク( ０ではなかったらエラー )

#define DX_HANDLETYPE_MASK_GRAPH					(DX_HANDLETYPE_GRAPH                  << DX_HANDLETYPE_ADDRESS)		// グラフィックハンドル
#define DX_HANDLETYPE_MASK_SOFTIMAGE				(DX_HANDLETYPE_SOFTIMAGE              << DX_HANDLETYPE_ADDRESS)		// ソフトウエアで扱うイメージハンドル
#define DX_HANDLETYPE_MASK_SOUND					(DX_HANDLETYPE_SOUND                  << DX_HANDLETYPE_ADDRESS)		// サウンドハンドル
#define DX_HANDLETYPE_MASK_SOFTSOUND				(DX_HANDLETYPE_SOFTSOUND              << DX_HANDLETYPE_ADDRESS)		// ソフトサウンドハンドル
#define DX_HANDLETYPE_MASK_MUSIC					(DX_HANDLETYPE_MUSIC                  << DX_HANDLETYPE_ADDRESS)		// ミュージックハンドル
#define DX_HANDLETYPE_MASK_MOVIE					(DX_HANDLETYPE_MOVIE                  << DX_HANDLETYPE_ADDRESS)		// ムービーハンドル
#define DX_HANDLETYPE_MASK_GMASK					(DX_HANDLETYPE_GMASK                  << DX_HANDLETYPE_ADDRESS)		// マスクハンドル
#define DX_HANDLETYPE_MASK_FONT						(DX_HANDLETYPE_FONT                   << DX_HANDLETYPE_ADDRESS)		// フォントハンドル
#define DX_HANDLETYPE_MASK_KEYINPUT					(DX_HANDLETYPE_KEYINPUT               << DX_HANDLETYPE_ADDRESS)		// 文字列入力ハンドル
#define DX_HANDLETYPE_MASK_NETWORK					(DX_HANDLETYPE_NETWORK                << DX_HANDLETYPE_ADDRESS)		// ネットワークハンドル
#define DX_HANDLETYPE_MASK_LIGHT					(DX_HANDLETYPE_LIGHT                  << DX_HANDLETYPE_ADDRESS)		// ライト
#define DX_HANDLETYPE_MASK_SHADER					(DX_HANDLETYPE_SHADER                 << DX_HANDLETYPE_ADDRESS)		// シェーダーハンドル
#define DX_HANDLETYPE_MASK_MODEL_BASE				(DX_HANDLETYPE_MODEL_BASE             << DX_HANDLETYPE_ADDRESS)		// ３Ｄモデル基本データ
#define DX_HANDLETYPE_MASK_MODEL					(DX_HANDLETYPE_MODEL                  << DX_HANDLETYPE_ADDRESS)		// ３Ｄモデル
#define DX_HANDLETYPE_MASK_VERTEX_BUFFER			(DX_HANDLETYPE_VERTEX_BUFFER          << DX_HANDLETYPE_ADDRESS)		// 頂点バッファハンドル
#define DX_HANDLETYPE_MASK_INDEX_BUFFER				(DX_HANDLETYPE_INDEX_BUFFER           << DX_HANDLETYPE_ADDRESS)		// インデックスバッファハンドル
#define DX_HANDLETYPE_MASK_FILE						(DX_HANDLETYPE_FILE                   << DX_HANDLETYPE_ADDRESS)		// ファイルハンドル
#define DX_HANDLETYPE_MASK_SHADOWMAP				(DX_HANDLETYPE_SHADOWMAP              << DX_HANDLETYPE_ADDRESS)		// シャドウマップハンドル
#define DX_HANDLETYPE_MASK_SHADER_CONSTANT_BUFFER	(DX_HANDLETYPE_SHADER_CONSTANT_BUFFER << DX_HANDLETYPE_ADDRESS)		// シェーダー用定数バッファハンドル
#define DX_HANDLETYPE_MASK_LIVE2D_CUBISM3_MODEL		(DX_HANDLETYPE_LIVE2D_CUBISM4_MODEL	  << DX_HANDLETYPE_ADDRESS)		// Live2D Cubism 4 モデルハンドル

// ハンドルの有効チェック

#define HANDLECHKFULL_ASYNC( TYPE, HANDLE, INFO )																\
	  ( ( HandleManageArray[ (TYPE) ].InitializeFlag == FALSE ) ||												\
		( ( (HANDLE) & DX_HANDLEERROR_MASK ) != 0 ) ||															\
		( ( (HANDLE) & DX_HANDLETYPE_MASK  ) != HandleManageArray[ (TYPE) ].HandleTypeMask ) ||					\
		( ( (HANDLE) & DX_HANDLEINDEX_MASK ) >= HandleManageArray[ (TYPE) ].MaxNum ) ||							\
		( ( INFO = HandleManageArray[ (TYPE) ].Handle[ (HANDLE) & DX_HANDLEINDEX_MASK ] ) == NULL ) ||			\
		( (int)( (INFO)->ID << DX_HANDLECHECK_ADDRESS ) != ( (HANDLE) & DX_HANDLECHECK_MASK ) ) )

#ifndef DX_NON_ASYNCLOAD

	#define HANDLECHKFULL( TYPE, HANDLE, INFO )																			\
		  ( ( HandleManageArray[ (TYPE) ].InitializeFlag == FALSE ) ||												\
			( ( (HANDLE) & DX_HANDLEERROR_MASK ) != 0 ) ||															\
			( ( (HANDLE) & DX_HANDLETYPE_MASK ) != HandleManageArray[ (TYPE) ].HandleTypeMask ) ||					\
			( ( (HANDLE) & DX_HANDLEINDEX_MASK ) >= HandleManageArray[ (TYPE) ].MaxNum ) ||							\
			( ( INFO = HandleManageArray[ (TYPE) ].Handle[ (HANDLE) & DX_HANDLEINDEX_MASK ] ) == NULL ) ||			\
			( (int)( (INFO)->ID << DX_HANDLECHECK_ADDRESS ) != ( (HANDLE) & DX_HANDLECHECK_MASK ) ) ||				\
			( (INFO)->ASyncLoadCount != 0 && ( MainThreadProcessASyncLoadData( (INFO)->ASyncDataNumber ) < 0 || HandleManageArray[ (TYPE) ].Handle[ (HANDLE) & DX_HANDLEINDEX_MASK ] == NULL ) ) )

#else // DX_NON_ASYNCLOAD

	#define HANDLECHKFULL( MANAGE, HANDLE, INFO )		HANDLECHKFULL_ASYNC( MANAGE, HANDLE, INFO )

#endif // DX_NON_ASYNCLOAD


#ifdef DX_NON_HANDLE_ERROR_CHECK

	#define HANDLECHK_ASYNC( TYPE, HANDLE, INFO )																	\
		  ( ( ( INFO = HandleManageArray[ (TYPE) ].Handle[ (HANDLE) & DX_HANDLEINDEX_MASK ] ) == NULL ) )

	#ifndef DX_NON_ASYNCLOAD

		#define HANDLECHK( TYPE, HANDLE, INFO )																			\
			  ( ( ( INFO = HandleManageArray[ (TYPE) ].Handle[ (HANDLE) & DX_HANDLEINDEX_MASK ] ) == NULL ) ||			\
				( (INFO)->ASyncLoadCount != 0 ) )

	#else // DX_NON_ASYNCLOAD

		#define HANDLECHK( MANAGE, HANDLE, INFO )		HANDLECHK_ASYNC( MANAGE, HANDLE, INFO )

	#endif // DX_NON_ASYNCLOAD


#else // DX_NON_HANDLE_ERROR_CHECK

	#define HANDLECHK_ASYNC( TYPE, HANDLE, INFO )	HANDLECHKFULL_ASYNC( TYPE, HANDLE, INFO )
	#define HANDLECHK( TYPE, HANDLE, INFO )			HANDLECHKFULL( TYPE, HANDLE, INFO )

#endif // DX_NON_HANDLE_ERROR_CHECK

// 構造体定義 --------------------------------------------------------------------

// シンプルリスト構造体
struct SIMPLELIST
{
	void					*Data ;								// データへのポインタ
	struct SIMPLELIST		*Prev, *Next ;						// リストの一つ前と次の要素へのポインタ
} ;

// ハンドルリスト構造体
struct HANDLELIST
{
	int						Handle ;							// ハンドル
	void					*Data ;								// データへのポインタ
	struct HANDLELIST		*Prev, *Next ;						// リストの一つ前と次の要素へのポインタ
} ;

// ハンドルの共通データ
struct HANDLEINFO
{
	int						ID ;								// エラーチェック用ＩＤ
	int						Handle ;							// 自身のハンドル値
	int						AllocSize ;							// メモリの確保サイズ
	int						*DeleteFlag ;						// ハンドル削除時に−１にする変数へのポインタ
#ifndef DX_NON_ASYNCLOAD
	int						ASyncLoadCount ;					// 非同期読み込み処理の対象となっている数
	int						ASyncLoadResult ;					// 非同期読み込み処理の結果
	int						ASyncDataNumber ;					// 非同期読み込み処理番号
	volatile int			ASyncLoadFinishDeleteRequestFlag ;	// 非同期読み込みが完了したらハンドルを削除するフラグ
	volatile void			( *ASyncLoadFinishCallback )( int Handle, void *Data ) ;	// 非同期読み込みが完了したら呼ばれるコールバック関数
	void					*ASyncLoadFinishCallbackData ;		// 非同期読み込みが完了したら呼ばれるコールバック関数に渡す引数
#endif
	HANDLELIST				List ;								// ハンドルリストの一つ前と次の要素へのポインタ
} ;

// ハンドル管理の共通データ
struct HANDLEMANAGE
{
	int						InitializeFlag ;					// 初期化フラグ
	HANDLEINFO				**Handle ;							// ハンドルへのポインタ配列
	HANDLELIST				ListFirst ;							// ハンドルリストの先頭
	HANDLELIST				ListLast ;							// ハンドルリストの末端
	int						HandleTypeMask ;					// ハンドルタイプマスク
	int						OneSize ;							// ハンドル一つ辺りの大きさ
	int						MaxNum ;							// ハンドルの最大サイズ
	int						Num ;								// ハンドルの数
	int						AreaMin ;							// ハンドル分布エリア最小値
	int						AreaMax ;							// ハンドル分布エリア最大値
	int						NextID ;							// 次のハンドルに割り当てるＩＤ
	DX_CRITICAL_SECTION		CriticalSection ;					// データアクセス時用クリティカルセクション
	int						( *InitializeFunction )( HANDLEINFO *HandleInfo ) ;	// ハンドルの初期化をする関数へのポインタ
	int						( *TerminateFunction )( HANDLEINFO *HandleInfo ) ;	// ハンドルの後始末をする関数へのポインタ
	int						( *DumpInfoFunction )( HANDLEINFO *HandleInfo ) ;	// ハンドルの情報を出力する関数へのポインタ
	const wchar_t			*Name ;								// ハンドル名
	char					NameUTF16LE[ 128 ] ;				// ハンドル名( UTF16LE )
} ;

// 内部大域変数宣言 --------------------------------------------------------------

extern HANDLEMANAGE HandleManageArray[ DX_HANDLETYPE_MAX ] ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// ハンドル共通関係
extern	int		InitializeHandleManage( int HandleType, int OneSize, int MaxNum, int ( *InitializeFunction )( HANDLEINFO *HandleInfo ), int ( *TerminateFunction )( HANDLEINFO *HandleInfo ), int ( *DumpInfoFunction )( HANDLEINFO *HandleInfo ), const wchar_t *Name ) ;	// ハンドル管理情報を初期化する( InitializeFlag には FALSE が入っている必要がある )
extern	int		TerminateHandleManage( int HandleType ) ;																		// ハンドル管理情報の後始末を行う

extern	int		AddHandle( int HandleType, int ASyncThread, int Handle /* = -1 */ ) ;										// ハンドルを追加する
extern	int		SubHandle( int Handle ) ;															// ハンドルを削除する
extern	int		ReallocHandle( int Handle, size_t NewSize ) ;										// ハンドルの情報を格納するメモリ領域のサイズを変更する、非同期読み込み中でないことが前提
extern	HANDLEINFO *GetHandleInfo( int Handle ) ;													// ハンドルの情報を取得する
extern	int		AllHandleSub( int HandleType, int (*DeleteCancelCheckFunction)( HANDLEINFO *HandleInfo ) = NULL ) ;	// ハンドル管理情報に登録されているすべてのハンドルを削除
#ifndef DX_NON_ASYNCLOAD
extern	int		IncASyncLoadCount( int Handle, int ASyncDataNumber ) ;								// ハンドルの非同期読み込み中カウントをインクリメントする
extern	int		DecASyncLoadCount( int Handle ) ;													// ハンドルの非同期読み込み中カウントをデクリメントする
extern	int		GetASyncLoadFinishDeleteFlag( int Handle ) ;										// ハンドルの非同期読み込み完了後に削除するかどうかのフラグを取得する
extern	int		WaitASyncLoad( int Handle ) ;														// ハンドルが非同期読み込み中だった場合、非同期読み込みが完了するまで待つ
#endif // DX_NON_ASYNCLOAD


// ハンドルリスト
extern	int		InitializeHandleList( HANDLELIST *First, HANDLELIST *Last ) ;					// リストの初期化
extern	int		AddHandleList( HANDLELIST *First, HANDLELIST *List, int Handle, void *Data ) ;	// リストへ要素を追加
extern	int		SubHandleList( HANDLELIST *List ) ;												// リストから要素を外す
extern	int		NewMemoryHandleList( HANDLELIST *List, void *Data ) ;							// リストが存在するメモリが変更された場合にリストの前後を更新する


// シンプルリスト
extern	int		InitializeSimpleList( SIMPLELIST *First, SIMPLELIST *Last ) ;					// シンプルリストの初期化
extern	int		AddSimpleList( SIMPLELIST *First, SIMPLELIST *List, void *Data ) ;				// シンプルリストへ要素を追加
extern	int		SubSimpleList( SIMPLELIST *List ) ;												// シンプルリストから要素を外す
extern	int		NewMemorySimpleList( SIMPLELIST *List, void *Data ) ;							// シンプルリストが存在するメモリが変更された場合にリストの前後を更新する


#ifndef DX_NON_NAMESPACE

}

#endif // DX_NON_NAMESPACE

#endif // DX_HANDLE_H
