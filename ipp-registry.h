/*
 * Common IPP registry definitions.
 *
 * Copyright © 2018 by The IEEE-ISTO Printer Working Group.
 * Copyright © 2008-2018 by Michael R Sweet
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 */

#ifndef __IPP_REGISTRY_H__
#  define __IPP_REGISTRY_H__

/* <registry id="foo"> values */
#  define IPP_REGISTRY_OBJECTS		"ipp-registrations-1"
#  define IPP_REGISTRY_ATTRIBUTES	"ipp-registrations-2"
#  define IPP_REGISTRY_KEYWORDS		"ipp-registrations-4"
#  define IPP_REGISTRY_ENUMS		"ipp-registrations-6"
#  define IPP_REGISTRY_ATTRIBUTE_GROUPS	"ipp-registrations-7"
#  define IPP_REGISTRY_OUT_OF_BANDS	"ipp-registrations-8"
#  define IPP_REGISTRY_ATTRIBUTE_SYNTAXES "ipp-registrations-9"
#  define IPP_REGISTRY_OPERATIONS	"ipp-registrations-10"
#  define IPP_REGISTRY_STATUS_CODES	"ipp-registrations-11"


/*
 * 'ipp_load_cb()' - Mini-XML load callback.
 */

static int				/* O - Type of child node */
ipp_load_cb(mxml_node_t *node)		/* I - Parent node */
{
  const char *name = mxmlGetElement(node);
					/* Name of parent element */


  if (!strcmp(name, "attribute") ||
      !strcmp(name, "collection") ||
      !strcmp(name, "created") ||
      !strcmp(name, "expert") ||
      !strcmp(name, "member_attribute") ||
      !strcmp(name, "name") ||
      !strcmp(name, "note") ||
      !strcmp(name, "registration_rule") ||
      !strcmp(name, "section") ||
      !strcmp(name, "sub-member_attribute") ||
      !strcmp(name, "syntax") ||
      !strcmp(name, "title") ||
      !strcmp(name, "type") ||
      !strcmp(name, "updated") ||
      !strcmp(name, "value") ||
      !strcmp(name, "xref"))
    return (MXML_OPAQUE);
  else
    return (MXML_IGNORE);
}
#endif /* !__IPP_REGISTRY_H__ */
