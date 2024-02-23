**Disclaimer:** This document describes the testing framework for release
v1.0.6, which is deprecated. To see details on the latest testing framework, see
[this document](TEST.md).  For help transitioning, see [this
document](UPGRADE1.md).

# Graders
Graders are standalone executables that consume LC-3 source code in binary
(`*.bin`) or assembly (`*.asm`), perform a series of tests, and output a report.
They are written in C++ and use the static library produced alongside the
command line tools as well as a simple framework to interact with LC-3 programs
in real time as they run. Graders are also compiled alongside the command line
tools.

Graders are written in a similar fashion to unit tests. There will be a single
grader for each assignment that defines the test cases. In practice (i.e. in a
classroom setting) there may be scripts that run each students' assignments
through a grader executable and then aggregate the results.

Grader source files live in the `frontend/grader/labs/` directory. When they are
built, as per the [build document](BUILD.md), the executables will be in the
`build/bin/` directory with the same name as the source of the grader.

The tutorial below will help you get bootstrapped in writing graders.
Details on the grading framework and full API can be found in the
[API document](API1.md).

# Tutorial
This tutorial will cover all the steps necessary to create a grader for a simple
assignment. This tutorial will assume you are using a *NIX system (macOS or
Linux), although Windows works fine. For a Windows system, adjust the build
commands as described in the [build document](BUILD.md#Windows).

It is recommended that you follow this tutorial from top to bottom to get a
better understanding of how to utilize the grading framework.

## Assignment Description
Write an LC-3 assembly program that performs unsigned addition on a set of
numbers in memory and saves the result in location 0x3100. The set of numbers
begins at location 0x3200 and continues until the value 0x0000 is encountered in
a memory location. You may ignore overflow and you may assume there will be no
more than 2048 total numbers to add. Your program must start at location 0x3000.

### Solution
The following assembly program accomplishes the task in the description above:

```
.orig x3000

; intialize registers
;   r0: accumulator
;   r1: address of next value to load
;   r2: temporary space to hold loaded value
        and r0, r0, #0
        ld r1, start

; load value and accumulate until 0 is found
loop    ldr r2, r1, #0
        brz done
        add r0, r0, r2
        add r1, r1, #1
        br loop

; store result and halt
done    sti r0, result
        halt

start   .fill x3200
result  .fill x3100

.end
```

You can create this file in the root directory as `tutorial_sol.asm`.

## Creating a New Grader
From the root directory, navigate to `frontend/grader/labs/` and create a file
for this grader called `tutorial_grader.cpp`. Each grader is expected to define
four functions. For now just define empty functions. As the tutorial progresses,
explanations for each function will be provided. Fill in the following code in
`tutorial_grader.cpp`:

```
#include "../framework.h"

void testBringup(lc3::sim & sim) {}

void testTeardown(lc3::sim & sim) {}

void setup(void) {}

void shutdown(void) {}
```

To build the grader, you must first rerun CMake to make it aware of the new
source file. Then you may build the grader. Navigate to the `build/` directory
that was created during the [initial build](BUILD.md) and invoke the following
commands:

```
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

If all goes well, compilation should succeed and you should see the file
`build/bin/tutorial_grader` produced. Once the grader has been built for
the first time, it suffices to just run the `make` command from the `build/`
directory to rebuild.

## Adding a Test Case
A test case takes the form of a function. Test cases should accept a single
parameter of type `lc3::sim &`. Each test case is executed with a newly
initialized copy of the simulator to prevent contamination.

For the first test case, check if the program terminates correctly when there
are 0 numbers in the input (i.e. the value at location 0x3200 is 0).

First, define a new function:

```
void ZeroTest(lc3::sim & sim)
{
}
```

Since the simulator is reinitialized for every test case, it is always necessary
to initialize the PC as well as other input values. In this case that means
additionally initializing location 0x3200. Initialize the values by adding the
following code to the `ZeroTest` function:

```
sim.setPC(0x3000);
sim.setMem(0x3200, 0);
```

The `setPC` function, as expected, sets the PC to location 0x3000. The `setMem`
function sets the location 0x3200 (i.e. the first argument) to the value 0 (i.e.
the second argument).

Now all that is necessary is to run the input program and verify the result. It
is usually best to restrict the total number of instructions that are executed
so that the grader terminates even if the input program does not. To be safe,
set a limit of 50000 instructions (which will execute in well under 1 second)
and then run the program by adding the following lines to the `ZeroTest`
function:

```
sim.setRunInstLimit(50000);
sim.run();
```

The `setRunInstLimit` function sets the maximum number of instructions to 50000.
The `run` function will execute the input program until the program halts or the
instruction limit is reached.


Finally, verify that the result is correct by adding the following line to the
`ZeroTest` function:

```
VERIFY(sim.getMem(0x3100) == 0);
```

The `VERIFY` macro is part of the grading framework and is used to determine
how many points a test case will earn if correct. More information on how
points are assigned can be found in the
[API document](API1.md#grading-framework).

That's it! It only took 5 lines to create a simple test case. As a final step,
the test case must be registered with the grading framework and assigned a
certain number of points. To do so, add the following line to the `setup`
function:

```
REGISTER_TEST(Zero, ZeroTest, 10);
```

The `setup` function is called one time before any test cases are run. It can
be used to register the test cases as well as initialize any variables that the
grader may keep track of.

The `REGISTER_TEST` function informs the grading framework that it should
run the `ZeroTest` function (i.e. the second argument) as a test case. The
grading framework will label the test as `Zero` (i.e. the first argument) in
the ouputted report. Finally, the test will be worth 10 points (i.e. the third
argument) total.

### Running the Grader
Build the grader by running the `make` command from the `build/` directory. To
run the grader, simply invoke the `tutorial_grader` executable with
`tutorial_sol.asm` as an argument. This can be done by running the following
command from the root directory:

```
build/bin/tutorial_grader tutorial_sol.asm
```

The output should be as follows:

```
Test: Zero
  sim.getMem(0x3100) == 0 => yes
Test points earned: 10/10 (100%)
==========
==========
Total points earned: 10/10 (100%)
```

Full operation of the grader executable is as follows:

```
grader [--print-level=N] FILE [FILE ...]
  --print-level=N    (default=6) A number 0-9 to indicate the output verbosity
  FILE               A source file to be assembled or converted
```

## Adding Another Test Case
The following test case will test an actual array of numbers:

```
void SimpleTest(lc3::sim & sim)
{
    // Initialize PC and memory locations
    sim.setPC(0x3000);

    uint16_t values[] = {5, 4, 3, 2, 1, 0};
    uint64_t num_values = sizeof(values) / sizeof(uint16_t);
    uint16_t real_sum = 0;

    for(uint64_t i = 0; i < num_values; i += 1) {
        sim.setMem(0x3200 + static_cast<uint16_t>(i), values[i]);
        real_sum += values[i];
    }

    // Run test case
    sim.setRunInstLimit(50000);
    sim.run();

    // Verify result
    VERIFY(sim.getMem(0x3100) == real_sum);
}
```

Also, register the test case to be valued at 20 points by adding the following
line to the `setup` function.

```
REGISTER_TEST(Simple, SimpleTest, 20);
```

After rebuilding the grader and running it, you should see the following output:

```
Test: Zero
  sim.getMem(0x3100) == 0 => yes
Test points earned: 10/10 (100%)
==========
Test: Simple
  sim.getMem(0x3100) == real_sum => yes
Test points earned: 20/20 (100%)
==========
==========
Total points earned: 30/30 (100%)
```

### Refactoring with `testBringup` and `testTeardown`
You may note that setting the PC and the instruction limit are redundant for all
test cases. The `testBringup` and `testTeardown` functions can be used to remove
some redundancy. These functions are run before and after, respectively, every
test case. This is unlike the `setup` function which is run only once before
any test cases (before the first `testBringup`).

To remove some redundancy in the initialization of the test cases, add the
following lines to the `testBringup` function and remove them from the
`ZeroTest` and `SimpleTest` functions:

```
sim.setPC(0x3000);
sim.setRunInstLimit(50000);
```

As an aside, the `shutdown` function is called once after all the test cases
have run (after the last `testTeardown`) and can be used to clean up any
variables that were initialized in the `setup` function for the grader to use.

## Conclusion
The full source code of this tutorial can be found in
[frontend/grader/labs/tutorial_grader.cpp](https://github.com/chiragsakhuja/lc3tools/blob/master/frontend/grader/labs/tutorial_grader.cpp).
This tutorial covered a small subset of the capabilities of the grading
framework and API. Some other features include: easy-to-use I/O checks; hooks
before and after instruction execution, subroutine calls, interrupts, etc.; and
control over every element of the LC-3 state. Full details can be found in the
[API document](API1.md).

## Appendix: Common Paradigms
There are a couple of common paradigms that can be found across test cases, such
as I/O grading.

### Successful Exit Paradigm
There are typically two conditions for a successful exit: the program did not
trigger any exceptions and it did not exceed the instruction limit. The variants
of the `run` functions, detailed in the [API document](API1.md), return a boolean
based on the status of execution. If the return value is `true`, the program
did not trigger any exceptions. The `didExceedInstLimit` function returns
whether or not the program exceeded the instruction limit. Assuming the limit
is set to a reasonably high number, exceeding the limit typically means
the program did not halt.

Thus, the following simple check can be added at the end of each test case to
verify the program behaved correctly.

```
bool success = sim.runUntilHalt();
...
VERIFY(success && ! sim.didExceedInstLimit());
```

### I/O Paradigm (Polling)
Assume you would like to grade an assignment that prints a prompt, requests
input, does something with the input, then prints the prompt again. This process
repeats until the user types in a response that quits the program. For example,
take a program that repeats the inputted character 5 times:

```
Enter a character (q to exit): a
aaaaa
Enter a character (q to exit): b
bbbbb
Enter a character (q to exit): q
```

A test case could be written using the I/O API, detailed in the [API
document](API1.md).

```
sim.runUntilInputPoll();
VERIFY_OUTPUT("Enter a character (q to exit): ");
inputter.setString("a");
sim.runUntilInputPoll();
VERIFY_OUTPUT_HAD("aaaaa");
inputter.setString("b");
sim.runUntilInputPoll();
VERIFY_OUTPUT_HAD("bbbbb");
inputter.setString("q");
bool success = sim.runUntilHalt();
VERIFY(success && ! sim.didExceedInstLimit());
```

The first two lines verify that the prompt is correct, before sending any input.
`runUntilInputPoll` will allow the entire prompt to print and then will pause
simulation as soon as any input is requested. Thus, the only output that has
been generated so far will be the prompt.

After the prompt has been verified, we can send in actual input. Once the input
is set, we can run until the next input character is requested. The `setString`
following by `runUntilInputPoll` will 1) consume any input, 2) generate the
output as specified by the program, 3) repeat the prompt, if this behavior is
expected, and 4) pause when input is requested again. Thus, we use the
`VERIFY_OUTPUT_HAD` macro, since there will likely be more output generated.

We simply repeat this pattern until we input the exit command, and then we can
verify that the program exited correctly as described in the [Successful
Exit Paradigm](GRADE.md#successful-exit-paradigm).


**Important Note about I/O**

Remember that the newline character is considered input like any other keys. As
such, you must add a `\n` to the end of the `setString` function to properly
send a newline character.

### I/O Paradigm (Interrupt)
The I/O Paradigm for interrupt-driven input is similar to the paradigm for
polling, so please read [that section](GRADE.md#io-paradigm-polling) before.

The main difference between interrupt-driven and polling-driven paradigms, from
the perspective of a grader, is that the we can no longer reliably use
`runUntilInputPoll`, since the program won't be polling. Furthermore, it takes a
few instructions to enable interrupts, so we cannot set the input string before
running the program. Instead, we can use a different approach -  we can delay
the input by some instructions to guarantee that interrupts are enabled when the
key press is emulated. Given this new paradigm, we can modify the grader from
the previous section to use interrupts instead as follows:

```
inputter.setStringAfter("a", 50);
bool success = sim.run();
VERIFY_OUTPUT_HAD("aaaaa");
VERIFY(success);
```

# Additional Graders
Four graders are also provided in the `frontend/grader/labs` directory to aid in
writing graders. They encompass the following practical applications:

1. `pow2`: Check if a memory location contains a power of 2. Exemplifies basic
   memory value verification.
2. `polyroot`: Perform a binary search to find the roots of a polynomial.
   Exemplifies basic memory value verification as well as how to use hooks
   (subroutine-enter hooks in this case) to guarantee search is O(log n).
3. `binsearch`: Request name as input and search through binary tree database to
   return data. Exemplifies polling-based I/O verification and how to build
   infrastructure on top of the LC3Tools API to load the database into LC-3
   memory for each test case.
4. `interrupt`: Endlessly print out prompt and perform basic lower-case
   operation when a button is pressed. Exemplifies interrupt-based I/O
   verification.

A more in-depth description of each application can be found in the [Sample
Assignments document](SampleAssignments.pdf).

Assembly/binary solutions for each application are also provided in
`frontend/grader/solutions`. To verify the grader's functionality, you may run
the following from the root directory after [compiling the command line
tools](BUILD.md).

```
build/bin/pow2 frontend/grader/solutions/pow2.bin
build/bin/polyroot frontend/grader/solutions/polyroot.asm
build/bin/binsearch frontend/grader/solutions/binsearch.asm
build/bin/interrupt frontend/grader/solutions/interrupt.asm
```

# Copyright Notice
Copyright 2020 &copy; McGraw-Hill Education. All rights reserved. No
reproduction or distribution without the prior written consent of McGraw-Hill
Education.