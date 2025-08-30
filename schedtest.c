#include "types.h"
#include "stat.h"
#include "user.h"

// Function to do CPU-intensive work and count iterations
void busywork(int *counter, int duration)
{
    int start = uptime();

    // Work until duration ticks have passed
    while (uptime() - start < duration)
    {
        (*counter)++; // Use counter here to prevent from overflow
        // Do some computation to burn CPU cycles
        for (int i = 0; i < 10000; i++)
            ;
    }
}

int main(int argc, char *argv[])
{
    int pid1, pid2;
    int counter1 = 0, counter2 = 0;
    int duration = 50;

    if (argc > 1)
    {
        duration = atoi(argv[1]);
        if (duration <= 0)
            duration = 50;
    }

    printf(1, "Proportional Share Scheduling Test\n");
    printf(1, "==================================\n");

    // First test with standard scheduler
    printf(1, "Running with standard scheduler for %d ticks...\n", duration);

    pid1 = fork();
    if (pid1 < 0)
    {
        printf(1, "Fork failed\n");
        exit();
    }

    if (pid1 == 0)
    {
        // Child process 1
        counter1 = 0;
        busywork(&counter1, duration);
        printf(1, "Process 1 (standard): %d iterations\n", counter1);
        exit();
    }

    pid2 = fork();
    if (pid2 < 0)
    {
        printf(1, "Fork failed\n");
        kill(pid1);
        exit();
    }

    if (pid2 == 0)
    {
        // Child process 2
        counter2 = 0;
        busywork(&counter2, duration);
        printf(1, "Process 2 (standard): %d iterations\n", counter2);
        exit();
    }

    // Wait for processes to complete
    wait();
    wait();

    // Switch to lottery scheduler
    printf(1, "\nSwitching to lottery scheduler...\n");
    if (setscheduler(1) < 0)
    {
        printf(1, "Failed to set scheduler mode\n");
        exit();
    }

    printf(1, "Running with 10:90 share ratio for %d ticks...\n", duration);

    // Create first child process (10% share)
    pid1 = fork();
    if (pid1 < 0)
    {
        printf(1, "Fork failed\n");
        exit();
    }

    if (pid1 == 0)
    {
        // Child process 1 (10% share)
        counter1 = 0;
        busywork(&counter1, duration);
        printf(1, "Process 1 (10%%): %d iterations\n", counter1);
        exit();
    }

    // Create second child process (90% share)
    pid2 = fork();
    if (pid2 < 0)
    {
        printf(1, "Fork failed\n");
        kill(pid1);
        exit();
    }

    if (pid2 == 0)
    {
        // Child process 2 (90% share)
        counter2 = 0;
        busywork(&counter2, duration);
        printf(1, "Process 2 (90%%): %d iterations\n", counter2);
        exit();
    }

    // Set process shares
    if (setprocshare(pid1, 10) < 0) // 10% of CPU
        printf(1, "Failed to set share for process 1\n");

    if (setprocshare(pid2, 90) < 0) // 90% of CPU
        printf(1, "Failed to set share for process 2\n");

    // Wait for processes to complete
    wait();
    wait();

    // Calculate and display the ratio
    printf(1, "\nExpected ratio (process2:process1) is approximately 9:1\n");

    // // Switch to stride scheduler
    // printf(1, "\nSwitching to stride scheduler...\n");
    // if (setscheduler(2) < 0)
    // {
    //     printf(1, "Failed to set stride scheduler mode\n");
    //     exit(1);
    // }

    // printf(1, "Running with 10:90 share ratio for %d ticks using stride scheduling...\n", duration);

    // // Create first child process (10% share)
    // pid1 = fork();
    // if (pid1 < 0)
    // {
    //     printf(1, "Fork failed\n");
    //     exit(1);
    // }

    // if (pid1 == 0)
    // {
    //     // Child process 1 (10% share)
    //     counter1 = 0;
    //     busywork(&counter1, duration);
    //     printf(1, "Process 1 (stride 10%%): %d iterations\n", counter1);
    //     exit(0);
    // }

    // // Create second child process (90% share)
    // pid2 = fork();
    // if (pid2 < 0)
    // {
    //     printf(1, "Fork failed\n");
    //     kill(pid1);
    //     exit(1);
    // }

    // if (pid2 == 0)
    // {
    //     // Child process 2 (90% share)
    //     counter2 = 0;
    //     busywork(&counter2, duration);
    //     printf(1, "Process 2 (stride 90%%): %d iterations\n", counter2);
    //     exit(0);
    // }

    // // Set process shares
    // if (setprocshare(pid1, 10) < 0) // 10% of CPU
    //     printf(1, "Failed to set share for process 1\n");

    // if (setprocshare(pid2, 90) < 0) // 90% of CPU
    //     printf(1, "Failed to set share for process 2\n");

    // // Wait for processes to complete
    // wait();
    // wait();

    // // Calculate and display the ratio
    // printf(1, "\nExpected ratio (process2:process1) is approximately 9:1\n");
    // printf(1, "Stride scheduling should provide more deterministic results than lottery scheduling\n");

    // Switch back to standard scheduler
    if (setscheduler(0) < 0)
        printf(1, "Failed to revert scheduler mode\n");
    printf(1, "Switched back to standard scheduler\n");

    printf(1, "\nTest completed\n");
    exit();
}
