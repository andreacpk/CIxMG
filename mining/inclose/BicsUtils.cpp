#include "stdafx.h"
#include "BicsUtils.h"
#include <math.h>

void openPrintFile()
{
	if (g_writeFile[0]) g_filebics.open(g_outputFilename);
	if (g_writeFile[1]) g_filedtframe.open(g_dtframeFilename);

	if (g_output == 2) // python
	{
		g_filebics << "#!/usr/bin/env python3" << endl << endl;
		g_filebics << "bics = []" << endl;
	}
}

void printBic(const pbic_t &bic, const col_t m, const row_t &n, pair <double,double> zdc)
{

	if (g_output == 1) // matlab
            {
                g_filebics << "A{" << g_cont << "} = [";
		for (row_t i = 0; i < bic->sizeA; ++i)
			g_filebics << bic->A[i] << " ";
		g_filebics << "];\nB{" << g_cont << "} = [";

		for (col_t i = 0; i < m; ++i)
		{
			if (bic->B[i])
				g_filebics << i << " ";
		}
		g_filebics << "];\n";
            }
            else // python
            {
                g_filebics << "bics.append([[";
                for (row_t i = 0; i < bic->sizeA; ++i)
                    g_filebics << bic->A[i]  << ",";
                g_filebics << "],[";
                for (col_t i = 0; i < m; ++i)
                {
                    if (bic->B[i])
			g_filebics << i << ",";
                }
                g_filebics << "]])" << endl;
            }

	
}

void printBic2(const pbic_t &bic, const col_t m, const row_t &n, pair <double,double> zdc)
{
	for (row_t i = 0; i < bic->sizeA; ++i)
		g_filebics << bic->A[i] << " ";

    g_filebics << "\n";
	for (col_t i = 0; i < m; ++i)
	{
		if (bic->B[i])
			g_filebics << i << " ";
	}
    g_filebics << "\n";

	// calculating the max confidence
	/*
	for (unsigned short i = 0; i < g_maxLabel; ++i)
	{
		row_t ib = g_contClassBic[i]; // number of samples of label i in the bicluster

		double t = bic->sizeA, max_conf;
		if (t != 0.0) max_conf = ib / t;
		else max_conf = 0.0;

		//g_filebics << i << " = " << max_conf << " ";
	}
	*/

        //g_filebics << zdc.first << " " << zdc.second << "\n";

}

void printDtframe(const pbic_t &bic, const row_t &n, pair <double,double> zdc)
{
	// print data frame with cols = 
	//['idxBic', 'sizeRows', 'sizeCols', 'ClassLabel', 'sizeRowsClass', 'rsupX', 'rsupY', 'rsupXY', 
	//'Confidence', 'Completeness', 'Lift', 'Leverage', 'AttrWeight', 'AttrCost', 'chi_squared']

	unsigned short c = 0;
	row_t mojority_c = 0;
	for (unsigned short i = 0; i < g_maxLabel; ++i) 
	{
		if (g_contClassBic[i] > mojority_c) 
		{
			mojority_c = g_contClassBic[i];
			c = i;
		}
	}

	double conf = g_contClassBic[c]/double(bic->sizeA);
	double rsupX = bic->sizeA/double(n);
	double rsupY = g_contClassGeral[c]/double(n);
	double rsupXY = g_contClassBic[c]/double(n);
	
	ofstream myfile;
	myfile.open(g_dtframeFilename, ofstream::app);

	myfile << (g_cont-1) << "," << bic->sizeA << "," << bic->sizeB << "," << c << "," << g_contClassBic[c] << ",";
	myfile << rsupX << "," << rsupY << "," << rsupXY  << "," << conf  << ",";
	myfile << (g_contClassBic[c]/double(g_contClassGeral[c]))  << "," << (conf/rsupY) << ",";
	myfile << (rsupXY - rsupX * rsupY) << ",1,1," << zdc.first << endl;

	myfile.close();

}

void closePrintFile()
{
	if (g_writeFile[0]) g_filebics.close();
	if (g_writeFile[1]) g_filedtframe.close();
}

void countClassBicUn(const row_t *A, const row_t &sizeA, row_t &nUn, const col_t &col)
{
	// counting the representativeness of each class label and the unavoidable transactions of each class label
	for (unsigned short i = 0; i < g_maxLabel; ++i)
	{
		g_contClassBic[i] = 0; // initialize vector
		g_contClassBicUn[i] = 0; // initialize vector
	}
	
	for (row_t i = 0; i < sizeA; ++i) {
		++g_contClassBic[ g_classes[A[i]] ]; 
		if (col >= g_unav[A[i]])
		{
			++g_contClassBicUn[ g_classes[A[i]] ];
			++nUn;
		}
	}
}


