#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char* path, char* file)
{
    char buffer[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // Open the path, exit if it fails
    if((fd = open(path, O_RDONLY)) < 0)
    {
        printf("Find: Cannot open %s\n", path);
        return;
    }

    // Get path information, exit if it fails
    if(fstat(fd, &st) < 0)
    {
        printf("Find: Cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Check if st is a directory
    if(st.type == T_DIR)
    {
        // Check the length of the file
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buffer))
        {
            printf("Find: path too long\n");
        }

        // p is a pointer to the buffer path
        strcpy(buffer, path);
        p = buffer + strlen(buffer);
        *p++ = '/';

        // Traverse through child objects
        while(read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if(de.inum == 0) continue;

            // Create path to child object
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // Do not recurse into "." and ".."
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            {
                continue;
            }

            // Read information of the temporary path
            if(stat(buffer, &st) < 0)
            {
                continue;
            }

            // Compare file name with the file to find, print to screen if matched
            if(strcmp(de.name, file) == 0)
            {
                printf("%s\n", buffer);
            }

            // If child object is a directory, recursively call into that directory
            if(st.type == T_DIR)
            {
                find(buffer, file);
            }
        }
        close(fd);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}