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

void usage(void){

}

int rsFile(char *file, char* dumpster){

	int result;
	char *cwd;
	char buffer[PATH_MAX + 1];
	char *file_dest = malloc(MAXSTR*(sizeof(char)));
	char *file_ind = malloc(MAXSTR*(sizeof(char)));

	cwd = getcwd(buffer,PATH_MAX + 1);
	strcpy(file_dest,cwd);
	strcat(file_dest,"/");
	strcat(file_dest,file);

	strcpy(file_ind,dumpster);
	strcat(file_ind,"/");
	strcat(file_ind,file);

	//access the complete path of file
	result = access(file_ind, R_OK);
	if(result != 0){
    	int errsv = errno;
    	perror("ERROR:");

    	//if(errsv == ENOENT){
  		//if the file do not exist
      	exit(-1);//ERRER
    	//}
  	}

  	//get the stat of the file in the dumpster
  	int ret;
  	struct stat buf;
  	ret = stat(file_ind, &buf);
  	//fail to get stat
  	if (ret != 0) {
    	perror("Failed:");
    	exit(ret);
  	}

  	//if the thing to be delete is a direcotry
  	if(S_ISDIR(buf.st_mode)){
  		mode_t temp = buf.st_mode;
  		mkdir(file_dest,temp);    

    	//open the direcotry to loop through
    	DIR *dp = opendir(file_ind);
    	if (dp == NULL) {
     		perror("open()");
      		return -1;     /* error */
    	}

    	/* Loop, reading each file and printing until done all. */
    	struct dirent *d = readdir(dp);
    	while (d) {
        	//make sure no . or .. are counted
        	if (strcmp (d->d_name, ".") || strcmp (d->d_name, "..")){
        		//new file to be recovered
        		char *newfilename = malloc(MAXSTR*sizeof(char));
        		strcat(newfilename,d->d_name);

        		//restore the file 
        		rsFile(newfilename,file_ind);
      		}
	  		//printf("%s\n", d->d_name);
      		d = readdir(dp);
    	}

    	/* Done, so close directory. */
    	if (closedir(dp) != 0)  {
    		perror("closedir()");
      		return 2;
    	}

    	int status;
    	status = rmdir(file_ind);
    	if(status){
      		perror("ERROR:");
    	}
    	return 0;     /* success of the direcotry dump */
  	}
	
	//get the partition of current working directory
	int ret_d;
	struct stat buf_de;
	ret_d = stat(cwd, &buf_de);
	//if the file is a file not a directory
	if(S_ISREG(buf.st_mode)){
		//not at the same partition
		if(buf.st_dev != buf_de.st_dev){
			printf("processing 8\n");
      		char ch;
      		FILE *source, *target;

      		source = fopen(file_ind, "r");

		      if( source == NULL ){
		        //nothing to be opened
		        exit(EXIT_FAILURE);
		      }

		      target = fopen(file_dest, "w");

		      if( target == NULL ){
		        fclose(source);
		        exit(EXIT_FAILURE);
		      }
		 
		      while( ( ch = fgetc(source) ) != EOF )
		      fputc(ch, target);
		 
		      printf("File copied successfully.\n");
		 
		      fclose(source);
		      fclose(target);
		      //file alreadied copied

		      //set permission
		      //TODO: should use chmod
		      mode_t temp = buf.st_mode; 
      		  chmod(file_dest,temp);
		      //set time changed
		      //should use something elase
		      utime(file_dest,NULL);

		      int status;
		      status = unlink(file_ind);
		      if(status){
		        perror("ERROR:");
		      }		    
		}

		//at the same partition
		else if(buf.st_dev == buf_de.st_dev){
			rename(file_ind,file_dest);
			printf("moved file %s from the dumpster to the current directory.\n",file_ind);
		}

		//shouldn't get here
		else{}
	}
	
	//can be access but not a file or a direcotry
	else{}
	//shouldn't reach here
	return -1;
}

int main(int argc, char* argv[]){

	char* dumpster;
	int hflag = 0;
	extern int optind, opterr;
	char *file[] = {""};

	dumpster = getenv("DUMPSTER");

	if(!dumpster){
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
    	    usage();
    	    break;
    	  default:
    	    break;
    	}
  	}

  	if(hflag > 0){
    	usage();
  	}

  	for (int index = optind; index < argc; index++){
   		file[i] = argv[index];
  	  	i++;
  	}

  	for (int a = 0 ; a < i;a++){
    	printf("remove file %d\n",a);
    	rsFile(file[a],dumpster);
    }



}

/*
{joe = 'Shabi'
if joe = 'Shabi'
	Printf('you are a douche
')}
*/