# Algorithm specifications

These documents describe OpenCiv2's algorithms in enough detail for a cleanroom
implementation. Each specification defines the inputs, outputs, ordered steps,
constants, and compatibility quirks needed to reproduce the existing behavior
without consulting its source code.

## Specifications

| Algorithm | Coverage |
| --- | --- |
| [Map generation](map-generation.md) | Land creation, terrain, rainfall, age operations, rivers, polar edits, region labeling, coarse connectivity, fertility, resources, and huts, including exact random-call ordering and conformance checks. |

## Adding a specification

Place each algorithm in its own descriptively named Markdown file and add a
summary to the table above. State the compatibility target and scope, define
arithmetic and data conventions, and include pseudocode, edge cases, and
conformance examples. Keep source references in a traceability section so the
algorithm description is usable independently of the implementation.
