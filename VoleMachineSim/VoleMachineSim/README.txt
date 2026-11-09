==============================================================================
  VOLE MACHINE SIMULATOR
  Console version + Qt graphical version (C++17)
==============================================================================

This package contains SOURCE CODE ONLY. It does not include any .exe files
or DLL/library files. You build the program yourself (steps below) and then
run the program you built.


------------------------------------------------------------------------------
1. WHAT IT DOES
------------------------------------------------------------------------------

Simulates the Vole machine: 16 registers (R0-RF) of 1 byte each, 256 bytes
of memory (addresses 00-FF) and a CPU that runs 2-byte instructions.

You load a program written as hex digits in a .txt file, then run it all at
once or one instruction at a time. Writing a value to memory address 00 shows
it on the screen.

Two versions are provided (both use the same simulator code):
  - Console version : text menu in the terminal.
  - Qt GUI version  : desktop window (dark console design).


------------------------------------------------------------------------------
2. WHAT YOU NEED
------------------------------------------------------------------------------

Operating system: Windows 10 or 11.

Console version:
  - A C++17 compiler with g++ (MinGW-w64). Either of these works:
      * MSYS2 (mingw64 environment), or
      * the MinGW compiler that comes with the Qt installer.
  - Any terminal: PowerShell, cmd, or the VS Code terminal.

Qt GUI version (in addition):
  - Qt 6 with the "MinGW 64-bit" kit.
  - CMake and MinGW from the Qt installer (under "Tools").
  Install them with the Qt Online Installer from https://www.qt.io
  (select Qt 6.x > MinGW 64-bit, plus Tools > CMake and MinGW).


------------------------------------------------------------------------------
3. FILES
------------------------------------------------------------------------------

Simulator core (used by both versions):

  ALU.h / ALU.cpp
  ControlUnit.h / ControlUnit.cpp
  CPU.h / CPU.cpp
  DisplayOutput.h / DisplayOutput.cpp
  ExecutionError.h / ExecutionError.cpp
  ExecutionStatus.h
  Instruction.h / Instruction.cpp
  Memory.h / Memory.cpp
  RegisterFile.h / RegisterFile.cpp
  VoleMachine.h / VoleMachine.cpp

Console version only:

  main.cpp                 text menu (has its own main function)

Qt GUI version only:

  qt_main.cpp              starts the GUI (has its own main function)
  MainWindow.h / .cpp      the window
  CMakeLists.txt           build file (CMake)
  VoleGui.pro              build file (qmake) - use this OR CMakeLists.txt

IMPORTANT: a program may contain only ONE main function. Never build
main.cpp and qt_main.cpp together. Easiest way to avoid mistakes: use two
separate folders.

  Folder "console" : the 12 simulator pairs + ExecutionStatus.h + main.cpp
  Folder "gui"     : the same files WITHOUT main.cpp, plus
                     MainWindow.h, MainWindow.cpp, qt_main.cpp,
                     CMakeLists.txt

Always use one consistent set of files. Mixing old and new copies of the
simulator files causes errors such as "has no member named ...".


------------------------------------------------------------------------------
4. BUILD AND RUN THE CONSOLE VERSION
------------------------------------------------------------------------------

1) Open a terminal in the "console" folder
   (in VS Code: Terminal > New Terminal).

2) Build (PowerShell):

     g++ -std=c++17 (Get-ChildItem *.cpp).Name -o vole

   Build (cmd):

     g++ -std=c++17 *.cpp -o vole

   This creates vole.exe in the same folder. If you see errors, no
   vole.exe is created. (An old vole.exe from a previous build may still be
   there, so always check that the build finished without errors.)

3) Run:

     .\vole.exe          (PowerShell)
     vole.exe            (cmd)

   You can also give a program file directly:

     .\vole.exe test01_hello.txt

Using the menu:

     1) Load program from a .txt file   (asks for the file name, then the
                                          start address; press Enter for 0A)
     2) Run program (until HALT or error)
     3) Execute a single step
     4) Show CPU status (PC, IR, registers)
     5) Dump memory                      (press Enter twice for all 256 cells)
     6) Reset machine (clears memory)
     0) Exit

Quick test:  1 -> test01_hello.txt -> Enter -> 2
Expected screen output: H, i, !


------------------------------------------------------------------------------
5. BUILD AND RUN THE QT GUI VERSION
------------------------------------------------------------------------------

Option A - Qt Creator (easiest)

  1) Open Qt Creator.
  2) File > Open File or Project... and choose CMakeLists.txt in the
     "gui" folder.
  3) Select the "Desktop Qt 6.x MinGW 64-bit" kit and click Configure.
  4) Press the green Run button.

Option B - command line

  1) Start menu > search "Qt 6.x.x (MinGW ...)" and open it. This is a normal
     command prompt with Qt already set up.

  2) Add CMake and the compiler to the PATH for this window (adjust the
     folders if your Qt version/folders are named differently):

       set PATH=C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\mingw1310_64\bin;%PATH%

     To find the correct CMake folder on your PC:

       where /r C:\Qt cmake.exe

  3) Go to the gui folder. Use quotes if the path contains spaces, and /d
     if it is on another drive. Type each command on its own line:

       cd /d "C:\path\to\gui"

  4) Build:

       cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
       cmake --build build

  5) Run it. From the same Qt window:

       build\VoleGui.exe

  6) (Optional) To make the program start from anywhere, including by
     double-clicking it, copy the needed Qt libraries next to the exe on
     YOUR computer:

       windeployqt build\VoleGui.exe

     This creates extra files (DLLs) inside the build folder. They are not
     part of the source package and should not be added to it.

