#include <string.h>

int hash(char* str)
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