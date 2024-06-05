#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
// #include <io.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 1024

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
    int TotalInodes;
    int FreeInode;
}SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode 
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int Permission; // 1  23
    struct inode *next;
}INODE, *PINODE, **PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode; // 1  2  3
    PINODE ptrinode;
}FILETABLE, *PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

void CreateDILB()
{
    int i = 1;   // inode number.
    PINODE newn = NULL;
    PINODE temp = head;

    while(i<=MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE));

        newn->LinkCount = 0;
        newn->ReferenceCount = 0;
        newn->FileType = 0;
        newn->FileSize = 0;

        newn->Buffer = NULL;
        newn->next = NULL;

        newn->InodeNumber = i;

        if(temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
    printf("DILB created Successfully...\n");
}

void InitialiseSuperBlock()
{
    int i = 0;
    while(i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;
}

void ls_file()
{
    int i = 0;
    PINODE temp = head;

    if(SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("ERROR : There are no files\n");
        return;
    }

    printf("\nFile Name\tInode Number\tFile Size\tLink Count\n");
    printf("----------------------------------------------------------\n");
    while(temp != NULL)
    {
        if(temp->FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName, temp->InodeNumber, temp->FileActualSize, temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("----------------------------------------------------------\n"); 
}

void CloseAllFile()
{
    int i = 0;
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
            break;
        }
        i++;
    }
}

void DisplayHelp()
{
    printf("ls : To List out all files.\n");
    printf("clear : To Clear the console.\n");
    printf("open : To Open a file.\n");
    printf("close : To Close the opened file.\n");
    printf("closeall : To Close all opened files.\n");
    printf("read :To Read the contents from a file.\n");
    printf("write : To Write contents in a file.\n");
    printf("exit : to Terminate the VIRTUAL FILE SYSTEM.\n");
    printf("stat : To Display information of the file using it's name.\n");
    printf("fstat : To Display information of the file using file descriptor.\n");
    printf("truncate : To Remove all data from a file.\n");
    printf("rm : To Delete the file.\n");
}

int stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
    {
        return -1;
    }

    while(temp != NULL)
    {
        if(strcmp(name, temp->FileName) == 0)
        {
            break;
        }
        temp = temp->next;
    }

    if(temp == NULL)
    {
        return -2;
    }

    printf("\n---------Statistical Information About The File---------\n");
    printf("File Name : %s\n",temp->FileName);
    printf("Inode Number : %d\n", temp->InodeNumber);
    printf("File Size : %d\n", temp->FileSize);
    printf("Actual File Size : %d\n", temp->FileActualSize);
    printf("Link Count: %d\n", temp->LinkCount);
    printf("Reference Count : %d\n", temp->ReferenceCount);

    if(temp->Permission == 1)
    {
        printf("File Permission : Read Only.\n");
    }
    else if(temp->Permission == 2)
    {
        printf("File Permission : Write Only.\n");
    }
    else if(temp->Permission == 3)
    {
        printf("File Permission : Read & Write.\n");
    }
    printf("-------------------------------------------------------\n\n");

    return 0;
}

int fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0;

    if(fd < 0)
    {
        return -1;
    }
    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -2;
    }

    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("\n---------Statistical Information About The File---------\n");
    printf("File Name : %s\n",temp->FileName);
    printf("Inode Number : %d\n", temp->InodeNumber);
    printf("File Size : %d\n", temp->FileSize);
    printf("Actual File Size : %d\n", temp->FileActualSize);
    printf("Link Count: %d\n", temp->LinkCount);
    printf("Reference Count : %d\n", temp->ReferenceCount);

    if(temp->Permission == 1)
    {
        printf("File Permission : Read Only.\n");
    }
    else if(temp->Permission == 2)
    {
        printf("File Permission : Write Only.\n");
    }
    else if(temp->Permission == 3)
    {
        printf("File Permission : Read & Write.\n");
    }
    printf("-------------------------------------------------------\n\n");

    return 0;

}

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}

int GetFDFromName(char *name)
{
  int i = 0;

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
        {
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name) == 0)
            {
                break;
            }
            i++;
        }
        if(i == 50)
        {
            return -1;
        }
        else
        {
            return i;
        }
    }
}

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);
    if(i == -1)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}

// rm_File("Demo.txt")
int rm_File(char *name)
{
    int fd = 0;

    fd = GetFDFromName(name);
    if(fd == -1)
    {
        return -1;
    }

    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
    // free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
        free(UFDTArr[fd].ptrfiletable);
    }
    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)++;
}

