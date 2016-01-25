/* project1
   Qiaoyu Liao
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <dirent.h>

#define MAXSTR 256

char *getenv(const char *name);

void usage(void){

}

//return number about the extention it shoud get
int getExtention(char *name){
    printf("In the get extention\n" );
    char* fname = malloc(MAXSTR*sizeof(char));
    strcpy(fname,name);
    int n = 0;
      //TODO: should be access dumpster
    while(access(fname,F_OK) == 0 && n < 10){
      //file exist
      // increment n and change the file name 
      n++;
      printf("%d\n",n);
      memset(fname,0,strlen(fname));
      strcpy(fname,name);
      strcat(fname,".");
      char num = n;
      strcat(fname,&num);
    }

    if(n >= 9){
      //no more extention available, dump
      return -1;
    }

    return n;
    
}

int rmFile(char *name, char *dumpster, int rflag){

    char *filename = malloc(MAXSTR*sizeof(char));
    char *dest = malloc(MAXSTR*sizeof(char));
    char *dname, *bname;//for directory name and base name

    printf("processing 1\n");
    strcpy(filename,name);
    printf("%s\n", );("processing 10\n");
    dname = dirname(name);
    bname = basename(name);

    printf("the direcotry name is %s\n",dname);
    printf("the file name is %s\n",bname);
    strcpy(dest,dumpster);
    strcat(dest,"/");
    strcat(dest,bname);
    printf("Destination name: %s\n",dest);
    
    int result;
    result = access(filename,R_OK);

    printf("the result of accesss is %d\n",result);

    //if file don't exist
    //Report Error
    if(result != 0){
      int errsv = errno;
      perror("ERROR:");

      if(errsv == ENOENT){
        printf("processing 6\n");
        exit(-1);//ERRER
      }
    }

    //get the extention
    int count;
    printf("processing 2\n");      
    count = getExtention(dest);
    if(count == -1){
        //ERROR
        exit(-1);
    }
    printf("extention number %d",count);
    if(count != 0){
      char* myChar = malloc(10*sizeof(char));
      sprintf(myChar, "%d", count);
      strcat(dest,".");
      strcat(dest,myChar);
    }

    printf("the final destination of the file is %s\n",dest);

    

    printf("processing 4\n");
    int ret;
    struct stat buf;
    ret = stat(filename, &buf);

    //fail to get stat
    if (ret != 0) {
      perror("Failed:");
      exit(ret);
    }

    printf("processing 5\n");
    //find out the dumpster partition
    int ret_d;
    struct stat buf_d;
    ret_d = stat(dumpster,&buf_d);


    //file is a directory
    if(S_ISDIR(buf.st_mode)){

      printf("processing 7\n");
      if(rflag == 0){
        perror("ERROR: deleting a not recurse directory");
        exit(-1);
      }

      mode_t temp = buf.st_mode;
      mkdir(dest,temp);    

      //open the direcotry to loop through
      DIR *dp = opendir(filename);
      if (dp == NULL) {
        perror("open()");
        exit(-1);     /* error */
      }

      /* Loop, reading each file and printing until done all. */
      struct dirent *d = readdir(dp);
      while (d) {
          //make sure no . or .. are counted
          if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0){
            //new file to be dumped
            printf("the current file is %s",d->d_name);
            char *newfilename = malloc(MAXSTR*sizeof(char));
            strcpy(newfilename,filename);
            strcat(newfilename,"/");
            strcat(newfilename,d->d_name);

            //dump the file 
            rmFile(newfilename,dest,1);
          }
        d = readdir(dp);
      }

      /* Done, so close directory. */
      if (closedir(dp) != 0)  {
        perror("closedir()");
        return 2;
      }

      int status;
      status = rmdir(filename);
      if(status){
        perror("ERROR:");
      }
      return 0;     /* success of the direcotry dump */

    }


    //it is a file not a dirctory
    if(S_ISREG(buf.st_mode)){
      //if they are not in the same directory
      if(buf.st_dev != buf_d.st_dev){
        printf("processing 8\n");
        char ch;
        FILE *source, *target;

        source = fopen(filename, "r");

        if( source == NULL ){
          //nothing to be opened
          exit(EXIT_FAILURE);
        }

        target = fopen(dest, "w");

        if( target == NULL ){
          fclose(source);
          exit(EXIT_FAILURE);
        }
   
        while( ( ch = fgetc(source) ) != EOF )
        fputc(ch, target);
   
        printf("File copied successfully.\n");
   
        fclose(source);
        fclose(target);
        
        //set the access permissions
        mode_t temp = buf.st_mode; 
        chmod(dest,temp);
        //set time changed
        utime(dest,NULL);

        int status;
        status = unlink(filename);
        if(status){
          perror("ERROR:");
        }
      }

      //if file is on the same partition
      else{
        printf("processing 9\n");
        rename(filename,dest);
        printf("moved file %s to the dumpster %s\n",bname, dest);
      }
      //unlink old file 
    }
    
  return -1;
}







//TODO: add parsing string
int main(int argc, char *argv[]) {
  extern char **environ;	/* externally declared */
  char *match;
  char *file[] = {""};	
  char *dumpster = malloc(256*sizeof(char));
  int fflag = 0, hflag = 0, rflag = 0, errflag = 0;
  extern int optind, opterr;

  if (argc < 2) {
    fprintf(stderr,"Usage: env {var}\n");
    exit(1);
  }

  //setup dumpster environment
  match = getenv("DUMPSTER");
  //if no match, set up the dumpster environmenet
  if (!match){
    printf("No match for '%s' in environment\n", "DUMPSTER");
    char *home;  
    home = getenv("HOME");   
    strcpy(dumpster,getenv("HOME"));
    strcat(dumpster,"/dumpster");
    mkdir(dumpster,0755);
    setenv("DUMPSTER", dumpster,0);
    match = getenv("DUMPSTER");
    /*
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    strcpy(dumpster,cwd);
    strcat(dumpster,"/dumpster");
    setenv("DUMPSTER",dumpster,0);
    */
    printf("%s=%s\n", "DUMPSTER", match);
    }
  
  //if match then move on 
  else{
    printf("%s=%s\n", "DUMPSTER", match);
  }

  printf("processing the argument: %d\n",optind);

  int i = 0, c;
  while((c = getopt(argc, argv, "fhr")) != EOF){
    switch(c){
      case 'f':
        fflag++;
        break;
      case 'h':
        hflag++;
        break;
      case 'r':
        rflag++;
        break;
      case '?':
        usage();
        break;
      default:
        break;
    }
  }

  int index;
  for (index = optind; index < argc; index++){
    file[i] = argv[index];
    i++;
  }
  printf("File name:%s\n",file[0]);

  if(hflag > 0){
    usage();
  }

  if(fflag > 0){
    for(int a = 0; a < i; a++){
      remove(file[a]);
    }
  }

  printf("processing 1\n");
  for (int a = 0 ; a < i;a++){
    printf("remove file %d\n",a);
    rmFile(file[a],dumpster,rflag);
    }

}

