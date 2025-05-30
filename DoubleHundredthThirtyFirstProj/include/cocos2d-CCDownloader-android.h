/****************************************************************************
 Copyright (c) 2015 Chukong Technologies Inc.

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

#pragma once

#include "cocos2d-CCIDownloaderImpl.h"

class _jobject;

namespace cocos2d { namespace network
    {
        class DownloadTaskAndroid;
        class DownloaderHints;

        class DownloaderAndroid : public IDownloaderImpl
        {
        public:
            DownloaderAndroid(const DownloaderHints& hints);
            virtual ~DownloaderAndroid();

            virtual IDownloadTask *createCoTask(std::shared_ptr<const DownloadTask>& task) override;

            // designed called by internal
            void _onProcess(int taskId, int64_t dl, int64_t dlNow, int64_t dlTotal);
            void _onFinish(int taskId, int errCode, const char *errStr, std::vector<unsigned char>& data);
        protected:
            int _id;
            _jobject* _impl;
            std::unordered_map<int, DownloadTaskAndroid*> _taskMap;
        };

        // Load java downloader class from main thread, to void addImageAsync function
        //  perform in sub-thread cause java downloader unusable
        void _preloadJavaDownloaderClass();
    }}  // namespace cocos2d::network

