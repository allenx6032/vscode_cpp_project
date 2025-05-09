//-------------------------------------------------------------------------------------------------
// file:    Error.ixx
// author:  Andy Ellinger
// brief:   Handler for error messages
//
// Copyright � 2022 DigiPen, All rights reserved.
//-------------------------------------------------------------------------------------------------
#pragma once
// module;

#include <string_view>
#include <string>

// export module Errors;

namespace DGL
{

 using std::string_view;
 using std::string;

//------------------------------------------------------------------------------------ ErrorHandler

 class ErrorHandler
{
public:
    // Sets the global pointer
    ErrorHandler();

    // Returns the current error string
    const char* GetLastError() const;

    // Creates the error string including the error number
    void SetError(string_view text, long hr);

    // Sets the current error string
    void SetError(string_view text);

private:
    string mLastError;
};

// Global pointer for accessing the system
 extern ErrorHandler* gError;

} // namepspace DGL
