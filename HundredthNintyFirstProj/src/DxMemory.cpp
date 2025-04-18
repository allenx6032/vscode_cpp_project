// -------------------------------------------------------------------------------
// 
// 		ＤＸライブラリ		メモリ関連プログラム
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

// ＤＸライブラリ作成時用定義
#define __DX_MAKE

// インクルード ------------------------------------------------------------------
#include "DxMemory.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxBaseFunc.h"
#include "DxSystem.h"
#include "DxLog.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// マクロ定義 --------------------------------------------------------------------

// 構造体定義 --------------------------------------------------------------------

// テーブル-----------------------------------------------------------------------

// 内部大域変数宣言 --------------------------------------------------------------

MEMORYDATA MemData ;

// 関数プロトタイプ宣言-----------------------------------------------------------

// メモリ確保系関数
static	void		DxPrintAlloc( ALLOCMEM *Alloc, int DataOut = TRUE ) ;						// 確保情報を出力する
static	void		DxPrintAllocSize( void ) ;													// メモリの確保総量を出力する
static	void		DxCopyAlloc( ALLOCMEM *Alloc ) ;											// 確保情報の複製を保存する

// プログラム --------------------------------------------------------------------

// メモリ確保系関数

#if defined( DX_THREAD_SAFE ) || defined( DX_THREAD_SAFE_NETWORK_ONLY )

#define DX_ALLOC_ENTER_CRITICAL_SECTION\
	if( MemData.InitMemoryAllocCriticalSection == 0 )\
	{\
		/* メモリ確保、解放処理用クリティカルセクションの初期化 */\
		CriticalSection_Initialize( &MemData.MemoryAllocCriticalSection ) ;\
		MemData.InitMemoryAllocCriticalSection = 1 ;\
	}\
\
	/* メモリ確保関係の処理を実行中の場合は待つ */\
	CRITICALSECTION_LOCK( &MemData.MemoryAllocCriticalSection ) ;

#define DX_ALLOC_LEAVE_CRITICAL_SECTION\
		/* この関数を使用中、フラグを倒す */\
		CriticalSection_Unlock( &MemData.MemoryAllocCriticalSection ) ;

#else

#define DX_ALLOC_ENTER_CRITICAL_SECTION
#define DX_ALLOC_LEAVE_CRITICAL_SECTION

#endif

// メモリを確保する
extern void *NS_DxAlloc( size_t AllocSize, const char *File, int Line )
{
	ALLOCMEM *mem ;

	DX_ALLOC_ENTER_CRITICAL_SECTION

	// メモリの確保
	mem = (ALLOCMEM *)AllocWrap( AllocSize + DXADDSIZE ) ;
	if( mem == NULL )
	{
		// メモリが足りなかったらその時のメモリをダンプする
		NS_DxDumpAlloc() ;

		// エラー情報も出力する
		NS_DxErrorCheckAlloc() ;

		DX_ALLOC_LEAVE_CRITICAL_SECTION

		return NULL ;
	}

	// メモリ情報追加処理
	{
		// デバッグ領域を特定値で埋める
		_MEMSET( mem->Name, 0xa5, sizeof( mem->Name ) ) ;

		// 情報の保存
		mem->Size = ( unsigned int )AllocSize ;
		if( File != NULL )
		{
			int len = _STRLEN( File ) ;
			_STRCPY( mem->Name, &File[ len < 15 ? 0 : len - 15 ] ) ;
		}
		else
		{
			mem->Name[0] = '\0' ;
		}
		mem->Line = ( unsigned short )Line ;

		mem->ID = MemData.AllocMemoryID ;
		MemData.AllocMemoryID ++ ;

		// リストに追加
		if( MemData.AllocMemoryFirst == NULL )
			MemData.AllocMemoryFirst = &MemData.AllocMemoryAnchor ;
		MemData.AllocMemoryFirst->Next = mem ;
		mem->Back = MemData.AllocMemoryFirst ;
		MemData.AllocMemoryFirst = mem ;
		mem->Next = NULL ;

		// 複製を保存
		DxCopyAlloc( mem ) ;
		DxCopyAlloc( mem->Back ) ;

		// 確保したメモリの総量と総数を加算する
		MemData.AllocMemorySize += AllocSize ;
		MemData.AllocMemoryNum ++ ;
	}

	// 条件が揃っている場合はログを出力する
	if( (int)MemData.AllocTrapSize < 0 || MemData.AllocTrapSize == AllocSize || MemData.AllocMemoryPrintFlag == TRUE )
	{
		DXST_ERRORLOG_ADDW( L"mem alloc  " ) ;
		DxPrintAlloc( mem ) ;
	}

	// 確保しているメモリの総量を出力する
	if( MemData.AllocMemorySizeOutFlag == TRUE )
		DxPrintAllocSize() ;

	// メモリ破壊のチェック
	if( MemData.AllocMemoryErrorCheckFlag == TRUE )
		NS_DxErrorCheckAlloc() ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION
	
	// メモリアドレスを返す
	return (char *)mem + DXADDRESS ;
}

