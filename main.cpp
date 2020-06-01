#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "Window.h"


void printHelpMessage()
{
	printf("Launch options: GraphDrawer.exe [input_file_name] [-x]\n");
	printf("Where:\n");
	printf("\t -x -- read and draw x values on axis;\n");
	printf("\t -h -- print this help message and exit;\n");
}

char strEqual(const char* str1, const char* str2)
{
	char res;
	int i;
	res = 1;
	i = 0;
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
		{
			res = 0;
			break;
		}
		i++;
	}

	if (str1[i] != '\0' || str2[i] != '\0') //	prejdevremenno vyshel iz cikla
		res = 0;

	return res;
}

/*
Return values:
	0 - OK;
	1 - file not found;
	2 - error on reading or end of file reached
*/
int ReadInput(char* inFileName, int *graphCounts, int **sizes, char** withArgs, double ***G, double ***X)
{
	int i, j, succReadCount;
	FILE *in;
	double temp;
	in = fopen(inFileName, "r");
	if (in == NULL)
		return 1;

	succReadCount = fscanf(in, "%d", graphCounts);
	if (succReadCount != 1)
		return 2;

	*sizes = (int*)malloc(*graphCounts * sizeof(int));
	*withArgs = (char*)malloc(*graphCounts * sizeof(char));
	*G = (double**)malloc(*graphCounts * sizeof(double*));
	*X = (double**)malloc(*graphCounts * sizeof(double*));
	for (j = 0; j < *graphCounts; j++)
	{
		succReadCount = fscanf(in, "%d %d", (*sizes + j), (*withArgs + j));
		if (succReadCount != 2)
			return 2;

		(*G)[j] = (double*)malloc((*sizes)[j] * sizeof(double));
		i = 0;
		while (i < (*sizes)[j] && fscanf(in, "%lf", &temp) != EOF)
		{
			(*G)[j][i] = temp;
			i++;
		}
		if (feof(in) && i < (*sizes)[j])
			return 2;

		if ((*withArgs)[j])
		{
			(*X)[j] = (double*)malloc((*sizes)[j] * sizeof(double));
			i = 0;
			while (i < (*sizes)[j] && fscanf(in, "%lf", &temp) != EOF)
			{
				(*X)[j][i] = temp;
				i++;
			}
			if (feof(in) && i < (*sizes)[j])
				return 2;
		}
		/*for (i = 0; i < (*sizes)[j]; i++)
		{
			fscanf(in, "%lf", &temp);
			(*G)[j][i] = temp;
		}
		if ((*withArgs)[j])
		{
			(*X)[j] = (double*)malloc((*sizes)[j] * sizeof(double));
			for (i = 0; i < (*sizes)[j]; i++)
			{
				fscanf(in, "%lf", &temp);
				(*X)[j][i] = temp;
			}
		}*/
	}

	fclose(in);
	return 0;
}

/*
Return values:
	0 - OK;
	1 - input file not found;
	2 - Got wrong input;
*/
int main(int argc, char** argv)
{
	int i, j, k, errCode, retCode;
	int graphCounts;
	int *sizes;						// amount of points of each graph
	double **graphs;				// array of graphs
	double **Args;					// array of arguments of each graph
	char* withArgs;					// array of flags that signs, does this graph have arguments
	Window Win(0, 0, 500, 500);
	
	retCode = 0;
	//char* inFile = "input.txt";
	char *inFile = NULL;
	srand(time(NULL));

	// interface variables
	char  flag_printHelp = 0, flag_Xvalues;
	//*******************

	if (argc == 1)
	{
		printHelpMessage();
		goto end;
	}

	// interface code
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strEqual(argv[i] + 1, "x"))
				flag_Xvalues = 1;
			else if (strEqual(argv[i] + 1, "h") || strEqual(argv[i] + 1, "?"))
			{
				flag_printHelp = 1;
				printHelpMessage();
				goto end;
			}
			else
			{
				printf("Unknown parameter");
				return 2;
			}
		}
		else
		{
			if (inFile == NULL)
				inFile = argv[i];
		}
	}

	if (inFile == NULL)
		inFile = "input.txt";
	
	/************* Reading input data ********************/
	//printf("infile = %s\n", inFile);
	errCode = ReadInput(inFile, &graphCounts, &sizes, &withArgs, &graphs, &Args);
	if (errCode == 1)
	{
		printf("File not found!!!!\n");
		retCode = 1;
		goto end;
	}
	if (errCode == 2)
	{
		printf("Error on reading file or got wrong input.\n");
		retCode = 2;
		goto end;
	}
	/*printf("graphCounts = %d\n", graphCounts);
	for (k = 0; k < graphCounts; k++)
	{
		printf("********************************\n");
		printf("Size = %d\n", sizes[k]);
		for (i = 0; i < sizes[k]; i++)
		{
			printf("%.5lf\n", graphs[k][i]);
		}
		printf("\n");
	}*/
	Win.Show();
	for (i = 0; i < graphCounts; i++)
	{
		if (withArgs[i])
			Win.AddGraph(Args[i], graphs[i], sizes[i], RGB(rand() % 200, rand() % 200, rand() % 200)); // %200 - for ignoring white
		else
			Win.AddGraph(graphs[i], sizes[i], RGB(rand() % 200, rand() % 200, rand() % 200));
	}
	Win.DrawAxis();
	Win.Redraw(0, 0, 100, 100); //  sdvig X, sdvig Y, maschtab X, maschtab Y

	Win.StartEventSystem();

end:
	//system("echo 'Hahahaha'");
	//getchar();
	return retCode;
}