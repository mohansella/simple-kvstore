# simple-kvstore

A file-based key-value data store written in C++

Note: This is a generic CMake project which should work on Windows, Linux or Mac. But it is recommended to use the below environment.

#### Recomended Environment
   - Windows OS
   - MSVC (Visual studio 2019)
   - CMake installed in system path
   - Conan Package manager (from python pip)
   - Ninja-Build in system path

#### Recomended Build Step
   - Open this repository in VS Code after installing the addons - "C/C++" and "CMake Tools".
   - In the VS Code actions list, find and hit "CMake: Build" which should automatically download boost dependency and starts compiling.
   - If build successful, both library file and a commandline tool named SimpleKVStoreApp.exe can be found under build directory.
