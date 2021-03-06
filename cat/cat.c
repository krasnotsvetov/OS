#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int cat(int fd)
{
	const int bufferSize = 1024;
	char buffer[bufferSize];
	if (fd >= 0)
	{
		int readAmount;
		while (readAmount = read(fd, buffer, bufferSize))
		{
			if (readAmount < 0) {
				if (errno == EINTR) {
					continue;
				}
				fprintf(stderr, "error while reading file");
				return -1;
			}
			int writeAmount = 0;
			int offset = 0;
			while (readAmount - writeAmount > 0) {
				writeAmount = 0;
				writeAmount = write(STDOUT_FILENO, buffer + offset, readAmount - offset);
				if (writeAmount < 0) {
					if (errno == EINTR) {
						continue;
					}
					fprintf(stderr, "error while writing");
					return -1;
				}

				readAmount -= writeAmount;
				offset += writeAmount;

			}
		}

		if (fd != 0) {
			if (close(fd) != 0) {
				return -1;			
			}
		}

	}
	else {
		fprintf(stderr, "error read file");
		return -1;
	}
	return 0;
}


int main(int argc, char* argv[])
{
	
	if (argc < 2)
	{
		if (cat(STDIN_FILENO) != 0) {
			return -1;
		}
	} else {
        	for (int i = 1; i < argc; i++) {
			if (cat(open(argv[i], O_RDONLY)) != 0) {
				return -1;
			}
		}
	}
	return 0;
}

