# Pending Registrations

This directory contains pending registrations, if any.

Filenames must be of the form:

    pwg5100.*.txt
    vendor-*.txt

The first line provides a durable URL to the registration information.  The
remaining lines contain the plain text registration template.

## Attribute Template

    Document Description attributes:	Reference
    --------------------------	---------
    name (type)	[REFERENCE]

    Document Status attributes:	Reference
    --------------------------	---------
    name (type)	[REFERENCE]

    Document Template attributes:	Reference
    --------------------------	---------
    name (type)	[REFERENCE]

    Job Description attributes:	Reference
    --------------------------	---------
    name (type)	[REFERENCE]

    Job Status attributes:	Reference
    --------------------------	---------
    name (type)	[REFERENCE]

    Job Template attributes:	Reference
    --------------------------	---------
    name (type)	[REFERENCE]

    Operation attributes:	Reference
    --------------------	---------
    name (type)	[REFERENCE]

    Printer Description attributes:	Reference
    ------------------------------	---------
    name (type)	[REFERENCE]

    Printer Status attributes:	Reference
    ------------------------------	---------
    name (type)	[REFERENCE]

    Subscription Description attributes:	Reference
    ------------------------------	---------
    name (type)	[REFERENCE]

    Subscription Status attributes:	Reference
    ------------------------------	---------
    name (type)	[REFERENCE]

    Subscription Template attributes:	Reference
    ------------------------------	---------
    name (type)	[REFERENCE]


## Attribute Keyword Value Template

    Attributes (attribute syntax)
      Keyword Attribute Value	Reference
      -----------------------	---------
    name (type2 keyword)	[REFERENCE]
      value-1	[REFERENCE]
      value-2	[REFERENCE]
    name-supported (1setOf type2 keyword)	[REFERENCE]
      < all name values >	[REFERENCE]


## Attribute Enum Value Template

    Attributes (attribute syntax)
      Enum Value	Enum Symbolic Name	Reference
      ----------	------------------	---------
    name (type2 enum)		[REFERENCE]
      3	value-3	[REFERENCE]
      4	value-4	[REFERENCE]

    operations-supported (1setOf type2 enum)	[RFC2911]
      0xXXXX	Operation-Name	[REFERENCE]


## Operation Template

    Operation Name	Reference
    --------------	---------
    Operation-Name	[REFERENCE]
    Existing-Operation-Name (Extension)	[REFERENCE]


## Status Code Template

    Value    Status Code Name	Reference
    ------   ---------------------------------------------	---------
    0x0400:0x04FF - Client Error:
      0x04XX	client-error-name	[REFERENCE]
    0x0500:0x05FF - Server Error:
      0x05XX	server-error-name	[REFERENCE]
