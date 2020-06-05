#include "wirte.h"

void testWirte(const char* filename, const void* context){
    int fd;
    struct stat sb;
    void *dst;

    fd = open(filename, O_WRONLY);
    if(fd == -1)
        handle_error("Open");
    
    if(fstat(fd, &sb) == -1)
        handle_error("fstat");

    dst = mmap(NULL, getpagesize(), PROT_WRITE, MAP_SHARED, fd, ~(_SC_PAGE_SIZE) - 1);
    if(dst == MAP_FAILED)
        handle_error("mmap");
   
    memcpy(dst, context, strlen(context) + 1);
   munmap(addr, 0); 
}
