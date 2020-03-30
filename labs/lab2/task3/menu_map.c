#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
/* Gets a char c,  and if the char is 'q' , ends the program with exit code 0. Otherwise returns c. */
char quit(char c){
  if(c == 'q')
    exit(0);
  return c;
}

 /* Gets a char c and returns its encrypted form by adding 3 to its value. 
    If c is not between 0x20 and 0x7E it is returned unchanged
  */
char encrypt(char c){
  if(c >= 32 && c <= 126)
    return c+3;
  return c;
}

 /* Gets a char c and returns its decrypted form by reducing 3 to its value. 
    If c is not between 0x20 and 0x7E it is returned unchanged
 */
char decrypt(char c){
  if(c >= 32 && c <= 126)
    return c-3;
  return c;
}
 /* dprt prints the value of c in a decimal representation followed by a 
    new line, and returns c unchanged.
 */
char dprt(char c){
  printf("%d\n",c);
  return c;
}
/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
   by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
   the value of c unchanged.
*/
char cprt(char c){
  if(c >= 32 && c <= 126){
    printf("%c\n",c);
    return c;
  }
  printf(".\n");
  return c;
}
/* Ignores c, reads and returns a character from stdin using fgetc. */
 
char my_get(char c){
  char new = getc(stdin);
  return new;
}

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

char* map(char *array, int array_length, char (*f) (char)){
    int i;
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for(i=0;i<array_length;i++){
        *(mapped_array+i)= f(*(array+i));
    }
    return mapped_array;
}

struct fun_desc {
  char *name;
  char (*fun)(char);
};

struct fun_desc menu[] = {
                            {"Censor", &censor}, {"Encrypt", &encrypt}, {"Decrypt", &decrypt},
                            {"Print dec", &dprt}, {"Print string", &cprt}, {"Get string", &my_get},
                            {"Quit", &quit}, {"Junk",&menu}, { NULL, NULL }
                          };

int main(int argc, char **argv){
  char* carray = malloc(5);
  char c;
  carray[0] = '\0';
  char received[1];
  int i, j, choosenOption,counter = 0;

  //count functions
  while(menu[counter].name != NULL)
    counter++;
  const size_t UPPER_BOUND = counter;

  while(1){
    //function choosing
    puts("Please choose a function:");
    i = 0;
    while(menu[i].name != NULL){
      printf("%d)%s\n",i,menu[i].name);
      i++;
    }

    //recieving function
    j = 0;
    while((c=fgetc(stdin))!= '\n'){
      received[j] = c;
      j++;
    }
    choosenOption = atoi(received);

    //check the choosen is between boundaries
    printf("Option: %d\n",choosenOption);
    if(choosenOption >= 0 && choosenOption <= UPPER_BOUND){
      puts("Within bounds");
    }
    else{
      puts("Not within bounds");
      break;
    }

    // running function
    carray = map(carray,5,menu[choosenOption].fun);
    puts("DONE.\n");    
  }

  free(carray);
  return 0;
}