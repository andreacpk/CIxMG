#include "stdafx.h"
#include "globalsv.h"
#include "Utils.h"
#include "talkyg.h"

bool readDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m);
bool readConfigFile(const string &configfilename);
vector<item_t> createVerticalRepresentationWithFItems(const dataset_t &D, const row_t &n, const col_t &m, deque<pnode_t> *root);
bool readClassLabels(const string &fileName, const row_t &n);
bool readThresholdsFile(const string &fileName, const row_t &n);
void printSummary(float tempo);

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
	if (!readConfigFile(argv[1]))
	{
		cout << "\nConfig file was not loaded!";
		exit(1);
	}

	/*
	if (argc != 6)
	{
		cout << "\n!!! Wrong Arguments !!!" << endl << endl;
		cout << "List of the arguments:" << endl;
		cout << "1 - Dataset's filename;" << endl;
		cout << "2 - Name of the file with the minsup values for each class label;" << endl;
		cout << "3 - Output filename for the list of patterns;" << endl;
		cout << "4 - Class labels' filename;" << endl;
		cout << "5 - Minimum confidence [0,1];" << endl;
		exit(1);
	}
	*/

    /*
	if (!readConfigFile())
	{
		cout << "\nConfiguration file was not loaded.\n";
		cout << "Default configuration:" << endl;
		cout << "Value that represents a Missing Value: " << MVS << endl;
		cout << "Output format (1 - matlab; 2 - python): " << g_output << endl;
	}
	*/

    dataset_t matrix; // pointer to the dataset
	row_t n; // number of dataset's rows
	col_t m; // number of dataset's columns

	if (!readDataset(g_datasetFilename, matrix, n, m))
	{
		cout << "\nDataset was not loaded!";
		exit(1);
	}
	//printf("\nDataset loaded: %dx%d\n\n", n, m);

	g_classes = new unsigned short[n];

	// Read the class label of each sample
	if (!readClassLabels(g_labelsFilename, n))
	{
		cout << "\nClass labels' file was not loaded!";
		exit(1);
	}
	//printf("Class labels loaded\n\n");

	// List the user parameters
	/*
	cout << "\nArguments: " << endl;
	cout << "Dataset's filename: " << argv[1] << endl;
	cout << "minsup: " << argv[2] << endl;
	cout << "File with the list of patterns: " << argv[3] << endl;
	cout << "Class labels' filename: " << argv[4] << endl;
	cout << "Minimum confidence: " << g_minconf << endl;
	*/

	// Read the minsup of each class label
	if (!readThresholdsFile(g_thresholdFilename, n))
	{
		cout << "\nthresholds' file was not loaded!";
		exit(1);
	}
	//printf("thresholds loaded\n\n");

	cout << "Creating the vertical representation of the dataset..." << endl;
	deque<pnode_t> *root = new deque<pnode_t>;
	vector<item_t> items = createVerticalRepresentationWithFItems(matrix, n, m, root);
	//cout << "Number of Frequent Items = " << items.size() << endl;

	float tempo;
	openPrintFile();
	//cout << "\nRunning..." << endl;
	tempo = runTalkyG(items, root);
	closePrintFile();

	if (g_writeFile[2]) printSummary(tempo);

    /*
	cout << "\n\nRuntime(s): " << tempo << endl;
	cout << "Number of patterns = " << g_cont << endl;

	// Guarda qtd de bics e runtime num txt
	ofstream myfile;
	myfile.open("summary.txt", ofstream::app);
	myfile << g_cont << '\t';
	myfile << tempo << '\t';
	myfile << endl;
	myfile.close();
	*/

	//system("pause");
	return 0;
}

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

