#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char* argv[]	)
{
    const int bufferSize = 1024;
    char buffer[bufferSize];

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd >= 0)
        {
            int readAmount;
            while (readAmount = read(fd, buffer, bufferSize))
	    {
		 
		int writeAmount = 0;
		int offset = 0;
                while (readAmount - writeAmount > 0) {
		    writeAmount = 0;
		    writeAmount = write(STDOUT_FILENO, buffer, readAmount); 
		    if (writeAmount < 0) {
			fprintf(stderr, "error while writing");
			break;  
		    }
 	            
		    readAmount -= writeAmount;
	 	    offset += writeAmount;
                    
                }
	    }
	    
	    if (readAmount < 0) {
		fprintf(stderr, "error while reading file");  
            }


        } else {
	    fprintf(stderr, "error read file");  	
	}
    }
    return 0;
}