void man(char *name)
{
    if(name == NULL)
    {
        return;
    }
    else if(strcmp(name, "create") == 0)
    {
        printf("Description : Used to create a new regular file.\n");
        printf("Usage : create File_name Permission\n"); 
    }
    else if(strcmp(name, "read") == 0)
    {
        printf("Description : Used to read Data from a regular file.\n");
        printf("Usage : read File_name No_Of_Bytes_To_Read\n");
    }
    else if(strcmp(name, "write") == 0)
    {
        printf("Description : Used to write into a regular file.\n");
        printf("Usage : write File_name\n After this enter the data to be written in the file.\n");
    }
    else if(strcmp(name, "ls") == 0)
    {
        printf("Description : Used to List all the information of files.\n");
        printf("Usage : ls\n");
    }
    else if(strcmp(name, "stat") == 0)
    {
        printf("Description : Used to Display the information of a file.\n");
        printf("Usage : stat File_name\n");
    }
    else if(strcmp(name, "fstat") == 0)
    {
        printf("Description : Used to Display the information of a file.\n");
        printf("Usage : fstat File_Descriptor\n");
    }
    else if(strcmp(name, "truncate") == 0)
    {
        printf("Description : Used to remove data from a file.\n");
        printf("Usage : truncate File_name\n");
    }
    else if(strcmp(name, "open") == 0)
    {
        printf("Description : Used to Open an existing file.\n");
        printf("Usage : open File_name MODE.\n");
    }
    else if(strcmp(name, "close") == 0)
    {
        printf("Description : Used to Close an open file.\n");
        printf("Usage : close File_name\n");
    }
    else if(strcmp(name, "closeall") == 0)
    {
        printf("Description : Used to Close all opened files.\n");
        printf("Usage : closeall\n");
    }
    else if(strcmp(name, "lseek") == 0)
    {
        printf("Description : Used to change the file offset.\n");
        printf("Usage : lseek File_name ChangeInOffset StartPoint\n");
    }
    else if(strcmp(name, "rm") == 0)
    {
        printf("Description : Used to Delete a file.\n");
        printf("Usage : rm File_Name\n");
    }
    else 
    {
        printf("ERROR : No Manual entry available.\n");
    }
}

int WriteFile(int fd, char *arr, int isize)
{
    if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode) != READ+WRITE))
    {
        return -1;
    }

    if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != READ+WRITE))
    {
        return -1;
    }

    if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
    {
        return -2;
    }

    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
    {
        return -3;
    }

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)  = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;

    return isize;
}

int truncate_File(char *name)
{
    int fd = GetFDFromName(name);

    if(fd == -1)
    {
        return -1;
    }

    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer, 0, 1024);
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;
}

PINODE Get_Inode(char *name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
        return NULL;

    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName) == 0)
        
            break;
        
        temp = temp->next;
    }

    return temp;
}

int CreateFile(char *name, int permission)
{
    int i = 3;
    PINODE temp = head;

    if((name == NULL) || (permission == 0) || (permission > 3))
    {
        return -1;
    }

    if(SUPERBLOCKobj.FreeInode == 0)
    
        return -2;
    
    (SUPERBLOCKobj.FreeInode)--;

    if(  (name) != NULL)
    
        return -3;
    
    while(temp != NULL)
    {
        if(temp->FileType == 0)
            break;
        temp = temp->next;
    }
    
    while(i < MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;

    UFDTArr[i].ptrfiletable->ptrinode = temp;

    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName, name);

    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->Permission = permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);
    
    return i;
}

