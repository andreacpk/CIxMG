#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Mar  5 15:59:46 2021

@author: rve
"""

import numpy as np


# one-vs-all
def chi_squared(p, n, d_plus, d_minus):
    d = d_minus + d_plus
    pn = p + n

    p1n = (p-(pn/d)*d_plus)**2
    p1d = pn/d*d_plus
    p2n = (n-(pn/d)*d_minus)**2
    p2d = pn/d*d_minus
    p3n = (d_plus-p-(d-pn)/d*d_plus)**2
    p3d = (d-pn)/d*d_plus
    p4n = (d_minus-n-(d-pn)/d*d_minus)**2
    p4d = (d-pn)/d*d_minus

    if p1d != 0:
        p1 = p1n / p1d
    else:
        p1 = 0

    if p2d != 0:
        p2 = p2n / p2d
    else:
        p2 = 0

    if p3d != 0:
        p3 = p3n / p3d
    else:
        p3 = 0

    if p4d != 0:
        p4 = p4n / p4d
    else:
        p4 = 0

    return (p1 + p2 + p3 + p4)/d


# multiple classes
def chi_squared_multiple(labels_bic, labels_dtset):
    d = np.sum(labels_dtset)
    sizeA = np.sum(labels_bic)    
    
    aux = 0
    
    for i in range(len(labels_bic)):
        p = labels_bic[i]
        pt = labels_dtset[i]
        
        # belongs to bic
        p1n = (p-(sizeA/d)*pt)**2
        p1d = sizeA/d*pt
        
        # do not belong to bic
        p3n = (pt-p-(d-sizeA)/d*pt)**2
        p3d = (d-sizeA)/d*pt

        if p1d != 0:
            p1 = p1n / p1d
        else:
            p1 = 0

        if p3d != 0:
            p3 = p3n / p3d
        else:
            p3 = 0
        aux = aux+ p1+p3

    return aux/d


def ub_chi_squared(p, n, d_plus, d_minus):
    return max([chi_squared(0, n, d_plus, d_minus), chi_squared(p, 0, d_plus, d_minus)])
