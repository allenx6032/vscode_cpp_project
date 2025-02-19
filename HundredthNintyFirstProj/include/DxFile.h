// -------------------------------------------------------------------------------
// 
// 		�c�w���C�u����		�t�@�C���A�N�Z�X�v���O�����w�b�_�t�@�C��
// 
// 				Ver 3.13f
// 
// -------------------------------------------------------------------------------

#ifndef __DXFILE_H__
#define __DXFILE_H__

// �C���N���[�h ------------------------------------------------------------------
#include "DxCompileConfig.h"
#include "DxLib.h"
#include "DxStatic.h"
#include "DxHandle.h"

#ifdef DX_USE_NAMESPACE

namespace DxLib
{

#endif // DX_USE_NAMESPACE

// �}�N����` --------------------------------------------------------------------

// ���C�u���������Ŏg�p����t�@�C���A�N�Z�X�p�֐�
#define FSYNC( handle )                             {while( FIDLECHK( handle ) == FALSE ) Thread_Sleep(0);}
#define FOPEN( path )								StreamOpen( (path), FALSE, TRUE, FALSE )
#define FOPENT( path )								StreamOpenT( (path), FALSE, TRUE, FALSE )
#define FOPEN_CACHE( path )							StreamOpen( (path),  TRUE, TRUE, FALSE )
#define FOPEN_ASYNC( path )							StreamOpen( (path), FALSE, TRUE,  TRUE )
#define FCLOSE( handle )							StreamClose( (DWORD_PTR)(handle) )
#define FREAD( buf, length, num, handle )			StreamRead( (void *)(buf), (size_t)(length), (size_t)(num), (DWORD_PTR)(handle) )
#define FSEEK( handle, pos, mode )					StreamSeek( (DWORD_PTR)(handle), (LONGLONG)(pos), (int)(mode) )
#define FTELL( handle )								StreamTell( (DWORD_PTR)(handle) )
#define FEOF( handle )								StreamEof( (DWORD_PTR)(handle) )
#define FIDLECHK( handle )							StreamIdleCheck( (DWORD_PTR)(handle) )
#define FSETDIR( path )								StreamChDir( (path) )
#define FGETDIR( buffer )							StreamGetDir( (buffer) )
#define FFINDFIRST( path, buffer )					StreamFindFirst( (path), (buffer) )
#define FFINDNEXT( handle, buffer )					StreamFindNext( (DWORD_PTR)(handle), (buffer) )
#define FFINDCLOSE( handle )						StreamFindClose( (DWORD_PTR)(handle) )

// �t�@�C���A�N�Z�X��p�X���b�h�ւ̎w��
#define FILEACCESSTHREAD_FUNCTION_OPEN		(0)				// �t�@�C�����J��
#define FILEACCESSTHREAD_FUNCTION_CLOSE		(1)				// �t�@�C�������
#define FILEACCESSTHREAD_FUNCTION_READ		(2)				// �t�@�C����ǂݍ���
#define FILEACCESSTHREAD_FUNCTION_SEEK		(3)				// �t�@�C���|�C���^���ړ�����
#define FILEACCESSTHREAD_FUNCTION_EXIT		(4)				// �I������

#define FILEACCESSTHREAD_DEFAULT_CACHESIZE	(128 * 1024)	// �f�t�H���g�̃L���b�V���T�C�Y

// �n���h���^�C�v
#define FILEHANDLETYPE_NORMAL				(0)				// �ʏ�̃t�@�C���n���h��
#define FILEHANDLETYPE_FULLYLOAD			(1)				// �t�@�C���̓��e���������ɓǂݍ��񂾏����p�̃n���h��

// �t�@�C���p�X�̍ő咷
#define FILEPATH_MAX						(512 * 3)

// �\���̒�` --------------------------------------------------------------------

// �t�@�C���A�N�Z�X�n���h��
struct FILEACCESSINFO
{
	HANDLEINFO				HandleInfo ;						// �n���h�����ʃf�[�^
	int						HandleType ;						// �n���h���^�C�v( FILEHANDLETYPE_NORMAL �Ȃ� )

	// FILEHANDLETYPE_NORMAL �ł̂ݎg�p
	DWORD_PTR				FilePointer ;						// �t�@�C���|�C���^

