#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (chflags(argv[0], 0) != 0) {
        return 1;
    }
    return 0;
}
