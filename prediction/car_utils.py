import pandas as pd


class Dataset:
    def __init__(self, D_train, labels_train, D_test, labels_test, bics):
        self.D_train = D_train
        self.labels_train = labels_train
        self.D_test = D_test
        self.labels_test = labels_test
        self.bics = bics


class Arguments:
    def __init__(self, arg_filename):
        self.train_fn = None
        self.train_lb_fn = None
        self.test_fn = None
        self.test_lb_fn = None
        self.bics_fn = None
        self.default_label = None
        self.rank = None
        self.sql_fn = None
        self.readArguments(arg_filename)
        self.dtset = self.readfiles()

    def readArguments(self, arg_filename):
        with open(arg_filename) as f:
            lines = [line.rstrip() for line in f]

        self.train_fn = lines[0]
        self.train_lb_fn = lines[1]
        self.test_fn = lines[2]
        self.test_lb_fn = lines[3]
        self.bics_fn = lines[4]
        self.k = self.train_fn[-1::]

        # Set the defaul label
        self.default_label = int(lines[5])

        r1 = [s.strip() for s in (lines[6].split(","))]
        r2 = [ele == "True" for ele in lines[7].split(",")]
        if len(r1) == len(r2):
            self.rank = (r1, r2)
        else:
            raise Exception('Ranking doesnt match the number of elements')
        self.sql_fn = lines[8]

    def readfiles(self):
        try:

            print('loading train dataset')
            D_train = pd.read_csv(self.train_fn, sep=' ', header=None, na_values=999999)
            print('D_train.shape = ', D_train.shape)
            labels = pd.read_csv(self.train_lb_fn, sep=' ', header=None, na_values=999999).squeeze("columns")

            print('loading bics file')
            bics = readBicOutput(self.bics_fn)

            print('loading test dataset')
            D_test = pd.read_csv(self.test_fn, sep=' ', header=None, na_values=999999)
            print('test.shape = ', D_test.shape)
            labels_test = pd.read_csv(self.test_lb_fn, sep=' ', header=None, na_values=999999).squeeze("columns")

            return Dataset(D_train, labels, D_test, labels_test, bics)

        except Exception:
            raise Exception('Read files error!')

    @property
    def createInsertInit(self):
        return "insert into ac (output_file, n_bics, default_label, rank_order, "

    @property
    def createInsertFinal(self):
        return ") values ( '" + self.bics_fn + "', " + str(len(self.dtset.bics)) + ", " + str(self.default_label) \
                      + ", '" + str(self.rank) + "', "

def readBicOutput(filename):
    bics = []
    with open(filename) as bicfile:
        for line in bicfile:
            exec(line)
    return bics


# print rules
def printRules(rules, filename):
    with open(filename, 'w') as file:
        for rule in rules:
            cols = rule.cols
            lbs = rule.dbicMin
            ubs = rule.dbicMax
            for col in cols:
                if lbs[col] != ubs[col]:
                    aux = '%d[%.2f,%.2f],' % (col, lbs[col], ubs[col])
                    # aux = '%03d[%.2f,%.2f],' % (col, lbs[col], ubs[col])
                    # aux = '%s[%.2f,%.2f],' % (attrNames[col], lbs[col], ubs[col])
                else:
                    aux = '%d=%.2f,' % (col, lbs[col])
                    # aux = '%s=%.2f,' % (attrNames[col], lbs[col])
                file.write(aux)
            file.write(' -> ' + str(rule.classLabel) + '\n')

# print used rules
def printUsedRules(rules, idx_rules, filename):
    with open(filename, 'w') as file:
        for idx in idx_rules:
            if idx != -1:
                rule = next(x for x in rules if x.idxbic == idx)
                cols = rule.cols
                lbs = rule.dbicMin
                ubs = rule.dbicMax
                for col in cols:
                    if lbs[col] != ubs[col]:
                        aux = '%d[%.2f,%.2f],' % (col, lbs[col], ubs[col])
                        # aux = '%03d[%.2f,%.2f],' % (col, lbs[col], ubs[col])
                        # aux = '%s[%.2f,%.2f],' % (attrNames[col], lbs[col], ubs[col])
                    else:
                        aux = '%d=%.2f,' % (col, lbs[col])
                        # aux = '%s=%.2f,' % (attrNames[col], lbs[col])
                    file.write(aux)
                file.write(' -> ' + str(rule.classLabel) + '\n')
            else:
                file.write('default label'+ '\n')
