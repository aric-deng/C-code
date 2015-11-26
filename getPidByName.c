static char cPid[16];
char *getPidByName(char * task_name)
{
  if(task_name == NULL){
	printf("dhs task_name null !! \n");
	return NULL;
  }
	
  FILE *fp;
#define CMD_LEN	128  
  char cmdBuf[CMD_LEN]={0};

  /* Open the command for reading. */
  snprintf(cmdBuf, CMD_LEN-1, "pidof %s", task_name);
  //printf("dhs cmd:%s \n", cmdBuf);
  fp = popen(cmdBuf, "r");
  if (fp == NULL) {
    printf("=== dhs Failed to run command pidof \n" );
    return NULL;
  }

  /* Read the output a line - output it. */
  if (fgets(cPid, sizeof(cPid)-1, fp) != NULL) {
    printf("cPid: %s \n", cPid);
    return cPid;
  }
  return NULL;
}