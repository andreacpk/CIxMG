#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Mar  5 15:59:46 2021

"""

from sklearn.metrics import accuracy_score
from sklearn.metrics import balanced_accuracy_score
from sklearn.metrics import f1_score
from car_metrics import *
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

def predictors(rules, D_test, labels_test, default_label, summary):
    preds = []

    print('classifying by decisionListPredictor')
    pred_labels, nnm = decisionListPredictor(rules, D_test, default_label)
    preds.append(Predictor('decisionListPredictor', labels_test, pred_labels, nnm))

    print('classifying by decisionSetPredictor')
    pred_labels, nnm = decisionSetPredictor(rules, D_test, default_label)
    preds.append(Predictor('decisionSetPredictor', labels_test, pred_labels, nnm))

    return preds

