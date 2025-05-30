// ============================================================================
//
//		ファイル関係のライブラリ
//
//		Creator			: 山田　巧
//		Creation Data	: 09/17/2004
//
// ============================================================================

#ifndef __FILELIB
#define __FILELIB

// include --------------------------------------
#include <stdio.h>
#include "DxDataType.h"

// define ---------------------------------------

// data type ------------------------------------

// ファイルの日付データ
typedef struct tagFILE_DATE
{
	u64 Create ;			// 作成時間
	u64 LastAccess ;		// 最終アクセス時間
	u64 LastWrite ;			// 最終更新時間
} FILE_DATE ;

// ファイル情報
typedef struct tagFILE_INFO
{
	char *FileName ;			// ファイル名
	char *RelDirectoryPath ;	// 列挙処理時のカレントディレクトリからの相対ディレクトリパス( 語尾に '\' が着いている )
	char *AbsDirectoryPath ;	// 絶対ディレクトリパス( 語尾に '\' が着いている )
	u32 Size ;					// ファイルのサイズ(ディレクトリの場合は何も入っていない)
	u32 IsDirectory ;			// ディレクトリか否か
	FILE_DATE Date ;			// 日付データ
	u32 Attributes ;			// ファイル属性
} FILE_INFO ;

// ファイル情報リスト
typedef struct tagFILE_INFOLIST
{
	int Num ;					// ディレクトリ内のファイルの数
	FILE_INFO  *List ;			// ディレクトリ内のファイルの情報を格納した配列へのポインタ
} FILE_INFOLIST ;

// data -----------------------------------------

// function proto type --------------------------

extern void EasyEncode( void *Data , unsigned int Size ) ;					// データを簡易暗号化する関数
extern void EasyEncodeFileWrite( void *Data, int Size, FILE *fp ) ;			// データを簡易暗号化してファイルに書き出す関数
extern void EasyEncodeFileRead( void *Data, int Size, FILE *fp ) ;			// データを簡易暗号化してファイルから読み込む関数

extern int LoadFileMem( const char *Path, void **DataBuf, int *Size ) ;		// ファイルの内容をメモリに読み込む( 0:成功  -1:失敗 )
extern int LoadFileMem( const char *Path, void *DataBuf, int *Size ) ;		// ファイルの内容をメモリに読み込む( 0:成功  -1:失敗 )
extern int SaveFileMem( const char *Path, void *Data,   int Size ) ;		// メモリの内容をファイルに書き出す 

// 指定のディレクトリを作成する、中間のディレクトリも存在しない場合は作成する
// 最後尾に '\' があっても無視する
// ドライブ名の後に '\' がない場合は正常に動作しない
extern int __CreateDirectory( const char *Path ) ;

// 指定のパスが示しているものがディレクトリかどうかを得る( 1:ディレクトリ  0:ファイル )
extern int IsDirectory( const char *Path ) ;

extern int CreateFileInfo( const char *Path, FILE_INFO *FileInfoBuffer ) ;	// 指定のパスの情報を得る(-1:エラー発生 0:正常終了)
extern int ReleaseFileInfo( FILE_INFO *FileInfo ) ;							// パス情報の後始末を行う
extern int SetFileTimeStamp( const char *Path, FILE_INFO *FileInfo ) ;		// 指定のパスのファイルのタイムスタンプを FileInfo に格納されているタイムスタンプにする
extern int CmpFileTimeStamp( FILE_INFO *FileInfo1, FILE_INFO *FileInfo2, bool Create = true, bool LastAccess = true, bool LastWrite = true );	// 二つのファイルのタイムスタンプが一致しているかどうかを得る(0:一致している -1:一致していない)

// 指定のディレクトリのファイルリストを取得する
// パス文字列の最後に『\』が無くても常に最後の文字列をディレクトリと判断する
// ディレクトリではなかった場合一つだけ情報を返す
// フルパスではない場合は現在のカレントディレクトリからの相対パスとなる
// FileListInfo が 0 ではない場合使用後 ReleaseFileObjectList を呼ぶ必要がある
// SubDirectory を 1 にするとサブディレクトリ内のファイルも全て列挙する
// NULL 、もしくは文字が一つもない文字列を渡した場合カレントディレクトリのファイルが列挙される
// 戻り値 : -1 = エラー  0以上 = 列挙したファイルの数
// OmitDirectory を 1 にするとディレクトリは列挙しない
// OmitName に除外したいファイル名を渡すとその文字列を除外してくれる( ';'で複数の文字列を連結可能 )
// OmitExName に除外したい拡張子を渡すとその拡張子を持つファイルを除外してくれる( ';'で複数の文字列を連結可能 )
// ValidExName に有効にしたい拡張子を渡すとその拡張子を持つファイルのみ列挙してくれる( Omit系よりも優先度が高い, ';'で複数の文字列を連結可能 )
extern int CreateFileList( const char *DirectoryPath, FILE_INFOLIST *FileListInfo = 0,
							int OmitDirectory = 0, int SubDirectory = 0,
							const char *OmitName = 0, const char *OmitExName = 0,
							const char *ValidExName = 0 ) ;
extern int ReleaseFileList( FILE_INFOLIST *DirectoryInfo ) ;

// 特定のパス文字列から色々な情報を取得する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が 0 の場合は実際のカレントディレクトリを使用する )
extern int AnalyseFilePath(
	const char *Src, // DirPath の終端には \ は付かない
	char *FullPath, char *DirPath, char *FileName, char *Name, char *ExeName, const char *CurrentDir = 0 );

// フルパスではないパス文字列をフルパスに変換する
// ( CurrentDir はフルパスである必要がある(語尾に『\』があっても無くても良い) )
// ( CurrentDir が 0 の場合は実際のカレントディレクトリを使用する )
extern int ConvertFullPath__( const char *Src, char *Dest, const char *CurrentDir = 0 ) ;

// ファイル名も一緒になっていると分かっているパス中からファイル名とディレクトリパスを分割する
// フルパスである必要は無い、ファイル名だけでも良い
// DirPath の終端に ￥ マークは付かない
extern int AnalysisFileNameAndDirPath( const char *Src, char *FileName = 0, char *DirPath = 0 ) ;

// ファイルパスからファイル名と拡張子を取得する
extern int AnalysisFileNameAndExeName( const char *Src, char *Name = 0, char *ExeName = 0 ) ;

// ファイルパスの拡張子を変えた文字列を得る
extern int GetChangeExeNamePath( const char *Src, char *Dest, const char *ExeName ) ;

extern void SetEnMark( char *PathBuf ) ;			// 語尾に『\』がついていない場合は付ける
extern void SetChr( char *PathBuf, char chr ) ;		// 語尾に指定の文字がない場合はつける
extern void DelChr( char *PathBuf, char chr ) ;		// 語尾に指定の文字がある場合は削除する

extern int GetExName( const char *Path, char *ExNameBuf ) ;				// 拡張子を得る
extern int SetExName( const char *Path, char *ExName, char *DestBuf ) ;	// 拡張子を変更する

extern int CheckTextData( void *buffer, int size ) ;		// テキストデータかどうかを判定する( 1:テキストデータ  0:バイナリデータ )
extern int CheckTextFile( const char *Path ) ;				// テキストファイルかどうかを判定する( 1:テキストデータ  0:バイナリデータ )

// ２バイト文字か調べる( TRUE:２バイト文字 FALSE:１バイト文字 ) 
extern int CheckMultiByteChar( char *Buf ) ;

#endif