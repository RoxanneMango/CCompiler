#include "file_io.h"

int
filterOutCommentsFromFile(const char * filePATH, const char * outPATH)
{
	DEBUG_PRINT("filterOutComments() ...\n");

	FILE * FP = fopen(filePATH, "r");
	FILE * FP_OUT = fopen(outPATH, "w");
	
	if(!FP)
	{
		return -1;
	}
	if(!FP_OUT)
	{
		return -1;
	}
	
	char chunk[128];
	
	size_t len = sizeof(chunk);
	char * line = malloc(len);
	
	if(line == NULL)
	{
		return -1;
	}
		
	bool isCommentBlock = false;
	
	while(fgets(chunk, sizeof(chunk), FP) != NULL)
	{
		// empty the line buffer
		line[0] = '\0';
		
		// Resize the line buffer if necessary
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if(len - len_used < chunk_used)
		{
			len *= 2;
			if((line = realloc(line, len)) == NULL)
			{
				printf("Unable to reallocate memory for the line buffer\n");
				free(line);
				return -1;
			}
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		// Check if line contains '\n', if yes process the line of text
		// the preprocessing happens here ...
		if(line[len_used - 1] == '\n')
		{			
			// filter out comments
			char * substr = strstr(line, "//");
			char * substr2 = strstr(line, "/*");
			if(substr)
			{
				// if its at index 0, ignore the rest of the line, its definitely a comment
				if(line != substr)
				{
					// get starting position of comment
					int a = abs(line - substr);
					
					// if not part of string, remove comment
					if(!partOfString(line, substr))
					{
						line[a] = ' ';
						line[a+1] = '\n';
						line[a+2] = '\0';
					}
					if(!isOnlyWhiteSpaces(line))
					{
						fputs(line, FP_OUT);
					}
				}
			}
			else if(substr2)
			{
				if(partOfString(line, substr2))
				{
					fputs(line, FP_OUT);
				}
				else
				{					
					isCommentBlock = true;
					char * substr3 = strstr(line, "*/");
					
					// inline comment
					if(substr3)
					{
						DEBUG_PRINT("inline comment\n");
						isCommentBlock = false;
						int a = abs(line - substr2);
						int b = abs(line - substr3) + 2;
						DEBUG_PRINT("a: %d ; b: %d\n", a, b);
						
						char split1[strlen(line)]; split1[0] = '\0';
						char split2[strlen(line)]; split2[0] = '\0';
						
						strncat(split1, line, a);
						strncat(split2, (line+b), strlen(line)-b);
						
						DEBUG_PRINT("split1: %s ; split2: %s\n", split1, split2);
											
						char splitstitch[strlen(line)]; splitstitch[0] = '\0';
						strcat(splitstitch, split1);
						strcat(splitstitch, split2);

						DEBUG_PRINT("splitstitch: %s\n", splitstitch);
						
						splitstitch[b] = ' ';
						splitstitch[b+1] = '\0';
						
						if(!isOnlyWhiteSpaces(splitstitch))
						{
							fputs(splitstitch, FP_OUT);
						}
					}
				}
			}
			else if(isCommentBlock)
			{
				char * substr3 = strstr(line, "*/");
				if(substr3)
				{
					isCommentBlock = false;
					int b = abs(line - substr3) + 2;
					if(!isOnlyWhiteSpaces(line+b))
					{
						fputs((line+b), FP_OUT);
					}
				}
			}
			else
			{
				if(!isOnlyWhiteSpaces(line))
				{
					fputs(line, FP_OUT);
				}
			}
		}
	}

	fclose(FP);
	fclose(FP_OUT);

	free(line);
	
	return 0;
}

int
readFromFile(const char * filePATH, _LinkedStringList * fileList)
{	
	// parse the file before processing it
	FILE * FP = fopen(filePATH, "r");
	
	if(!FP)
	{
		return -1;
	}
	
	char chunk[128];
	
	size_t len = sizeof(chunk);
	char * line = malloc(len);
	
	if(line == NULL)
	{
		return -1;
	}
	
	// empty the line buffer
	line[0] = '\0';
	while(fgets(chunk, sizeof(chunk), FP) != NULL)
	{		
		// Resize the line buffer if necessary
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if(len - len_used < chunk_used)
		{
			len *= 2;
			if((line = realloc(line, len)) == NULL)
			{
				printf("Unable to reallocate memory for the line buffer\n");
				free(line);
				return -1;
			}
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		// Check if line contains '\n', if yes process the line of text
		if( (line[len_used - 1] == '\n') )
		{
			fileList->addNode(fileList, line, 0);
			// empty the line buffer
			line[0] = '\0';
		}
	}
	
	// add the last line
	if(line && strlen(line))
	{
		fileList->addNode(fileList, line, 0);
	}
	
	fclose(FP);
	free(line);
	
	return 0;
}

bool 
fileExistsInDirectory(const char * PATH, const char * file)
{
	DIR * DP = opendir(PATH);
	if(!DP)
	{
		DEBUG_PRINT("Could not open directory @ %s\n", PATH);
		return -1;
	}
	
	struct dirent * dir;
	bool fileExists = false;
	while((dir = readdir(DP)) && !fileExists)
	{
//		printf("d_name: %s ; file: %s\n", dir->d_name, file);
		if(strcmp(dir->d_name, file) == 0)
		{
			fileExists = true;
		}
	}
	closedir(DP);

	return fileExists;
}

int
getFilesFromDirectory(_LinkedStringList * list, const char * PATH)
{	
	DIR * DP = opendir(PATH);
	if(!DP)
	{
		DEBUG_PRINT("Could not open directory @ %s\n", PATH);
		return -1;
	}
	
	struct dirent * dir;
	while((dir = readdir(DP)))
	{
		if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
		{
			list->addNode(list, dir->d_name, 0);
//			printf("-> %s\n", dir->d_name);
		}
	}
	closedir(DP);

	return 0;
}


int 
validateInclude(const char * fileName)
{
	// check for what kind of includes;
	int fileNameLen = strlen(fileName);
	if(fileNameLen < 2)
	{
		return -1;
	}
	
	// check if it is valid
	if( ((fileName[0] == '<') && (fileName[fileNameLen-1] == '>')) ||
		((fileName[0] == '\"') && (fileName[fileNameLen-1] == '\"')) )
	{
		bool isFileFound = false;
		bool isLocalInclude = (fileName[0] == '\"');
		
		// remove first and last character to sanitize file name
		
		char file[strlen(fileName)-2]; file[0] = '\0';
		strncat(file, (fileName+1), strlen(fileName));
		file[strlen(file)-1] = '\0';
		
		// check if . directory needs to be included in the search path ...
		if(isLocalInclude)
		{
			if(fileExistsInDirectory(".", file))
			{
				isFileFound = true;
				DEBUG_PRINT("Local file exists: %s/%s\n", ".", file);
			}
		}
		// search through standard library headers
		if(!isFileFound)
		{
			for(int i = 0; (i < _StdHeaderLocationNum) && !isFileFound; i++)
			{
				if(fileExistsInDirectory(_StandardLibraryHeaderLocations[i], file))
				{
					DEBUG_PRINT("File exists: %s/%s\n", _StandardLibraryHeaderLocations[i], file);
					isFileFound = true;
				}
			}
		}
		// could not find file at all!
		if(!isFileFound)
		{
			DEBUG_PRINT("Could not find #included file %s\n", fileName);
			return 0;
		}
	}
	else
	{
		DEBUG_PRINT("Invalid include syntax!\n");
		return 0;
	}
	
	// final pass to check for illigal characters
	int forbiddenLen = 9;
	char * forbiddenCharacters[] = { "\\", "/", ":", "*", "?", "\"", "<", ">", "|" };
	for(int i = 0; i > forbiddenLen; i++)
	{
		if(strstr(fileName, forbiddenCharacters[i]))
		{
			return 0;
		}
	}
	
	return 1;
}