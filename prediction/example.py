#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Mar  8 13:01:55 2021

@author: rve
"""

import sys
import lbic_car
import lbic_car_utils as utils
import time
import faulthandler

faulthandler.enable()
arg = utils.Arguments(sys.argv[1])

print('evaluating biclusters')
start_timeE = time.time()
summary, bicsRowsClass = lbic_car.getInfoOptimalRulePerBic(arg.dtset.bics, arg.dtset.labels_train, attrWeights=None,
                                                           attrCosts=None)
end_timeE = time.time() - start_timeE

print('runtime evaluating = ', end_timeE)
print('n bics: ', len(summary))
classCov = lbic_car.getClassCoverage(arg.dtset.labels_train, bicsRowsClass)

#lbic_car.car_ACCF(arg, summary)
lbic_car.car_CBA(arg, summary, bicsRowsClass, 1)
lbic_car.car_LazyCBA(arg, summary, bicsRowsClass)

#   tests
#selections = [('coverage', 1), ('coverage', 3), ('bestkRules', 100), ('bestkRulesPerLabel', 50), ('lazyPruneL3', 1)]
#selections = [('coverage', 1), ('lazyPruneL3', 1)]
#for selecT, param in selections:
#    classifierCAR = lbic_car.car_classifier_tests(arg, summary, bicsRowsClass, selecT, param)
#    for pred in classifierCAR.preds:
#        pred.printmeasures()