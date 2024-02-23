import unittest
from gradescope_utils.autograder_utils.decorators import partial_credit, visibility


class TestLab(unittest.TestCase):
    def _get_score(self, file):
        print(file)
        try:
            with open(file, 'r') as f:
                lines = f.read()
                print(lines)
                lines = lines.splitlines()
                if lines[-1].startswith('Total points earned:'):
                    return float(lines[-1].split()[-1][1:-2])
                else:
                    return 0.0
        except:
            return 0.0

    @partial_credit(30)
    @visibility('visible')
    def test_public(self, set_score=None):
        """Public test case"""
        set_score(self._get_score(
            '/autograder/submission/public.out') / 100 * 30)

    @partial_credit(50)
    @visibility('after_published')
    def test_private(self, set_score=None):
        """Private test case"""
        set_score(self._get_score(
            '/autograder/submission/private.out') / 100 * 50)
