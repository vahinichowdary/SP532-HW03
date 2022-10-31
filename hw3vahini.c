#include <stdio.h>

#include <stdlib.h>

#include <dirent.h>

#include <string.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <unistd.h>

#include <time.h>

#include <sys/wait.h>


char * filetype(unsigned char type) {
  char * str;
  switch (type) {
  case DT_BLK:
    str = "block device";
    break;
  case DT_CHR:
    str = "character device";
    break;
  case DT_DIR:
    str = "directory";
    break;
  case DT_FIFO:
    str = "named pipe (FIFO)";
    break;
  case DT_LNK:
    str = "symbolic link";
    break;
  case DT_REG:
    str = "regular file";
    break;
  case DT_SOCK:
    str = "UNIX domain socket";
    break;
  case DT_UNKNOWN:
    str = "unknown file type";
    break;
  default:
    str = "UNKNOWN";
  }
  return str;
}

/* FUNCTION POINTER */
int fun(int a) {
  return a;
}

/* For Regular */
void recursive(char * basePath,
  const int root) {
  int i;
  char path[1000];
  struct dirent * dp;
  DIR * dir = opendir(basePath);

  if (!dir)
    return;

  while ((dp = readdir(dir)) != NULL) {
    if (strcmp(dp -> d_name, ".") != 0 && strcmp(dp -> d_name, "..") != 0) {
      for (i = 0; i < root; i++) {
        if (i % 2 == 0 || i == 0)
          printf("    ");
        else
          printf("    ");

      }

      printf("%s (%s)\n", dp -> d_name, filetype(dp -> d_type));

      strcpy(path, basePath);
      strcat(path, "/");
      strcat(path, dp -> d_name);
      recursive(path, root + 2);
    }
  }

  closedir(dir);
}

/* For -S */
void recursiveS(char * basePath,
  const int root) {
  int i;
  char path[1000];
  struct dirent * dp;
  DIR * dir = opendir(basePath);
  struct stat buf;
  char * duplicatePath = (char * ) malloc(2000);
  if (!dir)
    return;

  while ((dp = readdir(dir)) != NULL) {
    strcpy(duplicatePath, basePath);
    duplicatePath = strcat(duplicatePath, "/");
    duplicatePath = strcat(duplicatePath, dp -> d_name);
    stat(duplicatePath, & buf);
    int n = buf.st_mode;
    int bcount = 0;
    while (n != 0) {
      n /= 10;
      ++bcount;
    }
    char * filemode = (char * ) malloc(bcount + 2);
    char * filepermission = (char * ) malloc(11);
    snprintf(filemode, bcount + 2, "%o", buf.st_mode);
    if (dp -> d_type == DT_DIR) {
      strcpy(filepermission, "d");
    } else {
      strcpy(filepermission, "-");
    }
    n = 2;
    while (n >= 0) {
      switch (filemode[strlen(filemode) - (n + 1)]) {
      case '0':
        strcat(filepermission, "---");
        break;
      case '1':
        strcat(filepermission, "--x");
        break;
      case '2':
        strcat(filepermission, "-w-");
        break;
      case '3':
        strcat(filepermission, "-wx");
        break;
      case '4':
        strcat(filepermission, "r--");
        break;
      case '5':
        strcat(filepermission, "r-x");
        break;
      case '6':
        strcat(filepermission, "rw-");
        break;
      case '7':
        strcat(filepermission, "rwx");
        break;
      }
      n--;
    }
    if (strcmp(dp -> d_name, ".") != 0 && strcmp(dp -> d_name, "..") != 0) {
      for (i = 0; i < root; i++) {
        if (i % 2 == 0 || i == 0)
          printf("    ");
        else
          printf("    ");

      }

      printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
      recursiveS(duplicatePath, root + 2);
    }
  }

  closedir(dir);
}

