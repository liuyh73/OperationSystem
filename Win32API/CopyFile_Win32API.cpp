#include <Windows.h>
#include <locale.h>
#include <tchar.h>
#include <stdio.h>
int main(){

	TCHAR inputFileName[50]={0}, outputFileName[50]={0};
	setlocale(LC_ALL, "chs");

	printf("Please enter your input file's name: \n");
	scanf("%s", inputFileName);

	printf("Please enter your output file's name: \n");
	scanf("%s", outputFileName);

	// DWORD inputFileError = GetFileAttributes(inputFileName);
	// DWORD outputFileError = GetFileAttributes(outputFileName);
	// if(INVALID_FILE_ATTRIBUTES == inputFileError){
	// 	printf("Your input file doesn't exist!");
	// 	return 0;
	// }
	// if(INVALID_FILE_ATTRIBUTES != outputFileError){
	// 	printf("Your output file has existed!");
	// 	return 0;
	// }
	HANDLE inputFileHandle = CreateFile(inputFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(inputFileHandle == INVALID_HANDLE_VALUE){
		printf("Open inputFile failed(%ld), the input file doesn't exist!", GetLastError());
		CloseHandle(inputFileHandle);
		return 0;
	}
	HANDLE outputFileHandle = CreateFile(outputFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(outputFileHandle == INVALID_HANDLE_VALUE){
		printf("Create outputFile failed(%ld), the output file has existed!", GetLastError());
		CloseHandle(outputFileHandle);
		return 0;
	}

	DWORD len = 0;
	TCHAR readBuf[1024];
	//DWORD filesize=GetFileSize(inputFileHandle,NULL);
	while(ReadFile(inputFileHandle, readBuf, 1, &len, NULL) && len>0){
		readBuf[len]='\0';
		printf("%s\n" , readBuf);
		//filesize-=len;
		WriteFile(outputFileHandle, readBuf, 1, &len, NULL);
		FlushFileBuffers(outputFileHandle);
		//if(filesize<=0)	break;
	}

	printf("Write completed!\n");

	return 0;
}