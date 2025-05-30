/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "cocos2d-CCPlatformConfig.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#ifndef __AUDIO_PLAYER_H_
#define __AUDIO_PLAYER_H_

#include <string>
#include <condition_variable>
#include <thread>
#ifdef OPENAL_PLAIN_INCLUDES
#include <al.h>
#else
#include <AL_al.h>
#endif
#include "cocos2d-CCPlatformMacros.h"
#include "cocos2d-AudioCache.h"

NS_CC_BEGIN
namespace experimental{

class AudioCache;
class AudioEngineImpl;

class CC_DLL AudioPlayer
{
public:
    AudioPlayer();
    AudioPlayer(const AudioPlayer&);
    ~AudioPlayer();
    
    //queue buffer related stuff
    bool setTime(float time);
    float getTime() { return _currTime;}
    bool setLoop(bool loop);
    void notifyExitThread();

protected:
    void rotateBufferThread(int offsetFrame);
    bool play2d(AudioCache* cache);
    
    AudioCache* _audioCache;
    
    float _volume;
    bool _loop;
    std::function<void (int, const std::string &)> _finishCallbak;
    
    bool _ready;
    ALuint _alSource;
    
    //play by circular buffer
    float _currTime;
    bool _timeDirty;
    bool _streamingSource;
    ALuint _bufferIds[QUEUEBUFFER_NUM];
    std::thread _rotateBufferThread;
    std::mutex _sleepMutex;
    std::condition_variable _sleepCondition;
    bool _exitThread;
    bool _readForRemove;
    
    friend class AudioEngineImpl;
};

}
NS_CC_END
#endif // __AUDIO_PLAYER_H_
#endif //CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