/* For -s */
int recursives(DIR * parentDir, char * path, int size) {
  struct stat buf;
  struct dirent * dirent;
  int count = 1;
  char * duplicatePath = (char * ) malloc(2000);

  parentDir = opendir(path);
  if (parentDir == NULL) {
    printf("Error opening directory '%s'\n", dirent -> d_name);
    exit(-1);
  }
  while ((dirent = readdir(parentDir)) != NULL) {
    strcpy(duplicatePath, path);
    duplicatePath = strcat(duplicatePath, "/");
    duplicatePath = strcat(duplicatePath, dirent -> d_name);
    lstat(duplicatePath, & buf);
    if (dirent -> d_type == DT_DIR) {
      if (strcmp(dirent -> d_name, ".") == 0 || (strcmp(dirent -> d_name, "..") == 0)) {
        continue;
      }
      printf("[%d] %s\n", count, dirent -> d_name);
      count++;
      recursives(parentDir, duplicatePath, size);
    }
    if (dirent -> d_type != DT_DIR) {
      lstat(dirent -> d_name, & buf);
      if (size >= (long long) buf.st_size) {
        printf("    ");
        printf("[%d] %s (%lld)\n", count, dirent -> d_name, (long long) buf.st_size);
        count++;
      }
    }
  }
  closedir(parentDir);
  return 0;
}

/* for -f */
int recursivef(DIR * parentDir, char * path, char * sub, int sub1) {
  struct dirent * dirent;
  int count = 1;
  char * duplicatePath = (char * ) malloc(2000);
  char * contain = (char * ) malloc(2000);

  parentDir = opendir(path);
  if (parentDir == NULL) {
    printf("Error opening directory '%s'\n", dirent -> d_name);
    exit(-1);
  }
  while ((dirent = readdir(parentDir)) != NULL && sub1 >= 0) {
    if (dirent -> d_type == DT_DIR) {
      if (strcmp(dirent -> d_name, ".") == 0 || (strcmp(dirent -> d_name, "..") == 0)) {
        continue;
      }
      strcpy(duplicatePath, path);
      duplicatePath = strcat(duplicatePath, "/");
      duplicatePath = strcat(duplicatePath, dirent -> d_name);
      contain = strstr(dirent -> d_name, sub);
      if (contain) {
        printf("[%d] %s\n", count, dirent -> d_name);
        count++;
      }
      recursivef(parentDir, duplicatePath, sub, sub1 - 1);
    }
    if (dirent -> d_type != DT_DIR) {
      contain = strstr(dirent -> d_name, sub);
      if (contain) {
        printf("    ");
        printf("[%d] %s\n", count, dirent -> d_name);
        count++;
      }
    }
  }
  closedir(parentDir);
  return 0;
}

