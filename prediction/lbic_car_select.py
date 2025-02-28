import numpy as np
import pandas as pd


# Select rules until each sample is covered by delta rules (if possible)
def coverageSelection(bicsRowsClass, summary, n_samples, ranking, minCov=1, delta=1):
    newSummary = summary.copy()
    newSummary = newSummary.sort_values(ranking[0], ascending=ranking[1])

    covered = np.zeros(shape=[n_samples, ], dtype=int)
    idxSelectedBics = []
    for idxBic in newSummary.idxBic:
        # from the samples yet uncovered by delta rules: check if this rule can classify at
        # least minCov samples correctly
        if sum(covered[bicsRowsClass[idxBic]] < delta) >= minCov:
            covered[bicsRowsClass[idxBic]] += 1
            idxSelectedBics.append(idxBic)

        if (covered >= delta).all():
            break
    return idxSelectedBics


# Refs:
# 1 - Baralis, E., & Garza, P. (2002). A lazy approach to pruning classification rules.
# In 2002 IEEE International Conference on Data Mining, 2002. Proceedings. (pp. 35-42).
# 2 - Baralis, E., Chiusano, S., & Garza, P. (2007). A lazy approach to associative classification.
# IEEE Transactions on Knowledge and Data Engineering, 20(2), 156-171.
# It discards exclusively rules that only misclassify training data.
# All other rules are selected to the classifier and they are divided into 2 levels
def lazyPruneL3(bics, bicsRowsClass, summary, n_samples, ranking):
    newSummary = summary.copy()
    newSummary = newSummary.sort_values(ranking[0], ascending=False) #ranking[1])

    covered = np.zeros(shape=[n_samples, ], dtype=int)
    level1 = []
    harmful = []
    for idxBic in newSummary.idxBic:
        nrr = sum(covered[bicsRowsClass[idxBic]] < 1)  # number of right classified samples
        wr = list(set(bics[idxBic][0]) - set(bicsRowsClass[idxBic]))
        nwr = sum(covered[wr] < 1)  # number of wrong classified samples
        if nrr > 0:
            level1.append(idxBic)
            covered[bics[idxBic][0]] = covered[bics[idxBic][0]] + 1
        elif nwr > 0:
            harmful.append(idxBic)

        if (covered >= 1).all():
            break

    level2 = list(set(range(len(bics))) - set(level1) - set(harmful))
    return level1, level2, harmful
