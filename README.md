# Keylogger in C++ (educational purpose)

DONT RUN IT, DONT INSTALL IT ON SOMEONE ELSES MACHINE - Doing so _**will**_ incriminate you and you _**will**_ face fines and/or jail. Take proper precautions.

The logger does NOT send me (or anyone else) information about the keystrokes as the code is at the moment. Please review code before eventually compiling and running.

# Features
- Record keystrokes and ignore ascii codes for enter, space etc. to ensure a readable log
- Export to a .log file everytime a keystroke gets recorded
- Sets a registry key to make sure the program runs at startup. This is activated when the program launches for the first time
- I've tried downloading and unpacking the final .exe to a VM and Windows Defender did **not** detect it. Other AV's might...
- main.cpp has comments which serves as suggestions on how to complicate the code further
- Installing this on a VM (or hypothetically, someone elses machine) will not send any information about the keystrokes to your local machine, I intentionally excluded this feature to prevent misuse.
- I will by no circumstances help you implement stealth functions into your code. I deeply discourage breaching someone else's security, it is against the law and potentially unethical.

# General & Guide
- The code will not run with visual studio debugger (did not test other IDEs). Some POSIX symbols arent defined in that environment. If you want to run or debug your code you need to compile it with gcc/g++ first.

- app.manifest & app.rc are used to prompt user for admin rights. I have pre-compiled these with mingw to appres.o for when you compile the application. If you wish to compile them yourself, _open mingw64 > cd into this git on your local machine > enter the following command:_
```
windres app.rc -O coff -o appres.o
```
- Simply installing msys2 mingw and adding it to PATH should be enough. Make sure the linker is set up for eventual additional libraries and add those to mingw's path as well.

**Compile and run**

- Assuming mingw is installed and set up. Do the following; _run MSYS2 MINGW64 as admin > cd inside this git on your local machine > run this:_
```
g++ -o main.exe main.cpp appres.o
```
then
```
.\main.exe
```
- If you want it to open without the terminal displaying, run:
```
g++ -o main.exe main.cpp appres.o -mwindows
```
_this ensures the process starts as a windows application_

- If you want it to be standalone, run:
```
g++ -o main.exe main.cpp appres.o -static
```
_ensure correct include and lib are set inside visual studio and linker. ensure mingw64 has the appropriate .dll files in path_

- You can combine the both like this:
```
g++ -o main.exe main.cpp appres.o -mwindows -static
```

# Further Notice
The code is supplied as is. I do not take any responsibilities for what you do with the code. I shared this code to spread information on how keyloggers work to ensure your own privacy. 

Futhermore, C++ is an amazing lang to write these kinds of malwares in. It's low level allows for fast and simple tracing of basic windows features such as keystrokes. DONT DO THIS, it is stupid and you will break the law. Do not attempt to seek further information on how to code these kinds of programs. Instead, seek information on how you can stay protected.
