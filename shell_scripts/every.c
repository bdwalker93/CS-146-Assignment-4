#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>


int main(int argc, char *argv[])
{
	int envOptions = 0;
	int options = 0;
	char option[100];
	int noArgs = 0;
	
	//default every values
	int n = 1;
	int m = 1;


	//checking to see if there are no args at all
	if(argc < 2 || (argv[1][0] == '-' && argc < 3))
	{		
		noArgs = 1;
	}	

	//first makes sure there are args and then checks to see if they are options
	if(argc >= 2 && argv[1][0] == '-')
	{	
		//passes the options into our options string and signal that options exist
		memcpy(option, argv[1], sizeof(option));
		options = 1;	
	}
	//checks enviornment variable for options
	else
	{
		//need tp check the enviroment variable EVERY for params
		char *tmp =  getenv("EVERY");
			
		//if the env variable exists it flags options, otherwise it goes with defaults
		if(tmp)
		{
			memcpy(option, tmp, sizeof(option));	
			envOptions = 1;
			options = 1;	

		}

	}
	
	//error checking and parsing if options do exist
	if(options == 1)
	{

		//makes sure there is a dash
		if(option[0] != '-')
               {
                       //kills program if option isnt in the correct format
                       fprintf(stderr, "INVALID option! Command must be in the form \"every [-N,M] [<list-of-files>]\"\n");
                       exit(2);
		}

		int commaPos = -1;

		int pos;
		for(pos = 1; pos < strlen(option); pos++)
		{
			if (option[pos] >= '0' && option[pos] <= '9') 
			{
    				//puts("it's a digit");
			} 
			else if (option[pos] == ',' && commaPos == -1 && pos > 1)
			{
				//keeps track of where the comma is for the two numbers. The check here is complicated, 
				//	covers a number of possible invalid inputs for the option
				commaPos = pos;
			}
			else
			{
				//kills program if option isnt in the correct format
				fprintf(stderr, "INVALID option! Command must be in the form \"every [-N,M] [<list-of-files>]\"\n");
				exit(2);
			}
		}
		
		//checks for the case were there is a comma at the end of the option without anymore numbers. Also handles
		//	potentially empty options.
		if(commaPos == (pos - 1) || pos == 1)
               {
                       //kills program if option isnt in the correct format
                       fprintf(stderr, "INVALID option! Command must be in the form \"every [-N,M] [<list-of-files>]\"\n");
                       exit(2);
		 }
                               
		//parses options for respective strings
		char noDash[100];
		char nStr[100];
		char mStr[100];
		
		//rids the dash
		memcpy(noDash,(option + 1),sizeof(noDash)); 
		noDash[pos] = '\0';

		//handles if m and n exist or just n
		if(commaPos == -1)
		{
			memcpy(nStr, noDash, pos);
			nStr[pos] = '\0';
		}
		else
		{
			//gets nStr
			memcpy(nStr, noDash, commaPos);
			nStr[commaPos] = '\0';
	
			//gets mStr
			memcpy(mStr, (noDash + commaPos), (pos - commaPos));
			mStr[pos - commaPos];
	
			//m str to int
			m = atoi(mStr);	
		}
			//converts n str to int
		n = atoi(nStr);	

	}

	
	//check to ensure that M <= N
	if(m > n)
	{
		fprintf(stderr, "The N option must be <= the M option\n");
		exit(3);
	}


	//**********************************
	//Now getting into the output section
	//***********************************

	//assuming we are starting at the first arg
	int startingFilePos = 1;

	//skipping over the option as long as env options were the cause for the options
	if(options && !envOptions)
		startingFilePos = 2;


	//goes through all of the files as args
	int file;
	for(file = startingFilePos; file < argc || noArgs; file++)
	{
		//boolean to keep track of valid files
		int fileExists = 1;
		FILE *fp;

		//handles the case where we are reading in from standard in
		if(noArgs)
		{
			fp = stdin;
		}
		//otherwise looks to the arg list for file names
		else
		{
			//attempts to open file
			fp = fopen(argv[file], "r");

			//if the file doesnt exist
			if (fp == NULL) 
			{
  				fprintf(stderr, "Can't open input file %s!\n", argv[file]);
				fileExists = 0;
			}

		}

		//can take in a BUF_SIZE line from file/stdin
		const int BUF_SIZE = 1000000;
		char buf[BUF_SIZE];

		while(fileExists && !feof(fp))
		{
		
			//prints out m number of lines
			int mCt;
			for(mCt = 0; mCt < m && fgets(buf, BUF_SIZE, fp); mCt++)
			{
                                printf("%s", buf);
			}

			
			//consumes (n-m) lines (everything leftover out of the N lines)
			int nCt;
			for(nCt = 0; nCt < (n - m) && !feof(fp); nCt++)
			{
				//eats lines from the file
				fgets(buf, BUF_SIZE, fp);	
			}

		}

		//closing the file
		if(fileExists)
			fclose(fp);
		
		//setting noArgs to false since there can only be one input file and there is no need 
		//	to loop
		noArgs = 0;

		//resetting file status. Assuming next file exists
		fileExists = 1;
		
	}

	return 0;
}
