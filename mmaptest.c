#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int pid;
    uint shared_addr;

    printf(1, "Testing mmap shared memory...\n");

    // Map shared memory
    shared_addr = (uint)mmap();

    if (shared_addr == 0)
    {
        printf(1, "mmap failed\n");
        exit();
    }

    printf(1, "Mapped shared memory at 0x%x\n", shared_addr);

    // Write to shared memory
    int *shared_data = (int *)shared_addr;
    *shared_data = 42;
    printf(1, "Parent wrote: %d\n", *shared_data);

    // Fork a child process
    pid = fork();
    if (pid < 0)
    {
        printf(1, "fork failed\n");
        exit();
    }

    if (pid == 0)
    {
        // Child process
        printf(1, "Child read: %d\n", *shared_data);

        // Modify the shared data
        *shared_data = 100;
        printf(1, "Child wrote: %d\n", *shared_data);

        // Test unmapping in child
        if (munmap((void *)shared_addr) < 0)
            printf(1, "Child: munmap failed\n");
        else
            printf(1, "Child: munmap succeeded\n");

        exit();
    }
    else
    {
        // Parent process
        wait();

        // Verify the child's modification is visible
        printf(1, "Parent read after child: %d\n", *shared_data);

        // Unmap shared memory
        if (munmap((void *)shared_addr) < 0)
            printf(1, "Parent: munmap failed\n");
        else
            printf(1, "Parent: munmap succeeded\n");
    }

    exit();
}
