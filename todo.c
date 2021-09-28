#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>

#define SAVEFILE      "/tmp/.todo"
#define MAX           4096
#define ERROR         -1
#define RED           "\033[0;31m"
#define GREEN         "\033[0;32m"
#define ITALIC        "\033[0;3;2m"
#define NORMAL        "\033[0m"
#define STRIKETHROUGH "\033[0;2;9m"

#define X             'X'
#define WORK(TODO)    &TODO[1]
#define STATUS(TODO)  TODO[0]
#define DONE(TODO)    STATUS(TODO) == X
#define IS(A, B)      !strcmp(A, B)

void error(char *color, char *message)
{
	if (isatty(STDERR_FILENO))
		fprintf(stderr, "%s", color);
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

void print(char **todo)
{
	bool tty;

	if (!*todo)
		printf("%s%s\n", GREEN, "nothing to do for now");
	tty = isatty(STDOUT_FILENO);
	for (int i = 0; todo[i]; i++)
	{
		if (tty) printf("%s", ITALIC);
		printf("%d ", i);
		if (tty) printf("%s", DONE(todo[i]) ? STRIKETHROUGH : NORMAL);
		printf("%s\n", WORK(todo[i]));
	}
}

char **lines_from(int file)
{
	char **lines;
	char *token, content[MAX];
	char *next;
	int	bytes, result, count = 0;
	
	bytes = read(file, &content, MAX); 
	if (bytes == ERROR || bytes == MAX)
		error(RED, bytes == MAX ? "file too big" : strerror(errno));
	next = content;
	while (next && (next = strchr(next, '\n')))
		count++, next++;
	lines = malloc(sizeof(char *) * (count + 1));
	token = strtok(content, "\n");
	for (int i = 0; token; i++)
	{
		lines[i] = strdup(token);
		token = strtok(NULL, "\n");
	}
	lines[count] = NULL;
	return (lines);
}

void load(char ***todo, int *file)
{
	*file = open(SAVEFILE, O_RDONLY | O_CREAT, 0777);
	if (*file == ERROR)
		error(RED, strerror(errno));
	*todo = lines_from(*file);
}

void save(char **todo, int file)
{
	close(file);
	file = open(SAVEFILE, O_TRUNC | O_WRONLY, 0777);
	for (int i = 0; todo[i]; i++)
		dprintf(file, "%s\n", todo[i]);
}

void clear(char **todo)
{
	for (int i = 0; todo[i]; i++)
		free(todo[i]);
	*todo = NULL;
	print(todo);
}

int how_many(char **todo)
{
	int count = 0;

	while (todo[count])
		count++;
	return (count);
}

void assign(char *task, char **todo, int i)
{
	size_t length = strlen(task) + 1;

	todo[i] = malloc(sizeof(char) * (length + 1));
	STATUS(todo[i]) = ' ';
	strlcpy(WORK(todo[i]), task, length);
}

void add(char **argument, int count, char ***todo_)
{
	char **todo, **original = *todo_;
	int i, j, size = how_many(original);

	*todo_ = malloc(sizeof(char *) * (size + count + 1));
	todo = *todo_;
	for (i = 0; original[i]; i++)
		todo[i] = original[i];
	for (i = 0, j = size; argument[i]; i++, j++)
		assign(argument[i], todo, j);
	todo[j] = NULL;
	print(todo);
}

int	get_index(char *string)
{
	if (!isdigit(string[0]))
		error(RED, "invalid index");
	return (atoi(string));
}

void swap(char **argument, int count, char **todo)
{
	int a, b, last;
	char *todo_a;

	if (count != 2)
		error(RED, "swap takes only two indices as input");
	a = get_index(argument[0]);
	b = get_index(argument[1]);
	last = how_many(todo) - 1;
	if (a > last || b > last || a < 0 || b < 0)
		error(RED, "exeeding index range");
	todo_a = todo[a];
	todo[a] = todo[b];
	todo[b] = todo_a;
	print(todo);
}

void toggle(char **argument, char **todo)
{
	int index, last;

	last = how_many(todo) - 1;
	for (int i = 0; argument[i]; i++)	
	{
		index = get_index(argument[i]);
		if (index > last || index < 0)
			error(RED, "exeeding index range");
		todo[index][0] = todo[index][0] == X ? ' ' : X;
	}
	print(todo);
}

void remove_(char **argument, char **todo)
{
	int index, last;

	last = how_many(todo) - 1;
	for (int i = 0; argument[i]; i++)
	{
		index = get_index(argument[i]);
		if (index > last || index < 0)
			error(RED, "exeeding index range");
		free(todo[index]);
		for (int j = index; todo[j]; j++)
			todo[j] = todo[j+1];
	}
	print(todo);
}

void help()
{
	char *instruction = "\
usage: todo [<command>] [<args>]    \n\
                                    \n\
These are the only commands:        \n\
                                    \n\
            Add tasks               \n\
            Toggle indices' status  \n\
  ls        List all tasks          \n\
  rm        remove indices          \n\
  swap      Swap two indices' order \n\
  clear     Clear all tasks         \n\
";
	printf("%s", instruction);
}

#define COMMAND *argument
int	main(int count, char **argument)
{
	char **todo;
	int file;

	argument++, count--;
	if (COMMAND)
	{
		load(&todo, &file);
		if (IS(COMMAND, "ls"))
			print(todo);
		else if (IS(COMMAND, "swap"))
			swap(++argument, --count, todo);
		else if (IS(COMMAND, "rm"))
			remove_(++argument, todo);
		else if (IS(COMMAND, "clear"))
			clear(todo);
		else if (isdigit(**argument))
			toggle(argument, todo);
		else
			add(argument, count, &todo);
		save(todo, file);
	}
	else help();
}
