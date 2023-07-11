#include <dirent.h>

int main(void)
{
    struct dirent entry;
    return entry.d_type == DT_UNKNOWN;
}
