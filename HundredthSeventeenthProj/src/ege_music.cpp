/*
filename  music.cpp

MUSIC类的定义
*/

#include "ege_head.h"
#include "ege_common.h"

#include "ege_music.h"

#include <mmsystem.h>
#include <digitalv.h>

#ifndef MUSIC_ASSERT_TRUE
#   ifdef _DEBUG
#       include <cassert>
#       define MUSIC_ASSERT_TRUE(e) assert((e) != MUSIC_ERROR)
#   else
#       define MUSIC_ASSERT_TRUE(e) (void(0))
#   endif
#endif

namespace ege
{

// Class MUSIC Construction
MUSIC::MUSIC()
{
    m_DID        = MUSIC_ERROR;
    m_dwCallBack = 0;
}

// Class MUSIC Destruction
MUSIC::~MUSIC()
{
    if (m_DID != MUSIC_ERROR) {
        Close();
    }
}

// open a music file. szStr: Path of the file
DWORD MUSIC::OpenFile(const char* _szStr)
{
    const std::wstring& wszStr = mb2w(_szStr);
    return OpenFile(wszStr.c_str());
}

// open a music file. szStr: Path of the file
DWORD MUSIC::OpenFile(const wchar_t* _szStr)
{
    MCIERROR        mciERR = ERROR_SUCCESS;
    MCI_OPEN_PARMSW mci_p  = {0};

    mci_p.lpstrElementName = _szStr;
    mci_p.lpstrDeviceType  = NULL;
    mci_p.dwCallback       = (DWORD_PTR)m_dwCallBack;

    if (m_DID != MUSIC_ERROR) {
        Close();
    }

    mciERR = dll::mciSendCommandW(0, MCI_OPEN, MCI_OPEN_SHAREABLE | MCI_NOTIFY | MCI_OPEN_ELEMENT, (DWORD_PTR)&mci_p);

    if (mciERR != ERROR_SUCCESS) {
        mciERR = dll::mciSendCommandW(0, MCI_OPEN, MCI_NOTIFY | MCI_OPEN_ELEMENT, (DWORD_PTR)&mci_p);
    }

    if (mciERR == ERROR_SUCCESS) {
        m_DID = mci_p.wDeviceID;

        // Set time format with milliseconds
        {
            MCI_SET_PARMS mci_p = {0};
            mci_p.dwTimeFormat  = MCI_FORMAT_MILLISECONDS;
            // DWORD dw =
            dll::mciSendCommandW(m_DID, MCI_SET, MCI_NOTIFY | MCI_SET_TIME_FORMAT, (DWORD_PTR)&mci_p);
        }
    }

    return mciERR;
}

// mciPlay(DWORD dwFrom, DWORD dwTo, DWORD dwCallBack)
// play the music stream.
DWORD MUSIC::Play(DWORD dwFrom, DWORD dwTo)
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCIERROR       mciERR = ERROR_SUCCESS;
    MCI_PLAY_PARMS mci_p  = {0};
    DWORD          dwFlag = MCI_NOTIFY;

    mci_p.dwFrom     = dwFrom;
    mci_p.dwTo       = dwTo;
    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;

    if (dwFrom != MUSIC_ERROR) {
        dwFlag |= MCI_FROM;
    }

    if (dwTo != MUSIC_ERROR) {
        dwFlag |= MCI_TO;
    }

    mciERR = dll::mciSendCommandW(m_DID, MCI_PLAY, dwFlag, (DWORD_PTR)&mci_p);

    return mciERR;
}

DWORD MUSIC::RepeatPlay(DWORD dwFrom, DWORD dwTo)
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCIERROR       mciERR = ERROR_SUCCESS;
    MCI_PLAY_PARMS mci_p  = {0};
    DWORD          dwFlag = MCI_NOTIFY | MCI_DGV_PLAY_REPEAT;

    mci_p.dwFrom     = dwFrom;
    mci_p.dwTo       = dwTo;
    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;

    if (dwFrom != MUSIC_ERROR) {
        dwFlag |= MCI_FROM;
    }

    if (dwTo != MUSIC_ERROR) {
        dwFlag |= MCI_TO;
    }

    mciERR = dll::mciSendCommandW(m_DID, MCI_PLAY, dwFlag, (DWORD_PTR)&mci_p);

    return mciERR;
}