int OpenFile(char *name, int mode)
{
    int i = 0;
    PINODE temp = NULL;

    if(name == NULL  ||  mode <= 0)
    {
        return -1;
    }

    temp = Get_Inode(name);
    
    if(temp == NULL)
    {
        return -2;
    }

    if(temp->Permission < mode)
    {
        return -3;
    }

    while(i < 50)
    {
        if(UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    if(UFDTArr[i].ptrfiletable == NULL)
    {
        return -1;
    }

    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = mode;
    
    if(mode == READ + WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }   
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }

    UFDTArr[i].ptrfiletable->ptrinode = temp;

    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

    return i;
}

int LseekFile(int fd, int size, int from)
{
    if((fd < 0) || (from > 2))
    {
        return -1;
    }

    if(UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }

    if(UFDTArr[fd].ptrfiletable->mode == READ || (UFDTArr[fd].ptrfiletable->mode == READ + WRITE))
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
        }
        else if(from == START)
        {
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }
            if(size < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
            {
                return -1;
            }
            
            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
        }
        else if(from == START)
        {
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }

            if(size < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if(from == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
            {
                return -1;
            }

            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;

        }
    }
    else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
            }
        }
        else if(from == START)
        {
            if(size > MAXFILESIZE)
            {
                return -1;
            }

            if(size < 0)
            {
                return -1;
            }

            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            }

            (UFDTArr[fd].ptrfiletable->writeoffset) = size;
        }
        else if (from == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }

            if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
            {
                return -1;
            } 

            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
}

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80], arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    // shell
    while(1)
    {
        fflush(stdin);
        strcpy(str, "");

        printf("\nApex CVFS : >");

        fgets(str,80,stdin); //scanf("%[^'\n']s", str)

        // create   demo.txt     read

        count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);
        // strtok
        if(count == 1)
        {
            if(strcmp(command[0], "ls") == 0)
            {
                ls_file();
            }
            else if(strcmp(command[0], "closeall") == 0)
            {
                CloseAllFile();
                printf("All files closed successfully...\n");
                continue;
            }
            else if(strcmp(command[0], "clear") == 0)
            {
                system("cls");
                continue;
            }
            else if(strcmp(command[0], "help") == 0)
            {
                DisplayHelp();
                continue;
            }
            else if (strcmp(command[0], "exit") == 0)
            {
                printf("Terminating the Apex Virtual File System.\n");
                break;
            }
            else
            {
                printf("\nERROR : Command Not Found!!!\n");
                continue;
            }
        }
        else if(count == 2)
        {
            if(strcmp(command[0], "stat") == 0)
            {
                ret = stat_file(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                    continue;
                }
                if(ret == -2)
                {
                    printf("ERROR : No Such File Or Directory.\n");
                    continue;
                }
            }
            else if(strcmp(command[0], "fstat") == 0)
            {
                ret = fstat_file(atoi(command[1]));
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                }
                if(ret == -2)
                {
                    printf("ERROR : No Such File Or Directory.\n");
                }
                continue;
            }
            else if(strcmp(command[0], "close") == 0)
            {
                ret = CloseFileByName(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters.\n");
                }
                if(ret == -2)
                {
                    printf("ERROR : No Such File Or Directory.\n");
                }
                continue;
            }
            else if(strcmp(command[0], "rm") == 0)
            {
                ret = rm_File(command[1]);
                if(ret = -1)
                {
                    printf("ERROR : No Such File Or Directory.\n");
                }
                continue;
            }
            else if(strcmp(command[0], "man") == 0)
            {
                man(command[1]);
                continue;
            }
            else if(strcmp(command[0], "write") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect parameter\n");
                }
                printf("Enter the Data : \n");
                scanf("%[^'\n']", arr);

                ret = strlen(arr);
                if(ret == 0)
                {
                    printf("ERROR : Incorrect Parameter\n");

                    continue;
                }
                ret = WriteFile(fd,arr,ret);
                if(ret == -1)
                {
                    printf("ERROR : Permission Denied.\n");
                }
                if(ret == -2)
                {
                    printf("ERROR : Insufficient Memory.\n");
                }
                if(ret == -3)
                {
                    printf("ERROR : Not a regular file.\n");
                }
            }
            else if (strcmp(command[0], "truncate") == 0)
            {
                ret = truncate_File(command[1]);
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameter.\n");
                }
            }
            else 
            {
                printf("\nERROR : Command Not Found !!!\n");
            }
        }
        else if(count == 3)
        {
            if(strcmp(command[0], "create") == 0)
            {
                ret = CreateFile(command[1], atoi(command[2]));
                if(ret >= 0)
                {
                    printf("File is Successfully Created with file descriptor : %d.\n", ret);
                }
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                }
                if(ret == -2)
                {
                    printf("ERROR : There are no inodes available.\n");
                }
                if(ret == -3)
                {
                    printf("ERROR : File already exists.\n");
                }
                if(ret == -4)
                {
                    printf("ERROR : Memory allocation failure.\n");
                }
                continue;
            }
            else if(strcmp(command[0], "open") == 0)
            {
                ret = OpenFile(command[1], atoi(command[2]));
                if(ret >= 0)
                {
                    printf("File is successfully opened with file descriptor : %d\n", ret);
                }
                if(ret == -1)
                {
                    printf("ERROR : Incorrect Parameter.\n");
                }
                if(ret == -2)
                {
                    printf("ERROR : No Such File Or Directory.\n");
                }
                if(ret == -3)
                {
                    printf("ERROR : Permission Denied.\n");
                }
                continue;
            }
            else if (strcmp(command[0], "read") == 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect Parameter.\n");
                    continue;
                }

                ptr = (char *)malloc(atoi((command[2]))+1);

                if(ptr == NULL)
                {
                    printf("ERROR : Memory allocation faliuer.\n");\
                    continue;
                }

                if(ret == -1)
                {
                    printf("ERROR : No Such File Or Directorry.\n");
                }

                if(ret == -2)
                {
                    printf("ERROR : Permission Denied.\n");
                }

                if(ret == -3)
                {
                    printf("ERROR : Reached the end of the file\n");
                }

                if(ret > 0)
                {
                    write(2,ptr,ret);
                }
                continue;
            }
            else
            {
                printf("\nERROR: Command Not Found !!!\n");
                continue;
            }
        }
        else if(count == 4)
        {
            if(strcmp(command[0], "lseek") == 0)
            {
                fd = GetFDFromName(command[1]);

                if(fd == -1)
                {
                    printf("ERROR : Incorrect Parameter\n");
                    continue;
                }
                ret = LseekFile(fd,atoi(command[2]), atoi(command[3]));
                if(ret == -1)
                {
                    printf("UNABLE TO PERFORM LSEEK.\n");
                }
            }
            else
            {
                printf("\nERROR : Command Not Found !!!\n");
                continue;
            }
        }
    }
    return 0;
}