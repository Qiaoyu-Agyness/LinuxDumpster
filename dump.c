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
#include <limits.h>

#define MAXSTR 256

void usage(){
	
}

int dump(char *tobedumped){

	int result;
	result = access(tobedumped,F_OK);


	//if cannot access the file
	if(result != 0){
    	int errsv = errno;
      printf("Process 1 \n");
    	perror("ERROR:");
      exit(-1);//ERRER
  	}

    //printf("process 2\n");

  	//find the stats
  	int ret;
  	struct stat buf;
  	ret = stat(tobedumped, &buf);
  	if (ret != 0) {
    	perror("Failed:");
    	exit(ret);
  	}



  	//if the file is a directory
  	if(S_ISDIR(buf.st_mode)){

  		//open the direcotry to loop through
    	DIR *dp = opendir(tobedumped);
    	if (dp == NULL) {
     		perror("open()");
      		return -1;     /* error */
    	}

    	/* Loop, reading each file and printing until done all. */
    	struct dirent *d = readdir(dp);
    	while (d) {
        	//make sure no . or .. are counted
        	if (strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0){
        		//new file to be dumped
        		char *newfilename = malloc(MAXSTR*sizeof(char));
        		strcpy(newfilename,tobedumped);
        		strcat(newfilename,"/");
        		strcat(newfilename,d->d_name);
            printf("The new file name is %s\n",newfilename);
        		//restore the file 
        		dump(newfilename);
      		}
	  		//printf("%s\n", d->d_name);
      		d = readdir(dp);
    	}

    	/* Done, so close directory. */
    	if (closedir(dp) != 0)  {
    		perror("closedir()");
      		return -1;
    	}

    	int status;
    	status = rmdir(tobedumped);
    	if(status){
      		perror("ERROR:");
      		exit(-1);
    	}
    	return 0;
  	}


  	else if(S_ISREG(buf.st_mode)){
  		int status;
    	status = unlink(tobedumped);
    	if(status){
      		perror("ERROR:");
      		exit(-1);
    	}
    	return 0;/*sucessful completion of delete this file */
  	}

  	return -1;/*shouldn't reach here */
}

int main(int argc, char* argv[]){

	char* dumpster = "";
  int hflag = 0;
	extern int optind, opterr;
	char *file[] = {""};
  const char *dumpname = "DUMPSTER";


	dumpster = getenv("DUMPSTER");
  printf("Dumpster name is %s\n",dumpster);


	if(dumpster == NULL){
		perror("NO MATCHING FOR ENVIRONMENT VARIABLE DUMPSTER, PLEASE SET UP\n");
		exit(-1);
	}


	printf("processing the argument: %d\n",optind);

  	int i = 0, c;
  	while((c = getopt(argc, argv, "h")) != EOF){
    	switch(c){
    	  case 'h':
    	    hflag++;
    	    break;
    	  case '?':
    	  	//shouldn't be entering this?
    	    usage();
    	    break;
    	  default:
    	    break;
    	}
  	}

  	if(hflag > 0){
    	usage();
  	}

  	dump(dumpster);

}