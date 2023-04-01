# SysProP2

Our own implementation of a command-line shell. 

Partners
--------

Joshua Mondragon, jbm215

Luke Rivera, lbr58


We implemented extension 3 on this project


Test Plan (and descriptions)
-------------------

 - does our implentation of pwd work? -> enter pwd -> output should be current directory

 - does our implementation of cd work? -> enter pwd, enter cd [destination], enter pwd again -> should see different    
   directory name in output

 - does cd with no arguments bring user home? -> enter cd, enter pwd -> output should list home directory
 
 - does piping work? -> enter ls | sort -> expected output is sorted directory list

 - do we get an error message/ continuation of the loop upon bad syntax. -> enter nonsense -> expected output is error 
   message

 - does file redirection work? -> enter echo hello > newfile.txt -> if newfile.txt does not exist, mysh will create it.   
   regardless, it will then put text into file

 - do wildcards work? -> cat *.c -> will list code of all .c files in directory

Considering the edge cases

   multiple redirections:
 - program1 < input | program 2 > output -> expect error message; cannot change output more than twice  

 - program < input > output | program2 -> expect error message; cannot change output more than twice
  
    where are wildcards allowed?

    can you use a wildcard in the first word?
        ./m*sh
        yes!



