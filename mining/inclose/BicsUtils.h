#pragma once

#include "stdafx.h"
#include "globalsv.h"

void openPrintFile();
void printBic(const pbic_t &bic, const col_t m, const row_t &n, pair <double,double> zdc);
void printBic2(const pbic_t &bic, const col_t m, const row_t &n, pair <double,double> zdc);
void printDtframe(const pbic_t &bic, const row_t &n, pair <double,double> zdc);
void closePrintFile();
pair <double,double> getZDC(const row_t &n, const char &option, const row_t &nUn, const row_t &sizeRW);
double chi_squared_multi (const row_t *contClassBic, const row_t &n);
void countClassBicUn(const row_t *A, const row_t &sizeA, row_t &nUn, const col_t &col);
double chi_squared (const row_t &p, const row_t &n, const row_t &pt, const row_t &nt);
bool compareBics(const pbic_t &bic1, const pbic_t &bic2, const row_t &sizeBic);
double Confidence(const row_t &ib, const double &total);
void countAndPrintBic(const pbic_t &bic, const col_t m, const row_t &n);
bool ReachMinConf(const double &total);