int tre;
char * aargs[100]={NULL};
char ccdm[128] = "";
int k =0;
void allcon(char * basePath, int size, char * sub, int sub1,
  const int root, int per, int siz, int fil, int tab, char * otab, int ee, char *command[100]) {
  int i,j;
  char path[1000];
  int status;
  struct dirent * dp;
  DIR * dir = opendir(basePath);
  struct stat buf;
  char * duplicatePath = (char * ) malloc(2000);
  char * tpath = (char * ) malloc(2000);
  char * fulpath = (char * ) malloc(2000);
  char * contain = (char * ) malloc(2000);
  char filss[128] = "*.";
//   char ccdm[128] = "/bin/";
//   char * aargs[100];
  if (!dir)
    return;

  while ((dp = readdir(dir)) != NULL) {
    if (sub1 >= 0) {
      strcpy(duplicatePath, basePath);
      duplicatePath = strcat(duplicatePath, "/");
      duplicatePath = strcat(duplicatePath, dp -> d_name);
      stat(duplicatePath, & buf);
      if (fil == 1) {
        contain = strstr(dp -> d_name, sub);
      }
      int n = buf.st_mode;
      int bcount = 0;
      if (fil == 0) {
        contain = strstr(dp -> d_name, sub);
      }
      while (n != 0) {
        n /= 10;
        ++bcount;
      }
      char * filemode = (char * ) malloc(bcount + 2);
      char * filepermission = (char * ) malloc(11);
      snprintf(filemode, bcount + 2, "%o", buf.st_mode);
      if (dp -> d_type == DT_DIR) {
        strcpy(filepermission, "d");
      } else {
        strcpy(filepermission, "-");
      }
      n = 2;
      while (n >= 0) {
        switch (filemode[strlen(filemode) - (n + 1)]) {
        case '0':
          strcat(filepermission, "---");
          break;
        case '1':
          strcat(filepermission, "--x");
          break;
        case '2':
          strcat(filepermission, "-w-");
          break;
        case '3':
          strcat(filepermission, "-wx");
          break;
        case '4':
          strcat(filepermission, "r--");
          break;
        case '5':
          strcat(filepermission, "r-x");
          break;
        case '6':
          strcat(filepermission, "rw-");
          break;
        case '7':
          strcat(filepermission, "rwx");
          break;
        }
        n--;
      }
      if (strcmp(dp -> d_name, ".") != 0 && strcmp(dp -> d_name, "..") != 0) {

        if (ee == 1) {
          if (fil == 1 && siz == 1) {
            if (dp -> d_type == DT_DIR) {
              printf("%s\n", dp -> d_name);
            } else {
              if (size >= (long long) buf.st_size) {
                if (contain) {
                  char * args[40];
                  char cdm[128] = "/bin/";
                  strcat(cdm, command[0]);
		  for(j=0;command[j]!=NULL;j++)
		  {
			if(j==0)
				args[j] = cdm;	
			else
				args[j] = command[j];
		  }
                 
		  args[j++] = duplicatePath;
		  args[j++] = NULL;
                  if (fork() == 0)
                    execv(cdm, args); // child: call execv with the path and the args
                  else
                    wait( & status);
                  // printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
                }
              }
            }
            //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 1, 0, "", 1, strs, argss, namfil);
	      allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 1, 0, "", 1, command);

          } else if (siz == 1) {

            if (dp -> d_type == DT_DIR) {
              printf("%s\n", dp -> d_name);
            } else {
		
              if (size >= (long long) buf.st_size) {
                char * args[40];
                char cdm[128] = "/bin/";
                
				
		strcat(cdm, command[0]);
		  for(j=0;command[j]!=NULL;j++)
		  {
			if(j==0)
				args[j] = cdm;	
			else
				args[j] = command[j];
		  }
		  args[j++] = duplicatePath;
		  args[j++] = NULL;
		
                if (fork() == 0)
                  execv(cdm, args); // child: call execv with the path and the args
                else
                  wait( & status);
                // printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
              }
            }
            //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 0, 0, "", 1, strs, argss, namfil);
	      allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 0, 0, "", 1, command);


          } else if (fil == 1) {
            if (dp -> d_type == DT_DIR) {
              printf("%s\n", dp -> d_name);
            } else {
              if (contain) {
                char * args[40];
                char cdm[128] = "/bin/";
                
		strcat(cdm, command[0]);
		  for(j=0;command[j]!=NULL;j++)
		  {
			if(j==0)
				args[j] = cdm;	
			else
				args[j] = command[j];
		  }
		  args[j++] = duplicatePath;
		  args[j++] = NULL;
                if (fork() == 0)
                  execv(cdm, args); // child: call execv with the path and the args
                else
                  wait( & status);
              }
            }
            //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 0, 1, 0, "", 1, strs, argss, namfil);
	      allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 0, 1, 0, "", 1, command);


          }
        }
        else if (ee == 2) {
        if (siz == 1 && fil == 1) { 
     	    strcpy(ccdm,"/bin/");
		strcat(ccdm, command[0]);
		  for(j=0;command[j]!=NULL;j++)
		  {
			if(j==0)
				aargs[j] = ccdm;	
			else
				aargs[j] = command[j];
		  }
         
            if (dp -> d_type == DT_DIR) {
              for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s\n", dp -> d_name);
	    if(strstr(dp -> d_name, sub)){
	    aargs[tre] = strdup(duplicatePath);
                tre = tre + 1;
		}
            } else { 
              if (size >= (long long) buf.st_size && contain) {
               	  
                for (i = 0; i < root; i++) {
                  if (i % 2 == 0 || i == 0)
                    printf("    ");
                  else
                    printf("    ");
                }
                printf("%s \n", dp -> d_name);
		aargs[tre] = strdup(duplicatePath);
                tre = tre + 1;
                k = 1;
              }
            }
            //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 1, 0, "", 2, strs, argss, namfil);
	      allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 1, 0, "", 2, command);

          }
        else if (siz == 1) {  
	    strcpy(ccdm,"/bin/");
		strcat(ccdm, command[0]);
		  for(j=0;command[j]!=NULL;j++)
		  {
			if(j==0)
				aargs[j] = ccdm;	
			else
				aargs[j] = command[j];
		  }        
            if (dp -> d_type == DT_DIR) {
              for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s\n", dp -> d_name);	
	    aargs[tre] = strdup(duplicatePath);
                tre = tre + 1;

            } else { 
              if (size >= (long long) buf.st_size) {
                

                for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s\n", dp -> d_name);
                aargs[tre] = strdup(duplicatePath);
                tre = tre + 1;
                k = 1;
              }
            }
            //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 0, 0, "", 2, strs, argss, namfil);
	      allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 0, 0, "", 2, command);

          }
        else if (fil == 1) {
	    strcpy(ccdm,"/bin/");
		strcat(ccdm, command[0]);
		  for(j=0;command[j]!=NULL;j++)
		  {
			if(j==0)
				aargs[j] = ccdm;	
			else
				aargs[j] = command[j];
		  }          
            if (dp -> d_type == DT_DIR) {
              for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s \n", dp -> d_name);
            if(strstr(dp -> d_name, sub))
	    {	   
	    	aargs[tre] = strdup(duplicatePath);
                tre = tre + 1;
	    }
            } else { 
              if (contain) {
                

                for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s \n", dp -> d_name);
                strcpy(tpath,basePath);
                strcat(tpath, dp -> d_name);
                aargs[tre] = strdup(duplicatePath);
                tre = tre + 1;
                k = 1;
              }
            }
            //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 0, 1, 0, "", 2, strs, argss, namfil);
	      allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 0, 1, 0, "", 2, command);

          }
        } else if (per == 1 && fil == 1 && siz == 1) {
          if (dp -> d_type == DT_DIR) {
            for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
          } else {
            if (size >= (long long) buf.st_size) {
              if (contain) {
                for (i = 0; i < root; i++) {
                  if (i % 2 == 0 || i == 0)
                    printf("    ");
                  else
                    printf("    ");
                }
                printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
              }
            }
          }
          //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 1, 1, 1, 0, "", 0, strs, argss, namfil);
	    allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 1, 1, 1, 0, "", 0, command);

        } else if (per == 1 && siz == 1) {
          if (dp -> d_type == DT_DIR) {
            for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
          } else {
            if (size >= (long long) buf.st_size) {
              for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
            }
          }
          //allcon(duplicatePath, size, "", 1, root + 2, 1, 1, 0, 0, "", 0, strs, argss, namfil);
	    allcon(duplicatePath, size, "", 1, root + 2, 1, 1, 0, 0, "", 0, command);

        } else if (per == 1 && fil == 1) {
          if (dp -> d_type == DT_DIR) {
            for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
          } else {
            if (contain) {
              for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
            }
          }
          //allcon(duplicatePath, 0, sub, sub1 - 1, root + 2, 1, 0, 1, 0, "", 0, strs, argss, namfil);
	    allcon(duplicatePath, 0, sub, sub1 - 1, root + 2, 1, 0, 1, 0, "", 0, command);

        } else if (siz == 1 && fil == 1) {
          if (dp -> d_type == DT_DIR) {
            for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s\n", dp -> d_name);
          } else {
            if (size >= (long long) buf.st_size) {
              if (contain) {
                for (i = 0; i < root; i++) {
                  if (i % 2 == 0 || i == 0)
                    printf("    ");
                  else
                    printf("    ");
                }
                printf("%s \n", dp -> d_name);
              }
            }
          }
          //allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 1, 0, "", 0, strs, argss, namfil);
	    allcon(duplicatePath, size, sub, sub1 - 1, root + 2, 0, 1, 1, 0, "", 0, command);

        } else if (per == 1) {
          for (i = 0; i < root; i++) {
            if (i % 2 == 0 || i == 0)
              printf("    ");
            else
              printf("    ");
          }
          printf("%s (%s) (%lld) %s %s\n", dp -> d_name, filetype(dp -> d_type), (long long) buf.st_size, filepermission, ctime( & buf.st_atime));
          //allcon(duplicatePath, 0, "", 1, root + 2, 1, 0, 0, 0, "", 0, strs, argss, namfil);
	    allcon(duplicatePath, 0, "", 1, root + 2, 1, 0, 0, 0, "", 0, command);

        } else if (siz == 1) {
          if (dp -> d_type == DT_DIR) {
            for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s\n", dp -> d_name);
          } else {
            if (size >= (long long) buf.st_size) {
              for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s\n", dp -> d_name);
            }
          }
          allcon(duplicatePath, size, "", 1, root + 2, 0, 1, 0, 0, "", 0, command);
        } else if (fil == 1) {

          if (dp -> d_type == DT_DIR) {
            for (i = 0; i < root; i++) {
              if (i % 2 == 0 || i == 0)
                printf("    ");
              else
                printf("    ");
            }
            printf("%s \n", dp -> d_name);
          } else {
            if (contain) {
              for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s \n", dp -> d_name);
            }
          }
          allcon(duplicatePath, 0, sub, sub1 - 1, root + 2, 0, 0, 1, 0, "", 0, command);
        } else if (tab == 1) {
          if ( * otab == 'f') {
            if (dp -> d_type != DT_DIR) {
              for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s \n", dp -> d_name);
            }
            allcon(duplicatePath, 0, "", 1, root + 2, 0, 0, 0, 1, otab, 0, command);
          } else if ( * otab == 'd') {
            if (dp -> d_type == DT_DIR) {
              for (i = 0; i < root; i++) {
                if (i % 2 == 0 || i == 0)
                  printf("    ");
                else
                  printf("    ");
              }
              printf("%s \n", dp -> d_name);
            }
            allcon(duplicatePath, 0, sub, sub1 - 1, root + 2, 0, 0, 0, 1, otab, 0, command);
          } else {
            printf("Unkown argument for E/e");
          }
        } else {
          for (i = 0; i < root; i++) {
            if (i % 2 == 0 || i == 0)
              printf("    ");
            else
              printf("    ");
          }
          printf("%s \n", dp -> d_name);
          allcon(duplicatePath, 0, "", 1, root + 2, 0, 0, 0, 0, "", 0, command);
        }
      }
    }
  }
  closedir(dir);
}

