#include "mytable.h"

using namespace mytable;
char buff[32];

int main()
{
	sprintf(buff,"This is a context");
	TABLEHANDLE handle=create_table("TestTable.bin",true);
	addcol(handle,-1,strlen(buff),buff,"New Col");
	sprintf(buff,"Another Context");
	addcol(handle,0,strlen(buff),buff,"New Col X");
	sprintf(buff,"New Line Filled");
	addline(handle,-1,strlen(buff),buff);
	sprintf(buff,"Changed Line Filled");
	setrawdata(handle,0,1,strlen(buff),buff);
	close_table(handle);
}