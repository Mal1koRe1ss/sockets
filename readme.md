this ain't workin on windows.<br>i can't test it too because i'm using linux you can change the includes and change the code a little bit.<br>have a great day.<br>I wroted some explanations over there feel free to read them.

## Compiling

You need **G++/GCC** for compiling. (MSVC For windows will work too...)

Compiling with CMake : `make`<br>
Compiling with G++ Manually : `gcc -Wall -Wextra -pedantic client.c -o client && gcc -Wall -Wextra -pedantic server.c -o server `