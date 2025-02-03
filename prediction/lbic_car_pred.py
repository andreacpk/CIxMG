#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Mar  5 15:59:46 2021

@author: rve
"""

from sklearn.metrics import accuracy_score
from sklearn.metrics import balanced_accuracy_score
from sklearn.metrics import f1_score
from lbic_car_metrics import *
#import pygmo as pg
from collections import Counter


class Predictor:
    def __init__(self, name, labels_test, pred_labels, nnm):
        self.name = name
        self.acc = accuracy_score(labels_test, pred_labels)
        self.bacc = balanced_accuracy_score(labels_test, pred_labels)
        self.f1 = f1_score(labels_test, pred_labels, average='weighted')
        self.nnm = nnm

    def printmeasures(self):
        #print('Predictor name = ', self.name)
        print('Acurracy = ', self.acc)
        print('Balanced Accuracy = ', self.bacc)
        print('F1-Score (weighted)= ', self.f1)
        print("Number of test samples that do not match any classifer's rules = ", self.nnm)


def decisionList(rules, row, default_label):
    flag = 0
    for rule in rules:
        vlrs = row[rule.cols]
        if (vlrs >= rule.dbicMin).all() and (vlrs <= rule.dbicMax).all():
            flag = 1
            return rule.classLabel, flag, rule.idxbic

    return default_label, flag, -1


# Get the predicted labels for new samples
# if there are no rules matching a test sample, the default class label is assigned to it
def decisionListPredictor(rules, D_test, default_label):
    yhat = [default_label for i in range(len(D_test.index))]
    cont = 0 # store the number of test samples that match some classifier's rule
    used_rules = []
    for index, row in D_test.iterrows():
        lab, flag, idx_bic = decisionList(rules, row, default_label)
        yhat[index] = lab
        cont += flag

        used_rules.append(idx_bic)

    
    return yhat, len(D_test.index) - cont, used_rules


# Get the predicted labels for new samples
# if there are no rules matching a test sample, the first rule that contains one exactly
# same value in antecedent at least
def decisionListPredictorACCF (rules, D_test, default_label):
    yhat = [default_label for i in range(len(D_test.index))]
    without_rule = []
    used_rules = []

    cont = 0  # store the number of test samples that match some classifier's rule
    for index, row in D_test.iterrows():

        match = 0
        lab, flag, idx_bic = decisionList(rules, row, default_label)
        yhat[index] = lab
        cont += flag
        match = flag
        
        if match == 0:
            without_rule.append((index, row))
            used_rules.append(match)
        else:
            used_rules.append(idx_bic)

    for index, row in without_rule:
        for rule in rules:
            vlrs = row[rule.cols]
            if (vlrs >= rule.dbicMin).any() and (vlrs <= rule.dbicMax).any():
                yhat[index] = rule.classLabel
                used_rules[index] = rule.idxbic
                cont += 1
                break

    return yhat, len(D_test.index) - cont, used_rules


# Get the predicted labels for new samples
# if there are no rules matching a test sample, the first rule that contains one exactly
# same value in antecedent at least
def decisionListPredictorACCF2(rules, D_test, default_label):
    yhat = [default_label for i in range(len(D_test.index))]
    without_rule = []

    cont = 0 # store the number of test samples that match some classifier's rule
    for index, row in D_test.iterrows():

        match = 0
        lab, flag = decisionList(rules, row, default_label)
        yhat[index] = lab
        cont += flag
        match = flag
        
        if match ==0:
            without_rule.append((index, row))

    for index, row in without_rule:
        bag_of_labels = []
        for rule in rules:
            vlrs = row[rule.cols]
            if (vlrs >= rule.dbicMin).any() and (vlrs <= rule.dbicMax).any():
                bag_of_labels.append(rule.classLabel)
        
        if bag_of_labels:
            c = Counter(bag_of_labels)
            elem, count = c.most_common(1)[0]
            yhat[index] = elem
            cont += 1

    return yhat, len(D_test.index) - cont


# Lakkaraju, H., Bach, S.H. and Leskovec, J., 2016, August. Interpretable decision sets:
# A joint framework for description and prediction. In Proceedings of the 22nd ACM SIGKDD international
# conference on knowledge discovery and data mining (pp. 1675-1684).
def decisionSetPredictor(rules, D_test, default_label):
    yhat = [default_label for i in range(len(D_test.index))]
    groupR = []
    groupL = []

    cont = 0 # store the number of test samples that match some classifier's rule
    for index, row in D_test.iterrows():

        for rule in rules:
            vlrs = row[rule.cols]
            if (vlrs >= rule.dbicMin).any() and (vlrs <= rule.dbicMax).any():
                groupR.append(rule)
                groupL.append(rule.classLabel)
        
        if len(set(groupL)) == 1:
            yhat[index] = groupL[0]
            cont += 1

        elif len(set(groupL)) > 1:
            pred_l, contDL = decisionList(rules, row, default_label)
            cont += contDL
            yhat[index] = pred_l

    return yhat, len(D_test.index) - cont


def findInFront(rules, row, default_label, front):
    flag = 0
    for f in front:
        for point in f:
            vlrs = row[rules[point].cols]
            if (vlrs >= rules[point].dbicMin).all() and (vlrs <= rules[point].dbicMax).all():
                flag = 1
                return rules[point].classLabel, flag

    return default_label, flag


# def decisionSetPredictorMO(rules, D_test, default_label, summary):
#     yhat = [default_label for i in range(len(D_test.index))]
#     groupR = []
#     groupL = []
#     # preparing for mo
#     metric_points = [[-summary.loc[summary['idxBic'] == rule.idxbic, 'Confidence'].values[0], -summary.loc[summary['idxBic'] == rule.idxbic, 'rsupXY'].values[0], -summary.loc[summary['idxBic'] == rule.idxbic, 'chi_squared'].values[0]] for rule in rules]

#     cont = 0  # store the number of test samples that match some classifier's rule
#     for index, row in D_test.iterrows():
        
#         for rule in rules:
#             vlrs = row[rule.cols]

#             if (vlrs >= rule.dbicMin).any() and (vlrs <= rule.dbicMax).any():
#                 groupR.append(rule)
#                 groupL.append(rule.classLabel)
        
#         if len(set(groupL)) == 1:
#             yhat[index] = groupL[0]
#             cont += 1

#         elif len(set(groupL)) > 1:
#             ndf, dl, dc, ndr = pg.fast_non_dominated_sorting(metric_points)
#             pred_l, contDL = findInFront(rules, row, default_label, ndf)
#             cont += contDL
#             yhat[index] = pred_l

#     return yhat, len(D_test.index) - cont


# Get the predicted labels for new samples
# if there are no rules matching a test sample, the first rule that contains one exactly
# same value in antecedent at least
# falta implementacao certa do chi2 que ele usa
def decisionGroupPredictorCMAR(rules, D_test, default_label, summary, labels):
    yhat = [default_label for i in range(len(D_test.index))]
    groupR = []
    groupL = []

    cont = 0 # store the number of test samples that match some classifier's rule
    for index, row in D_test.iterrows():

        for rule in rules:
            vlrs = row[rule.cols]
            if (vlrs >= rule.dbicMin).any() and (vlrs <= rule.dbicMax).any():
                groupR.append(rule)
                groupL.append(rule.classLabel)
        
        if len(set(groupL)) == 1:
            yhat[index] = groupL[0]
            cont += 1

        elif len(set(groupL)) > 1:
            classL, strength = compareStrength(groupR, summary, labels)
            if strength > 0:
                yhat[index] = classL
                cont += 1

    return yhat, len(D_test.index) - cont


def compareStrength(groupR, summary, labels):
    labels_len = labels.value_counts()
    groups = [[] for _ in range(len(labels.unique()))]
    max_strength = 0
    max_strength_label = 0
   
    # split groups of rules by class
    for rule in groupR:
        groups[rule.classLabel].append(rule)

    for i in range(len(labels.unique())):
        if groups:
            wchi2 = weighted_chi2(groups[i], labels_len[i], len(labels), summary)
            if wchi2 > max_strength:
                max_strength = wchi2
                max_strength_label = i

    return max_strength_label, max_strength


def weighted_chi2(group, label_len, t_labels, summary):
    wchi2 = 0

    for rule in group:
        sizeBic = summary.loc[summary['idxBic'] == rule.idxbic, 'sizeRows'].values[0]
        sizeBicClass = summary.loc[summary['idxBic'] == rule.idxbic, 'sizeRowsClass'].values[0]
        max_chi2 = ub_chi_squared(sizeBicClass, sizeBic - sizeBicClass, label_len, t_labels)
        chi2 = sizeBic = summary.loc[summary['idxBic'] == rule.idxbic, 'chi_squared'].values[0]

        wchi2 += chi2*chi2/max_chi2
    
    return wchi2


def predictors(rules, D_test, labels_test, default_label, summary):
    preds = []

    print('classifying by decisionListPredictor')
    pred_labels, nnm = decisionListPredictor(rules, D_test, default_label)
    preds.append(Predictor('decisionListPredictor', labels_test, pred_labels, nnm))

    print('classifying by decisionListPredictorACCF')
    pred_labels, nnm = decisionListPredictorACCF(rules, D_test, default_label)
    preds.append(Predictor('decisionListPredictorACCF', labels_test, pred_labels, nnm))

    print('classifying by decisionListPredictorACCF2')
    pred_labels, nnm = decisionListPredictorACCF2(rules, D_test, default_label)
    preds.append(Predictor('decisionListPredictorACCF2', labels_test, pred_labels, nnm))

    print('classifying by decisionSetPredictor')
    pred_labels, nnm = decisionSetPredictor(rules, D_test, default_label)
    preds.append(Predictor('decisionSetPredictor', labels_test, pred_labels, nnm))

    print('classifying by decisionSetPredictorMO')
    pred_labels, nnm = decisionSetPredictorMO(rules, D_test, default_label, summary)
    preds.append(Predictor('decisionSetPredictorMO', labels_test, pred_labels, nnm))

    return preds

