
Controlling software for the Gamble DCX console, see https://www.gambleboards.com/

![image](https://github.com/user-attachments/assets/4ded374f-9d01-4de6-a0a8-e62548c3efd2)

Since no hardware is available WORK OFFLINE


![image](https://github.com/user-attachments/assets/e04dc41b-6674-419d-86cf-958b7bd8a4b8)


To build project need to checkout GServerLite and Vacs-Client to the same level directory as they have dependencies on each other at the moment. 

To build project with Visual Studio 2022 load the solution build all.

To build the project using CMake on Windows (assuming MSVC and CMake 3.28+ installed):

1. Create or navigate to a build directory, e.g., `mkdir build-cmake && cd build-cmake` (use build-cmake/ to separate from any existing VS build artifacts).

2. Configure the build: `cmake ..`

   - This generates Visual Studio project files (default generator on Windows).
   - If you need a specific VS version, add `-G "Visual Studio 17 2022"` (adjust for your VS install, e.g., 16 2019).
   - For Debug build, add `-DCMAKE_BUILD_TYPE=Debug`; for Release, `-DCMAKE_BUILD_TYPE=Release`. Or specify later during build.

3. Build the project: `cmake --build . --config Release` (or `--config Debug`).

   - This compiles the executable (Vacs.exe in bin/) and libraries (DLLs in bin/, static libs in lib/).
   - The post-build step automatically copies required DLLs (commport.dll, ConsoleDefinition.dll, etc.) to the bin/ directory alongside the executable.

4. Run the executable: From the build-cmake/bin/ directory, execute `Vacs.exe` (or the full path).

If you encounter issues (e.g., missing dependencies like ODBC), ensure Windows SDK and VS Build Tools are installed. The project links to system libs (winmm, comctl32, etc.), so no external installs needed. Test in a clean build dir to avoid cache issues.