bool readConfigFile(const string &configfilename)
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

				} else if(file_t.compare("MVS") == 0){
					MVS = stoi(line.substr(found+1));

				} else if(file_t.compare("OUT") == 0){
					if ((line.substr(found+1)).compare("matlab") == 0){
						g_output = 1;
						cout << "matlab!" << endl;
					    
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

    /*
	unordered_map<string,string> params;
	ifstream myStream;
	string line;

	myStream.open("config.txt", ifstream::in);

	if (!myStream.is_open())
		return false;
		

	myStream.seekg(0);
	while (myStream.good())
	{
		getline(myStream, line);
		size_t found = line.find("=");
		if (found!=string::npos)
		{
			params[line.substr(0,found)] = line.substr(found+1);
		}
	}

	cout << "myparams contains:" << endl;
	for (auto& x: params)
		cout << x.first << ": " << x.second << endl;
	
	// Setando as variaveis com os parametros
	MVS = stoi(params["MVS"]);
	if (params["OUT"].compare("matlab") == 0)
		g_output = 1;
	else
		g_output = 2;

	myStream.close();
	return true;

	*/
}

// Creates the vertical representation of dataset, ignoring the infrequent items
vector<item_t> createVerticalRepresentationWithFItems(const dataset_t &D, const row_t &n, const col_t &m, deque<pnode_t> *root)
{
	row_t *support =  new row_t[g_maxLabel];
	unsigned short label;
	row_t biggerSup;

	col_t countItems = 0;
	vector<item_t> items;
	pair<data_t, row_t> *rwp = new pair<data_t, row_t>[n];
	for (col_t j = 0; j < m; ++j)
	{
		row_t qnmv = 0; // number of non-missing values
		for (row_t i = 0; i < n; ++i)
		{
			if (D[i][j] != MVS)
			{
				rwp[qnmv].first = D[i][j];
				rwp[qnmv].second = i;
				++qnmv;
			}
		}
		if (qnmv >= g_smallerMinsup)
		{
			sort(rwp, rwp + qnmv);
			row_t p1 = 0, p2 = 0;
			while (p1 <= qnmv - g_smallerMinsup)
			{
				biggerSup = 0;
				for (unsigned short i = 0; i < g_maxLabel; ++i) support[i] = 0; // initialize vector
				label = g_classes[rwp[p1].second];
				support[label] = 1;
				if (support[label] >= g_minsups[label])
				{
					if (support[label] > biggerSup) biggerSup = support[label];
				}

				while (p2 < qnmv - 1 && rwp[p2 + 1].first == rwp[p1].first)
				{
					++p2;
					label = g_classes[rwp[p2].second];
					support[label]++;
					if (support[label] >= g_minsups[label])
					{
						if (support[label] > biggerSup) biggerSup = support[label];
					}
				}
				row_t support = p2 - p1 + 1;
				if (biggerSup > 0)
				{
					item_t item;
					item.idx = j;
					item.value = rwp[p1].first;
					items.push_back(item);

					pnode_t node = new node_t;
					node->length = 1;
					node->idxItems = new col_t[1];
					node->idxItems[0] = countItems;
					node->sup = support;
					node->biggerSup = biggerSup;
					node->tidset = new row_t[node->sup];
					for (row_t i2 = p1; i2 <= p2; ++i2) node->tidset[i2 - p1] = rwp[i2].second;
					root->push_back(node);

					++countItems;
				}
				p1 = p2 + 1;
				p2 = p1;
			}
		}
	}

	/*
	cout << "Frequent Items:" << endl;
	for (col_t i = 0; i < items.size(); ++i)
	{
		cout << "Item " << i << ": ";
		cout << "idx = " << items[i].idx << ", ";
		cout << "value = " << items[i].value << endl;
	}
	cout << endl;
	cout << "root contains:" << endl;
	for (deque<pnode_t>::iterator it = root->begin(); it != root->end(); ++it)
	{
		cout << "idxItems = " << (*it)->idxItems[0] << ", ";
		cout << "sup = " << (*it)->sup << ", ";
		cout << "tidset = " ;
		for (row_t i = 0; i < (*it)->sup; ++i) cout << (*it)->tidset[i] << " ";
		cout << endl;
	}
	*/

	return items;
}

bool readClassLabels(const string &fileName, const row_t &n)
{
	// Read tha class label of each object, and
	// set g_maxLabel

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

	return true;
}

// Read the file with the minsup of each class label
bool readThresholdsFile(const string &fileName, const row_t &n)
{
	g_smallerMinsup = n;
	g_biggerMinsup = 0;
	g_minsups = new row_t[g_maxLabel];
	g_minconf = new double[g_maxLabel];
	unsigned short label;

	ifstream myStream;
	myStream.open(fileName, ifstream::in);

	if (!myStream.is_open())
		return false;
	

	//cout << "minsup for each class label: " << endl;
	myStream.seekg(0);
	while (myStream.good())
	{
	    for (unsigned short j = 0; j < g_maxLabel; ++j)
	    {
		    myStream >> label;
            myStream >> g_minsups[label];
            //cout << "Label " << label << ": " << g_minsups[label] << endl;
            if (g_minsups[label] < g_smallerMinsup) g_smallerMinsup = g_minsups[label];
            if (g_minsups[label] > g_biggerMinsup) g_biggerMinsup = g_minsups[label];

        }

        for (unsigned short j = 0; j < g_maxLabel; ++j)
	    {
		    myStream >> label;
            myStream >> g_minconf[label];
        }
	}

	myStream.close();

	//cout << "g_smallerMinsup = " << g_smallerMinsup << endl;
	//cout << "g_biggerMinsup = " << g_biggerMinsup << endl;

	return true;
}

void printSummary(float tempo)
{
	// store summary info
	ofstream myfile;
	myfile.open(g_summaryFilename, ofstream::app);
	// dataset filename
	myfile << "dataset filename = " << g_datasetFilename << endl;

	// minrows
	myfile << "minrows = ";
	for (row_t i = 0; i < g_maxLabel; ++i)
		myfile << g_minsups[i] << " ";
	myfile << endl;

	// minconfs
	myfile << "min conf = ";
	for (row_t i = 0; i < g_maxLabel; ++i)
		myfile << g_minconf[i] << " ";
	myfile << endl;

 	myfile << "mined = " << g_cont << endl;  //mined
	myfile << "runtime = " << tempo << endl;
	myfile.close();
}
