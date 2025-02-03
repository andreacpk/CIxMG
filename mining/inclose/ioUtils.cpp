#include "stdafx.h"
#include "ioUtils.h"


bool readDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m)
{
	ifstream myStream;
	myStream.open(dataSetName, ifstream::in);

	if (!myStream.is_open())
		return false;

	//Discovering the number of rows
	n = count(istreambuf_iterator<char>(myStream), istreambuf_iterator<char>(), '\n');

	//Discovering the number of columns
	data_t dbltmp;
	string line;
	m = 0;
	myStream.seekg(0);
	getline(myStream, line);
	stringstream stream(line);
	while (stream.good())
	{
		stream >> dbltmp;
		++m;
	}

	//Allocating memory
	matrix = new data_t*[n];
	for (row_t i = 0; i < n; ++i)
		matrix[i] = new data_t[m];

	//Storing the data
	myStream.seekg(0);
	for (row_t i = 0; i < n; ++i)
	{
		for (col_t j = 0; j < m; ++j)
			myStream >> matrix[i][j];
	}

	myStream.close();
	return true;
}

void printDataLoaded(row_t &n, col_t &m, row_t *minrows)
{
	printf("\nDataset loaded: %dx%d\n", n, m);
	//printData(matrix, n, m);

	printf("Class labels loaded: ");
	for (row_t i = 0; i < g_maxLabel; ++i)
		cout << g_contClassGeral[i] << " ";

	printf("\nThresholds loaded:\n");
	for (unsigned short j = 0; j < g_maxLabel; ++j)
		cout << "class " << j << ": minRow = " << minrows[j] << " minZDC = " << g_minConf[j] << '\n';

	cout << "minZDC = " << g_minZDC << "\n";
}

void printSummary(float tempo, col_t &minCol, row_t *minrows)
{
	// store summary info
	ofstream myfile;
	myfile.open(g_summaryFilename, ofstream::app);
	// dataset filename
	myfile << "dataset filename = " << g_datasetFilename << endl;
	// class distribution
	myfile << "class distribution = ";
	for (row_t i = 0; i < g_maxLabel; ++i)
		myfile << g_contClassGeral[i] << " ";
	myfile << endl;

	// minrows
	myfile << "minrows = ";
	for (row_t i = 0; i < g_maxLabel; ++i)
		myfile << minrows[i] << " ";
	myfile << endl;

	myfile << "min col = " << minCol << endl;

	// minconfs
	myfile << "min conf = ";
	for (row_t i = 0; i < g_maxLabel; ++i)
		myfile << g_minConf[i] << " ";
	myfile << endl;

	myfile << "min zdc = " << g_minZDC << endl;

 	myfile << "mined = " << g_cont+g_contFails << endl;  //mined
	myfile << "pass zdc = " << g_cont << endl;              // pass zdc
	myfile << "pass conf = " << g_cont_conf << endl;         // pass conf
	myfile << "pass zdc and conf = " << g_cont_general << endl;      // pass zdc and conf
	myfile << "runtime = " << tempo << endl;
	myfile.close();
}

void printArguments(char *argv[])
{
	cout << "\nArguments: " << endl;
	cout << "Dataset's filename: " << g_datasetFilename << endl;
	cout << "minCol: " << atoi(argv[3]) << endl;


	if(atoi(argv[6]) == 1)
	{
		cout << "Cut option: sup and conf";
	} else if (atoi(argv[6]) == 2)
	{
		cout << "Cut option: zdc";
	} else if (atoi(argv[6]) == 0)
        {
                cout << "Cut option: In-Close5_CARM";
        } else
	{
		cout << "\nInvalid cut option.";
		exit(1);
	}
}

void printData(const dataset_t &matrix, const row_t &n, const col_t &m)
{
	for (row_t i = 0; i < n; ++i)
	{
		for (col_t j = 0; j < m; ++j)
			cout << matrix[i][j] << '\t';
		cout << endl;
	}
}

bool readConfig(const string &configfilename)
{

	ifstream file(configfilename);
	if (file.is_open())
	{
		size_t found;
		std::string file_t, line;
		while(getline(file, line))
		{
			found = line.find("=");
			if (found!=string::npos)
			{
				file_t = line.substr(0,found);
				if (file_t.compare("BICFILE") == 0){
					g_outputFilename = line.substr(found+1);
					g_writeFile[0] = 1;

				} else if(file_t.compare("DTFRAME") == 0){
					g_dtframeFilename = line.substr(found+1);
					g_writeFile[1] = 1;

				} else if(file_t.compare("SUMMARY") == 0){
					g_summaryFilename = line.substr(found+1);
					g_writeFile[2] = 1;

				} else if(file_t.compare("DATASET") == 0){
					g_datasetFilename = line.substr(found+1);

				} else if(file_t.compare("LABELS") == 0){
					g_labelsFilename = line.substr(found+1);

				} else if(file_t.compare("THRESHOLDS") == 0){
					g_thresholdFilename = line.substr(found+1);

				} else if(file_t.compare("CUTOPTION") == 0){
					g_cutOption = stoi(line.substr(found+1));

				} else if(file_t.compare("USEUNAVOIDABLE") == 0){
					g_unavFlag = stoi(line.substr(found+1));

				} else if(file_t.compare("ZDC") == 0){
					g_zdc = stoi(line.substr(found+1));

				} else if(file_t.compare("OUT") == 0){
					if ((line.substr(found+1)).compare("matlab") == 0){
						g_output = 1;
					    
					}else
						g_output = 2;

				} else{
					cout << "Wrong tag in config file!" << endl;
					return false;
				}
			}
		}

		return true;
	}
	return false;
}
bool readThresolds(const string &filename, row_t *minrows, col_t &minCol)
{
	ifstream myStream;
	myStream.open(filename, ifstream::in);

	if (!myStream.is_open())
		return false;

	//Storing the minrow thresholds
	for (unsigned short j = 0; j < g_maxLabel; ++j)
	{
		myStream >> minrows[j];

	}
	myStream >> minCol;
	//Storing the minzdc thresholds
	for (unsigned short j = 0; j < g_maxLabel; ++j)
	{
		myStream >> g_minConf[j];

	}

	myStream >> g_minZDC;


	myStream.close();
	return true;
}

bool readClassLabels(const string &fileName, const row_t &n)
{
	// Read tha class label of each object, and
	// set g_maxLabel, and g_contClassGeral, and allocate g_contClassBic

	g_maxLabel = 0;

	ifstream myStream;
	myStream.open(fileName, ifstream::in);

	if (!myStream.is_open())
		return false;

	//Storing the data
	myStream.seekg(0);
	for (row_t i = 0; i < n; ++i)
	{
		myStream >> g_classes[i];
		if (g_classes[i] > g_maxLabel) g_maxLabel = g_classes[i];
	}

	myStream.close();
	++g_maxLabel;


	g_contClassGeral = new row_t[g_maxLabel];
	for (row_t i = 0; i < g_maxLabel; ++i) g_contClassGeral[i] = 0; // initialize vector
	for (row_t i = 0; i < n; ++i) ++g_contClassGeral[ g_classes[i] ]; // counting the representativeness of each class label
	g_contClassBic = new row_t[g_maxLabel];
	g_contClassBicUn = new row_t[g_maxLabel];

	return true;
}
