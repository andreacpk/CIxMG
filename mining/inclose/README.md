# In-Close5

In-Close5 is a fast formal concept miner

Author's implementation: https://sourceforge.net/projects/inclose/

Related works:
- Andrews, S. (2018). A new method for inheriting canonicity test failures in close-by-one type algorithms.
- Andrews, S. (2017). Making use of empty intersections to improve the performance of cbo-type algorithms. In International Conference on Formal Concept Analysis (pp. 56-71).
- Andrews, S. (2015). A ‘Best-of-Breed’approach for designing a fast algorithm for computing fixpoints of Galois Connections. Information Sciences, 295, 633-649.
- Andrews, S. (2011). In-close2, a high performance formal concept miner. In International Conference on Conceptual Structures (pp. 50-62).
- Andrews, S. (2009). In-close, a fast algorithm for computing formal concepts.


## Compiling
To compile the program, run the file  './MakeFile.sh' in the directory containing the source files.

## Running
To run the program, type InClose5' and the filename of the configuration file.
This file shoud contain the following tags:

DATASET=exampleDataset
LABELS=exampleLabels
THRESHOLDS=exampleThresholds
BICFILE=exampleOutput.txt   (optional)
SUMMARY=exampleSummary.txt  (optional)
DTFRAME=exampleDtframe.txt  (optional)
CUTOPTION=1                 (option for experimental tests)
USEUNAVOIDABLE=0            (option for experimental tests)
ZDC=1                       (only option available for now)


For instance:
./InClose5 config_exampleDataset

Config file:

DATASET                        input file containing the data set with 0's and 1's separeted by tab or space between columns.
LABELS                         input file containing the label index of each row. it should match with the rows in dataset (same order and size)
THRESHOLDS                     input parameters about the thresholds of prunning (more details below)
BICFILE                        output file containing the rows and columns indexes of mined patterns (biclusters)
SUMMARY                        output file containing details about the parameters of the execution
DTFRAME                        output file containing the data frame used in classifier (optional next step)
CUTOPTION                      type of prunning
USEUNAVOIDABLE                 using the concepts of unavoidable transaction for prunning
ZDC                            which ZDC equation to use. only chi-square (1) available in thin version


THRESHOLDS File:

Values of the four thresholds available for pruning (one in each row).

minimum support                (one for each class separeted by space. Integer from 0 to size of each class)
minimum of columns             (one integer from 0 to size of columns in data set) 
minimum of Confidence          (one for each class separeted by space. Decimal from 0.0 to 1.0)
minimum of ZDC                 (Decimal from 0.0 to 1.0)

