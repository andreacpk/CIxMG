#pragma once

#include "stdafx.h"
#include "globalsv.h"

void openPrintFile();
void printPattern(const pnode_t pattern, const vector<item_t> &items);
void closePrintFile();
