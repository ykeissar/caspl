#include <stdio.h>
#include <string.h>

#define	MAX_LEN 34			/* maximal input string size */
					/* enough to get 32-bit string + '\n' + null terminator */
extern void convertor(char* buf);

int main(int argc, char** argv){
  char buf[MAX_LEN ];
  while(1){
    fgets(buf, MAX_LEN, stdin);		/* get user input string */ 
    if(buf[0] == 'q')
      break;
    convertor(buf);			/* call your assembly function */
  }
  return 0;
}