# BNRS - Bipart Network Recommemder System 1.0 (Apr 2015)

### valgrind
* `valgrind --tool=memcheck --leak-check=full -v --track-origins=yes --log-file=./xx ./run -m -i ~/Downloads/movielens_2c.txt -d 0.1`

### Usage
usage: bnrs [OPTION]

OPTION:
  -h:  help
  -m:  Calculate the result of mass algorithm
  -e:  Calculate the result of heats algorithm
  -H:  Calculate the result of hybrid algorithm
  -N:  Calculate the result of HNBI algorithm
  -S:  Calculate the result of mass score algorithm
  -w:  Calculate the result of mass score (only the third step) algorithm

OPTION with arguments:
  -i filename:
       File containing full dataset
       if -i is used, then -T and -t will be ignored.
       else, -T and -t both have to be present.

  -T filename:
       File containing train dataset

  -t filename:
       File containing test dataset

  -u filename:  
       File containing extra attribute of the recommending objects

  -d doubleValue:  
       Rate used to divide full dataset to train and test dataset
       only valid when -i option is used

  --hybrid-rate doubleValue:  
       Rate used in hybrid algorithm.
       only valid when -H option is used, otherwize this arg will be ignored.
       default: 0.2

  --HNBI-rate doubleValue:  
       Rate used in HNBI algorithm.
       only valid when -N option is used, otherwize this arg will be ignored.
       default: -0.8

  --mass-score-rate doubleValue:  
       Rate used in mass score algorithm.
       only valid when -S option is used, otherwize this arg will be ignored.
       default: -0.8

  --mass-score-third-rate doubleValue:  
       Rate used in mass score (only the third step algorithm.
       only valid when -w option is used, otherwize this arg will be ignored.
       default: -0.8

  -l intValue:  
       Number of times which the algorthim calculation need to be performed
       in order to get reasonable average result

  -L intValue:  
       Number of the recommended objects which will be used to computer metrics

  -s unsignedlongValue: 
       Random seed

OPTION addtional:
  -o logfilename:
       File the log system will output log to
