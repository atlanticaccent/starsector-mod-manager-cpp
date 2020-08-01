# starsector-mod-manager
A Mod Manager for Starsector mods, written in C++ and using wxWidgets.

# Current Features

- Mod installation (from an already unzipped folder)
- Mod uninstallation/deletion
- Mod activation/deactivation
- Mod meta data (author, version, description) display
- Basic Starsector version incompatibility checking
- Installation directory selection

# Planned Features

- Mod installation from zip/archive
- Version Checker support
- Upgrade or replace mods
- vmparams editing
- Mod profiles (potentially)

# Compiling

## Prerequisites

- Download, install and compile the wxWidgets library. The current VS .sln file is configured to look for the wxWidgets files under a directory described by the WXWIN environment variable.

- A copy of the header file for the JSON for Modern C++ library is included in this repository.

- Currently, this project only targets and is tested on Windows. Automated project configuration is provided by the included .sln file. Howver, it is likely that this project can be compiled for Linux/OSX with little to no issues - full cross platform support is planned for v0.3.0.

- A C++ compiler that supports C++17 onwards.

## Build

Load the folder or sln file into Visual Studio and run Build. No further steps should be required.

# Libraries Used

This project utilises the wxWidgets Cross-Platform GUI Library, licensed under the wxWindows Library License. Copyright © 2020 wxWidgets.

This project also uses the JSON for Modern C++ library by Niels Nohlmann, licensed under the MIT license. Copyright © 2013-2019 [Niels Nohlmann](http://nlohmann.me/).
