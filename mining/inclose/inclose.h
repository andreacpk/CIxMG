#pragma once

#include "stdafx.h"
#include "globalsv.h"
#include "BicsUtils.h"

float runInClose(const dataset_t &D, const row_t &n, const col_t &m, const row_t *minrows, const col_t &minCol);
void InClose_CARM(const dataset_t &D, const col_t &m, const row_t *minrows, const col_t &minCol, const pbic_t &bic, const row_t &n);
void InCloseMMR_Unv(const dataset_t &D, const col_t &m, const row_t *minrows, const col_t &minCol, const pbic_t &bic, const row_t &n);
void InCloseMMR(const dataset_t &D, const col_t &m, const row_t *minrows, const col_t &minCol, const pbic_t &bic, const row_t &n);
void InCloseZDC_Unv(const dataset_t &D, const col_t &m, const col_t &minCol, const pbic_t &bic, const row_t &n);
void InCloseZDC(const dataset_t &D, const col_t &m, const col_t &minCol, const pbic_t &bic, const row_t &n);
bool IsCanonical(const dataset_t &D, const col_t &y, const row_t &sizeRW, const pbic_t &bic, col_t &fcol);
void canonicity_test(const dataset_t &D, const row_t &j, const row_t &sizeRW, const pbic_t &bic, queue<pbic_t> &children);
bool ReachMinRows(const row_t *minrows);
bool ReachMinConfUnavoid(const row_t &nUn);
row_t computingRW(const dataset_t &D, const pbic_t &bic, const row_t &j);
void createChild(pbic_t &child, const row_t &sizeRW, const row_t &j);
void closeChild(pbic_t &child, const pbic_t &bic, const col_t &m);
