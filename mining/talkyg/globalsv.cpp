#include "stdafx.h"
#include "globalsv.h"

data_t MVS = 0; // value that represents a Missing Value
unsigned short g_output = 1; // format of the output file: 1 - matlab; 2 - python

unsigned g_cont = 0; // number of patterns in the output

unordered_map<string, forward_list<itemset_t>> g_st; // store the mined itemsets to avoid redundancy

// ----- Variables for the search using class labels -----
unsigned short *g_classes; // vector to store the class label of each object
unsigned short g_maxLabel; // maximum label
double *g_minconf; // vector of minimum confidence thresholds
row_t *g_minsups; // vector to store the minsup of each class label
row_t g_smallerMinsup, g_biggerMinsup; // smaller and bigger minsup in *g_minsups

// ----- Variables for the input and output files -----
unsigned short *g_writeFile;

ofstream g_filebics; // pointer to the output file
ofstream g_filedtframe;

std::string g_datasetFilename;
std::string g_labelsFilename;
std::string g_thresholdFilename;
std::string g_outputFilename;
std::string g_summaryFilename;
std::string g_dtframeFilename;
// --------------------------------------------------------

