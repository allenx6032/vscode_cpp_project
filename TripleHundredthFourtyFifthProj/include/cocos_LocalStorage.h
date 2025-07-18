/*

Copyright (c) 2012 - Zynga Inc.

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

*/

/*
 Local Storage support for the JS Bindings for iOS.
 Works on cocos2d-iphone and cocos2d-x.
 */

#ifndef __JSB_LOCALSTORAGE_H
#define __JSB_LOCALSTORAGE_H

#include <string>
#include "cocos_CCPlatformMacros.h"

/** Initializes the database. If path is null, it will create an in-memory DB */
void CC_DLL localStorageInit( const std::string& fullpath = "");

/** Frees the allocated resources */
void CC_DLL localStorageFree();

/** sets an item in the LS */
void CC_DLL localStorageSetItem( const std::string& key, const std::string& value);

/** gets an item from the LS */
std::string CC_DLL localStorageGetItem( const std::string& key );

/** removes an item from the LS */
void CC_DLL localStorageRemoveItem( const std::string& key );

#endif // __JSB_LOCALSTORAGE_H
