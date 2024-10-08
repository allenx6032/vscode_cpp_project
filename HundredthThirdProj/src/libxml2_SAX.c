/*
 * SAX.c : Old SAX v1 handlers to build a tree.
 *         Deprecated except for compatibility
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */


#define IN_LIBXML
#include "libxml2.h"
#include <stdlib.h>
#include <string.h>
#include <libxml2_xmlmemory.h>
#include <libxml2_tree.h>
#include <libxml2_parser.h>
#include <libxml2_parserInternals.h>
#include <libxml2_valid.h>
#include <libxml2_entities.h>
#include <libxml2_xmlerror.h>
#include <libxml2_debugXML.h>
#include <libxml2_xmlIO.h>
#include <libxml2_SAX.h>
#include <libxml2_uri.h>
#include <libxml2_valid.h>
#include <libxml2_HTMLtree.h>
#include <libxml2_SAX2.h>

#ifdef LIBXML_LEGACY_ENABLED
#ifdef LIBXML_SAX1_ENABLED
/**
 * initxmlDefaultSAXHandler:
 * @hdlr:  the SAX handler
 * @warning:  flag if non-zero sets the handler warning procedure
 *
 * Initialize the default XML SAX version 1 handler
 * DEPRECATED: use xmlSAX2InitDefaultSAXHandler() for the new SAX2 blocks
 */
void
initxmlDefaultSAXHandler(xmlSAXHandlerV1 *hdlr, int warning)
{

    if(hdlr->initialized == 1)
	return;

    hdlr->internalSubset = xmlSAX2InternalSubset;
    hdlr->externalSubset = xmlSAX2ExternalSubset;
    hdlr->isStandalone = xmlSAX2IsStandalone;
    hdlr->hasInternalSubset = xmlSAX2HasInternalSubset;
    hdlr->hasExternalSubset = xmlSAX2HasExternalSubset;
    hdlr->resolveEntity = xmlSAX2ResolveEntity;
    hdlr->getEntity = xmlSAX2GetEntity;
    hdlr->getParameterEntity = xmlSAX2GetParameterEntity;
    hdlr->entityDecl = xmlSAX2EntityDecl;
    hdlr->attributeDecl = xmlSAX2AttributeDecl;
    hdlr->elementDecl = xmlSAX2ElementDecl;
    hdlr->notationDecl = xmlSAX2NotationDecl;
    hdlr->unparsedEntityDecl = xmlSAX2UnparsedEntityDecl;
    hdlr->setDocumentLocator = xmlSAX2SetDocumentLocator;
    hdlr->startDocument = xmlSAX2StartDocument;
    hdlr->endDocument = xmlSAX2EndDocument;
    hdlr->startElement = xmlSAX2StartElement;
    hdlr->endElement = xmlSAX2EndElement;
    hdlr->reference = xmlSAX2Reference;
    hdlr->characters = xmlSAX2Characters;
    hdlr->cdataBlock = xmlSAX2CDataBlock;
    hdlr->ignorableWhitespace = xmlSAX2Characters;
    hdlr->processingInstruction = xmlSAX2ProcessingInstruction;
    if (warning == 0)
	hdlr->warning = NULL;
    else
	hdlr->warning = xmlParserWarning;
    hdlr->error = xmlParserError;
    hdlr->fatalError = xmlParserError;

    hdlr->initialized = 1;
}

#ifdef LIBXML_HTML_ENABLED

/**
 * inithtmlDefaultSAXHandler:
 * @hdlr:  the SAX handler
 *
 * Initialize the default HTML SAX version 1 handler
 * DEPRECATED: use xmlSAX2InitHtmlDefaultSAXHandler() for the new SAX2 blocks
 */
void
inithtmlDefaultSAXHandler(xmlSAXHandlerV1 *hdlr)
{
    if(hdlr->initialized == 1)
	return;

    hdlr->internalSubset = xmlSAX2InternalSubset;
    hdlr->externalSubset = NULL;
    hdlr->isStandalone = NULL;
    hdlr->hasInternalSubset = NULL;
    hdlr->hasExternalSubset = NULL;
    hdlr->resolveEntity = NULL;
    hdlr->getEntity = xmlSAX2GetEntity;
    hdlr->getParameterEntity = NULL;
    hdlr->entityDecl = NULL;
    hdlr->attributeDecl = NULL;
    hdlr->elementDecl = NULL;
    hdlr->notationDecl = NULL;
    hdlr->unparsedEntityDecl = NULL;
    hdlr->setDocumentLocator = xmlSAX2SetDocumentLocator;
    hdlr->startDocument = xmlSAX2StartDocument;
    hdlr->endDocument = xmlSAX2EndDocument;
    hdlr->startElement = xmlSAX2StartElement;
    hdlr->endElement = xmlSAX2EndElement;
    hdlr->reference = NULL;
    hdlr->characters = xmlSAX2Characters;
    hdlr->cdataBlock = xmlSAX2CDataBlock;
    hdlr->ignorableWhitespace = xmlSAX2IgnorableWhitespace;
    hdlr->processingInstruction = xmlSAX2ProcessingInstruction;
    hdlr->comment = xmlSAX2Comment;
    hdlr->warning = xmlParserWarning;
    hdlr->error = xmlParserError;
    hdlr->fatalError = xmlParserError;

    hdlr->initialized = 1;
}

#endif /* LIBXML_HTML_ENABLED */

#endif /* LIBXML_SAX1_ENABLED */

#endif /* LIBXML_LEGACY_ENABLED */
