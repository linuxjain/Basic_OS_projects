#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define LINE_SIZE 8
#define INPUT_BUFFER_SIZE 100


void delete_node();
int print_value = 0;  // to avoid cursor to indicate first character each time.
char *file_name;

char *argvv[10];



typedef struct nodes
{
	char line[LINE_SIZE];
	int line_no;
	struct nodes  *next;
	struct nodes  *previous;

}node;

typedef struct files
{
	node *HEAD;
	node *TAIL;
	int lines;
	node *current_line;
	int current_position;
	char current_element;

}file;



file file_buffer;
int count;


void current()
{
	printf("**************************************************************************************************************************");
	printf("\n");

	printf("Current Line -> %d\n",file_buffer.current_line->line_no);
	printf("Current Position -> %d\n",file_buffer.current_position);
	printf("Current Element -> %x\n",file_buffer.current_line->line[file_buffer.current_position-1]);

	printf("\n");
	printf("**************************************************************************************************************************");
	printf("\n");
}


/*this function will read content of a binary file*/

void read(file *file_buffer_ptr,char *buffer_ptr)
{
	node *newnode;

	newnode = (node *)malloc(sizeof(node));

	if(newnode == NULL)
	{
		printf("Not enough memory.\n");
		exit(0);
	}

	strcpy(newnode->line,buffer_ptr);
	newnode->next = NULL;
	file_buffer_ptr->lines = file_buffer_ptr->lines + 1;
	newnode->line_no=file_buffer_ptr->lines;
	
	if(file_buffer_ptr->HEAD==NULL)
	{
		newnode->previous = NULL;
		file_buffer_ptr->HEAD=newnode;
		file_buffer_ptr->TAIL=newnode;
	}
	else
	{
		newnode->previous = file_buffer_ptr->TAIL;
		file_buffer_ptr->TAIL->next=newnode;
		file_buffer_ptr->TAIL=newnode;
	}

}

/*parse function,it will convert command line arguments in tokens*/

void parse(char *input_buffer_ptr,char **argv_ptr)
{
	count++;

	while(*input_buffer_ptr != '\0')
	{
		while(*input_buffer_ptr == '\t' || *input_buffer_ptr == ' ' || *input_buffer_ptr == '\n' )
		{
			*input_buffer_ptr = '\0';
			input_buffer_ptr++;
		}

		*argv_ptr = input_buffer_ptr;
		count++;
		argv_ptr++;
		

		while(*input_buffer_ptr != '\t' && *input_buffer_ptr != '\0' && *input_buffer_ptr != '\n' && *input_buffer_ptr != ' ')
		{
			input_buffer_ptr++;
		}


	}

	*argv_ptr = '\0';
}

/*Function to show the content of a file.*/

void print()
{
	node *ptr;
	int i,j;

	ptr = file_buffer.HEAD;

	for(i=1;i<=file_buffer.lines;i++)
	{
		printf(" <  %d  >    ",ptr->line_no);
		printf(" <  %p  >    ",ptr->line);

		for(j=0;j<LINE_SIZE;j++)
		{
			printf("%x ",ptr->line[j]);
		}

		ptr = ptr->next;

		printf("\n");
	}

	if(print_value == 0)
	{
		file_buffer.current_line = file_buffer.HEAD;
		file_buffer.current_position = 1;
		file_buffer.current_element = file_buffer.current_line->line[file_buffer.current_position-1];
		print_value = 1;
	}
	current();
}

/* h-> command to move cursor right one position.*/

void right_shift()
{
	if(file_buffer.lines == file_buffer.current_line->line_no  && file_buffer.current_position == 8)
	{
		printf("Cursor on last character.\nCursor can not be moved right.\n");
	}
	else if(file_buffer.current_position == 8)
	{
		file_buffer.current_line = file_buffer.current_line->next;
		file_buffer.current_position = 1;
	}
	else
	{
		file_buffer.current_position++;
	}

	current();

}

/* l-> command to move cursor one position left.*/

void left_shift()
{
	if(file_buffer.current_line->line_no == 1 && file_buffer.current_position == 1)
	{
		printf("Cursor on first character.\nCursor can not be moved left.\n");
	}
	else if(file_buffer.current_position == 1)
	{
		file_buffer.current_line = file_buffer.current_line->previous;
		file_buffer.current_position = 8;
	}
	else
	{
		file_buffer.current_position--;
	}

	current();
}

/* j-> command to move cursor down by one line.*/

void jump_down()
	{
		if(file_buffer.current_line->line_no  == file_buffer.lines)
	{
		printf("Cursor is on last line.");
	}
	else
	{
		file_buffer.current_line = file_buffer.current_line->next;
	}

	current();
}

/* o-> command to make a new line.*/

void open_line()
{
	int i;
	char string[2];
	unsigned int a;
	node *newnode,*ptr;

	newnode = (node *)malloc(sizeof(node));

	if(newnode == NULL)
	{
		printf("Not enough memory.");
		return;
	}

	for(i=0;i<8;i++)
	{
		printf("Enter character %d : ",i+1);
		gets(string);

		sscanf(string,"%x",&a);

		newnode->line[i]=a;
	}

	ptr = file_buffer.current_line->next;

	newnode->line_no = file_buffer.current_line->line_no + 1;
	newnode->previous = file_buffer.current_line;
	if(file_buffer.current_line->line_no == file_buffer.lines)
	{
		newnode->next = NULL;
	}
	else
	{
		newnode->next = file_buffer.current_line->next;
	}
	file_buffer.current_line->next = newnode;


	while(ptr != NULL)
	{
		ptr->line_no = ptr->line_no + 1;

		ptr = ptr->next;
	}

	file_buffer.lines++;

	current();

}

