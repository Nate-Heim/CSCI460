#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
// 333 is the system call number in syscall_64.tbl
#define SYS_HELLOWORLD 333

int main() {
    long res;

    // System call here
    res = syscall(SYS_HELLOWORLD);

    if (res == 0) {
        printf("Ummm..... sure... the system call executed successfully.\n");
    } else {
        printf("Umm....... no. The system call failed horrendously.\n Heres the error: %ld (%s)\n", res, strerror(errno));
    }

    return 0;
}

