#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
//#include<io.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFTLESIZE 2048

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
   int TotalInodes;
   int FreeInode;
}SUPERBLOCK,*PSUPERBLOCK;

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
    int Permission;  //1  23
    struct inode *next;
}INODE,*PINODE,**PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode; //1 2 3 
    PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
   PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head =NULL;

void man(char *name)
{
    if(name ==NULL) return;

    if(strcmp(name,"create")==0)
    {
        printf("Description : Used to creat new regular file \n");
        printf(" usage : create File_name Permission \n");
    }
    else if(strcmp(name,"read")==0)
    {
        printf("Description : Used to read new regular file \n");
        printf(" usage : read File_name No_Of_Bytes_To_Read \n");
    }
     else if(strcmp(name,"write")==0)
    {
        printf("Description : Used to write new regular file \n");
        printf(" usage : write File_name \n After this enter data that we want to writen \n");
    }
    else if(strcmp(name,"ls")==0)
    {
        printf("Description :Used to list all inforation of file\n");
        printf("usage :ls\n");
    }
    else if(strcmp(name,"stat")==0)
    {
        printf("Description :Used to display inforation of file\n");
        printf("usage :stat_file_name\n");
    }
    else if(strcmp(name,"fstat")==0)
    {
        printf("Description :Used to display inforation of file\n");
        printf("usage :stat File_Descriptor\n");
    }
    else if(strcmp(name,"truncate")==0)
    {
        printf("Description :Used to remove data from  file\n");
        printf("usage :truncate File_name\n");
    }
     else if(strcmp(name,"open")==0)
    {
        printf("Description :Used to open existing file\n");
        printf("usage :open File_name mode\n");
    }
    else if(strcmp(name,"close")==0)
    {
        printf("Description :Used to close opened file\n");
        printf("usage :close File_name \n");
    }
     else if(strcmp(name,"closeall")==0)
    {
        printf("Description :Used to close all opened file\n");
        printf("usage :closeall\n");
    }
     else if(strcmp(name,"lseek")==0)
    {
        printf("Description :Used to change file offset\n");
        printf("usage :lseek File_name ChangeInOffset StartPointer\n");
    }
     else if(strcmp(name,"rm")==0)
    {
        printf("Description :Used to delete the file\n");
        printf("usage :rm File_name \n");
    }
    else
    {
        printf("ERROR :No manual entry available.");
    }
}

void DisplayHelp()
{
    printf("ls : To List out All files \n");
    printf("clear : To clear console \n");
    printf("open : To open the file \n");
    printf("close:To close file \n");
    printf("closeall :To close all open file \n");
    printf("read : To Read the contents from file \n");
    printf("write : To Write content from file \n");
    printf("exit : To Terminate File system \n");
    printf("stat: To Display information of file using name \n");
    printf("fstat : To Display information of file using file descritor \n");
    printf("truncate :To Remove all data from file \n");
    printf("rm :To Delete the file \n");
}

int GetFDFromName(char *name)
{
    int i=0;

    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable != NULL)
           if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
               break;
               i++;
    }

    if(i==MAXINODE)  return -1;
    else       return i;
}

PINODE Get_Inode(char * name)
{
    PINODE temp =head;
    int i=0;

    if(name==NULL)
       return NULL;

    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName)==0)
           break;
         temp=temp->next;  
    }   
    return temp;
}

void CreateDILB()
{
    int i=1;
    PINODE newn =NULL;
    PINODE temp =head;

    while(i<= MAXINODE)
    {
        newn=(PINODE)malloc(sizeof(INODE));
        
        newn->LinkCount=0;
        newn->ReferenceCount=0;
        newn->FileType=0;
        newn->FileSize=0;

        newn->Buffer=NULL;
        newn->next=NULL;

        newn->InodeNumber=i;

        if(temp==NULL)
        {
            head=newn;
            temp=head;
        }
        else
        {
            temp->next=newn;
            temp=temp->next;
        }
        i++;
    }
   printf("DILB created succefull \n");
}

