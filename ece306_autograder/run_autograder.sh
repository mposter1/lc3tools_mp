lab=$1

/autograder/source/lc3tools_mp/build/bin/./${lab}_private  /autograder/submission/$lab.asm > /autograder/submission/private.out
/autograder/source/lc3tools_mp/build/bin/./${lab}_public --tester-verbose /autograder/submission/$lab.asm > /autograder/submission/public.out

python3 run_tests.py