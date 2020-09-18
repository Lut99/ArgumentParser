# PYTHON ARGPARSE.py
#   by Lut99
#
# Created:
#   12/08/2020, 14:36:56
# Last edited:
#   12/08/2020, 14:44:02
# Auto updated?
#   Yes
#
# Description:
#   This file is here to test the functionaly of the argparse library, and
#   especially check how it deals with things.
#

import argparse


parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", help="Try me!")
parser.add_argument("-1", "--123", help="How u do dis!", action="store_true")
parser.add_argument("test")
parser.parse_args()
