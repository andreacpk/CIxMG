#include "stdafx.h"
#include "globalsv.h"

unsigned short g_output = 2; // format of the output file: 1 - matlab; 2 - python
row_t *g_RW;  // // vector to store the extent of a possible formal concept (maximal bicluster of ones)
unsigned g_cont = 0; // number of biclusters in the output
ofstream g_filebics; // pointer to the output file
ofstream g_filedtframe; // pointer to the output file

std::string g_datasetFilename;
std::string g_labelsFilename;
std::string g_thresholdFilename;
std::string g_outputFilename;
std::string g_summaryFilename;
std::string g_dtframeFilename;

// ----- Variables for the search using class labels -----
unsigned short *g_classes; // vector to store the class label of each object
unsigned short g_maxLabel; // maximum label
unsigned short g_cutOption; // cut option
unsigned short g_unavFlag; // flag for using unavoidable transaction as part of the cut
unsigned short *g_writeFile; // vector to store the flag of printing file 0 - bicfile / 1 - dtframe / 2 - summary
unsigned short g_zdc; // name of the zdc measure
double g_minZDC = 0; // threshold of the ZDC measure
double *g_minConf; // threshold of the confidence
row_t *g_contClassGeral; // stores the number of samples of each class label
row_t *g_contClassBic; // stores the number of samples of each class label in a bicluster
row_t *g_contClassBicUn; // stores the number of unavoidable transactions
unsigned g_contFails = 0; // number of biclusters that were mined but discarded by zdc threshold
unsigned g_cont_conf = 0; // // number of biclusters in the output that reach the minimum confidence
unsigned g_contFails_conf = 0; // number of biclusters that were mined but discarded by confidence threshold
unsigned g_cont_general = 0; // // number of biclusters in the output that reach the minimum confidence and the minunimum zdc
unsigned g_contFails_general = 0; // number of biclusters that were mined but discarded by confidence and the minunimum zdc threshold
col_t *g_unav; // store the column from which the unavoidable transactions
// -------------------------------------------------------
