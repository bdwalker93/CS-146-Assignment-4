#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>


//for using exit()
#include <stdlib.h>


//prototypes
char* getContents(char *basePath, char* fileName, int LFlag); //probably should just make L a global variable, but
								//its fine since were only implementing a few
								//options
static int cmpStr(const void *p1, const void *p2);
int isDir(char *path);


int main(int argc, char *args[]) 
{ 

	//boolean flags for options	
	int aFlag = 0;
	int AFlag = 0;
	int LFlag = 0;

	int opt;
  	while ((opt = getopt (argc, args, "aAL")) != -1)
  	{
    		switch (opt)
    		{
      		case 'a':
                	aFlag = 1;
                	break;
      		case 'A':
                	AFlag = 1;
                	break;
      		case 'L':
                	LFlag = 1;
                	break;
		case '?':
			//kills program if an invalid option is enered. Verbose output 
			//	replicating ls.
			exit(1);
    		}
 	}

	
	//stores there were options
	int options = 0;
	
	//stores if there are file arguments
	int cmdFiles = 0;

	//shifts the file ct appropriatly based on if there were options or not
	if(aFlag || AFlag || LFlag)
		options = 1;

	//Default handles scenario of no options or args
	int startingPos = 0;

	
	//handles both no option existing and potential files
	if(!options && argc > 1)
	{
		cmdFiles = 1;
		startingPos = 1;

	}
	else if(!options)
		startingPos = 0;

	//handles both option existing and potential files
	if(options && argc > 2)
	{
		startingPos = 2;
		cmdFiles = 1;	
	}
	else if(options)
		startingPos = 1;

	//used to create the N x N array for storing file info
	const int F_SIZE = 1000;

	//stores all of the files and their long listing format (wastefull with space, but okay for now)
	//	(Switched this to an array of pointers because qsort doesnt like a 2d char array)
	char * files[F_SIZE];
	
	//stores the number of files in the directory
	int fileCt = 0;

	int j;
	//for loop iterating changing the directory path for the passed file parameters
	for(j = startingPos; j < argc; j++)
 	{

		//holds the current directory path. Defaults to the cd of lss.c
		char *currDirPath = ".";

		if(cmdFiles)
			currDirPath = args[j];


		//is file or dir
		if(isDir(currDirPath) == 0)
		{
			//THIS CASE HANDLES INDIVIDUAL FILES

			//allocates space for the file
			files[fileCt] = malloc(F_SIZE * sizeof(char));
				
			//error checking malloc to make sure we were allocated memory
			if(!files[fileCt])
			{
				char errorOutput[F_SIZE];

				//creates the error string with the file name
				sprintf(errorOutput, "lss: cannot allocate memory for %s", currDirPath );

				//uses perror to output error
				perror(errorOutput);
			}
			else
			{
				strcpy(files[fileCt], getContents("./" , currDirPath, LFlag )); 	

				//increments the file count
				fileCt++;
			}

		}
		else if(isDir(currDirPath) == 1)
		{
	
			//stores the directory
			DIR *currDir;


			//opens a dir
			currDir = opendir(currDirPath);
	
			//making sure the directory exists and can be opened
			if(!currDir)
			{
				char errorOutput[F_SIZE];

				//creates the error string with the file name
				sprintf(errorOutput, "lss: cannot open %s", currDirPath);

				//uses perror to output error
				perror(errorOutput);
			}
			else
			{

				//struct with a file's informaiton
				struct dirent *currFile;


				//loops through the files while they exist (readdir moved pointer along automatically)
				while (currFile = readdir(currDir))
				{
					//making sure there were no issues reading the file from the directory
					if(!currFile)
					{
						char errorOutput[F_SIZE];

						//creates the error string with the file name
						sprintf(errorOutput, "lss: cannot access %s", currDirPath);

						//uses perror to output error
						perror(errorOutput);
					}
					else
					{

						//allocates space for the file
						files[fileCt] = malloc(F_SIZE * sizeof(char));
				
						//error checking malloc to make sure we were allocated memory
						if(!files[fileCt])
						{
							char errorOutput[F_SIZE];

							//creates the error string with the file name
							sprintf(errorOutput, "lss: cannot allocate memory for %s", currFile->d_name);

							//uses perror to output error
							perror(errorOutput);
						}
						else
						{
							//Handling -a and -A options
							if(aFlag)
							{
								strcpy(files[fileCt], getContents(currDirPath , currFile->d_name, LFlag )); 	

								//increments the file count
								fileCt++;
							}
							else if(AFlag)
							{
								//ignores implied "." and ".." directories and files beginning with "."
								if(strcmp(".", currFile->d_name) != 0 && strcmp("..", currFile->d_name) != 0)
								{
									strcpy(files[fileCt], getContents(currDirPath , currFile->d_name, LFlag )); 	

									//increments the file count
									fileCt++;
								}
							}
							else
							{ 
								//ignores implied "." and ".." directories and files beginning with "."
								if(strcmp(".", currFile->d_name) != 0 && strcmp("..", currFile->d_name) != 0 && (*currFile->d_name != '.'))
								{
									strcpy(files[fileCt], getContents(currDirPath , currFile->d_name, LFlag )); 	

									//increments the file count
									fileCt++;
								}
							}
						}
					}	
				}

			}

		}
		else
		{
			char errorOutput[F_SIZE];

			//creates the error string with the file name
			sprintf(errorOutput, "lss: cannot access %s", args[j]);

			//uses perror to output error
			perror(errorOutput);

		}
	}



	//calls qsort on the array with the file names. Comparator sorts on file size
  	qsort(files, fileCt, sizeof(char*), cmpStr);


	//ouputs ls
	int i;
	for(i = 0 ; i < fileCt; i++)
	{
		int status = printf("%s\n", files[i]);
		if(status < 0)
		{
			char errorOutput[F_SIZE];

			//creates the error string with the file name
			sprintf(errorOutput, "lss: cannot print %s", files[i]);

			//uses perror to output error
			perror(errorOutput);
		}
	}

	return 0;
} 

