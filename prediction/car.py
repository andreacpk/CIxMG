#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Mar  5 15:59:46 2021

"""
import math

import time
import pandas as pd
import car_pred as pred
import car_select as selec
import car_utils as utils
from car_metrics import *


class Classifier:
    def __init__(self, rules, selection_type, param, preds):
        self.rules = rules
        self.selection_type = selection_type
        self.param = param
        self.preds = preds

    def bestAccPred(self):
        best_acc = 0
        best = None
        for p in self.preds:
            if p.acc > best_acc:
                best_acc = p.acc
                best = p

        return best

    def bestBaccPred(self):
        best_bacc = 0
        best = None
        for p in self.preds:
            if p.bacc > best_bacc:
                best_bacc = p.bacc
                best = p

        return best

    def bestf1Pred(self):
        best_f1 = 0
        best = None
        for p in self.preds:
            if p.f1 > best_f1:
                best_f1 = p.f1
                best = p

        return best

    def bestNNMPred(self):
        best_nnm = math.inf
        best = None
        for p in self.preds:
            if p.nnm < best_nnm:
                best_nnm = p.nnm
                best = p

        return best


class Rule:
    def __init__(self, cols, dbicMin, dbicMax, classLabel, idxbic):
        self.cols = cols
        self.dbicMin = dbicMin
        self.dbicMax = dbicMax
        self.classLabel = classLabel
        self.idxbic = idxbic


# Get the dataset coverage and the global overlap
# n: number of samples of the training dataset
# m: number of attributes of the training dataset
def getCovGO(n, m, bics):
    mc = np.zeros((n, m), dtype=int)
    mg = np.zeros((n, m), dtype=int)
    
    for bic in bics:
        rows = np.array(bic[0])
        cols = np.array(bic[1])
        mc[np.ix_(rows, cols)] = 1
        mg[np.ix_(rows, cols)] = mg[np.ix_(rows, cols)] + 1
          
    cov = mc.sum()
    go = (mg.sum() - cov) / cov
    
    return cov, go


# Get information about the optimal rule for each bicluster in bics
# bics: list of biclusters
# labels: class label of each sample in the training dataset
# attrWeights: weight of each attribute
# attrCosts: cost of each attribute
def getInfoOptimalRulePerBic(bics, labels, attrWeights=None, attrCosts=None):
    n = len(labels)
    counts = labels.value_counts()
    res = []
    bicsRowsClass = []
    weight = None
    cost = None

    for i, bic in enumerate(bics):
        rows, cols = bic
        sizeRows = len(rows)
        sizeCols = len(cols)
        (classe, rowsClass) = getClassBic(np.array(rows), labels)
        sizeRowsClass = len(rowsClass)
        rsupX = sizeRows / n
        rsupY = counts[classe] / n
        rsupXY = sizeRowsClass / n
        conf = sizeRowsClass / sizeRows
        comp = sizeRowsClass / counts[classe]
        lift = conf / rsupY
        leverage = rsupXY - rsupX * rsupY
        bicsRowsClass.append(rowsClass)
        chi2 = chi_squared(sizeRowsClass, sizeRows - sizeRowsClass, counts[classe], n - counts[classe])

        if attrWeights:
            weight = attrWeights[cols].values.sum()
        if attrCosts:
            cost = attrCosts[cols].values.sum()

        res.append([i, sizeRows, sizeCols, classe, sizeRowsClass, rsupX, rsupY, rsupXY, conf, comp, lift, leverage,
                    weight, cost, chi2])

    df = pd.DataFrame(res, columns=['idxBic', 'sizeRows', 'sizeCols', 'ClassLabel', 'sizeRowsClass', 'rsupX', 'rsupY',
                                    'rsupXY', 'Confidence', 'Completeness', 'Lift', 'Leverage', 'AttrWeight',
                                    'AttrCost', 'chi_squared'])

    return df, bicsRowsClass


# Get the main class label of a bicluster and the rows that belong to that class label
def getClassBic(rows, labels):

    labelsOfRows = labels.iloc[rows]
    counts = labelsOfRows.value_counts()
    classe = counts.idxmax()

    rowsClass = rows[labelsOfRows == classe]
    #rowsClass = []
    #for r in rows:
    #    if labels.iloc[r] == classe:
    #        rowsClass.append(r)

    return classe, rowsClass


# Get the number of samples of a class label and its coverage
def getClassCoverage(labels, bicsRowsClass):

    cov = np.zeros(len(labels), dtype=int)
    for rows in bicsRowsClass:
        cov[rows] = 1

    count1 = labels.value_counts()
    count2 = labels[cov == 1].value_counts()
    df = pd.concat([count1, count2], axis=1, sort=False)
    df.columns = ['Total', 'Covered']

    return df


# Build CARs with the bics in idxSelectedBics
def build_CARs(bics, idxSelectedBics, summary, D_train):
    rules = []
    for idx in idxSelectedBics:
        r, c = bics[idx]
        dbic = D_train.iloc[r, c]
        rules.append(Rule(c, dbic.min(), dbic.max(), summary.ClassLabel[idx], idx))

    return rules


def car_Coverage(arg, summary, bicsRowsClass, param):

    print('Coverage')
    start_time = time.time()
    idxSelectedBics = selec.coverageSelection(bicsRowsClass, summary, n_samples=len(arg.dtset.D_train),
                                                       ranking=arg.rank, minCov=1, delta=param)
    print('# selected = ', len(idxSelectedBics))
    print('building rules')
    rules = build_CARs(arg.dtset.bics, idxSelectedBics, summary, arg.dtset.D_train)

    print('classifying')
    pred_labels, nnm, used_rules = pred.decisionListPredictor(rules, arg.dtset.D_test, arg.default_label)
    utils.printUsedRules(rules, used_rules, 'used_' + filename)
    print('Coverage runtime = ', time.time() - start_time)
    pred = pred.Predictor('decisionListPredictor', arg.dtset.labels_test, pred_labels, nnm)
    pred.printmeasures()

def car_Lazy(arg, summary, bicsRowsClass):

    print('Lazy selection')
    start_time = time.time()
    idxSelectedBics, level2, harmful = selec.lazyPruneL3(arg.dtset.bics, bicsRowsClass, summary,
                                                                  n_samples=len(arg.dtset.D_train), ranking=arg.rank)
    idxSelectedBics.extend(level2)
    print('# selected = ', len(idxSelectedBics))
    print('building rules')
    rules = build_CARs(arg.dtset.bics, idxSelectedBics, summary, arg.dtset.D_train)

    print('classifying')
    pred_labels, nnm, used_rules = pred.decisionListPredictor(rules, arg.dtset.D_test, arg.default_label)
    utils.printUsedRules(rules, used_rules, 'used_' + filename)
    print('Lazy runtime = ', time.time() - start_time)
    pred = pred.Predictor('decisionListPredictor', arg.dtset.labels_test, pred_labels, nnm)
    pred.printmeasures()
