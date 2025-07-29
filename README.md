# WFR_Lib
WFR's reusable firmware component library (focused on ESP32 for now)

## Folder Structure
└── WFR_Lib/
    ├── CMakeLists.txt  <-- New Source files need to be registered here
    ├── include/
    │   └── WFR_Lib/    <-- this exists so that includes are denoted as "WFR_Lib/headerfile.hpp
    │       └── CAN.hpp
    │       └── ...
    └── src/            <-- source files go here
        └── CAN.cpp
        └── ...

## Installation

*this needs to be filled out*

If intelisense is giving you a hard time with red squiggles even if the project is building correctly add `${workspaceFolder}/WFR_Lib/include` to your include path 