void InitialiseSuperBlock()
{
    int i=0;
    while(i<MAXINODE)
    {
        UFDTArr[i].ptrfiletable=NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes=MAXINODE;
    SUPERBLOCKobj.FreeInode=MAXFTLESIZE;
}   

int CreateFile(char *name,int permission)
{
    int i=0; //3 on normal operating system
    PINODE temp=head;

    if((name==NULL) ||(permission==0)||(permission>3))
         return -1;

    if(SUPERBLOCKobj.FreeInode==0)
      return -2;

      (SUPERBLOCKobj.FreeInode)--;

     if(Get_Inode(name) !=NULL)
        return -3;

    while(temp != NULL)
    {
        if(temp->FileType==0)
          break;
        temp=temp->next;
    } 

    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable==NULL)
           break;
           i++;
    }

    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));

    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode=permission;
    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;

    UFDTArr[i].ptrfiletable->ptrinode=temp;

    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFTLESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=0;
    UFDTArr[i].ptrfiletable->ptrinode->Permission=permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char *)malloc(MAXFTLESIZE);

    return i;
}

// rm_File("Demo.txt")
int rm_File(char * name)
{
    int fd=0;

    fd=GetFDFromName(name);
    if(fd==-1)
          return -1;

       (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

      if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount==0)
      {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType=0;
        //free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
        free(UFDTArr[fd].ptrfiletable);
      }

      UFDTArr[fd].ptrfiletable=NULL;
     (SUPERBLOCKobj.FreeInode)++;
}  

int ReadFile(int fd,char *arr,int isize)
{
    int read_size=0;

    if(UFDTArr[fd].ptrfiletable==NULL)    return -1;

    if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)   return -2;

    if(UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->Permission!= READ+WRITE)   return -2;

    if(UFDTArr[fd].ptrfiletable->readoffset ==  UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)   return -3;

    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR) )  return -4;

    read_size=((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->readoffset));
     if(read_size<isize)
     {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);

        UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->readoffset+read_size;
     }
     else
     {
        strncpy(arr,(UFDTArr->ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),isize);
        
        (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset)+isize;
     }

     return isize;
}

int WriteFile(int fd,char *arr,int isize)
{
    if(((UFDTArr[fd].ptrfiletable->mode)!=WRITE) && ((UFDTArr[fd].ptrfiletable->mode)!=READ+WRITE)) return -1;

    if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission)!=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission)!=READ+WRITE)) return -1;

    if((UFDTArr[fd].ptrfiletable->writeoffset)==MAXFTLESIZE)  return -2;

    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!=REGULAR)  return -3;

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);  //
    
     (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset)+isize;

     (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;

      return isize;
}

int OpenFile(char *name,int mode)
{
    int i=0;
    PINODE temp=NULL;

    if(name==NULL ||mode <= 0)
        return -1;

    temp=Get_Inode(name);
    if(temp==NULL)
       return  -2;

       if(temp->Permission <mode)
          return -3;
    
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable==NULL)
          break;
          i++;
    }

    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable==NULL)   return -1;
    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode=mode;
    if(mode==READ + WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
        UFDTArr[i].ptrfiletable->writeoffset=0;
    }
    else if(mode==READ)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
    }
    else if(mode==WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset=0;
    }
    UFDTArr[i].ptrfiletable->ptrinode=temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

    return i;
}

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset=0;
    UFDTArr[fd].ptrfiletable->writeoffset=0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}

int CloseFileByName(char *name)
{
    int i=0;
    i=GetFDFromName(name);
    if(i==-1)
       return -1;

    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}

void CloseAllFile()
{
    int i=0;
    while(i<50)
    {
      if(UFDTArr[i].ptrfiletable!=NULL)
      {  
        UFDTArr[i].ptrfiletable->readoffset=0;
        UFDTArr[i].ptrfiletable->writeoffset=0;
       (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
         break;
      }
    i++;
   }
}

int LseekFile(int fd,int size ,int from)
{
    if((fd<0 )|| (from>2)) return -1;
    if(UFDTArr[fd].ptrfiletable == NULL)  return -1;

     if((UFDTArr[fd].ptrfiletable->mode==READ) || (UFDTArr[fd].ptrfiletable->mode==READ+WRITE))
     {
        if(from == CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)>UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)  return -1;
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0) return -1;
            (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset)+size;
        }
        else if (from ==START)
        {
            if(size >(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))  return -1;
            if(size<0)  return -1;
            (UFDTArr[fd].ptrfiletable->readoffset)=size;
        }
        else if(from ==END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size> MAXFTLESIZE) return -1;
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)  return -1;
              (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
        }
     }
     else if(UFDTArr[fd].ptrfiletable->mode ==WRITE)
     {
        if(from ==CURRENT)
        {
           if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>MAXFTLESIZE)  return -1;
           if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)  return  -1;
           if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
             (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
             (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
         }
         else if(from ==START)
         {
            if(size>MAXFTLESIZE) return -1;
            if(size <0) return -1;
            if(size >(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size > MAXFTLESIZE)  return -1;
               (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=size;
             (UFDTArr[fd].ptrfiletable->writeoffset) =size;
           }
        else if(from ==END)
       {
        if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size>MAXFTLESIZE)   return -1;
        if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)  return  -1;
         (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
       }
   }
}