pair <double,double> getZDC(const row_t &n, const char &option, const row_t &nUn, const row_t &sizeRW)
{
/*
option = 'u' => upper bound
option = 'a' => actual value
*/

	double zdc;
	pair <double,double> maior;
	maior.first = 0;
	maior.second = 0;


	for (unsigned short k = 0; k < g_maxLabel; ++k)
	{
		zdc = chi_squared(g_contClassBic[k], sizeRW - g_contClassBic[k], g_contClassGeral[k], n - g_contClassGeral[k]);
		if (zdc > maior.first) maior.first = zdc;
	}
	
	if (option == 'u')
	{
		for (unsigned short i = 0; i < g_maxLabel; ++i)
		{
			// chi-squared multiclass
			/*
			for (unsigned short k = 0; k < g_maxLabel; ++k) contClassBic[k] = 0; // initialize vector
			contClassBic[i] = g_contClassBic[i];
			zdc = chi_squared_multi(contClassBic, n);
			*/

			// chi-squared 2 classes
			/*
			row_t ib, ig, og;
			ib = g_contClassBic[i]; // number of samples of label i in the bicluster
			ig = g_contClassGeral[i];  // number of samples of label i in the dataset
			og = n - g_contClassGeral[i];
			zdc = chi_squared(ib, 0, ig, og);
			*/

			//upper bound calculation (theorem 3 paper multiclass correlation pattern minig)
			double zdc = ((n - g_contClassGeral[i]) * (g_contClassBic[i] / (double)g_contClassGeral[i]) * n) / (n - g_contClassBic[i])/n;
			if (zdc > maior.second) maior.second = zdc;
			
			//unav - 1xall
			if (g_maxLabel == 2)
			{
				zdc = chi_squared(g_contClassBic[i], nUn - g_contClassBicUn[i], g_contClassGeral[i], n - g_contClassGeral[i]);
				if (zdc > maior.first) maior.first = zdc;
			}
			else
			{
				zdc = chi_squared(g_contClassBic[i], nUn - g_contClassBicUn[i], g_contClassGeral[i], n - g_contClassGeral[i]);
				if (zdc > maior.first) maior.first = zdc;
				zdc = chi_squared(sizeRW - g_contClassBic[i], g_contClassBicUn[i], g_contClassGeral[i], n - g_contClassGeral[i]);
				if (zdc > maior.first) maior.first = zdc;
			}
		}
	}
	
	return maior;
}

double chi_squared_multi (const row_t *contClassBic, const row_t &n)
{
	double aux = 0;
	double t = n;
	row_t sizeA = 0;

	for (unsigned short i = 0; i < g_maxLabel; ++i) sizeA = sizeA + contClassBic[i];

	for (unsigned short i = 0; i < g_maxLabel; ++i)
	{
		double p = contClassBic[i];
		double pt = g_contClassGeral[i];

		double Ei1 = sizeA/t*pt;
		double Ei2 = (t-sizeA)/t*pt;

		double p1n = pow(p-Ei1, 2);
		double p3n = pow(pt-p-Ei2, 2);

		double p1 = 0, p3 = 0;
    
		if (Ei1 != 0) p1 = p1n / Ei1;
		if (Ei2 != 0) p3 = p3n / Ei2;

		aux = aux + p1 + p3;

	}

	return aux / t;
}

double chi_squared (const row_t &p, const row_t &n, const row_t &pt, const row_t &nt)
{
    double t = nt + pt;
    double p1n = pow(p-((p+n)/t)*pt, 2);
    double p1d = (p+n)/t*pt;
    double p2n = pow(n-((p+n)/t)*nt, 2);
    double p2d = (p+n)/t*nt;
    double p3n = pow(pt-p-(t-(p+n))/t*pt, 2);
    double p3d = (t-(p+n))/t*pt;
    double p4n = pow(nt-n-(t-(p+n))/t*nt,2);
    double p4d = (t-(p+n))/t*nt;
	double p1 = 0, p2 = 0, p3 = 0, p4 = 0;
    
    if (p1d != 0) p1 = p1n / p1d;
    if (p2d != 0) p2 = p2n / p2d;
    if (p3d != 0) p3 = p3n / p3d;    
    if (p4d != 0) p4 = p4n / p4d;

    return (p1 + p2 + p3 + p4) / t;
}

bool compareBics(const pbic_t &bic1, const pbic_t &bic2, const row_t &sizeBic)
{
	row_t count_aux = 0;
	for (row_t k = 0; k < sizeBic; ++k)
		if (bic1->A[k] == bic2->A[k]) count_aux++;


	if (count_aux == sizeBic)
		return true;

	return false;
}

bool ReachMinConf(const double &total)
{
	double conf;
	for (unsigned short i = 0; i < g_maxLabel; ++i)
	{
		row_t ib = g_contClassBic[i]; 
		conf = Confidence(ib, total);

		if(conf >= g_minConf[i])
			return true;
	}
	
	return false;
}

double Confidence(const row_t &ib, const double &total)
{

	double conf;
	if (total != 0.0) conf = ib / total;
	else conf = 0.0;

	return conf;
}

void countAndPrintBic(const pbic_t &bic, const col_t m, const row_t &n)
{
	row_t nUn = 0; 
	countClassBicUn(bic->A, bic->sizeA, nUn, m);

	pair <double,double> zdc = getZDC(n, 'a', nUn, bic->sizeA);


	if (zdc.first >= g_minZDC) ++g_cont;
	else ++g_contFails;

	if (ReachMinConf((double)bic->sizeA))
	{
		 ++g_cont_conf;
		
	}
	else ++g_contFails_conf;

	if (zdc.first >= g_minZDC && ReachMinConf((double)bic->sizeA)) 
	{
		++g_cont_general;
		if (g_writeFile[0]) printBic(bic, m, n, zdc);
		if (g_writeFile[1]) printDtframe(bic, n, zdc);

	}
	else 
	{
		
		++g_contFails_general;
		
	}

}
