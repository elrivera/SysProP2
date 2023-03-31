# SysProP2

Our own implementation of a command-line shell. 

Partners
--------

Joshua Mondragon, jbm215

Luke Rivera, lbr58


We implemented extension 3 on this project


Test Plan (and descriptions)
-------------------

 does our implementation of cd work?
 
 does our implentation of pwd work?
 
 does piping work?

 do we get an error message/ continuation of the loop upon bad syntax.

 how does the shell deal with nonsense?


 Considering the edge cases

 - program1 < input | program 2 > output
    fine!

 - program < input > output | program2
    problem! does output of program 1 go to output1 or the pipe?
   whould we just not allow this and report an error?


    where are wildcards allowed?

    can you use a wildcard in the first word?
        ./m*sh
        yes!



