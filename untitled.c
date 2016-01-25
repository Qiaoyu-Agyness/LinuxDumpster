/* project1
   Qiaoyu Liao
*/

 #include <stdlib.h>
 #include <stdio.h>

char *getenv(const char *name);


int main(int argc, char *argv[]) {
  extern char **environ;	/* externally declared */
  char *match;			

  if (argc != 2) {
    fprintf(stderr,"Usage: env {var}\n");
    exit(1);
  }

  match = getenv(argv[1]);
  //if no match, set up the dumpster environmenet
  if (!match){
    printf("No match for '%s' in environment\n", argv[1]);
    char *home;
    home = getenv("HOME");
    strcpy(dumpster,home, strlen(home));
    strcat(dumpster,"/dumpster");
    setenv("DUMPSTER", dumpster,0);
    match = getenv(argv[1]);
    printf("%s=%s\n", argc[1], match);
}
	
  //if match then move on 
  else
    printf("%s=%s\n", argv[1], match);
}