/* for -t */
int recursivet(DIR * parentDir, char * path, char * sub) {
  struct dirent * dirent;
  int count = 1;
  char * duplicatePath = (char * ) malloc(2000);
  char * contain = (char * ) malloc(2000);

  parentDir = opendir(path);
  if (parentDir == NULL) {
    printf("Error opening directory '%s'\n", dirent -> d_name);
    exit(-1);
  }
  while ((dirent = readdir(parentDir)) != NULL) {
    if (dirent -> d_type == DT_DIR) {
      if (strcmp(dirent -> d_name, ".") == 0 || (strcmp(dirent -> d_name, "..") == 0)) {
        continue;
      }
      strcpy(duplicatePath, path);
      duplicatePath = strcat(duplicatePath, "/");
      duplicatePath = strcat(duplicatePath, dirent -> d_name);
      if (strcmp(sub, "d") == 0) {
        printf("dir: [%d] %s\n", count, dirent -> d_name);
        count++;
      }
      recursivet(parentDir, duplicatePath, sub);
    }
    if (dirent -> d_type != DT_DIR && strcmp(sub, "f") == 0) {
      {
        printf("    ");
        printf("file: [%d] %s\n", count, dirent -> d_name);
        count++;
      }
    }
  }
  closedir(parentDir);
  return 0;
}