// メモリを確保して０で初期化する
extern void *NS_DxCalloc( size_t AllocSize, const char *File, int Line )
{
	void *buf ;

	// メモリの確保
	buf = NS_DxAlloc( AllocSize, File, Line ) ;
	if( buf == NULL ) return NULL ;
	
	// 確保したメモリを初期化
	_MEMSET( buf, 0, AllocSize ) ;

	// 確保したメモリのアドレスを返す
	return buf ;
}

// メモリの再確保を行う
extern void *NS_DxRealloc( void *Memory, size_t AllocSize, const char *File, int Line )
{
	ALLOCMEM *mem, *back ;
	void *Result ;

	DX_ALLOC_ENTER_CRITICAL_SECTION

	// メモリの再確保
	back = (ALLOCMEM *)( (char *)Memory - DXADDRESS ) ;
	if( Memory == NULL )
	{
		Result = NS_DxAlloc( AllocSize, File, Line ) ;

		DX_ALLOC_LEAVE_CRITICAL_SECTION

		return Result ;
	}
	else
	{
		if( back->Size != AllocSize )
		{
			mem = (ALLOCMEM *)ReallocWrap( (char *)Memory - DXADDRESS, AllocSize + DXADDSIZE ) ;
		}
		else
		{
			mem = back ;
		}
	}
	if( mem == NULL )
	{
		// メモリが足りなかったらその時のメモリをダンプする
		NS_DxDumpAlloc() ;

		DX_ALLOC_LEAVE_CRITICAL_SECTION

		return NULL ;
	}

	// 情報の修正
	{
		// 情報の保存
		_MEMSET( mem->Name, 0xa5, sizeof( mem->Name ) ) ;
		if( File != NULL )
		{
			int len = _STRLEN( File ) ;
			_STRCPY( mem->Name, &File[ len < ALLOCNAMELENGTH ? 0 : len - ALLOCNAMELENGTH ] ) ;
		}
		else
		{
			mem->Name[0] = '\0' ;
		}
		mem->Line = ( unsigned short )Line ;
		mem->ID = MemData.AllocMemoryID ;
		MemData.AllocMemoryID ++ ;

		// 前後の情報の更新
		mem->Back->Next = mem ;
		if( mem->Next != NULL ) mem->Next->Back = mem ;
		if( Memory != NULL && back == MemData.AllocMemoryFirst ) MemData.AllocMemoryFirst = mem ;

		// 確保したメモリの総量の修正
		MemData.AllocMemorySize -= mem->Size ;
		mem->Size = AllocSize ;
		MemData.AllocMemorySize += AllocSize ;

		// 複製を保存
		DxCopyAlloc( mem ) ;
		DxCopyAlloc( mem->Back ) ;
		if( mem->Next != NULL ) DxCopyAlloc( mem->Next ) ;
	}

	// 条件が揃っている場合はログを出力する
	if( (int)MemData.AllocTrapSize < 0 || MemData.AllocTrapSize == AllocSize || MemData.AllocMemoryPrintFlag == TRUE )
	{
		DXST_ERRORLOG_ADDW( L"mem realloc  " ) ;
		DxPrintAlloc( mem ) ;
	}

	// 確保しているメモリの総量を出力する
	if( MemData.AllocMemorySizeOutFlag == TRUE )
		DxPrintAllocSize() ;

	// メモリ破壊のチェック
	if( MemData.AllocMemoryErrorCheckFlag == TRUE )
		NS_DxErrorCheckAlloc() ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// 確保したメモリアドレスを返す
	return (char *)mem + DXADDRESS ;
}

