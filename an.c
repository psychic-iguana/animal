/*
 * Animal - copyright HCR Corporation, Toronto, Canada, 1989
 *
 * author: Stacey Campbell
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "scr.h"

#define YES_ANS 1
#define NO_ANS 0
#define BAD_FILE 0
#define BAD_OPTS 1
#define BOTTOM "$$"

typedef struct node_t
{
	char *question;
	struct node_t *yes_p;
	struct node_t *no_p;
} node_t;

static void
DumpAnimals(FILE * fp, node_t * node_p)
{
	if (node_p == 0)
		fprintf(fp, "%s\n", BOTTOM);
	else
	{
		fprintf(fp, "%s\n", node_p->question);
		DumpAnimals(fp, node_p->yes_p);
		DumpAnimals(fp, node_p->no_p);
	}
}

void
ToLower(char *str)
{
	while (*str)
	{
		if (*str >= 'A' && *str <= 'Z')
			*str += 'a' - 'A';
		str++;
	}
}

static int
YesNo(char *str)
{
	int ret_val;

	ToLower(str);
	if (strcmp(str, "yes") == 0 || strcmp(str, "y") == 0)
		ret_val = YES_ANS;
	else
		ret_val = NO_ANS;

	return ret_val;
}

static void
SaveAnimals(node_t * top)
{
	char fn[256];
	char buf[256];
	FILE *fp;

	if (YesNo(FileQuestion("Do want to save the data?")) == NO_ANS)
		return;
	do
	{
		strcpy(fn, FileQuestion("input a datafile name:"));
		if ((fp = fopen(fn, "w")) == NULL)
		{
			sprintf(buf, "Cannot open %s, try again?", fn);
			if (YesNo(FileQuestion(buf)) == NO_ANS)
				return;
		}
	}
	while (fp == NULL);
	DumpAnimals(fp, top);
	fclose(fp);
}

node_t *
GetNode(char *question)
{
	node_t *node_p;

	if ((node_p = (node_t *) malloc(sizeof(node_t))) == NULL)
		return NULL;
	node_p->question = malloc(strlen(question) + 2);
	strcpy(node_p->question, question);
	node_p->yes_p = 0;
	node_p->no_p = 0;
	return node_p;
}

static void
CollectAnimals(FILE * fp, node_t ** node_pp)
{
	static char read_buf[256];

	if (fgets(read_buf, 256, fp) == NULL)
		return;
	read_buf[strlen(read_buf) - 1] = '\0';
	if (strcmp(read_buf, BOTTOM) == 0)
		return;
	else
	{
		*node_pp = GetNode(read_buf);
		CollectAnimals(fp, &(*node_pp)->yes_p);
		CollectAnimals(fp, &(*node_pp)->no_p);
	}
}

static node_t *
LoadFile(char *fn)
{
	FILE *fp;
	node_t *top;

	if ((fp = fopen(fn, "r")) == NULL)
		return 0;
	CollectAnimals(fp, &top);
	fclose(fp);
	return top;
}

static void
Usage(int status, char *str)
{
	char buf[256];

	switch (status)
	{
	case BAD_FILE:
		sprintf(buf, "Cannot open datafile: %s", str);
		PutFileMsg(buf);
		break;
	case BAD_OPTS:
	default:
		sprintf(buf, "Usage: %s {data-file}", str);
		PutFileMsg(buf);
		break;
	}
	EndCurses();
	exit(1);
}

static node_t *
InitTop(void)
{
	node_t *top;
	char fn[256];

	if (YesNo(FileQuestion("Do you wish to read a datafile?")) == YES_ANS)
	{
		strcpy(fn, FileQuestion("Name of datafile?"));
		top = LoadFile(fn);
		if (top == 0)
			Usage(BAD_FILE, fn);
	}
	else
	{
		top = GetNode("was or is it a living organism");
		top->yes_p = GetNode("an ant");
		top->no_p = GetNode("a concrete brick");
	}
	return top;
}

static int
GetAnswer(void)
{
	char input[256];

	GetQuestLine("", input);
	return YesNo(input);
}

static void
NewAnimal(node_t * node_p)
{
	char animal[256];
	char question[256];
	char buf[256];
	node_t *old_object, *new_query, *new_object;

	old_object = (node_t *) malloc(sizeof(node_t));
	*old_object = *node_p;
	GetQuestLine("What are you thinking of?", animal);
	new_object = GetNode(animal);
	PutMsg("Type a question such that:");
	PutMsg("  (a) a yes or no answer would distinguish");
	sprintf(buf, "      between %s and %s", animal, node_p->question);
	PutMsg(buf);
	PutMsg("and");
	PutMsg("  (b) it does not explicitly mention either of these things.");
	GetQuestLine("question:", question);
	new_query = GetNode(question);
	*node_p = *new_query;

	sprintf(buf, "For %s the answer would be", animal);
	PutQuestion(buf);
	if (GetAnswer() == YES_ANS)
	{
		node_p->yes_p = new_object;
		node_p->no_p = old_object;
	}
	else
	{
		node_p->yes_p = old_object;
		node_p->no_p = new_object;
	}
}

static void
Query(node_t * node_p)
{
	if (node_p->yes_p)
	{
		PutQuestion(node_p->question);
		if (GetAnswer() == YES_ANS)
			Query(node_p->yes_p);
		else
			Query(node_p->no_p);
	}
	else
	{
		PutFinalQuestion(node_p->question);
		if (GetAnswer() == YES_ANS)
			PutMsg("Yay!");
		else
			NewAnimal(node_p);
	}
}

static int
KeepGoing(void)
{
	StartBold();
	PutQuestion("Another query");
	EndBold();
	return GetAnswer();
}

main(int argc, char *argv[])
{
	node_t *top;
	char buf[256];

	InitCurses();
	switch (argc)
	{
	case 1:
		top = InitTop();
		break;
	case 2:
		top = LoadFile(argv[1]);
		if (top == 0)
			Usage(BAD_FILE, argv[1]);
		sprintf(buf, "using datafile: %s", argv[1]);
		PutFileMsg(buf);
		break;
	default:
		Usage(BAD_OPTS, argv[0]);
		break;
	}

	do
	{
		PutMsg("Think of something...");
		Query(top);
	}
	while (KeepGoing() == YES_ANS);

	SaveAnimals(top);
	EndCurses();
	return 0;
}
