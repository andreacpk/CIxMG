#include "stdafx.h"
#include "globalsv.h"
#include "inclose.h"
#include "BicsUtils.h"
#include "ioUtils.h"
#include <string>
#include <math.h>

using namespace std;

void calcMinRows(const row_t &n, row_t *minrows, const double minZDC);


int main(int argc, char* argv[])
{

	if (argc != 2)
	{
		cout << "\n!!! Wrong Arguments !!!" << endl << endl;
		cout << "List of the arguments:" << endl;
		cout << "1) Configuration's filename;" << endl;
		exit(1);
	}

	g_writeFile = new unsigned short[3]; // number of output files
	if (!readConfig(argv[1]))
	{
		cout << "\nConfig file was not loaded!";
		exit(1);
	}
/*
	if (argc != 10)
	{
		cout << "\n!!! Wrong Arguments !!!" << endl << endl;
		cout << "List of the arguments:" << endl;
		cout << "1) Dataset's filename;" << endl;
                cout << "2) Filename of Thresholds;" << endl;
		cout << "3) minCol;" << endl;
		cout << "4) Output filename for the list of biclusters;" << endl;
		cout << "5) Class labels' filename;" << endl;
		cout << "6) Cut option: 1 - sup and conf or 2 - zdc;" << endl;
		cout << "7) Use Unavoidable Transactions: 0 - No or 1 - Yes;" << endl;
		cout << "8) ZDC: 1 - chi-square ;" << endl;
		cout << "9) Write in File: 0 - No or 1 - Yes ;" << endl;
		exit(1);
	}
*/
	dataset_t matrix; // pointer to the dataset
	row_t n; // number of dataset's rows
	col_t m; // number of dataset's columns
	row_t *minrows = NULL;   // pointer to the minrows
	col_t minCol;
	
        /*
	g_cutOption = atoi(argv[6]);
	g_unavFlag = atoi(argv[7]);
	g_zdc = atoi(argv[8]);
	*/

	float tempo;


	if (!readDataset(g_datasetFilename, matrix, n, m))
	{
		cout << "\nDataset was not loaded!";
		exit(1);
	}
	

	// Le as classes dos objetos
	g_classes = new unsigned short[n];

	if (!readClassLabels(g_labelsFilename, n))
	{
		cout << "\nClass labels' file was not loaded!";
		exit(1);
	}

	minrows = new row_t[g_maxLabel];
	g_minConf = new double[g_maxLabel];

	//calcMinRows(n, minrows, minzdcs[0]);
        if (!readThresolds(g_thresholdFilename, minrows, minCol))
	{
		cout << "File with the minrows was not loaded!";
		exit(1);
	}

	//printArguments(argv);
	//printDataLoaded(n, m, minrows);

    //cout << m << " cols" << endl;

	openPrintFile();
	//cout << "\nRunning..." << endl;
	tempo = runInClose(matrix, n, m, minrows, minCol);
	closePrintFile();

	if (g_writeFile[2]) printSummary(tempo, minCol, minrows);


	/*
	cout << "\nRuntime = " << tempo << endl;
        cout << "Number of biclusters mined = " << g_cont+g_contFails << endl;
	cout << "Number of biclusters in the output by zdc threshold = " << g_cont << endl;
	cout << "Number of discarded biclusters by zdc threshold = " << g_contFails << endl;
	cout << "Number of biclusters in the output by confidence threshold = " << g_cont_conf << endl;
	cout << "Number of discarded biclusters by confidence threshold = " << g_contFails_conf << endl;
	*/



	//cout << tempo << " " << g_cont << " " << g_contFails << " " << g_cont_conf << " " << g_contFails_conf << " " << endl;
	/*
	cout << "insert into input_param values (NULL, '" << argv[1] << "', " << n << ", " << m << ", " << g_maxLabel << ", NULL, '";
	for (unsigned short j = 0; j < g_maxLabel; ++j) cout << minrows[j] << " ";
	cout << "', " <<  minCol << ", '" << argv[4] << "', '" << argv[5] << "', '";
	for (row_t i = 0; i < g_maxLabel; ++i) cout << g_contClassGeral[i] << " ";
	cout << "', " << g_cutOption << ", " << g_minZDC << ", '";
	for (row_t i = 0; i < g_maxLabel; ++i) cout << g_minConf[i] << " ";
	cout << "', " << "'onevsAll', " << g_unavFlag << ", " << g_zdc << ", " << g_cont << ", " << g_contFails << ", " << g_cont_conf << ", " << g_contFails_conf << ", 1, " << tempo << ", '" <<argv[1] << "-onevsAll-"<< g_zdc << "-"<< g_minZDC << "-";
	for (row_t i = 0; i < g_maxLabel; ++i) cout << g_minConf[i] << "-";
	cout << g_cutOption << "-delljulia-False" << "', 'delljulia', False, NULL);" << endl;
	*/
	return 0;
}

void calcMinRows(const row_t &n, row_t *minrows)
{

	double t;
	double theta = g_minZDC*n;
	for (row_t i = 0; i < g_maxLabel; ++i)
	{
		t = pow(n,2) - double (g_contClassGeral[i]) * double (n) + theta * double (g_contClassGeral[i]);
		minrows[i] = ceil (theta * double(n) / t * g_contClassGeral[i]);
		
	}
}