// pause the music stream.
DWORD MUSIC::Pause()
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCIERROR          mciERR = ERROR_SUCCESS;
    MCI_GENERIC_PARMS mci_p  = {0};

    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;

    mciERR = dll::mciSendCommandW(m_DID, MCI_PAUSE, MCI_NOTIFY, (DWORD_PTR)&mci_p);

    return mciERR;
}

// stop the music stream.
DWORD MUSIC::Stop()
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCIERROR          mciERR = ERROR_SUCCESS;
    MCI_GENERIC_PARMS mci_p  = {0};

    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;

    mciERR = dll::mciSendCommandW(m_DID, MCI_STOP, MCI_NOTIFY, (DWORD_PTR)&mci_p);

    return mciERR;
}

DWORD MUSIC::SetVolume(float value)
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCIERROR                mciERR = ERROR_SUCCESS;
    MCI_DGV_SETAUDIO_PARMSW mci_p  = {0};
    mci_p.dwItem                   = MCI_DGV_SETAUDIO_VOLUME;
    mci_p.dwValue                  = (DWORD)(value * 1000); // 此处就是音量大小 (0--1000)

    mciERR = dll::mciSendCommandW(m_DID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD_PTR)&mci_p);

    return mciERR;
}

// seek the music stream playposition to `dwTo`
DWORD MUSIC::Seek(DWORD dwTo)
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCIERROR       mciERR = ERROR_SUCCESS;
    MCI_SEEK_PARMS mci_p  = {0};

    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;
    mci_p.dwTo       = dwTo;

    mciERR = dll::mciSendCommandW(m_DID, MCI_SEEK, MCI_NOTIFY, (DWORD_PTR)&mci_p);

    return mciERR;
}

// close the music stream.
DWORD MUSIC::Close()
{
    if (m_DID != MUSIC_ERROR) {
        MCIERROR          mciERR = ERROR_SUCCESS;
        MCI_GENERIC_PARMS mci_p  = {0};

        mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;

        mciERR = dll::mciSendCommandW(m_DID, MCI_CLOSE, MCI_NOTIFY, (DWORD_PTR)&mci_p);

        m_DID = MUSIC_ERROR;
        return mciERR;
    } else {
        return ERROR_SUCCESS;
    }
}

// get the playing position. return by milliseconds
DWORD MUSIC::GetPosition()
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCI_STATUS_PARMS mci_p = {0};

    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;
    mci_p.dwItem     = MCI_STATUS_POSITION;

    dll::mciSendCommandW(m_DID, MCI_STATUS, MCI_NOTIFY | MCI_STATUS_ITEM, (DWORD_PTR)&mci_p);

    return (DWORD)mci_p.dwReturn;
}

// get the length of the music stream. return by milliseconds
DWORD MUSIC::GetLength()
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCI_STATUS_PARMS mci_p = {0};

    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;
    mci_p.dwItem     = MCI_STATUS_LENGTH;

    dll::mciSendCommandW(m_DID, MCI_STATUS, MCI_NOTIFY | MCI_STATUS_ITEM, (DWORD_PTR)&mci_p);

    return (DWORD)mci_p.dwReturn;
}

DWORD MUSIC::GetPlayStatus()
{
    MUSIC_ASSERT_TRUE(m_DID);
    MCI_STATUS_PARMS mci_p = {0};

    mci_p.dwCallback = (DWORD_PTR)m_dwCallBack;
    mci_p.dwItem     = MCI_STATUS_MODE;

    dll::mciSendCommandW(m_DID, MCI_STATUS, MCI_NOTIFY | MCI_STATUS_ITEM, (DWORD_PTR)&mci_p);

    return (DWORD)mci_p.dwReturn;
}

} // namespace ege
