This doesn't works on Windows.<br>Only Linux/MacOS.<br>If you want to use the Windows version change the includes and code.(I'm going to open a branch in the future)<br>Have a great day.<br>I wroted some explanations over there feel free to read them.

## Compiling

You need **G++/GCC** for compiling. (MSVC For windows will work too...)

Compiling with Makefile : `make`<br>
Compiling with G++ Manually : `gcc -Wall -Wextra -pedantic client.c -o client && gcc -Wall -Wextra -pedantic server.c -o server `