	// FILEHANDLETYPE_FULLYLOAD �ł̂ݎg�p
	void					*FileImage ;						// �ǂݍ��񂾃t�@�C�����i�[�����������̈�ւ̃|�C���^
	LONGLONG				FileSize ;							// �t�@�C���T�C�Y
} ;

// �t�@�C�����S�ǂݍ��݃f�[�^
struct FILEFULLYLOADINFO
{
} ;

// �������ϐ��錾 --------------------------------------------------------------

// �f�t�H���g�X�g���[���t�@���N�V����
extern STREAMDATASHREDTYPE2W StreamFunctionW ;

// �֐��v���g�^�C�v�錾-----------------------------------------------------------

// �t�@�C���A�N�Z�X�����̏������E�I���֐�
extern	int			InitializeFile( void ) ;														// �t�@�C���A�N�Z�X�����̏�����
extern	int			TerminateFile( void ) ;															// �t�@�C���A�N�Z�X�����̌�n��



// �t�@�C���A�N�Z�X�֐�
extern	DWORD_PTR	ReadOnlyFileAccessOpen( const wchar_t *Path, int UseCacheFlag, int BlockReadFlag, int UseASyncReadFlag  ) ;
extern	int			ReadOnlyFileAccessClose( DWORD_PTR Handle ) ;
extern	LONGLONG	ReadOnlyFileAccessTell( DWORD_PTR Handle ) ;
extern	int			ReadOnlyFileAccessSeek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType ) ;
extern	size_t		ReadOnlyFileAccessRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle ) ;
extern	int			ReadOnlyFileAccessEof( DWORD_PTR Handle ) ;
extern	int			ReadOnlyFileAccessIdleCheck( DWORD_PTR Handle ) ;
extern	int			ReadOnlyFileAccessChDir( const wchar_t *Path ) ;
extern	int			ReadOnlyFileAccessGetDir( wchar_t *Buffer ) ;
extern	DWORD_PTR	ReadOnlyFileAccessFindFirst( const wchar_t *FilePath, FILEINFOW *Buffer ) ;				// �߂�l: -1=�G���[  -1�ȊO=FindHandle
extern	int			ReadOnlyFileAccessFindNext( DWORD_PTR FindHandle, FILEINFOW *Buffer ) ;					// �߂�l: -1=�G���[  0=����
extern	int			ReadOnlyFileAccessFindClose( DWORD_PTR FindHandle ) ;									// �߂�l: -1=�G���[  0=����

// �X�g���[���f�[�^�A�N�Z�X�֐�
extern	DWORD_PTR	StreamOpen(  const wchar_t *Path, int UseCacheFlag, int BlockFlag, int UseASyncReadFlag ) ;
extern	DWORD_PTR	StreamOpenT( const TCHAR   *Path, int UseCacheFlag, int BlockFlag, int UseASyncReadFlag ) ;
extern	int			StreamClose( DWORD_PTR Handle ) ;
extern	LONGLONG	StreamTell( DWORD_PTR Handle ) ;
extern	int			StreamSeek( DWORD_PTR Handle, LONGLONG SeekPoint, int SeekType ) ;
extern	size_t		StreamRead( void *Buffer, size_t BlockSize, size_t DataNum, DWORD_PTR Handle ) ;
extern	int			StreamEof( DWORD_PTR Handle ) ;
extern	int			StreamIdleCheck( DWORD_PTR Handle ) ;
extern	int			StreamChDir(  const wchar_t *Path ) ;
extern	int			StreamChDirT( const TCHAR   *Path ) ;
extern	int			StreamGetDir(  wchar_t *Buffer ) ;
extern	int			StreamGetDirT( TCHAR   *Buffer ) ;
extern	DWORD_PTR	StreamFindFirst(  const wchar_t *FilePath, FILEINFOW *Buffer ) ;		// �߂�l: -1=�G���[  -1�ȊO=FindHandle
extern	DWORD_PTR	StreamFindFirstT( const TCHAR   *FilePath, FILEINFO  *Buffer ) ;		// �߂�l: -1=�G���[  -1�ȊO=FindHandle
extern	int			StreamFindNext(  DWORD_PTR FindHandle, FILEINFOW *Buffer ) ;			// �߂�l: -1=�G���[  0=����
extern	int			StreamFindNextT( DWORD_PTR FindHandle, FILEINFO  *Buffer ) ;			// �߂�l: -1=�G���[  0=����
extern	int			StreamFindClose( DWORD_PTR FindHandle ) ;								// �߂�l: -1=�G���[  0=����
extern	const STREAMDATASHREDTYPE2W *StreamGetStruct( void ) ;