// メモリを解放する
extern void NS_DxFree( void *Memory )
{
	ALLOCMEM *mem ;

	// NULL が渡された場合は何もしない
	if( Memory == NULL ) return ;

	DX_ALLOC_ENTER_CRITICAL_SECTION

	mem = (ALLOCMEM *)( (char *)Memory - DXADDRESS ) ;

	// 解放するメモリの分だけ確保したメモリの総量と数を減らす
	MemData.AllocMemorySize -= mem->Size ;
	MemData.AllocMemoryNum -- ;

	// メモリ破壊のチェック
	if( MemData.AllocMemoryErrorCheckFlag == TRUE )
	{
		NS_DxErrorCheckAlloc() ;
	}
	else
	{
		DxCheckAlloc( mem ) ;
	}

	// 大域データアクセス
	{
		// リストから外す
		if( mem->Back != NULL )	mem->Back->Next = mem->Next ;
		if( mem->Next != NULL ) mem->Next->Back = mem->Back ;
		if( mem == MemData.AllocMemoryFirst ) MemData.AllocMemoryFirst = mem->Back ;

		// 複製を保存
		if( mem->Next != NULL )	DxCopyAlloc( mem->Next ) ;
		if( mem->Back != NULL ) DxCopyAlloc( mem->Back ) ;
	}
	
	// 条件が揃っている場合はログを出力する
	if( (int)MemData.AllocTrapSize < 0 || MemData.AllocTrapSize == mem->Size || MemData.AllocMemoryPrintFlag == TRUE )
	{
		DXST_ERRORLOG_ADDW( L"mem free  " ) ;
		DxPrintAlloc( mem ) ;
	}

	// 確保しているメモリの総量を出力する
	if( MemData.AllocMemorySizeOutFlag == TRUE )
		DxPrintAllocSize() ;
	
	// メモリの解放
#ifdef _DEBUG
//	_MEMSET( mem, 0xfe, mem->Size + DXADDSIZE ) ;
#endif // _DEBUG
	FreeWrap( mem ) ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION
}

// 列挙対象にするメモリの確保容量をセットする
extern size_t NS_DxSetAllocSizeTrap( size_t Size )
{
	size_t trapsize ;

	trapsize = MemData.AllocTrapSize ;
	MemData.AllocTrapSize = Size ;

	return trapsize ;
}

// ＤＸライブラリ内でメモリ確保が行われる時に情報を出力するかどうかをセットする
extern int NS_DxSetAllocPrintFlag( int Flag )
{
	int printflag ;
	
	printflag = MemData.AllocMemoryPrintFlag ;

	MemData.AllocMemoryPrintFlag = Flag ;

	return printflag ;
}

// 確保しているメモリサイズを取得する
extern size_t NS_DxGetAllocSize( void )
{
	return MemData.AllocMemorySize ;
}

// 確保しているメモリの数を取得する
extern int NS_DxGetAllocNum( void )
{
	return MemData.AllocMemoryNum ;
}

// 確保しているメモリを列挙する
extern void NS_DxDumpAlloc( void )
{
	ALLOCMEM *mem ;

	DX_ALLOC_ENTER_CRITICAL_SECTION

	DXST_ERRORLOG_ADDW( L"\n" ) ;
	DXST_ERRORLOG_ADDW( L"Alloc memory dump\n" ) ;

	for( mem = MemData.AllocMemoryFirst ; mem != NULL && mem != &MemData.AllocMemoryAnchor ; mem = mem->Back )
		DxPrintAlloc( mem ) ;

	DxPrintAllocSize() ;

	DXST_ERRORLOG_ADDW( L"\n" ) ;

	DX_ALLOC_LEAVE_CRITICAL_SECTION
}

// 確保したメモリ情報が破壊されていないか調べる( -1:破壊あり  0:なし )
extern int NS_DxErrorCheckAlloc( void )
{
	ALLOCMEM *mem, *back ;
	int counter ;

	DX_ALLOC_ENTER_CRITICAL_SECTION

	counter = 0 ;
	for( mem = MemData.AllocMemoryFirst ; mem != NULL && mem != &MemData.AllocMemoryAnchor ; mem = mem->Back, counter ++ )
	{
		if( DxCheckAlloc( mem ) < 0 )
		{
			return -1 ;
		}
		back = mem ;
	}

	DX_ALLOC_LEAVE_CRITICAL_SECTION

	// 何事も無く終了
	return 0 ;
}

// メモリの確保総量を出力する
static void DxPrintAllocSize( void )
{
	DXST_ERRORLOGFMT_ADDW(( L"\tTotal size:%d(%.3fkb)  Alloc num:%d",MemData.AllocMemorySize, MemData.AllocMemorySize / 1024.0F, MemData.AllocMemoryNum )) ;
}

