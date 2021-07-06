## Rationale

Each header file in `./inc/` corresponds to a folder in `./src/`, which contains its source files.

The `./src/` makefile (`src.mk`) will dig each folder and compile the `.cpp` files it finds to `.o` files.

These files are placed into the `./src/_obj/` folder, where they are referenced by the `./` makefile (`Makefile`).

*main.cpp is the main function app, it should be exposed*

----

## Editing Guide
**just some reminders when editing the source code**
- update your changes to routines and variables in the corresponding header file
- recompile changed object files before recompiling the main application
- if adding new libraries, make sure to update both `./lib/` and the corresponding makefile
- each source file should have its header file mapped as such; `#include "../../inc/<header>.h"`
- add each new file/header combo to the `./src/src.mk` (source makefile) queue
