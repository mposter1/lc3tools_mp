import unittest
from gradescope_utils.autograder_utils.decorators import partial_credit, visibility


class TestLab(unittest.TestCase):
    def _get_score_public(self, file):
        #print(file)
        try:
            with open(file, 'r') as f:
                lines = f.read()
                print(lines)
                lines = lines.splitlines()
                if lines[-1].startswith('Total points earned:'):
                    return float(lines[-1].split()[-1][1:-2])
                else:
                    return 0.0
        except Exception as e:
            print(e)
            return 0.0

    def _get_score_private(self, file):
        #print(file)
        try:
            with open(file, 'r') as f:
                lines = f.read()
                #print(lines)
                lines = lines.splitlines()


                for li,line in enumerate(lines):
                    line_items = line.split()
                    if line_items[0] == "Test:":
                        line_temp_index = li+1
                        while "earned" not in lines[line_temp_index]:
                            line_temp_index +=1
                        points_earned = lines[line_temp_index].split()[3]
                        print(line[:line.index("(")] + points_earned)
                #print(lines[0])



                if lines[-1].startswith('Total points earned:'):
                    print(lines[-1])
                    return float(lines[-1].split()[-1][1:-2])
                else:
                    return 0.0
        except Exception as e:
            print(e)
            return 0.0

    @partial_credit(40)
    @visibility('visible')
    def test_public(self, set_score=None):
        """Public test case"""
        set_score(self._get_score_public(
            '/autograder/submission/public.out') / 100 * 40)

    @partial_credit(40)
    @visibility('visible')
    def test_private(self, set_score=None):
        """Private test case"""
        set_score(self._get_score_private(
            '/autograder/submission/private.out') / 100 * 40)
