# Testing C++ vs Python

## Current Status

The Arbitre.java has been modified to support cross-language testing.

## Setup Instructions

### 1. Install C++ Compiler (if not already installed)

Download and install MinGW-w64:

1. Go to: https://www.mingw-w64.org/downloads/
2. Or use winlibs: https://winlibs.com/
3. Download and extract to C:\mingw64
4. Add C:\mingw64\bin to your PATH environment variable

### 2. Compile the C++ Bot

```powershell
cd "c:\Users\USER\Desktop\master 1\ai game programming\ai_game_project\c_version"
g++ -std=c++11 -O2 bot.cpp -o bot.exe
```

### 3. Test Python vs C++

Once bot.exe is compiled, modify Arbitre.java line 7-11 to use:

```java
Process A = Runtime.getRuntime().exec("python python_version\\bot.py 1");
Process B = Runtime.getRuntime().exec("c_version\\bot.exe 2");
```

Then compile and run:

```powershell
cd "c:\Users\USER\Desktop\master 1\ai game programming\ai_game_project"
javac Arbitre.java
java Arbitre
```

### Current Configuration

The Arbitre.java is currently set up to test:

- Player A: Python bot
- Player B: Java bot (change to C++ once compiled)

## Alternative: Use Online C++ Compiler

If you can't install a C++ compiler, you can:

1. Copy bot.cpp content to an online compiler (like onlinegdb.com)
2. Compile it there
3. Download the executable
4. Place it in the c_version folder as bot.exe
