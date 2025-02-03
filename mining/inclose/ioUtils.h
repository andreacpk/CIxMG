#pragma once

#include "stdafx.h"
#include "globalsv.h"

bool readDataset(const string &dataSetName, dataset_t &matrix, row_t &n, col_t &m);
void printData(const dataset_t &matrix, const row_t &n, const col_t &m);
void printArguments(char *argv[]);
bool readClassLabels(const string &fileName, const row_t &n);
bool readThresolds(const string &filename, row_t *minrows, col_t &minCol);
bool readConfig(const string &configfilename);
void printDataLoaded(row_t &n, col_t &m, row_t *minrows);
void printSummary(float tempo, col_t &minCol, row_t *minrows);