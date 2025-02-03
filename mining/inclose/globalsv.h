#pragma once

#include "stdafx.h"

extern unsigned short g_output;
extern row_t *g_RW;
extern unsigned g_cont;
extern ofstream g_filebics;
extern ofstream g_filedtframe;

extern std::string g_datasetFilename;
extern std::string g_labelsFilename;
extern std::string g_thresholdFilename;
extern std::string g_outputFilename;
extern std::string g_summaryFilename;
extern std::string g_dtframeFilename;

// ----- Variables for the search using class labels -----
extern unsigned short *g_classes;
extern unsigned short g_maxLabel;
extern unsigned short g_cutOption;
extern unsigned short g_unavFlag;
extern unsigned short *g_writeFile;
extern unsigned short g_zdc;
extern double g_minZDC;
extern double *g_minConf;
extern row_t *g_contClassGeral;
extern row_t *g_contClassBic;
extern row_t *g_contClassBicUn;
extern unsigned g_contFails;
extern unsigned g_cont_conf;
extern unsigned g_contFails_conf;
extern unsigned g_cont_general;
extern unsigned g_contFails_general;
extern col_t *g_unav;
// -------------------------------------------------------