int isDir(char *path)
{

	//stat struct to store all of the file information
        struct stat tmpStat;


	//stat'ing the file/dir to pull its inforamtion
	int status = stat(path, &tmpStat);


	//encounters a broken symbolic link. Program exists
	if(status == -1)
	{
		return -1;		
	}
	else
	{
		if(S_ISDIR(tmpStat.st_mode))
			return 1;
		else
			return 0;
	}		

}


/*
*Method takes a path to a file or directory and displays all of the long listing format information
*
*
*/
char* getContents(char *basePath, char* fileName, int LFlag)
{


	//instantiates path char array
	const int NAME_SIZE = 10000;
	char path[NAME_SIZE];

	//stat struct to store all of the file information
        struct stat tmpStat;

	//appends the base path and the file name together
	sprintf(path, "%s/%s", basePath, fileName);

	int status;

	//stat'ing the file/dir to pull its inforamtion
	if(LFlag)
		status = stat(path, &tmpStat);
	else
		status = lstat(path, &tmpStat);

	//stores the complete output (using dynamic allocation because we are 
	//	returning a pointer to the string) 
	char *output = malloc(NAME_SIZE * sizeof(char));

	//error checking the memory allocation
	if(!output)
	{

		//uses perror to output error
		perror("lss: cannot allocate memory");



		return "%s %s", "l????????? ? ?        ?         ?            ? cannot allocate memory";
	}
	else
	{

		//encounters a broken symbolic link. ignores brokenness if lflag is not triggered
		if(LFlag && (status == -1))
		{
			//creates the error string with the file name
			sprintf(output, "lss: cannot stat %s", fileName);

			//uses perror to output error
			perror(output);
		
			//defauly broken link info for list
			sprintf(output, "%s %s", "l????????? ? ?        ?         ?            ?", fileName); 

		
		}
		else
		{
	
			//printing permissions
   			sprintf(output, "%s", (S_ISDIR(tmpStat.st_mode))  ? "d" : "-");
			sprintf(output, "%s%s", output, (tmpStat.st_mode & S_IRUSR) ? "r" : "-");
 			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IWUSR) ? "w" : "-");
        			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IXUSR) ? "x" : "-");
    			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IRGRP) ? "r" : "-");
   			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IWGRP) ? "w" : "-");
    			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IXGRP) ? "x" : "-");
    			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IROTH) ? "r" : "-");
    			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IWOTH) ? "w" : "-");
    			sprintf(output, "%s%s", output,(tmpStat.st_mode & S_IXOTH) ? "x" : "-");
	


			//number of hard links
			sprintf(output, "%s%2d ", output, tmpStat.st_nlink);

			//user owner and group
			struct passwd *pwd = getpwuid(tmpStat.st_uid);
			struct group *grp = getgrgid(tmpStat.st_gid);

			//owner of file
			sprintf(output, "%s%s ", output, pwd->pw_name);

			//owner of group
			sprintf(output, "%s%s ", output,grp->gr_name);

			//size
			sprintf(output, "%s%1d ", output, tmpStat.st_size);

			//gets the mod time
			char *rawTime = ctime(&tmpStat.st_mtime);

			//creates a char array to store the time we want	
			char time[13];
		
			//copies the substr of the needed time
			memcpy(time, (rawTime + 4), 12);
			time[12] = '\0';

			//prints the time
  			sprintf(output, "%s%s ", output, time);




			//prints the file name
 			char target_path[256];
 			char* link_path = fileName;

 			int targetLen = readlink (link_path, target_path, sizeof (target_path));


			if (targetLen == -1 || LFlag) 
			{

				sprintf(output, "%s%s", output, fileName);
 			}
 			else 
			{
  				target_path[targetLen] = '\0';
				sprintf(output, "%s%s -> %s", output, fileName, target_path);

 			}

		
	
		}
	
		return output;	
	}
}




