# PWG Semantic Model Print3D Service Schema v1.0

This directory contains the complete PWG Semantic Model schema for the Print3D
service.  It is derived from the attributes and values in the IANA IPP Registry
and generated using the "regtosm" tool available at:

    https://istopwg.github.io/ippregistry


## Schema File Organization

The schema is organized in four files:

- "Print3DService.xsd": The main schema file with definitions for the service
  itself (capabilities, state, defaults, and jobs) along with the job ticket,
  job receipt, document ticket, and document receipt.

- "PwgCommon.xsd": A schema file that defines all of the semantic elements that
  correspond directly to IPP attributes.

- "PwgTypes.xsd": A schema file that defines all of the base semantic types for
  each IPP attribute syntax.

- "PwgWellKnownValues.xsd": A schema file that defines enumerated value types
  that correspond directly to registered IPP keyword and enum values.

## Print3DService.xsd Schema Overview

The Print3DService schema defines several top-level types including:

- "Print3DServiceType": A complex type containing elements for the service
  capabilities ("Print3DServiceCapabilities"), default job and document tickets
  ("Print3DServiceDefaults"), status ("Print3DServiceStatus"), and jobs
  associated with the service ("Print3DServiceJobs").

- "Print3DJobTicketType": A complex type containing description elements
  ("Print3DJobDescription") and processing elements ("Print3DJobProcessing")
  that are supplied when creating a job.  The associated element
  ("Print3DJobTicket") can be embedded in a 3D document file.

- "Print3DJobReceiptType": A complex type containing accounting elements that
  record the "Print3DJobProcessing" element values that were used when
  processing the job.  The associated element ("Print3DJobReceipt") is typically
  embedded in data exchange records for 3D printing workflows.
