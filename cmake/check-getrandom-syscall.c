#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/random.h>

int main(void)
{
    char buffer[1];
    const size_t buflen = sizeof(buffer);
    const int flags = GRND_NONBLOCK;
    /* ignore the result, Python checks for ENOSYS and EAGAIN at runtime */
    (void)syscall(SYS_getrandom, buffer, buflen, flags);
    return 0;
}
