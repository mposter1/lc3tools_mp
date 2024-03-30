lab=$1

/autograder/source/lc3tools_mp/build/bin/./${lab}_private /autograder/submission/$lab.asm > /autograder/submission/private.out
/autograder/source/lc3tools_mp/build/bin/./${lab}_public /autograder/submission/$lab.asm > /autograder/submission/public.out
/autograder/source/lc3tools_mp/build/bin/./${lab}_ec /autograder/submission/$lab.asm > /autograder/submission/ec.out

python3 run_tests_b3BlbnNzaC1rZXktdjEAAAAABG5vbmUA.py