If you prefer qmake: open VoleGui.pro in Qt Creator instead of
CMakeLists.txt (edit the file names in it if yours are capitalized
differently).


------------------------------------------------------------------------------
6. USING THE GUI
------------------------------------------------------------------------------

Left panel
  - Program box : type or paste hex, or press "Open .txt file..."
  - "Insert sample program" fills in a small "Hi!" program
  - Start address (hex, default 0A), then press "Load program"
  - Step, Run to HALT, Reset machine, Dump memory

Middle panel
  - Program listing in readable form, with the next instruction highlighted
  - "Dump memory" switches to a 16x16 hex grid (program bytes in blue)
  - Console at the bottom shows [SCREEN] output, messages and errors

Right panel
  - PC, IR and state (HALTED = green, ERROR = red)
  - The 16 registers; a register that just changed is shown in yellow


------------------------------------------------------------------------------
7. PROGRAM FILE FORMAT
------------------------------------------------------------------------------

  - Hex digits only. Each instruction is 4 hex digits (2 bytes).
  - Spaces, tabs and line breaks may be placed anywhere between bytes.
  - An optional 0x prefix is allowed on each token (0x20 0x4F).
  - Comments start with  #  or  ;  or  //  and run to the end of the line.
  - The total number of hex digits must be even.

Example:

     2048   # R0 = 'H'
     3000   # print R0 on the screen
     C000   # HALT

Programs are loaded at address 0A by default, and the PC starts there.


------------------------------------------------------------------------------
8. INSTRUCTION SET
------------------------------------------------------------------------------

Digits: opcode  R  X  Y   (XY together = a value or a memory address;
for 3-register instructions X = S and Y = T)

  1RXY  LOAD    R = memory[XY]
  2RXY  LOAD    R = XY (the value itself)
  3RXY  STORE   memory[XY] = R      (XY = 00 also prints R on the screen)
  40RS  MOVE    register S = register R
  5RST  ADD     R = S + T           (two's complement integers)
  6RST  ADD     R = S + T           (8-bit floating point)
  7RST  OR      R = S OR  T
  8RST  AND     R = S AND T
  9RST  XOR     R = S XOR T
  AR0X  ROTATE  rotate register R right by X steps
  BRXY  JUMP    if R equals R0, PC = XY   (B0XY always jumps)
  C000  HALT    stop

Invalid opcodes (0 and D-F) stop the program with an error.
After 1,000,000 instructions the machine stops (possible infinite loop).


------------------------------------------------------------------------------
9. TEST PROGRAMS (vole_tests.zip)
------------------------------------------------------------------------------

Load each file from the menu or GUI, then run it.

  test01_hello.txt             prints H, i, !
  test02_add_twos_complement   prints 0x08, then 0x00
  test03_logic                 prints 0xFC, 0x30, 0xCC
  test04_rotate                prints 0x60
  test05_memory_and_move       prints 'B'
  test06_jump_loop             prints 'A' three times
  test07_floating_point        prints 0x68
  test08_formats_and_comments  prints 'O' then 'K'

  error_invalid_opcode.txt     run  -> "Invalid instruction 0xD000"
  error_infinite_loop.txt      run  -> stopped after 1000000 instructions
  bad_odd_digits.txt           load -> "odd number of hex digits"
  bad_not_hex.txt              load -> "invalid hex value"


------------------------------------------------------------------------------
10. TROUBLESHOOTING
------------------------------------------------------------------------------

"redefinition of ..." errors
    A header contains function bodies AND the .cpp defines them again. Use
    the provided files (ExecutionError.h contains only declarations).

"has no member named ..." errors
    Old and new copies of the files are mixed. Use one clean, matching set.

"multiple definition of `main'"
    Two files with a main function are being built together
    (main.cpp, qt_main.cpp, VoleMachineSim.cpp ...). Keep only one.

".\vole.exe is not recognized"
    The build failed, so no exe was created. Fix the errors shown above the
    message, then build again.

"cmake is not recognized"
    You are not in the Qt command prompt, or CMake is not on the PATH. Use
    the Qt window from the Start menu and run the "set PATH=..." line from
    section 5.

"The system cannot find the path specified" on cd
    Put the path in quotes and use cd /d when the folder is on another drive.
    Type each command on its own line.

"VoleMachine.h: No such file or directory" (Qt)
    The simulator files are not in the same folder as CMakeLists.txt. Copy
    them there, then Build > Run CMake in Qt Creator.

"Qt6Widgets.dll was not found" when double-clicking the exe
    Run windeployqt build\VoleGui.exe once (section 5, step 6).

Visual Studio users
    Add all .cpp files of ONE version (console or GUI) to the project and
    make sure only one file contains main.


==============================================================================