// 確保情報の複製を保存する
static void DxCopyAlloc( ALLOCMEM *Alloc )
{
	_MEMCPY( (char *)Alloc               + DXALLOC_INFO_SIZE,     Alloc, DXALLOC_INFO_SIZE ) ;
	_MEMCPY( (char *)Alloc + Alloc->Size + DXALLOC_INFO_SIZE * 2, Alloc, DXALLOC_INFO_SIZE ) ;
}

// 確保情報が破壊されていないか調べる
extern int DxCheckAlloc( ALLOCMEM *Alloc )
{
	int res = 0 ;
	int res1 ;
	int res2 ;
	int res3 ;
	ALLOCMEM *FootP, *HeadP ;
	BYTE *p ;

	FootP = ( ALLOCMEM * )( (char *)Alloc + ( ( ALLOCMEM * )( (char *)Alloc + DXALLOC_INFO_SIZE ) )->Size + DXALLOC_INFO_SIZE * 2 ) ;
	HeadP = ( ALLOCMEM * )( (char *)Alloc + DXALLOC_INFO_SIZE ) ;
	res1 = _MEMCMP( HeadP, Alloc, DXALLOC_INFO_SIZE ) == 0 ? 0 : -1 ;
	res2 = _MEMCMP( FootP, Alloc, DXALLOC_INFO_SIZE ) == 0 ? 0 : -1 ;
	if( res1 < 0 || res2 < 0 )
	{
		res3 = _MEMCMP( HeadP, FootP, DXALLOC_INFO_SIZE ) == 0 ? 0 : -1 ;

		DXST_ERRORLOG_ADDUTF16LE( "\xe5\x4e\x0b\x4e\x6e\x30\xe1\x30\xe2\x30\xea\x30\xba\x78\xdd\x4f\xc5\x60\x31\x58\x4c\x30\x34\x78\x0d\x64\x57\x30\x7e\x30\x57\x30\x5f\x30\x01\x30\x37\x5f\x36\x52\x42\x7d\x86\x4e\x57\x30\x7e\x30\x59\x30\x0a\x00\x00"/*@ L"以下のメモリ確保情報が破損しました、強制終了します\n" @*/ ) ;

		p = ( BYTE * )Alloc ;
		DXST_ERRORLOGFMT_ADDW(( L"HEAD  BASE:%02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x    %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x",
			p[0x00], p[0x01], p[0x02], p[0x03],  p[0x04], p[0x05], p[0x06], p[0x07],  p[0x08], p[0x09], p[0x0a], p[0x0b],  p[0x0c], p[0x0d], p[0x0e], p[0x0f],
			p[0x10], p[0x11], p[0x12], p[0x13],  p[0x14], p[0x15], p[0x16], p[0x17],  p[0x18], p[0x19], p[0x1a], p[0x1b],  p[0x1c], p[0x1d], p[0x1e], p[0x1f] ));
		p = ( BYTE * )HeadP ;
		DXST_ERRORLOGFMT_ADDW(( L"HEAD TEMP1:%02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x    %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x",
			p[0x00], p[0x01], p[0x02], p[0x03],  p[0x04], p[0x05], p[0x06], p[0x07],  p[0x08], p[0x09], p[0x0a], p[0x0b],  p[0x0c], p[0x0d], p[0x0e], p[0x0f],
			p[0x10], p[0x11], p[0x12], p[0x13],  p[0x14], p[0x15], p[0x16], p[0x17],  p[0x18], p[0x19], p[0x1a], p[0x1b],  p[0x1c], p[0x1d], p[0x1e], p[0x1f] ));
		p = ( BYTE * )FootP ;
		DXST_ERRORLOGFMT_ADDW(( L"HEAD TEMP2:%02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x    %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x",
			p[0x00], p[0x01], p[0x02], p[0x03],  p[0x04], p[0x05], p[0x06], p[0x07],  p[0x08], p[0x09], p[0x0a], p[0x0b],  p[0x0c], p[0x0d], p[0x0e], p[0x0f],
			p[0x10], p[0x11], p[0x12], p[0x13],  p[0x14], p[0x15], p[0x16], p[0x17],  p[0x18], p[0x19], p[0x1a], p[0x1b],  p[0x1c], p[0x1d], p[0x1e], p[0x1f] ));
		DxPrintAlloc( Alloc ) ;
		DxPrintAlloc( HeadP, FALSE ) ;
		DxPrintAlloc( FootP, FALSE ) ;
		*( ( DWORD * )0 ) = 0xffffffff ;
		res = -1 ;
	}

	return res ;
}

