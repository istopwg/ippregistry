/*
 * Common IPP registry definitions...
 *
 * Copyright © 2008-2018 by Michael R Sweet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
