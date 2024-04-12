# NX Dxf file exporter

## Usage

There are 3 buttons on the HSS Toolbar to invoke the Dxf exporter

- Select Part(s): Launches a file selection dialog to choose a part or multiple parts to export.
- Work Part: Export only the current active (work) part
- All Open Parts: Export all parts that are currently open in NX

## Coercing the exporter's behaviour

There are some built in logic with naming conventions that will change what features will exported.

### Body exports

- If any body is named `SN_PART`, only that body will be exported
- If a body is named `SHIM`, that body will not be exported.


#### Process to rename a body
1) Right-click in the Part Navigator and unselect Timestamp Order
2) Right-click the body you wan't to rename and rename it via the Rename command

### Sketch inclusion

Only sketches in the following mapping will export

|Sketch name|Mapping|
|---|---|
|ZINC|Marking|
|DOR|No Cut|
|NOCUT, NO-CUT, NO_CUT| No Cut|

### File naming

- If the part file name ends in `_mfg` or `_copy`, these patterns are stripped from the exported file name
- If a body is named (not `SN_PART` or `SHIM`), it will export using the naming pattern `{filename}-{bodyname}`
- If there are multiple bodies in the part and they are not named, the parts will be exported using the naming pattern `{filename}_{id}` where `id` is an increasing numerical counter