/*k-> command to move current line one above.*/

void move_above()
{
	if(file_buffer.current_line->line_no == 1)
	{
		printf("Cursor at first line.\n");
	}
	else
	{
		file_buffer.current_line = file_buffer.current_line->previous;
	}

	current();
}

/*x-> command to delete a character.*/

void delete_one()
{
	int i;
	node *ptr;

	if(file_buffer.current_line->line_no == file_buffer.lines)
	{
		if(file_buffer.current_position == 8)
		{
			file_buffer.current_line->line[file_buffer.current_position-1] = '\0';
		}
		else if(file_buffer.current_position == 1 && file_buffer.current_line->line[file_buffer.current_position ] == EOF)
		{
			file_buffer.lines--;
			delete_node();
		}
		else
		{
			for(i=file_buffer.current_position-1;i<7;i++)
			{
				file_buffer.current_line->line[i] = file_buffer.current_line->line[i+1];
			}
		}
	}
	else
	{
		for(i=file_buffer.current_position-1;i<7;i++)
		{
			file_buffer.current_line->line[i] = file_buffer.current_line->line[i+1];
		}
		
		ptr = file_buffer.current_line->next;

		while(ptr != NULL)
		{
			ptr->previous->line[7] = ptr->line[0];

			for(i=0;i<7;i++)
			{
				ptr->line[i] = ptr->line[i+1];
			}

			ptr = ptr->next;
		}

		current();
		
	}
}

/* d-> delete node */

void delete_node()
{
	node *ptr,*ptr_line;
	int i;

	if(file_buffer.current_line->line_no == 1)
	{
		ptr = file_buffer.current_line;
		file_buffer.current_line = file_buffer.current_line->next;
		file_buffer.current_line->previous = NULL;
		file_buffer.HEAD = file_buffer.current_line;
		
		file_buffer.lines--;

		ptr_line = file_buffer.current_line;

		for(i=0;i<file_buffer.lines;i++)
		{
			ptr_line->line_no--;
			ptr_line = ptr_line->next;
		}


	}
	else if(file_buffer.current_line->line_no == file_buffer.lines)
	{
		ptr = file_buffer.current_line;
		file_buffer.current_line = file_buffer.current_line->previous;
		file_buffer.current_line->next = NULL;
		file_buffer.TAIL = file_buffer.current_line;
		file_buffer.lines--;
	}
	else
	{
		ptr = file_buffer.current_line;
		file_buffer.current_line = file_buffer.current_line->previous;
		file_buffer.current_line->next = ptr->next;

		ptr_line = file_buffer.current_line->next;

		for(i=ptr_line->line_no;i<=file_buffer.lines;i++)
		{
			ptr_line->line_no--;
			ptr_line = ptr_line->next;
		}

		file_buffer.lines--;
	}



	free(ptr);

	current();
}

void replace()
{
	unsigned int l;
	char  input[2];
	
	printf("Enter the character : ");
	gets(input);

	sscanf(input,"%x",&l);

	file_buffer.current_line->line[file_buffer.current_position-1] = l;

	current();




}

/*e-> command to terminate the editor.*/

void terminate()
{
	char choice;
	
	FILE *fs;
	node *ptr;
	int i;

	printf("Do u want to save the modifications :(y/n) ");
	scanf("%c",&choice);

	if(choice == 'y'  || choice == 'Y')
	{
		fs = fopen(file_name,"wb");

		if(fs == NULL)
		{
			printf("Content can not be saved.\n");
			exit(0);
		}
		
		ptr = file_buffer.HEAD;

		while(ptr != NULL)
		{
			for(i=0;i<8;i++)
			{
				fputc(ptr->line[i],fs);
			}

			ptr = ptr->next;
		}

		fclose(fs);

	}

	exit(0);
}

void main(int argc,char* argv[])
{
	char buffer[LINE_SIZE],input_buffer[INPUT_BUFFER_SIZE],cmd;

	FILE *fp;

	file_buffer.HEAD=NULL;	//initiating the file_buffer object
	file_buffer.TAIL=NULL;
	file_buffer.lines=0;

	if(argc<2)
	{
		printf("Few arguments.\n");
		exit(0);
	}

	if(argc>2)
	{
		printf("More arguments.\n");
		exit(0);
	}

	file_name = argv[1];

	fp=fopen(argv[1],"r");

	if(fp == NULL)
	{
		printf("There is a error in opening the file.\n");
		exit(0);
	}

	while(fread(buffer,LINE_SIZE,1,fp)) //reading content of a file , 8 characters at a time in buffer
	{
		read(&file_buffer,buffer);
	}

	file_buffer.current_line = file_buffer.HEAD;
	file_buffer.current_position = 1;
	file_buffer.current_element =  file_buffer.current_line->line[file_buffer.current_position-1];

	while(1)
	{
		printf("SDREAM EDITOR -> ");

		fgets(input_buffer,INPUT_BUFFER_SIZE,stdin);

		parse(input_buffer,argvv);
	
		cmd = *argvv[0];


		switch(cmd)
		{
			case 'p' : 	print();
					break;

			case 'e' :      terminate();
					break;
			
			case 'h' :	right_shift();
					break;

			case 'l' :	left_shift();
					break;

			case 'j' :	jump_down();
					break;

			case 'k' :	move_above();
					break;

			case 'o' : 	open_line();
					break;
		
			case 'x' : 	delete_one();
					break;

			case 'd' : 	delete_node();
					break;

			case 'r' :	replace();
					break;

			default  :	printf("Unknown command.\n");
		}
	

	}
}
