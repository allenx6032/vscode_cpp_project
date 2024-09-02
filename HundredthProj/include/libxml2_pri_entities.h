#ifndef XML_ENTITIES_H_PRIVATE__
#define XML_ENTITIES_H_PRIVATE__

#include <libxml2_tree.h>
#include <libxml2_xmlstring.h>

/*
 * Entity flags
 *
 * XML_ENT_PARSED: The entity was parsed and `children` points to the
 * content.
 *
 * XML_ENT_CHECKED: The entity was checked for loops and amplification.
 * expandedSize was set.
 *
 * XML_ENT_VALIDATED: The entity contains a valid attribute value.
 * Only used when entities aren't substituted.
 */
#define XML_ENT_PARSED      (1u << 0)
#define XML_ENT_CHECKED     (1u << 1)
#define XML_ENT_VALIDATED   (1u << 2)
#define XML_ENT_EXPANDING   (1u << 3)

XML_HIDDEN xmlChar *
xmlEncodeAttributeEntities(xmlDocPtr doc, const xmlChar *input);

#endif /* XML_ENTITIES_H_PRIVATE__ */
