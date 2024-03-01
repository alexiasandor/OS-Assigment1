#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>

struct helper{
  char sect_name[16];
  int sect_type;
  int sect_offset;
  int sect_size;
}h[300];
int ok = 0;

void revers(char *string)
{
  int l = strlen(string);
  char c;
  int i;
  for (i = 0; i < l / 2; i++)
  {
    c = string[i];
    string[i] = string[l - i - 1];
    string[l - i - 1] = c;
  }
}
//functie de listare clasica
void listare(const char *path, long int size_smaller, char *name_ends_with)
{
  DIR *dir = NULL;
  struct dirent *entry = NULL;
  struct stat statbuf;
  char fullpath[1024];
  char rev[100];
  char name_ends_with1[100];
  dir = opendir(path);
  if (dir == NULL)
  {
     printf("ERROR\n invalid directory path");
    return;
  }
   if (ok == 0)
  {
    printf("SUCCESS\n");
    ok = 1;
  }   

  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      snprintf(fullpath, 1024, "%s/%s", path, entry->d_name);

      if (lstat(fullpath, &statbuf) == 0)
      {
        if (size_smaller == -1 && name_ends_with[0] == '\0')
        {
          printf("%s\n", fullpath);
        }
      if (S_ISREG(statbuf.st_mode))
        {
          //printf("%ld", size_smaller);
          if (size_smaller != -1)
          {
            if (statbuf.st_size < size_smaller)
            {
              printf("%s\n", fullpath);
            }
          }
          else
          {
            if (name_ends_with[0] != '\0')
            {
              strcpy(rev, entry->d_name);
              revers(rev);
              strcpy(name_ends_with1, name_ends_with);
              revers(name_ends_with1);
              if(strncmp(rev,name_ends_with1,(strlen(name_ends_with1))-1)==0){
                printf("%s\n", fullpath);
              } 
            }
          }
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
          if (name_ends_with[0] != '\0')
          {
            strcpy(rev, entry->d_name);
            revers(rev);
            strcpy(name_ends_with1, name_ends_with);
            revers(name_ends_with1);
            if(strncmp(rev,name_ends_with1,(strlen(name_ends_with1))-1)==0){
               printf("%s\n", fullpath);
            }
          }
        }
      }
    }
  }
  closedir(dir);
}
// functie listare recursiva
void listareRecursiva(const char *path, long int size_smaller ,char *name_ends_with)
{
  DIR *dir = NULL;
  struct dirent *entry = NULL;
  struct stat statbuf;
  char fullpath[1024];
  char rev[100];
  char name_ends_with1[100];
  dir = opendir(path);
  if (dir == NULL)
  {
    printf("ERROR\n invalid directory path");
    return;
  }
  if (ok == 0)
  {
    printf("SUCCESS\n");
    ok = 1;
  }

  while ((entry = readdir(dir)) != NULL)
  {  
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {   
      snprintf(fullpath, 1024, "%s/%s", path, entry->d_name);
      if (lstat(fullpath, &statbuf) == 0)
    
      { if (size_smaller == -1 && name_ends_with[0] == '\0')
        {
          printf("%s\n", fullpath);
        }
        if (S_ISREG(statbuf.st_mode))
        {
          if (size_smaller != -1)
          {
            if (statbuf.st_size < size_smaller)
            {
              printf("%s\n", fullpath);
            }
          }
          else
          {
            if (name_ends_with[0] != '\0')
            {
              strcpy(rev, entry->d_name);
              revers(rev);
              strcpy(name_ends_with1, name_ends_with);
              revers(name_ends_with1);
             if(strncmp(rev,name_ends_with1,(strlen(name_ends_with1))-1)==0){
               printf("%s\n", fullpath);
            }
            }
          }
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
          if (name_ends_with[0] != '\0')
          {
            strcpy(rev, entry->d_name);
            revers(rev);
            strcpy(name_ends_with1, name_ends_with);
            revers(name_ends_with1);
            if(strncmp(rev,name_ends_with1,strlen(name_ends_with1)-1)==0){
               printf("%s\n", fullpath);
            }
          }
          listareRecursiva(fullpath,size_smaller,name_ends_with);
        }
      }
    }
  }
  closedir(dir);
}
// functie de parsare
int parse( const char *path, int *version, int *nr_of_sect)
{
  char magic;
  int header_size=0;
  int fd=-1;
  //deschidem fisierul
  fd = open(path,O_RDONLY);
  if(fd==-1){
    perror("Nu se poate deschide fisierul");
    return 1;
  }
  else{
    //pozitionam cursorul la final 
    lseek(fd,-1,SEEK_END);
    // read pentru magic
    read(fd,&magic,1);
     if(magic!='y'){
       return 2;
     }
    //pentru header
    lseek(fd,-3,SEEK_END);
    read(fd,&header_size,2);
    //pentru version
    lseek(fd,-header_size,SEEK_END);
    read(fd,version,1);
    if(*version<86 || *version>118){
       return 3;
    }
    //pentru number of sections
    read(fd,nr_of_sect,1);
    if(*nr_of_sect<7 || *nr_of_sect>11){
      return 4;
    }
    for(int i=0;i<*nr_of_sect;i++){
    read(fd,h[i].sect_name,15);
    read(fd,&h[i].sect_type,4);
    // pentru sec type
    if((h[i].sect_type!=80) && (h[i].sect_type!=83) &&(h[i].sect_type!=26) && (h[i].sect_type!=54) &&(h[i].sect_type!=75))
    {
      return 5;
    }
    read(fd,&h[i].sect_offset,4);
    read(fd,&h[i].sect_size,4);
    }
  }
  close(fd);
  return 0;
}
// functia extract
void extract(const char *path, int section, int line){
int fd=-1;
 int nr=1;
 int version=0;
 int nr_of_sect=0;
 bool k=0;
 char caracter;
 
// deschidem fisierul 
 fd=open(path,O_RDONLY);
 if(fd==-1){
   printf("ERROR\ninvalid file");
 }
  //verificam daca e sf
 if(parse(path,&version,&nr_of_sect)==0){
  //verificam sectiunea
  if(section<=nr_of_sect){
    // pozitionam cursorul la inceputul sectiunii
    lseek(fd,h[section-1].sect_offset,SEEK_SET);
      for(int c=0;c<h[section-1].sect_size;c++){
        read(fd,&caracter,1);
         if(nr==line){
          if(k==0){printf("SUCCESS\n");
          k=1;}
            printf("%c", caracter);
          }
         if(caracter=='\x0A'){
          nr++;
         }
      }
  }          
    else{
         printf("ERROR\ninvalid section");
      }
    }
  else{
     printf("ERROR\ninvalid file");
     }
}
// functia findAll
void findAll(const char *path){
  DIR *dir = NULL;
  struct dirent *entry = NULL;
  struct stat statbuf;
  char fullpath[1024];
  int nr_of_sect=0;
  int isType=0;
  int version=0;

  dir = opendir(path);

  if(dir == NULL)
  {
    printf("ERROR\ninvalid directory path\n");
    return;
  }
    if (ok == 0)
  {
    printf("SUCCESS\n");
    ok = 1;
  }
 // printf("SUCCESS\n");
   while ((entry = readdir(dir)) != NULL)
  {
    isType=0; 
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      snprintf(fullpath, 1024, "%s/%s", path, entry->d_name);
      if (lstat(fullpath, &statbuf) == 0){
        if(S_ISREG(statbuf.st_mode)){ 
          if(parse(fullpath,&version,&nr_of_sect)==0){
            for(int i=0;i<nr_of_sect;i++){
             if(h[i].sect_type==26){
              isType=1;
            }
          }
          if(isType==1){
            printf("%s\n", fullpath);
          }
          
        } 

        }
        //else{
        //       printf("ERROR\ninvalid directory path\n");
        //    }
         if(S_ISDIR(statbuf.st_mode)){
          findAll(fullpath);
          }
        }
      }
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
  char path [1024];
  long int size_smaller = -1;
  char name_ends_with[100] = {0};
  name_ends_with[0] = '\0';
  bool isRec = false;
  int version, nr_of_sect;
  int f=0;
  int p=0;
  int a=0;
  int e=0;
  int section ;
  int line;
  if (argc >= 2)
  {
    if (strcmp(argv[1], "variant") == 0)
    {
      printf("56939\n");
    }
   
     // verificam pe rand ce argv-urile pentru a apela functia corespunzatoare
      for (int i = 1; i < argc; i++)
      { if (strcmp(argv[i], "list") == 0){ 
            f=1;
         }
        if (strcmp(argv[i], "recursive") == 0)
        {
          isRec = true;
        } 
        if (strncmp(argv[i], "size_smaller=", 13) == 0)
        { 
          sscanf(argv[i], "size_smaller=%ld", &size_smaller);
        }
        if (strncmp(argv[i], "name_ends_with=", 15) == 0)
        {
          strcpy(name_ends_with, argv[i] + 15);
        }
        if (strncmp(argv[i], "path=", 5) == 0)
        {
          strcpy(path, argv[i] + 5);
          strcat(path, "\0");
        }
        else if (strcmp(argv[i],"parse")==0){
          p=1;
              
        }
        else if(strcmp(argv[i],"extract")==0){
          e=1;
        }
         else if(strncmp(argv[i],"section=", 8)==0){
          sscanf(argv[i], "section=%d", &section);
        }
        else if(strncmp(argv[i],"line=", 5)==0){
          sscanf(argv[i], "line=%d", &line);
        }
        else if(strcmp(argv[i],"findall")==0){
          a=1;
        }
      
  }
  if(f==1) {
    if (isRec == false)
    { 
      listare(path, size_smaller, name_ends_with);
    }
    else {
    
      listareRecursiva(path, size_smaller, name_ends_with);
    }
  }
  // verficare parse
  if(p==1){
    int parseToCheck=parse(path,&version, &nr_of_sect);
    if(parseToCheck==2){
      printf("ERROR\nwrong magic\n");
    }
    if(parseToCheck==3){
      printf("ERROR\nwrong version\n");
    }
    if(parseToCheck==4){
      printf("ERROR\nwrong sect_nr\n");
    }
    if(parseToCheck==5){
      printf("ERROR\nwrong sect_types\n");
    }
    if(parseToCheck==0){
      printf("SUCCESS\n");
      printf("version=%d\n",version);
      printf("nr_sections=%d\n",nr_of_sect);
      for(int i=0;i<nr_of_sect;i++){
        printf("section%d: %s %d %d\n",i+1, h[i].sect_name, h[i].sect_type, h[i].sect_size );
      }
    }
  }
  //verificare extract
  if(e==1){
    extract(path,section,line);  
  }
  // verificare findall
  if(a==1){
    findAll(path);
  }
 }
return 0;
}