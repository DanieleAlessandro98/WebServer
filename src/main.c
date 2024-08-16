#include <stdio.h>
#include "winsock_utils.h"

int main()
{
    int winsock_result = initialize_winsock();
    if (winsock_result != 0)
    {
        fprintf(stderr, "Failed to initialize Winsock: %d.\n", winsock_result);
        return 1;
    }

    printf("test\n");

    cleanup_winsock();
    return 0;
}