void ls_file()
{
    int i=0;
    PINODE temp=head;

    if(SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("Error : There are no File \n");
        return;
    }

    printf("\n File Name\tInode Number\tFile Size\tLink Count\n");
    printf("-----------------------------------------------------------------\n");
    while(temp !=NULL)
    {
        if(temp->FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp=temp->next;
    }
    printf("-----------------------------------------------------------------\n");
}

 int fstat_file(int fd)
 {
    PINODE temp=head;
    int i=0;

    if(fd<0)  return -1;

    if(UFDTArr[fd].ptrfiletable==NULL) return -2;

    temp=UFDTArr[fd].ptrfiletable->ptrinode;

    printf("\n----------Statistical Information about file---------\n");
    printf("File name :%s\n",temp->FileName);
    printf("Inode Number  %d\n",temp->InodeNumber);
    printf("File size : %d\n",temp->FileSize);
    printf("Actual File size : %d\n",temp->FileActualSize);
    printf("Link count : %d\n",temp->LinkCount);
    printf("Reference count : %d\n",temp->ReferenceCount);

    if(temp->Permission==1)
         printf("File Permission :Read Only \n");
   else if(temp->Permission ==2)
       printf("File Permission :Write\n");
   else if(temp->Permission ==3)
      printf("File Permission :Read & Write \n");
    printf("-----------------------------------------------------------------\n");

    return 0;
 }

int stat_file(char *name)
{
    PINODE temp=head;
    int i=0;

    if(name ==NULL) return -1;

    while(temp != NULL)
    {
        if(strcmp(name,temp->FileName)==0)
          break;
          temp=temp->next;
    }

    if(temp ==NULL) return -2;

   printf("\n----------Statistical Information about file---------\n");
    printf("File name :%s\n",temp->FileName);
    printf("Inode Number  %d\n",temp->InodeNumber);
    printf("File size : %d\n",temp->FileSize);
    printf("Actual File size : %d\n",temp->FileActualSize);
    printf("Link count : %d\n",temp->LinkCount);
    printf("Reference count : %d\n",temp->ReferenceCount);

    if(temp->Permission==1)
         printf("File Permission :Read Only \n");
   else if(temp->Permission ==2)
       printf("File Permission :Write\n");
   else if(temp->Permission ==3)
      printf("File Permission :Read & Write \n");
    printf("-----------------------------------------------------------------\n");

    return 0;
}

int truncate_File(char *name)
{
    int fd= GetFDFromName(name);
    if(fd==-1)
        return -1;

    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
    UFDTArr[fd].ptrfiletable->readoffset=0;
    UFDTArr[fd].ptrfiletable->writeoffset=0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=0;
}

int main()
{
   char *ptr =NULL;
   int ret=0,fd=0,count=0;
   char command[4][80],str[80],arr[1024];

   InitialiseSuperBlock();
   CreateDILB();

   while(1)   //shell
   {
      fflush(stdin);
      strcpy(str,"");


      printf("\nMarvellous VFS : >");

      fgets(str,80,stdin);  //scanf("%[^'\n']s",str);

      count=sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);

      if(count ==1)   //strtok
      {
         if(strcmp(command[0],"ls")==0)
         {
            ls_file();
         }
         else if(strcmp(command[0],"closeall")==0)
         {
            CloseAllFile();
            printf("All File closed successfully\n");
            continue;
         }
         else if(strcmp(command[0],"clear")==0)
         {
            system("cls");
            continue;
         }
         else if(strcmp(command[0],"help")==0)
         {
            DisplayHelp();
            continue;
         }
         else if(strcmp(command[0],"exit")==0)
         {
            printf("Terminating the Marvellous Virtual File System\n");
            break;
         }
         else
         {
            printf("\nERROR :Command not Fount !!!\n");
            continue;
         }
      }
      else if(count==2)
      {
        if(strcmp(command[0],"stat")==0)
         {
            ret=stat_file(command[1]);
            if(ret==-1)
                  printf("ERROR :Incorrect parameters \n");
            if(ret ==-2)
                  printf("ERROR : There is no such File\n");
              continue;
         }
         else if(strcmp(command[0],"fstat")==0)
         {
            ret=fstat_file(atoi(command[1]));
            if(ret==-1)
                 printf("ERROR : Incorrect Parameters \n");
            if(ret ==-2)
                printf("ERROR : There is no such File\n");
              continue;
         }
         else if(strcmp(command[0],"close")==0)
         {
            ret=CloseFileByName(command[1]);
            if(ret==-1)
                 printf("ERROR :There is no such file \n");
                 continue;
         }
         else if(strcmp(command[0],"rm")==0)
         {
            ret=CloseFileByName(command[1]);
            if(ret==-1)
              printf("ERROR :There is no such file\n");
              continue;
         }
         else if(strcmp(command[0],"man")==0)
         {
            man(command[1]);
         }
         else if(strcmp(command[0],"write")==0)
         {
            fd=GetFDFromName(command[1]);
            if(fd==-1)
            {
                printf("ERROR :Incorrect parameter \n");
                continue;
            }
            printf("Enter the Data :\n");
            scanf("%[^\n]",arr);

            ret=strlen(arr);
            if(ret ==0)
            {
                printf("Enter :Incorrect Parameter \n");
                continue;
            }
            ret=WriteFile(fd,arr,ret);
            if(ret==-1)
               printf("ERROR : Permission denied \n");
            if(ret==-2)
                printf("ERROR :There is no sufficient memory to write \n");
            if(ret ==-3)
                printf("ERROR :It is not regular file\n");
         }
         else if(strcmp(command[0],"truncate")==0)
         {
            ret=truncate_File(command[1]);
            if(ret==-1)
                  printf("ERROR :Incorrect Parameter \n");
         }
         else
         {
             printf("\nERROR :Command not Found !!! \n");
             continue;
         }
      }
      else if(count ==3)
      {
        if(strcmp(command[0],"create")==0)
        {
             ret=CreateFile(command[1],atoi(command[2]));
             if(ret==0)
                 printf("File is Successfully created with file descriptor :%d\n",ret);
             if(ret==-1)
                 printf("ERROR :Incorrect Parameters \n");
            if(ret==-2)
                printf("ERROR :There is no inodes \n");
            if(ret==-3)
                printf("ERROR : File Already exists \n");
            if(ret ==-4)
                printf("ERROR :Memory allocation failure \n");
                continue;
        }
        else if(strcmp(command[0],"open")==0)
        {
            ret=OpenFile(command[1],atoi(command[2]));
            if(ret>=0)
               printf("File is successfully opened with file description :%d \n",ret);
            if(ret==-1)
              printf("ERROR :Incorrect Parameters \n");
            if(ret==-2)
               printf("ERROR :Filr not Present \n");
            if(ret==-3)
              printf("ERROR :Permission denied \n");
              continue;
        }
        else if(strcmp(command[0],"read")==0)
        {
            fd=GetFDFromName(command[1]);
            if(fd==-1)
            {
                printf("ERROR :Incrrect Parameter \n");
                continue;
            }
          ptr=(char *)malloc(sizeof(atoi(command[2]))+1);
          if(ptr ==NULL)
          {
            printf("ERROR :Memory allocation failure \n");
            continue;
          }
          ret=ReadFile(fd,ptr,atoi(command[2]));
           if(ret==-1)
              printf("ERROR :File not Existing \n");
           if(ret==-2)
              printf("ERROR :permission denied \n");
           if(ret==-3)
              printf("ERROR :Reachead at end of file \n");
           if(ret ==-4)
              printf("ERROR :It is not regular file \n");
           if(ret==0)
              printf("ERROR :File Empty \n");
            if(ret>0)
            {
                write(2,ptr,ret);
            }
            continue;
        }
        else
        {
           printf("\nERROR : Command not found !!!\n");
           continue;
        }
      }
      else if(count ==4)
      {
        if(strcmp(command[0],"lseek")==0)
        {
            fd=GetFDFromName(command[1]);
            if(fd==-1)
            {
                printf("ERROR : Incorrect parameter \n");
                continue;
            }
            ret=LseekFile(fd,atoi(command[2]),atoi(command[3]));
            if(ret ==-1)
             {
                printf("ERROR :Unable to perform lseek \n");
             }
        }
        else 
        {
            printf("\nERROR :Command not found !!!\n");
            continue;
        }
      }
      else
      {
           printf("\nERROR :Command not found !!!\n");
           continue;
      }
   }
 

    return 0;
}