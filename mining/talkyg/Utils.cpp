#include "stdafx.h"
#include "Utils.h"

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

void printPattern(const pnode_t pattern, const vector<item_t> &items)
{

    //counting the sup of each class
    unsigned short label;
	row_t *support = new row_t[g_maxLabel];
	double *conf = new double[g_maxLabel];
	for (unsigned short i = 0; i < g_maxLabel; ++i) support[i] = 0; // initialize vector

	for (row_t j = 0; j < pattern->sup; ++j)
    {
        label = g_classes[pattern->tidset[j]];
        ++support[label]; // counting the representativeness of each class label
    }

    // calculating confidence for each class
    for (unsigned short i = 0; i < g_maxLabel; ++i)
    {
        conf[i] = support[i]/pattern->sup;

        if (support[i] >= g_minsups[i] && conf[i] >= g_minconf[i])
        {
            ++g_cont;

            if (g_output == 1) // matlab
            {
                g_filebics << "A{" << g_cont << "} = [";
                for (row_t k = 0; k < pattern->sup; ++k)
                    g_filebics << pattern->tidset[k] << " ";
                g_filebics << "];\nB{" << g_cont << "} = [";
                for (col_t i = 0; i < pattern->length; ++i)
                {
                    col_t idx = pattern->idxItems[i];
                    // g_filebics << items[idx].idx + 1 << "=" << items[idx].value << " " ;
                    g_filebics << items[idx].idx << " " ;
                }
                g_filebics << "];\n";
            }
            else // python
            {
                g_filebics << "bics.append([[";
                for (row_t i = 0; i < pattern->sup; ++i)
                    g_filebics << pattern->tidset[i]  << ",";
                g_filebics << "],[";
                for (col_t i = 0; i < pattern->length; ++i)
                {
                    col_t idx = pattern->idxItems[i];
                    g_filebics << items[idx].idx << ",";
                }
                g_filebics << "]])" << endl;
            }
            delete [] support;
            return;
        }
    }


	//if (g_minconf > 0 && pattern->biggerSup/(double)pattern->sup < g_minconf)
	//	return;



}

void closePrintFile()
{
	if (g_writeFile[0]) g_filebics.close();
	if (g_writeFile[1]) g_filedtframe.close();
}
