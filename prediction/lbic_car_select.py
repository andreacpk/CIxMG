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

# Select all rules sorted
#faltou a ordem lexicografica como criterio de desempate se conf, sup e size cols for igual
def ACCFSelection(summary, ranking):
    newSummary = summary.copy()
    newSummary = newSummary.sort_values(ranking[0], ascending=ranking[1])
    idxSelectedBics = newSummary.idxBic

    return idxSelectedBics

# Select the best k rules
def bestkRulesSelection(summary, k, ranking):
    newSummary = summary.copy()
    newSummary = newSummary.sort_values(ranking[0], ascending=ranking[1])
    idxSelectedBics = newSummary.idxBic[0:k]

    return idxSelectedBics


# Select the best k rules per label
def bestkRulesSelectionPerLabel(summary, k, ranking):
    classLabels = summary.ClassLabel.unique()
    df = pd.DataFrame(data=None, columns=summary.columns)
    newSummary = summary.copy()
    newSummary = newSummary.sort_values(ranking[0], ascending=ranking[1])

    for classLabel in classLabels:
        # indexes, _ = getSubsetOfSummary(newSummary, (('ClassLabel', '==', classLabel),))
        # idxSelectedBics.extend(indexes[0:k])
        subset = getSubsetOfSummary(newSummary, (('ClassLabel', '==', classLabel),))
        df = df.append(subset.iloc[0:k, :])

    df = df.sort_values(ranking[0], ascending=ranking[1])

    return df['idxBic']


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


# Chen, F., Wang, Y., Li, M., Wu, H., & Tian, J. (2014). Principal association mining: an efficient classification
# approach. Knowledge-Based Systems, 67, 16-25.
# ORIGINAL PAM: all the conflicting rules (rules with the same condition but with different class labels) that
# satisfy the user-defined confidence threshold are retained as candidate rules.
# THIS IMPLEMENTATION works as original PAM only if minConf > 50% because then we have only one optimal rule
# for each condition
# PAM selction is based on coverage, but it re-ranks the rules after the selection of one rule
def PAM_selection(bics, summary, bicsRowsClass, n_samples, minConf=0.85, Lambda=0.8, wGroup=True):
    # Pruning: rules below the minimal confidence threshold minconf is out of consideration
    selecao = summary.index[summary.Confidence >= minConf].tolist()
    newSummary = summary.iloc[selecao, :].copy()

    # Pruning by group
    if wGroup:
        newSummary = PAM_grouping(bics, newSummary)

    # Calculate the Principality for each rule in newSummary
    newSummary['Principality'] = np.zeros(len(newSummary))
    for index in newSummary.index:
        newSummary.at[index, 'Principality'] = Lambda * newSummary.at[index, 'Confidence'] + (1 - Lambda) * \
                                               newSummary.at[index, 'Completeness']

    # Rule Rank
    newSummary = newSummary.sort_values(["Principality", "Confidence", "rsupXY", "sizeCols"],
                                        ascending=(False, False, False, False))

    covered = set()
    idxSelectedBics = []
    while newSummary.shape[0] > 0:
        ibic = newSummary.index[0]  # index of the selected bicluster
        idxSelectedBics.append(ibic)
        covered = covered.union(set(bicsRowsClass[ibic]))

        # Update Principality for each rule in newSummary
        for index in newSummary.index:
            new_coverage = set(bicsRowsClass[index]) - covered
            newSummary.at[index, 'sizeRowsClass'] = len(new_coverage)
            newSummary.at[index, 'Completeness'] = (newSummary.at[index, 'sizeRowsClass'] / n_samples) / newSummary.at[
                index, 'rsupY']
            newSummary.at[index, 'Principality '] = \
                Lambda*newSummary.at[index, 'Confidence']+(1 - Lambda)*newSummary.at[index, 'Completeness']

        # Delete all rules that do not cover any object
        # (Therefore, rules that do not classify none of the remaining objects correctly)
        selecao = newSummary.index[newSummary.Completeness == 0].tolist()
        newSummary.drop(selecao, inplace=True)

        # Rule Rank
        newSummary = newSummary.sort_values(["Principality", "Confidence", "rsupXY", "sizeCols"],
                                            ascending=(False, False, False, False))

    # Return the selected biclusters (rules)
    return idxSelectedBics


