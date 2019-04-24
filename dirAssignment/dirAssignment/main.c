#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


/* struct dirent {
 ino_t          d_ino;       // inode number
 off_t          d_off;       // offset to the next dirent
 unsigned short d_reclen;    // length of this record
 unsigned char  d_type;      // type of file; not supported
 by all file system types
 char           d_name[256]; // filename
 };
*/


#define MAXENTRIES  1000

struct fileinfo {
    long filesize;
    int  isdirectory;
    char * filename;
};

int cmpstringp(const void *p1, const void *p2) {
    /* The actual arguments to this function are "pointers to
     pointers to char", but strcmp(3) arguments are "pointers
     to char", hence the following cast plus dereference */
    struct fileinfo * p1f = (struct fileinfo *) p1;
    struct fileinfo * p2f = (struct fileinfo *) p2;
    
    return strcmp(p1f->filename, p2f->filename);
}

// pld: isn't const wonderful? But the real issue is the double pointer


int printdir(char * dname, int indent) {
    char buf[500];
    struct dirent *direntp;  // Pointer for directory entry
    struct stat st;
    int isdirectory = 0;
    struct fileinfo files[MAXENTRIES];
    
    // opendir() returns a pointer of DIR type.
    DIR *dirfilep = opendir(dname);
    
    if (dirfilep == NULL)  // opendir returns NULL if couldn't open directory
    {
        fprintf(stderr, "Could not open directory %s\n", dname );
        return -1;
    }
    
    int filecount = 0;
    while ((direntp = readdir(dirfilep)) != NULL) {
        char * fname = direntp->d_name;
        strcpy(buf, dname);
        strcat(buf, "/");
        strcat(buf, fname);
        if (stat(buf, &st) == -1) {
            fprintf(stderr, "stat failed on fname %s\n", fname);
            perror("stat() failed");
        }
        if (direntp->d_type == DT_DIR) isdirectory = 1;
        else isdirectory = 0;
        int filesize = (int) st.st_size;
        //printf("%-35s\t%-20d\t%s\n", fname, filesize, isdirectory ? "Directory" : "");
        files[filecount].filename = (char*) malloc(strlen(fname) +1);
        strcpy(files[filecount].filename, fname);
        files[filecount].isdirectory = isdirectory;
        files[filecount].filesize = filesize;
        filecount++;
        if (filecount > MAXENTRIES) break;
    }
    closedir(dirfilep);
    /* */
    
    qsort(files, filecount, sizeof(struct fileinfo), cmpstringp);
    
    char new_dname[500];
    for (int i=0; i<filecount; i++) {
        char * filename = files[i].filename;
        for (int j=0; j<indent; j++) putchar(' '); // indent
        printf("%-25s\t%ld\n", filename, files[i].filesize);
        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0){
            if (files[i].isdirectory) {
                    for (int j=0; j<indent; j++) putchar(' ');
                    printf("Directory: %s\n", filename);
                    strcpy(new_dname, dname);
                    strcat(new_dname, "/");
                    strcat(new_dname, files[i].filename);
                    printdir(new_dname, indent+4);
                    printf("\n");
                }
        
        }
    }
    /* */
    return 0;
}

int main(void) {
    char * path = "/Users/anthonypeters/Desktop/College/Current Semester/COMP 264";
    printdir(path, 0);
}
