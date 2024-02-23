lab=$1

/autograder/source/lc3tools/build/bin/./${lab}_private /autograder/submission/$lab.asm > /autograder/submission/private.out
/autograder/source/lc3tools/build/bin/./${lab}_public /autograder/submission/$lab.asm > /autograder/submission/public.out

python3 run_tests.py