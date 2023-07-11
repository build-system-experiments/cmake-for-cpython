#include <unistd.h>

int main()
{
    setpgrp(0, 0);
    return 0;
}