# ORIGINAL PAM: all rules are categorized into groups according to their class labels.
# Then the rules with lower confidence subsumed by more specific rules with higher confidence will be pruned.
def PAM_grouping(bics, newSummary):
    labelsvc = newSummary.ClassLabel.value_counts()
    for label in labelsvc.index:
        subset = newSummary[newSummary.ClassLabel == label]
        subset = subset.sort_values(["Confidence", "Completeness", "sizeCols"], ascending=(False, False, False))
        if subset.Confidence.iloc[0] == 1 and subset.Completeness.iloc[0] == 1:
            print(label)
            selecao = subset.index[1:].tolist()
            newSummary.drop(selecao, inplace=True)
            continue
        print(label)
        deletar = np.zeros(shape=[subset.shape[0], ])
        for i1 in range(0, subset.shape[0] - 1):
            bici1 = bics[subset.index[i1]]
            Ai1 = set(bici1[0])
            Bi1 = set(bici1[1])
            for i2 in range(i1 + 1, subset.shape[0]):
                if deletar[i2] == 0 and subset.Confidence.iloc[i1] > subset.Confidence.iloc[i2]:
                    bici2 = bics[subset.index[i2]]
                    Ai2 = set(bici2[0])
                    Bi2 = set(bici2[1])
                    if Bi2.issubset(Bi1) and Ai1.issubset(Ai2):
                        deletar[i2] = 1
        selecao = subset.index[np.where(deletar == 1)].tolist()
        newSummary.drop(selecao, inplace=True)
    return newSummary


# Greedy search to get a small subset of the biclusters with the same coverage of the whole set
def getRepresBicsGreedy(bics, summary, bicsRowsClass, attrCosts, n_samples, minConf=0.85, minLift=0):
    # rules below minconf and minLift is out of consideration
    filter1 = summary.Confidence >= minConf
    filter2 = abs(summary.Lift - 1) >= minLift
    selecao = summary.index[filter1 & filter2].tolist()
    newSummary = summary.iloc[selecao, :].copy()

    # Rule Rank
    # newSummary = newSummary.sort_values(["Completeness", "AttrCost", "sizeCols", "Confidence"],
    # ascending = (False, True, True, False))
    newSummary = newSummary.sort_values(["Completeness", "AttrCost", "sizeCols", "Confidence", "rsupXY"],
                                        ascending=(False, True, True, False, False))

    attrNCov = np.ones(len(attrCosts), dtype=int)
    attrCosts_copy = attrCosts.copy()

    idxSelectedBics = []
    covered = set()
    while newSummary.shape[0] > 0:
        ibic = newSummary.index[0]  # index of the selected bicluster
        idxSelectedBics.append(ibic)

        # Update Completeness, AttrCost and sizeCols for each rule in newSummary
        covered = covered.union(set(bicsRowsClass[ibic]))
        attrCosts_copy[bics[ibic][1]] = 0  # set to 0 the costs of the selected attributes
        attrNCov[bics[ibic][1]] = 0  # set to 0 the counting of the selected attributes
        for index in newSummary.index:
            new_coverage = set(bicsRowsClass[index]) - covered
            newSummary.at[index, 'sizeRowsClass'] = len(new_coverage)
            newSummary.at[index, 'Completeness'] = (newSummary.at[index, 'sizeRowsClass'] / n_samples) / newSummary.at[
                index, 'rsupY']
            newSummary.at[index, 'AttrCost'] = attrCosts_copy[bics[index][1]].sum()
            newSummary.at[index, 'sizeCols'] = attrNCov[bics[index][1]].sum()

        # Delete all rules that do not cover any object
        selecao = newSummary.index[newSummary.Completeness == 0].tolist()
        newSummary.drop(selecao, inplace=True)

        # Rule Rank
        newSummary = newSummary.sort_values(["Completeness", "AttrCost", "sizeCols", "Confidence", "rsupXY"],
                                            ascending=(False, True, True, False, False))
    return idxSelectedBics


def getSubsetOfSummary(summary, filtros):
    aux = pd.Series(data=np.ones(len(summary), ), dtype=bool)
    for item in filtros:
        if item[1] == '==':
            aux2 = summary[item[0]] == item[2]
        elif item[1] == '<=':
            aux2 = summary[item[0]] <= item[2]
        else:
            aux2 = summary[item[0]] >= item[2]
        aux = aux & aux2
    selecao = summary.index[aux].tolist()
    newSummary = summary.iloc[selecao, :].copy()
    # return selecao, newSummary
    return newSummary