// �t�@�C������ wchar_t �ł� TCHAR �ł̃f�[�^�ϊ��֐�
extern	int			ConvFileIntoToFileInfoW( FILEINFO  *Src, FILEINFOW *Dest ) ;
extern	int			ConvFileIntoWToFileInfo( FILEINFOW *Src, FILEINFO  *Dest ) ;

// �X�g���[���f�[�^�A�N�Z�X�p�֐��\���̊֌W
extern	STREAMDATASHRED *GetFileStreamDataShredStruct( void ) ;								// �X�g���[���f�[�^�ǂ݂��ݐ���p�֐��|�C���^�\���̂̃t�@�C���p�\���̂𓾂�
extern	STREAMDATASHRED *GetMemStreamDataShredStruct( void ) ;								// �X�g���[���f�[�^�ǂ݂��ݐ���p�֐��|�C���^�\���̂���ؗp�\���̂𓾂�




// �t�@�C���n���h���֐�
extern	int			FileRead_open_UseGParam( const wchar_t *FilePath, int ASync, int ASyncLoadFlag = FALSE ) ;				// �t�@�C�����J��
extern	int			FileRead_seek_UseGParam( int FileHandle, LONGLONG Offset, int Origin, int ASyncLoadFlag = FALSE ) ;		// �t�@�C���|�C���^�̈ʒu��ύX����
extern	int			FileRead_read_UseGParam( int FileHandle, void *Buffer, int ReadSize, int ASyncLoadFlag = FALSE ) ;		// �t�@�C������f�[�^��ǂݍ���
extern	int			FileRead_fullyLoad_UseGParam( const wchar_t *FilePath, int ASyncLoadFlag = FALSE ) ;					// �w��̃t�@�C���̓��e��S�ă������ɓǂݍ��݁A���̏��̃A�N�Z�X�ɕK�v�ȃn���h����Ԃ�( �߂�l  -1:�G���[  -1�ȊO:�n���h�� )�A�g���I�������n���h���� FileRead_fullyLoad_delete �ō폜����K�v������܂�
extern	int			FileRead_scanf_base(        int       FileHandle, const void  *Format,        va_list Param ) ;	// �t�@�C�����珑�������ꂽ�f�[�^��ǂݏo��
extern	int			FileRead_scanf_baseCHAR(    DWORD_PTR FileHandle, const char  *Format,        va_list Param ) ;	// �t�@�C�����珑�������ꂽ�f�[�^��ǂݏo��
extern	int			FileRead_scanf_baseUTF16LE( DWORD_PTR FileHandle, const WORD  *FormatUTF16LE, va_list Param ) ;	// �t�@�C�����珑�������ꂽ�f�[�^��ǂݏo��



// �������ɒu���ꂽ�f�[�^���t�@�C���̃f�[�^�ɗႦ�Ă������߂̊֐�
extern	void*		MemStreamOpen( const void *DataBuffer, unsigned int DataSize ) ;
extern	int			MemStreamClose( void *StreamDataPoint ) ;



// �t�@�C���p�X�֌W

// �t���p�X�ł͂Ȃ��p�X��������t���p�X�ɕϊ�����
// ( CurrentDir �̓t���p�X�ł���K�v������(����Ɂw\�x�������Ă������Ă��ǂ�) )
// ( CurrentDir �� NULL �̏ꍇ�͌��݂̃J�����g�f�B���N�g�����g�p���� )
//extern int			ConvertFullPath_( const char *Src, char *Dest, const char *CurrentDir = NULL ) ; 
extern int			ConvertFullPathW_( const wchar_t *Src, wchar_t *Dest, const wchar_t *CurrentDir = NULL ) ; 
extern int			ConvertFullPathT_( const TCHAR *Src, TCHAR *Dest, const TCHAR *CurrentDir = NULL ) ; 

// �w��̃t�@�C���p�X���w��̃t�H���_�p�X���瑊�΃A�N�Z�X���邽�߂̑��΃p�X���쐬����
// ( FilePath �� StartFolderPath ���t���p�X�ł͂Ȃ������ꍇ�͊֐����Ńt���p�X������܂� )
// StartFolderPath �̖��[�� / or \ �������Ă���肠��܂���
//extern int			CreateRelativePath_( const char *FilePath, const char *StartFolderPath, char *Dest ) ;
extern int			CreateRelativePathW_( const wchar_t *FilePath, const wchar_t *StartFolderPath, wchar_t *Dest ) ;
extern int			CreateRelativePathT_( const TCHAR   *FilePath, const TCHAR   *StartFolderPath, TCHAR   *Dest ) ;

