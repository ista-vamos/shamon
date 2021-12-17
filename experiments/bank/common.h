#pragma once

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int hash(char* str)
{
	int h=2345607;
	int l=strlen(str);
	for(int i=l-1;i>=0;i--)
	{
		int c=(int)(str[i]);
		h = ((h+c)*65537)%114460513;
	}
	return h;
}

static char * linebuf[16] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
static int linebufpos = 0;

static void resetLinebuf()
{
	for(int i=linebufpos;i>0;)
	{
		i--;
		free(linebuf[i]);
		linebuf[i]=NULL;
	}
}

static char * getLine()
{
	if(linebufpos>=16)
	{
		return NULL;
	}
	size_t zero = 0;
	ssize_t	lineSize = getline(&linebuf[linebufpos], &zero, stdin);
	if(lineSize>=0)
	{
		return linebuf[linebufpos];
	}
	return NULL;
}

static void removeNewLineAtEnd(char* str)
{
	if(str[strlen(str)-1]=='\n')
	{
		str[strlen(str)-1]=0;
	}
}
static int strsplit(char* str, char delim, int count)
{
	int actual=1;
	while(count>0&&strlen(str)>0)
	{
		char* next=strchr(str, delim);
		if(next!=NULL)
		{
			next[0]=0;
			str=next+1;
			count--;
			actual++;
		}
		else
		{
			count=0;
		}
	}
	return actual;
}