#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Mar  8 13:01:55 2021

"""

import sys
import car
import car_utils as utils
import time
import faulthandler

faulthandler.enable()
arg = utils.Arguments(sys.argv[1])

print('evaluating biclusters')
start_timeE = time.time()
summary, bicsRowsClass = car.getInfoOptimalRulePerBic(arg.dtset.bics, arg.dtset.labels_train, attrWeights=None,
                                                           attrCosts=None)
end_timeE = time.time() - start_timeE

print('runtime evaluating = ', end_timeE)
print('n bics: ', len(summary))
classCov = car.getClassCoverage(arg.dtset.labels_train, bicsRowsClass)

car.car_Coverage(arg, summary, bicsRowsClass, 1)
car.car_Lazy(arg, summary, bicsRowsClass)