// メモリダンプ
extern void	MemoryDump( void *buffer, int size )
{
#ifndef DX_NON_LITERAL_STRING
	BYTE *dp ;
	int i, j, linenum ;
	wchar_t string[128], string2[128], *sp = NULL, *sp2 = NULL ;

	DXST_ERRORLOG_ADDW( L"\n" ) ;
	linenum = size / 16 ;
	dp = (BYTE *)buffer ;
	for( i = 0 ; i < linenum ; i ++, dp += 16 )
	{
		DXST_ERRORLOGFMT_ADDW(( L"%08x : %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x : %C%C%C%C %C%C%C%C %C%C%C%C %C%C%C%C\n",
			dp, dp[0], dp[1], dp[2], dp[3],  dp[4], dp[5], dp[6], dp[7],  dp[8], dp[9], dp[10], dp[11],  dp[12], dp[13], dp[14], dp[15],
			dp[0], dp[1], dp[2], dp[3],  dp[4], dp[5], dp[6], dp[7],  dp[8], dp[9], dp[10], dp[11],  dp[12], dp[13], dp[14], dp[15] )) ;
	}

	size -= linenum * 16 ;
	j     = 0 ;
	for( i = 0 ; i < size ; i ++, dp ++ )
	{
		if( j == 0 )
		{
			sp  = string  ;
			sp2 = string2 ;
			_SWPRINTF( sp, L"%08x : ", dp ) ;
			sp += 11 ;
		}

		_SWPRINTF( sp, L"%02x ", *dp ) ;
		sp += 3 ;
		sp2[0] = ( wchar_t )*dp ;
		sp2[1] = '\0' ;
		sp2 ++ ;

		j ++ ;
		if( j == 16 )
		{
			_SWPRINTF( sp, L": %s\n", string2 ) ;
			DXST_ERRORLOG_ADDW( string ) ;
			j = 0 ;
		}
	}
	if( j != 0 )
	{
		_SWPRINTF( sp, L": %s\n", string2 ) ;
		DXST_ERRORLOG_ADDW( string ) ;
	}
#endif
}

// メモリが確保、解放されるたびに確保しているメモリの容量を出力するかどうかのフラグをセットする
extern int NS_DxSetAllocSizeOutFlag( int Flag )
{
	int OutFlag ;

	OutFlag = MemData.AllocMemorySizeOutFlag ;
	MemData.AllocMemorySizeOutFlag = Flag ;

	return OutFlag ;
}

// メモリの確保、解放が行われる度に確保しているメモリ確保情報が破損していないか調べるかどうかのフラグをセットする
extern int NS_DxSetAllocMemoryErrorCheckFlag( int Flag )
{
	int CheckFlag ;

	CheckFlag = MemData.AllocMemoryErrorCheckFlag ;
	MemData.AllocMemoryErrorCheckFlag = Flag ;

	return CheckFlag ;
}

// 確保情報を出力する
static void DxPrintAlloc( ALLOCMEM *Alloc, int DataOut )
{
#ifndef DX_NON_LITERAL_STRING
	wchar_t str[256], *p ;
	BYTE *d ;
	wchar_t Name_w[ 512 ] ;
	size_t len, i ;

	ConvString( Alloc->Name, DX_CODEPAGE_SHIFTJIS, ( char * )Name_w, WCHAR_T_CODEPAGE ) ;

	_SWPRINTF( str, L"\tsize:%10d(%10.3fkb)  file:%-26s  line:%-5d  ID:%-5d  addr:%08x",Alloc->Size, Alloc->Size / 1024.0F, Name_w, Alloc->Line, (int)Alloc->ID, (DWORD_PTR)Alloc + DXADDRESS ) ;

	if( DataOut )
	{
		_WCSCPY( str + _WCSLEN(str), L"  data:<" ) ;
		len = 16 < Alloc->Size ? 16 : Alloc->Size ;
		p = str + _WCSLEN(str) ;
		d = ( BYTE * )Alloc + DXADDRESS ;
		for( i = 0 ; i < len ; i ++, p ++ )
		{
			*p = ( wchar_t )( d[i] < 0x20 ? '.' : d[i] ) ;
		}
		_WCSCPY( p, L"> [" ) ;
		p += 3 ;
		for( i = 0 ; i < len ; i ++, p += 3, d ++ )
		{
			_SWPRINTF( p, L"%02x ", *((unsigned char *)d) ) ;
		}
		_WCSCPY( &p[-1], L"]\n" ) ;
	}
	else
	{
		_WCSCPY( str + _WCSLEN(str), L"\n" ) ;
	}

	DXST_ERRORLOG_ADDW( str ) ;
#endif
}













#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE
