#include "stdafx.h"
#include "inclose.h"

float runInClose(const dataset_t &D, const row_t &n, const col_t &m, const row_t *minrows, const col_t &minCol)
{
	clock_t clocks = clock();
	g_RW = new row_t[n];

	// Creating the vector g_unav
	g_unav = new col_t[n];
	for (row_t i = 0; i < n; ++i)
	{
		col_t j;
		for (j = m; j > 0 && D[i][j-1]; --j);
		g_unav[i] = j;
	}

	// Creating the supremum
	pbic_t bic = new bic_t;
	bic->A = new row_t[n];
	for (row_t i = 0; i < n; ++i)
		bic->A[i] = i;
	bic->sizeA = n;
	bic->B = new bool[m];
	bic->Z = new bool[m];
	for (col_t i = 0; i < m; ++i)
	{
		bic->B[i] = false;
		bic->Z[i] = false;
	}
	bic->sizeB = 0;
	bic->col = 0;

	// call In-Close
	if (g_cutOption==0) InClose_CARM(D, m, minrows, minCol, bic, n);
        else if (g_cutOption==1 && g_unavFlag==1) InCloseMMR_Unv(D, m, minrows, minCol, bic, n); 
	else if (g_cutOption==1 && g_unavFlag==0) InCloseMMR(D, m, minrows, minCol, bic, n); 
	else if (g_cutOption==2 && g_unavFlag==1) InCloseZDC_Unv(D, m, minCol, bic, n);
	else if (g_cutOption==2 && g_unavFlag==0) InCloseZDC(D, m, minCol, bic, n); 

	clocks = clock() - clocks;
	return ((float)clocks) / CLOCKS_PER_SEC;
}

void InClose_CARM(const dataset_t &D, const col_t &m, const row_t *minrows, const col_t &minCol, const pbic_t &bic, const row_t &n)
{
	queue<pbic_t> children;
	
	// Iterating across the attributes
	for (col_t j = bic->col; j < m; ++j)
	{

		if (m - j + bic->sizeB < minCol)
			break;

		if (!bic->B[j] && !bic->Z[j])
		{

			cout << "\n Bic: ";
			for (row_t i = 0; i < bic->sizeA; ++i)
			{
				cout << bic->A[i] << " ";
			}
			// Computing RW
			row_t sizeRW = computingRW(D, bic, j);

			cout << "\nsizeRW" << sizeRW;
			
			// "Main routine"
			if (sizeRW == bic->sizeA)
			{
				bic->B[j] = true;
				++bic->sizeB;
			}
			else
			{
				row_t nUn = 0;
				countClassBicUn(g_RW, sizeRW, nUn, j);  // preenche variaveis globais contClassBic e contClassBicUn
				pair <double,double> zdc = getZDC(n, 'u', nUn, sizeRW);

				if (ReachMinRows(minrows) && zdc.second >= g_minZDC)
				{
					cout << "\npassou minrow ";
					for (row_t i = 0; i < g_maxLabel; ++i)
					{
						cout << g_contClassBic[i] << " ";
					}

					if (ReachMinConfUnavoid(nUn) && zdc.first >= g_minZDC)
					{
						cout << "\npassou T2 ";
						cout << "\nT2 = " << zdc.first << "\nT3 = " << zdc.second << " ";
						canonicity_test(D, j, sizeRW, bic, children);
					}
				}
				else bic->Z[j] = true;
			}
		}
	}

	// Print the formal concept and deallocate the memory of its extent
	if (bic->sizeB >= minCol) countAndPrintBic(bic, m, n);
	delete[] bic->A;

	// Closing the children
	while (!children.empty())
	{
		pbic_t child = children.front();
		closeChild(child, bic, m);
		InClose_CARM(D, m, minrows, minCol, child, n);
		children.pop();
	}
	delete[] bic->B;
	delete[] bic->Z;
	delete bic;
}

void InCloseMMR_Unv(const dataset_t &D, const col_t &m, const row_t *minrows, const col_t &minCol, const pbic_t &bic, const row_t &n)
{
	queue<pbic_t> children;
	
	// Iterating across the attributes
	for (col_t j = bic->col; j < m; ++j)
	{

		if (m - j + bic->sizeB < minCol)
			break;

		if (!bic->B[j] && !bic->Z[j])
		{
			// Computing RW
			row_t sizeRW = computingRW(D, bic, j);
			
			// "Main routine"
			if (sizeRW == bic->sizeA)
			{
				bic->B[j] = true;
				++bic->sizeB;
			}
			else
			{
				row_t nUn = 0;
				countClassBicUn(g_RW, sizeRW, nUn, j);  // preenche variaveis globais contClassBic e contClassBicUn
                                pair <double,double> zdc = getZDC(n, 'u', nUn, sizeRW);

				if (ReachMinRows(minrows))
				{
					if (zdc.first >= g_minZDC)
					{
						canonicity_test(D, j, sizeRW, bic, children);
					}
				}
				else bic->Z[j] = true;
			}
		}
	}

	// Print the formal concept and deallocate the memory of its extent
	if (bic->sizeB >= minCol) countAndPrintBic(bic, m, n);
	delete[] bic->A;

	// Closing the children
	while (!children.empty())
	{
		pbic_t child = children.front();
		closeChild(child, bic, m);
		InCloseMMR_Unv(D, m, minrows, minCol, child, n);
		children.pop();
	}
	delete[] bic->B;
	delete[] bic->Z;
	delete bic;
}

void InCloseMMR(const dataset_t &D, const col_t &m, const row_t *minrows, const col_t &minCol, const pbic_t &bic, const row_t &n)
{
	queue<pbic_t> children;
	
	// Iterating across the attributes
	for (col_t j = bic->col; j < m; ++j)
	{

		if (m - j + bic->sizeB < minCol)
			break;

		if (!bic->B[j] && !bic->Z[j])
		{
			// Computing RW
			row_t sizeRW = computingRW(D, bic, j);

			// "Main routine"
			if (sizeRW == bic->sizeA)
			{
				bic->B[j] = true;
				++bic->sizeB;
			}
			else
			{
				row_t nUn = 0;
				countClassBicUn(g_RW, sizeRW, nUn, j);  // preenche variaveis globais contClassBic e contClassBicUn

				if (ReachMinRows(minrows)) canonicity_test(D, j, sizeRW, bic, children);
				else bic->Z[j] = true;
			}
		}
	}

	// Print the formal concept and deallocate the memory of its extent
	if (bic->sizeB >= minCol) countAndPrintBic(bic, m, n);
	delete[] bic->A;

	// Closing the children
	while (!children.empty())
	{
		pbic_t child = children.front();
		closeChild(child, bic, m);
		InCloseMMR(D, m, minrows, minCol, child, n);
		children.pop();
	}
	delete[] bic->B;
	delete[] bic->Z;
	delete bic;
}

void InCloseZDC_Unv(const dataset_t &D, const col_t &m, const col_t &minCol, const pbic_t &bic, const row_t &n)
{
	queue<pbic_t> children;
	
	// Iterating across the attributes
	for (col_t j = bic->col; j < m; ++j)
	{

		if (m - j + bic->sizeB < minCol)
			break;

		if (!bic->B[j] && !bic->Z[j])
		{
			// Computing RW
			row_t sizeRW = computingRW(D, bic, j);
			
			// "Main routine"
			if (sizeRW == bic->sizeA)
			{
				bic->B[j] = true;
				++bic->sizeB;
			}
			else
			{
				row_t nUn = 0;
				countClassBicUn(g_RW, sizeRW, nUn, j);  // preenche variaveis globais contClassBic e contClassBicUn
					
				pair <double,double> zdc = getZDC(n, 'u', nUn, sizeRW);
				
				if (zdc.second >= g_minZDC)
				{
					if (zdc.first >= g_minZDC)
					{
						canonicity_test(D, j, sizeRW, bic, children);
					}
				}
				else bic->Z[j] = true;
			}
		}
	}

	// Print the formal concept and deallocate the memory of its extent
	if (bic->sizeB >= minCol) countAndPrintBic(bic, m, n);

	delete[] bic->A;

	// Closing the children
	while (!children.empty())
	{
		pbic_t child = children.front();
		closeChild(child, bic, m);
		InCloseZDC_Unv(D, m, minCol, child, n);
		children.pop();
	}
	delete[] bic->B;
	delete[] bic->Z;
	delete bic;
}

void InCloseZDC(const dataset_t &D, const col_t &m, const col_t &minCol, const pbic_t &bic, const row_t &n)
{
	queue<pbic_t> children;
	bool reach = true;
	
	// Iterating across the attributes
	for (col_t j = bic->col; j < m; ++j)
	{

		if (m - j + bic->sizeB < minCol)
			break;

		if (!bic->B[j] && !bic->Z[j])
		{
			// Computing RW
			row_t sizeRW = computingRW(D, bic, j);
			
			// "Main routine"
			if (sizeRW == bic->sizeA)
			{
				bic->B[j] = true;
				++bic->sizeB;
			}
			else
			{
				row_t nUn = 0;
				countClassBicUn(g_RW, sizeRW, nUn, j);  // preenche variaveis globais contClassBic e contClassBicUn
					
				pair <double,double> zdc = getZDC(n, 'u', nUn, sizeRW);
				
				if (zdc.second >= g_minZDC) 
					canonicity_test(D, j, sizeRW, bic, children);
				else bic->Z[j] = true;
			}
		}
	}

	// Print the formal concept and deallocate the memory of its extent
	if (bic->sizeB >= minCol) countAndPrintBic(bic, m, n);

	delete[] bic->A;

	// Closing the children
	while (!children.empty())
	{
		pbic_t child = children.front();
		closeChild(child, bic, m);
		InCloseZDC(D, m, minCol, child, n);
		children.pop();
	}
	delete[] bic->B;
	delete[] bic->Z;
	delete bic;
}

void canonicity_test(const dataset_t &D, const row_t &j, const row_t &sizeRW, const pbic_t &bic, queue<pbic_t> &children)
{
	col_t fcol;
	if (IsCanonical(D, j, sizeRW, bic, fcol))
	{
		//cout << "\nis canonical";
		pbic_t child = new bic_t;
		createChild(child, sizeRW, j);
		children.push(child);
	}
	else if (fcol < bic->col) bic->Z[j] = true;
}

row_t computingRW(const dataset_t &D, const pbic_t &bic, const row_t &j)
{
	row_t sizeRW = 0;
	for (row_t i = 0; i < bic->sizeA; ++i)
	{
		if (D[bic->A[i]][j])
			g_RW[sizeRW++] = bic->A[i];
	}
	return sizeRW;
}

void createChild(pbic_t &child, const row_t &sizeRW, const row_t &j)
{
	child->sizeA = sizeRW;
	child->A = new row_t[sizeRW];
	for (row_t i = 0; i < sizeRW; ++i)
		child->A[i] = g_RW[i];
	child->col = j + 1;
}

void closeChild(pbic_t &child, const pbic_t &bic, const col_t &m)
{
	child->B = new bool[m];
	child->Z = new bool[m];
	for (col_t j = 0; j < m; ++j)
	{
		child->B[j] = bic->B[j];
		child->Z[j] = bic->Z[j];
	}
	child->B[child->col - 1] = true;
	child->sizeB = bic->sizeB + 1;
}

bool IsCanonical(const dataset_t &D, const col_t &y, const row_t &sizeRW, const pbic_t &bic, col_t &fcol)
{
	row_t i;
	for (col_t j = 0; j < y; ++j)
	{
		if (!bic->B[j] && !bic->Z[j])
		{
			for (i = 0; i < sizeRW; ++i)
				if (!D[g_RW[i]][j])
					break;
			fcol = j; // if IsCanonical==false, it is used to keep the column where the test fail
			if (i == sizeRW)
				return false;
		}
	}
	return true;
}

bool ReachMinRows(const row_t *minrows)
{
	unsigned short count = 0;
	for (unsigned short i = 0; i < g_maxLabel; ++i)
	{
		if (g_contClassBic[i] < minrows[i])
			count++;
	}

	if(g_maxLabel == count)
		return false;

	return true;
}

bool ReachMinConfUnavoid(const row_t &nUn)
{
	double conf;
	for (unsigned short i = 0; i < g_maxLabel; ++i)
	{
		row_t ib, ob;
		ib = g_contClassBic[i]; // number of samples of label i in the bicluster
		ob = nUn - g_contClassBicUn[i];
		double t = ib + ob;

		conf = Confidence(ib, t);

		if (conf >= g_minConf[i]) 
			return true;
	}

	return false;
}

