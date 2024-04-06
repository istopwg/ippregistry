//
// Common IPP registry definitions.
//
// Copyright © 2018-2024 by The IEEE-ISTO Printer Working Group.
// Copyright © 2008-2018 by Michael R Sweet
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//

#ifndef IPP_REGISTRY_H
#  define IPP_REGISTRY_H
#  include <mxml.h>
#  include <ctype.h>
#  include <limits.h>
#  include <string.h>
#  include <time.h>


// <registry id="foo"> values
#  define IPP_REGISTRY_OBJECTS		"ipp-registrations-1"
#  define IPP_REGISTRY_ATTRIBUTES	"ipp-registrations-2"
#  define IPP_REGISTRY_KEYWORDS		"ipp-registrations-4"
#  define IPP_REGISTRY_ENUMS		"ipp-registrations-6"
#  define IPP_REGISTRY_ATTRIBUTE_GROUPS	"ipp-registrations-7"
#  define IPP_REGISTRY_OUT_OF_BANDS	"ipp-registrations-8"
#  define IPP_REGISTRY_ATTRIBUTE_SYNTAXES "ipp-registrations-9"
#  define IPP_REGISTRY_OPERATIONS	"ipp-registrations-10"
#  define IPP_REGISTRY_STATUS_CODES	"ipp-registrations-11"


//
// 'ipp_copy_string()' - Copy a string safely.
//

static void
ipp_copy_string(char       *dst,	// I - Destination buffer
                const char *src,	// I - Source string
                size_t     dstsize)	// I - Size of destination buffer
{
  size_t	srclen = strlen(src);	// Length of source string


  if (srclen >= dstsize)
    srclen = dstsize - 1;

  memcpy(dst, src, srclen);
  dst[srclen] = '\0';
}


//
// 'ipp_error_cb()' - Mini-XML error callback
//

static void
ipp_error_cb(const char *command,	// I - Command name
             const char *message)	// I - Error message
{
  fprintf(stderr, "%s: %s\n", command, message);
}


//
// 'ipp_type_cb()' - Mini-XML load type callback.
//

static mxml_type_t			// O - Type of child node
ipp_type_cb(void        *data,		// I - Callback data (unused)
            mxml_node_t *node)		// I - Parent node
{
  const char *name = mxmlGetElement(node);
					// Name of parent element


  (void)data;

  if (name && (!strcmp(name, "attribute") || !strcmp(name, "collection") || !strcmp(name, "created") || !strcmp(name, "expert") || !strcmp(name, "member_attribute") || !strcmp(name, "name") || !strcmp(name, "note") || !strcmp(name, "registration_rule") || !strcmp(name, "section") || !strcmp(name, "sub-member_attribute") || !strcmp(name, "syntax") || !strcmp(name, "title") || !strcmp(name, "type") || !strcmp(name, "updated") || !strcmp(name, "value") || !strcmp(name, "xref")))
    return (MXML_TYPE_OPAQUE);
  else
    return (MXML_TYPE_IGNORE);
}
#endif // !IPP_REGISTRY_H
