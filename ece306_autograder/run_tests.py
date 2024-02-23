import unittest
from gradescope_utils.autograder_utils.json_test_runner import JSONTestRunner
from sys import argv

if __name__ == '__main__':
    suite = unittest.defaultTestLoader.discover('autograder_tests', 'lab.py')
    with open('/autograder/results/results.json', 'w') as f:
        JSONTestRunner(visibility='visible', stream=f).run(suite)