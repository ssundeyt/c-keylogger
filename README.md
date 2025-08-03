# Keylogger in C++

If you download this or build upon the responsibility is on u. be an idiot if u want


Records keystrokes and ignore ascii codes for enter, space etc. to ensure a readable log then exports to a .log file everytime a keystroke gets recorded
Also sets a registry key to make sure the program runs at startup. This is activated when the program launches for the first time
Undetected by windef last time i checked

Installing this on a VM (or hypothetically, someone elses machine) will not send any information about the keystrokes to your local machine, I intentionally excluded this feature to prevent misuse.


The code will not run with visual studio debugger (did not test other IDEs). Some POSIX symbols arent defined in that environment. If you want to run or debug your code you need to compile it with gcc/g++ first.

app.manifest & app.rc are used to prompt user for admin rights. I have pre-compiled these with mingw to appres.o for when you compile the application. you can ofc compile yourself too

Just install msys2 mingw and adding to PATH should be enough to run this. Make sure the linker is set up for eventual additional libraries and add those to mingw's path as well.

Assuming mingw is installed and set up. Do the following; _run MSYS2 MINGW64 as admin > cd inside this git on your local machine > run this:_
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