/*

Programmer: Youssef Al Hindi
Licence: GPLv3

HEAP SUMMARY:
in use at exit: 0 bytes in 0 blocks
total heap usage: 934 allocs, 934 frees, 13,417 bytes allocated

All heap blocks were freed -- no leaks are possible
 
ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>						//needed for str functions

//This is the static lookup array
static char lookup[26] = {'a','b','c','d','e','f','g','h','i','j',
						'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

int lookupInt(char key) 				//convert char to int
{
	int i = 0;
	for(i=0;i<26;i++)
	{
		if(lookup[i] == key)
		{
			return i;
		}
	}
	return 0;
}

char lookupChar(int key)				//convert int to char
{
	if(key>25)
	{
		key = key % 26;
	}
	return lookup[key];
}

void printFile(FILE *text)				//print a text file for debug only
{
	char input = '\0';
	while((input = fgetc(text)) != EOF)
	{
		printf("%c", input);
	}
	printf("\n");
	rewind(text); //move file pointer back to start of file
	return;
	
}

int** parseKeyfile(FILE *keyfile, int *sizep)
{
	int i=0,z=0,input=0;
	char buffer[100], *token, sep[2] = ",";
	fscanf(keyfile, "%d\n", sizep);		//read size of key matrix
	//printf("Size: %d\n", *sizep);
	int **keyp = (int**) calloc(*sizep,sizeof(int*));		//allocate memory for key matrix
	for(i=0;i<*sizep;i++)
		keyp[i] = (int *)calloc(*sizep,sizeof(int));

	for(z=0;z<*sizep;z++)
	{
		input=0;
		fgets(buffer, 100, keyfile);		//read line
		//printf("Buffer: %s\n", buffer);
		token = strtok(buffer, sep);		//split line into tokens sep by commas
		for(i=0;i<*sizep && token != NULL;i++)
		{
			input = atoi(token);		//convert char pointer to int
			keyp[z][i] = input;
			//printf("z: %d i: %d Input: %d\n", z, i, input);
			token = strtok(NULL,sep);		//go to next token
		}
	}

	return keyp;
}

char* parseTextfile(FILE *textfile, int *sizep, int keySize)
{
	fseek (textfile, 0, SEEK_END);		//move file pt location to end of file
  	*sizep = ftell(textfile);			//get size of file
  	fseek (textfile, 0, SEEK_SET);		//move file pt back to start of file
  	char *buffer = (char*) calloc(*sizep + 2, sizeof(char));
  	buffer[*sizep + 1] = '\0';
  	if(buffer)							//memory allocation check
  	{
  		//read whole file 
  		fread (buffer, 1, *sizep, textfile);
  	}
	char *text = (char*) calloc(*sizep + keySize + 1, sizeof(char));
	int i = 0, count = 0;
	for(i=0; buffer[i]!= '\0'; i++)	//for every char in file string
	{
		if(isalpha(buffer[i]))		//check if its alphanumaric
		{
			text[count] = tolower(buffer[i]);	//make it lowercase
			count++;
		}
	}
	text[*sizep + keySize] = '\0';
	free(buffer);
	buffer = NULL;
	printf("\n\n%d\n%d\n%d\n\n", strlen(text), keySize, (strlen(text)) % keySize);
	if((strlen(text)) % keySize != 0)
	{
		//calculate the pad size
		int pad = keySize - (strlen(text)) % keySize, last = strlen(text);
		printf("%d\n", pad);
		for(i=0;i<pad;i++)
		{
			text[last + i] = 'x';	//insert a 'x' for size of pad at the end of parsed string
		}
		text[last+pad+1] = '\0';	//add required NULL char
	}
	return text;
}

//print a string in the required format
void printFormatedString(char *string)
{
	int i = 0;
	for(i=0;i<strlen(string);i++)
	{
		printf("%c", string[i]);
		if((i + 1) % 80 == 0)
		{
			printf("\n");
		}
	}
	printf("\n\n");
	return;
}

//print the key in the required format
void printFormatedKey(int **key, int ksize)
{
	int i=0,z=0;
	for (i = 0; i < ksize; i++)
	{
		for (z = 0; z < ksize; z++)
		{
			printf("%d ", key[i][z]);
		}
		printf("\n");
	}
	printf("\n");
	return;
}

//prosess a "block", a block is a string from a section of the plaintext of the size of the key
char* processBlock(char *block, int **key, int ksize)
{
	int i = 0, z = 0;
	char *buffer = (char*) calloc(ksize + 1, sizeof(char));
	buffer[ksize] = '\0';	
	int *temp = (int*) calloc(ksize, sizeof(int));
	for(i=0;i<ksize;i++) //for every row in key
	{
		temp[i] = 0;
		for(z=0;z<ksize;z++)	//for every column in key
		{
			temp[i] += key[i][z] * lookupInt(block[z]); //multiply the block's char with key and add them
		}
		buffer[i] = lookupChar(temp[i]);	//lookup the char
		
	}
	free(block);
	free(temp);
	return buffer;
}

char* encrypt(char *plaintext, int **key, int ksize)
{
	char *cyphertext = (char*) calloc(strlen(plaintext) + 1, sizeof(char));
	char *buffer = (char*) calloc(ksize+1, sizeof(char));	
	cyphertext[strlen(plaintext)] = '\0';
	buffer[ksize] = '\0';

	int i = 0,z = 0;

	for(i=0;i<strlen(plaintext);i++)	//for every letter in plaintext
	{
		if((i+1) % ksize == 0)		//when it is a factor of the keysize
		{
			for(z=0;z<ksize;z++)
			{
				buffer[z] = plaintext[i+1-ksize+z];	//put the plaintext block in the buffer
			}
			buffer = processBlock(buffer, key, ksize);	//encrypt the buffer block
			for(z=0;z<ksize;z++)
			{
				cyphertext[i+1-ksize+z] = buffer[z]; 	//add encrypted buffer to cyphertext
			}
		}
	}
	free(buffer);
	return cyphertext;
}

int main(int argc, char const *argv[])
{
	int ksize = 0, textSize = 0, i = 0;
	//Argument Check
	if (argc != 3)
	{
		printf("Useage: %s keyfile filename\n", argv[0]);
		return 0;
	}

	//File Checks
	FILE *keyfile = fopen(argv[1],"r");
	if(keyfile == NULL)
	{
		printf("Unable to open keyfile %s\n", argv[1]);
		return 0;
	}
	FILE *textfile = fopen(argv[2],"r");
	if(textfile == NULL)
	{
		printf("Unable to open textfile %s\n", argv[2]);
		return 0;
	}

	//debug function calls
	//printFile(keyfile);
	//printf("\n\n");
	//printFile(textfile);

	int **key = parseKeyfile(keyfile, &ksize);
	char *plainText = parseTextfile(textfile, &textSize, ksize);

	//print key
	printFormatedKey(key, ksize);

	//print parsed plaintext
	printFormatedString(plainText);

	//encrypt plaintext
	char *cyphertext = encrypt(plainText, key, ksize);

	//print cyphertext
	printFormatedString(cyphertext);

	//free leftover alloc'ed memory
	for(i=0;i<ksize;i++)
		free(key[i]);
	free(key);
	free(plainText);
	free(cyphertext);

	//close files
	fclose(textfile);
	fclose(keyfile);
	return 0;
}