// ����̃p�X�����񂩂�F�X�ȏ����擾����
// ( CurrentDir �̓t���p�X�ł���K�v������(����Ɂw\�x�������Ă������Ă��ǂ�) )
// ( CurrentDir �� 0 �̏ꍇ�͎��ۂ̃J�����g�f�B���N�g�����g�p���� )
extern int		AnalyseFilePathW_( const wchar_t *Src, // DirPath �̏I�[�ɂ� \ �͕t���Ȃ�
						wchar_t *FullPath, wchar_t *DirPath, wchar_t *FileName, wchar_t *Name, wchar_t *ExeName, const wchar_t *CurrentDir = 0 );
extern int		AnalyseFilePathT_( const TCHAR   *Src, // DirPath �̏I�[�ɂ� \ �͕t���Ȃ�
						TCHAR   *FullPath, TCHAR   *DirPath, TCHAR   *FileName, TCHAR   *Name, TCHAR   *ExeName, const TCHAR   *CurrentDir = 0 );

// �t�@�C�������ꏏ�ɂȂ��Ă���ƕ������Ă���p�X������t�@�C�����ƃf�B���N�g���p�X�𕪊�����
// �t���p�X�ł���K�v�͖����A�t�@�C���������ł��ǂ�
// DirPath �̏I�[�� �� �}�[�N�͕t���Ȃ�
//extern int		AnalysisFileNameAndDirPath_( const char *Src, char *FileName = 0, char *DirPath = 0 ) ;
extern int			AnalysisFileNameAndDirPathW_( const wchar_t *Src, wchar_t *FileName = 0, wchar_t *DirPath = 0 ) ;
extern int			AnalysisFileNameAndDirPathT_( const TCHAR   *Src, TCHAR   *FileName = 0, TCHAR   *DirPath = 0 ) ;

// �t�@�C���p�X����t�@�C�����Ɗg���q���擾����
//extern int		AnalysisFileNameAndExeName_( const char *Src, char *Name = 0, char *ExeName = 0 ) ;
extern int			AnalysisFileNameAndExeNameW_( const wchar_t *Src, wchar_t *Name = 0, wchar_t *ExeName = 0 ) ;
extern int			AnalysisFileNameAndExeNameT_( const TCHAR   *Src, TCHAR   *Name = 0, TCHAR   *ExeName = 0 ) ;

// �t�@�C���p�X�̊g���q��ς���������𓾂�
//extern int		GetChangeExeNamePath_( const char *Src, char *Dest, const char *ExeName ) ;

extern void			SetEnMarkT_( TCHAR   *PathBuf ) ;			// ����Ɂw\�x�����Ă��Ȃ��ꍇ�͕t����
extern void			SetEnMarkW_( wchar_t *PathBuf ) ;			// ����Ɂw\�x�����Ă��Ȃ��ꍇ�͕t����

// �n���ꂽ��������t���p�X������Ƃ��Ĉ����A�h���C�u��( :\ or :/ �̑O�܂� )
// ���̓l�b�g���[�N�t�H���_��( \ or / �̑O�܂� )���擾����
// �l�b�g���[�N�t�H���_�������ꍇ�͍ŏ��� \\ ���܂߂�
// �߂�l�͎擾����������̒���( �l�b�g���[�N�t�H���_�̏ꍇ�� \\ ��������ʂɊ܂܂�܂� )
// Src �̓t���p�X�ł���K�v������܂��A���΃p�X�ł͐���ɓ��삵�܂���
//extern int		AnalysisDriveName_( const char *Src, char *Dest ) ;
extern int			AnalysisDriveNameW_( const wchar_t *Src, wchar_t *Dest ) ;

// �n���ꂽ��������t�H���_�p�X������Ƃ��Ĉ����A�t�H���_��( \ or / �̑O�܂� )���擾���܂�
// �n�������񂪃t���p�X�ŁA�ŏ��Ƀh���C�u����������Ă����琳��Ȍ��ʂ������܂���
// ../ ���̉��ʃt�H���_�ɍ~��镶���񂪂������ꍇ�� .. �����o�͂���܂�
// �߂�l�͎擾����������̒����ł�
//extern int		AnalysisDirectoryName_( const char *Src, char *Dest ) ;
extern int			AnalysisDirectoryNameW_( const wchar_t *Src, wchar_t *Dest ) ;


#ifdef DX_USE_NAMESPACE

}

#endif // DX_USE_NAMESPACE

#endif // __DXFILE_H__