/***
*Comparator function for qsort()
*
*	Sorts based off of the size of the file/dir. Uses stat to determine sizes.
*		Somewhat inefficent to be calling stat for the size twice. Should
*		be creating a struct and calling it once, but im low on time.
***/
static int cmpStr(const void *p1, const void *p2)
{
	/*
	* EXPECTED STRING
	*	
	* drwx------ 3 walkerbd ugrad 2048 May 10 00:57 tstDir1
	*/

	//size of the file name info
	const int FILE_NAME_LEN = 1000;

	//initializing arrs to store info
	char file1[FILE_NAME_LEN];
	char file2[FILE_NAME_LEN];

	//coppies the strings
	strcpy(file1, *(char * const *)p1);
	strcpy(file2, *(char * const *)p2);

	
	/***
	*Need to do two completely different str tok because of the nature
	*	of the lib
	*/

	//stores the token of p1
	char *tok1;

	//delimter to skip columns
   	const char delim[4] = " \t";

	//number of columns to iterate over
	const int COL_NUM = 5;   

	//loads the  and gets the first token
   	tok1 = strtok(file1, delim);


   	//walk through other columns and gets the size 
	int i;
	for(i = 0; i <  COL_NUM - 1; i++)
      		tok1 = strtok(NULL, delim); 

	


	//SECOND STR TOK

	//stores the token of p2
	char *tok2;

	//loads the  and gets the first token
   	tok2 = strtok(file2, delim);


   	//walk through other columns and gets the size 
	for(i = 0; i <  COL_NUM - 1; i++)
      		tok2 = strtok(NULL, delim); 
	

/*
	//error checking

	//tokens
	printf( "This is the p1 column taken: %s\n", tok1 );
	printf( "This is the p2 column taken: %s\n", tok2 );

	//raw str
	printf("raw str:\n");
	printf( "In cmpstr printing p1: %s\n",  *(char * const *)p1);
	printf( "In cmpstr printing p2: %s\n",  *(char * const *)p2);

	//copied str
	printf("coppied str:\n");
	printf( "In cmpstr printing p1: %s\n",  *(char * const *)p1);
	printf( "In cmpstr printing p2: %s\n\n",  *(char * const *)p2);
*/



	//converting both token to inegers
	int fileSize1 = atoi(tok1);
	int fileSize2 = atoi(tok2);

	//returns in descending order
	return (fileSize2 - fileSize1);
}














