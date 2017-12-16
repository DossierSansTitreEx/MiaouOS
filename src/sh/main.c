#include <unistd.h>

int main()
{
    write(1, "Hello", 5);
    for (;;) {}
    return 0;
}

void _start()
{
    /* No CRT yet */
    main();
}
