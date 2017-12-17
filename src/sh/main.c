#include <unistd.h>

int main()
{
    write(1, "Hello\n", 6);
    for (;;) {}
    return 0;
}

void _start()
{
    /* No CRT yet */
    main();
}
