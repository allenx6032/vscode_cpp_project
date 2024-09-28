///////////////////////////////////////////////////////////////////////////////
//  Common classes for cross-platform C++ application development.
//
//  MIT License
//  Copyright © 2016-2019 Jingwood, unvell.com, all rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef jsonlexer_h
#define jsonlexer_h

#include <stdio.h>
#include <vector>

#include "ugm_lexer.h"
#include "ugm_string.h"
#include "ugm_jstypes.h"

namespace ucm {

class JSONReader
{
private:
	Lexer lexer;
  
public:
	JSONReader() { }
	JSONReader(const string& str);

  void init(const string& str);

  JSObject* readObject();

  const bool readKey(string* key);
  bool readValue(JSValue& value);
  bool readArray(std::vector<JSValue>** list);
};

}

#endif /* jsonlexer_h */
