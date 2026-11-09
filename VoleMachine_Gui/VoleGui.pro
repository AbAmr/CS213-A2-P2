QT += widgets
CONFIG += c++17

SOURCES += qt_main.cpp MainWindow.cpp \
           ALU.cpp Controlunit.cpp Cpu.cpp Displayoutput.cpp ExecutionError.cpp \
           Instruction.cpp Memory.cpp RegisterFile.cpp VoleMachine.cpp

HEADERS += MainWindow.h \
           ALU.h ControlUnit.h Cpu.h Displayoutput.h ExecutionError.h ExecutionStatus.h \
           Instruction.h Memory.h RegisterFile.h VoleMachine.h
