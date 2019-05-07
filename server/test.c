#include "../WTF.h"

int main()
{
	char * path = "projects/pr1/.Commit";
	printf("%s\n", getFileHash(readFromFile(open(path, O_RDONLY))));
	return 0;
}
