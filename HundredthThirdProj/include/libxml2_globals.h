/*
 * Summary: interface for all global variables of the library
 * Description: Deprecated, don't use
 *
 * Copy: See Copyright for the status of this software.
 */

#ifndef __XML_GLOBALS_H
#define __XML_GLOBALS_H

#include <libxml2_xmlversion.h>

/*
 * This file was required to access global variables until version v2.12.0.
 *
 * These includes are for backward compatibility.
 */
#include <libxml2_HTMLparser.h>
#include <libxml2_parser.h>
#include <libxml2_xmlerror.h>
#include <libxml2_xmlIO.h>
#include <libxml2_xmlsave.h>
#include <libxml2_threads.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xmlGlobalState xmlGlobalState;
typedef xmlGlobalState *xmlGlobalStatePtr;

XML_DEPRECATED XMLPUBFUN void
xmlInitializeGlobalState(xmlGlobalStatePtr gs);
XML_DEPRECATED XMLPUBFUN
xmlGlobalStatePtr xmlGetGlobalState(void);

#ifdef __cplusplus
}
#endif

#endif /* __XML_GLOBALS_H */
