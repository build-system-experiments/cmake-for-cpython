#include <stddef.h>
#include <sys/random.h>

int main(void)
{
    char buffer[1];
    const size_t buflen = sizeof(buffer);
    const int flags = 0;
    /* ignore the result, Python checks for ENOSYS at runtime */
    (void)getrandom(buffer, buflen, flags);
    return 0;
}
