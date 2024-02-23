lab=$1

/autograder/source/lc3tools_mp/build/bin/./${lab}_private --print-output --tester-verbose /autograder/submission/$lab.asm > /autograder/submission/private.out
/autograder/source/lc3tools_mp/build/bin/./${lab}_public --print-output --tester-verbose /autograder/submission/$lab.asm > /autograder/submission/public.out

python3 run_tests.py