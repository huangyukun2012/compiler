#include<stdio.h>
#include<ctype.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define MAXTOKENLEN 64
#define MAXTOKENS 100
#define MAXDECLLEN 200

struct token{
  char type;
  char string[MAXTOKENLEN];
};
struct decl{
  char string[MAXDECLLEN];
  int current_location;
};

struct decl mydecl;
int top=-1;
struct token this;//the current token
struct token stack[MAXTOKENS];//the tokens before the first identifer
#define IDENTIFIER 'I'
#define QUALIFIER 'Q'
#define TYPE 'T'
#define pop stack[top--]
#define push(s) stack[++top]=s
//#define debug 
char classify_string(const char *string);


/*
 * init the value of mydecl*/
int init_parse_decl(){
  if( fgets(mydecl.string,MAXDECLLEN,stdin) == NULL){
    perror("can not read from the stdin");
  }
  if(strlen(mydecl.string)==MAXDECLLEN-1)
    printf("you input is too long, you may get an error!");
  mydecl.current_location=0;

  #ifdef debug
  printf("init:we get last char of mydecl:%c,%d\n",mydecl.string[strlen(mydecl.string)],mydecl.string[strlen(mydecl.string)]);
  #endif
}


/*
  get a token from the current string,value the "this" and move the pointer
notice: we may get a '\0' at the end of the string
  */
int gettoken(){
  char *ch_pointer=this.string;
  //leave out the blank
  while(mydecl.string[mydecl.current_location]==' '){
    mydecl.current_location++;
  }

  *ch_pointer=mydecl.string[mydecl.current_location];
  if(isalnum(*ch_pointer)){
  	while(isalnum(*ch_pointer)){
	  mydecl.current_location++;
	  ch_pointer++;
	  *ch_pointer=mydecl.string[mydecl.current_location];
	}

	*ch_pointer='\0';
	this.type=classify_string(this.string );//indentifier,qualifier,type
	#ifdef debug 
	printf("we get token:%s ",this.string );
	#endif
	return 1;
  }

  switch(*ch_pointer){
    case '*':
    case '(':
    case ')':
    case '[':
    case ']':
	      this.type=*ch_pointer;
	      this.string[1]='\0';
	      mydecl.current_location++;
	      #ifdef debug 
	      printf("we get token:%s ",this.string );
	      #endif
	      break;
    case '\n':
    case '\0':this.type='\0'; 
	      strcpy(this.string,"then end");
	      return 0;

   default :
	      printf("we can not read this indentifier %d\n",*ch_pointer);
	      parse_error();
  }
  return 1;
}

char classify_string(const char *string){
  if(isspace(*string))
    return '\0';
  if(!strcmp(string,"const")) 
   return QUALIFIER;
  if(!strcmp(string,"volatile")) 
   return QUALIFIER;
  

  if(!strcmp(string,"void")) 
    return TYPE;
  if(!strcmp(string,"char")) 
    return TYPE;
  if(!strcmp(string,"signed")) 
    return TYPE;
  if(!strcmp(string,"unsigned")) 
    return TYPE;
  if(!strcmp(string,"short")) 
    return TYPE;
  if(!strcmp(string,"int")) 
    return TYPE;
  if(!strcmp(string,"long")) 
    return TYPE;
  if(!strcmp(string,"float")) 
    return TYPE;
  if(!strcmp(string,"struct")) 
    return TYPE;
  if(!strcmp(string,"union")) 
    return TYPE;
  if(!strcmp(string,"enum")) 
    return TYPE;

  return IDENTIFIER ;
}

/*follow the dec string until the first identifier,push token to a stack*/
int read_to_first_identifier(){
  if(gettoken()==0){
    printf("readtofirst:missing the identifier,please put in your string...\n");
    return 0;
  }
  //the token is a struct with member type and string
  while(this.type!=IDENTIFIER)
  {
    push(this);//a stack with element of token
    if(gettoken()==0){
      printf("readtofirst:missing the identifier\n");
      return 0;
    }
  }
  printf("%s is ", this.string);
  
  return 1;
}



/*
deal with the next token in diff ways according to the token type
we just go to right
1) "(":deal in a function way,deal_with_declaration,
2) "[":deal in a array way,deal_with_declaration,
3) ")":deal with a pointer way,deal_with_declaration
4) NULL:move_left,
*/
int  deal_with_declaration()
{
  if(gettoken()==0){
    move_left();
    return 1;
   }  
  //
  switch(this.type){
    case '(': deal_with_function();break;
    case ')': deal_with_pointer();break;
    case '[': deal_with_array();break;
    default : printf("there should not be a %c after the identifier\n",this.type );parse_error();
  }
  deal_with_declaration();
}


/*the current token is [,the content in the '[]' may be digtal,al,and other*/
int deal_with_array(){
  printf("array of ");
  while(this.type!=']'){
    gettoken();//we may get an "]" or digital
    if(isdigit(this.string[0])){
      printf("%d  ", atoi(this.string));
      //the next token must be ]
      gettoken();
      if(this.type!=']'){
	printf("the array miss the ']'before %s",this.string);
	parse_error();
      }
    }
  }
  return 0;
}

int parse_error(){
	printf("press any key to exit\n");
	getchar();
	exit(0);
}

/*the current token is ')' */
int deal_with_pointer(){
  while(stack[top].type=='*'){
    printf("pointer pointing to \n");
    pop;
  }
  if(stack[top].type!='('){
    printf(" missing an '(' after %s\n",stack[top].string );
    parse_error();
  }
  else{
    pop;
    return 1;

  }
}

/*the current token is '('*/
int deal_with_function(){

  while(this.type!=')'){
    gettoken();
    if(this.type=='\0'){
      printf(" missing an ')' before %s",this.string);
      parse_error();
    }
  }
  printf(" function returning ");
  return 1;
}

int move_left(){
  while(top>=0){
    switch (stack[top].type){
      case '*': printf(" pointer pointing to ");break;
      case  QUALIFIER: printf(" %s ",stack[top].string );break;
      case  TYPE: printf(" %s ",stack[top].string );break;
      default :printf("there is someting wrong about %s",stack[top].string);parse_error();break;
    }
    top--;
  }
}

int main(int argc, char *argv[])
{
  init_parse_decl();
  read_to_first_identifier();
  deal_with_declaration();
  printf("\n");
    return 0;
}
