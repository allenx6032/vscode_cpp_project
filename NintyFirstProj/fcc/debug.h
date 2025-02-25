#include "std.h"

#include "stdio.h"
#include "stdarg.h"

typedef struct type type;
typedef struct ast ast;
typedef struct sym sym;
typedef struct operand operand;
typedef struct architecture architecture;

typedef enum debugMode {
    debugFull,
    debugCompressed,
    debugMinimal,
    debugSilent
} debugMode;

void debugInit (FILE* log);

debugMode debugSetMode (debugMode mode);

void debugWait (void);

void debugEnter (const char* str);
void debugLeave (void);
void debugMsg (const char* format, ...);
void debugVarMsg (const char* format, va_list args);
void debugOut (const char* format, ...);
void debugVarOut (const char* format, va_list args);

void debugError (const char* functionName,
                 const char* format, ...);

bool debugAssert (const char* functionName,
                  const char* testName,
                  bool result);

void debugErrorUnhandled (const char* functionName,
                          const char* tagName,
                          const char* tagStr);

void debugErrorUnhandledInt (const char* functionName,
                             const char* className,
                             int classInt);

void debugErrorUnhandledChar (const char* functionName,
                              const char* className,
                              char classChar);

void report (const char* str);
void reportType (const type* DT);
void reportSymbol (const sym* Symbol);
void reportSymbolTree (const sym* Symbol, int level);
void reportNode (const ast* Node);
void reportRegs (void);
void reportOperand (const architecture* arch, const operand* R);

extern int internalErrors;
