# ac
Associative Classification

AC Library for already mined association rules.

command line for execute python script:

python3 example.py input_param.txt

The input_param file need to follow specific order of information per line:

 - path + filename of the training dataset
 - path + filename of the training dataset labels
 - path + filename of the test dataset
 - path + filename of the test dataset labels
 - path + filename of the rules mined
 - default label
 - columns name for ranking
 - bool for each column name indicating the ascending order or not

For now, the separator of the datasets needs to be set in the example.py file.
Also, there are 4 types of Selection and 4 types of Predictor that should be comment or not in the example.py file.

Work in Progress...