int main(int argc, char * argv[]) {
  DIR * parentDir;
  char * path;
  char sub[1000];
  int opt, size, sub1 = 9999;
  int sizearg;
  char * farg;
  int per, siz, fil, ee, tab, i=0, et = 0;
  char strs[128], est[1000];
  char pts[1000];
  pid_t pid;
  int status;
  char * cmds;
  char *commandgiven[100];
  char * argss;
  char * namfil;

  while ((opt = getopt(argc, argv, "Ss:f:e:E: t:")) != -1) {
    switch (opt) {
    case 'S':
      per = 1;
      break;
    case 's':
      siz = 1;
      size = atoi(optarg);
      break;
    case 'f':
      fil = 1;
      strcpy(sub, optarg);
        // for (; optind < argc; optind++) {
        //   sub1 = atoi(argv[optind]);
        // }
        // printf("%d\n",sub1);
      break;
    case 'E':
      ee = 2;
      strcpy(est, optarg);
      cmds = strtok(est, " ");
      while(cmds!=NULL)
      {
	commandgiven[i] = cmds;
	cmds = strtok(NULL," ");
	i++;
      }
      tre = i;
      break;
    case 'e':
      ee = 1;
      strcpy(strs, optarg);
      cmds = strtok(strs, " ");
      while(cmds!=NULL)
      {
	commandgiven[i] = cmds;
	cmds = strtok(NULL," ");
	i++;
      }
      
      break;
    case 't':
      tab = 1;
      strcpy(pts, optarg);
      break;
    }

  }
  parentDir = opendir(".");
  path = ".";
  int cc = 0;
  int arsr[100] = { 0 };
  for (; optind < argc; optind++) {
      arsr[cc]=optind;
      cc = cc +1;
  }
  //    USED FUCNTION POINTER HERE 
  int( * fun_ptr)(int) = & fun;
  if (argc < ( * fun_ptr)(2)) {
    parentDir = opendir(".");
    path = ".";
  } else {
      if (fil == 1){
          if (cc == 2){
              sub1 =atoi(argv[arsr[0]]);
              parentDir = opendir(argv[arsr[1]]);
              path = argv[arsr[1]];
          }
          else if (cc == 1){
              if(strstr(argv[arsr[0]], "/") != NULL){
                  sub1=5;
                  parentDir = opendir(argv[arsr[0]]);
                  path = argv[arsr[0]];
              }
              else{
                  sub1=atoi(argv[arsr[0]]);
              }
          }
          else if(cc ==0) {
              sub1 =5;
          }
          else {
          printf("error: unexpected arguments\n");
          exit(-1);
          }
      }
      else if (fil == 0){
        if (cc == 1){
            parentDir = opendir(argv[arsr[0]]);
                  path = argv[arsr[0]];
        }
      }
  }
  if (parentDir == NULL) {
    printf("Error opening directory '%s'\n", argv[1]);
    exit(-1);
  }
  printf("S:%d s:%d f:%d ee:%d\n",per,siz,fil,ee);
  //allcon(path, size, sub, sub1, 0, per, siz, fil, tab, pts, ee, cmds, argss, namfil);
    allcon(path, size, sub, sub1, 0, per, siz, fil, tab, pts, ee, commandgiven);

    if(k == 1 && ee ==2){
      aargs[tre]=NULL;
   
      if (fork() == 0)
        execv(ccdm, aargs); // child: call execv with the path and the args
       else
        wait( & status);
  }
  return 0;
}
