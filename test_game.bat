@echo off
REM Compile and test C++ vs Python

echo ========================================
echo Compiling C++ bot...
echo ========================================
cd c_version
C:\mingw64\bin\g++.exe -std=c++11 -O2 bot.cpp -o bot.exe
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile C++ bot
    cd ..
    pause
    exit /b 1
)
cd ..

echo.
echo ========================================
echo Compiling Java Arbitre...
echo ========================================
javac Arbitre.java
if %ERRORLEVEL% NEQ 0 (
    echo Failed to compile Java Arbitre
    pause
    exit /b 1
)

echo.
echo ========================================
echo Starting game: Python vs C++
echo ========================================
echo Player A: Python
echo Player B: C++
echo.

java Arbitre

echo.
echo ========================================
echo Game finished!
echo ========================================
pause
