# Questions to ask:
# 1) output for saveAsTextFile
# 2) N value for slaves
# 3) What to update in Makefile

from pyspark import SparkContext
import Sliding, argparse


def swap_map(value):
    return (value[1], value[0])


def bfs_map(value): #value is the (puzzle, level) tuple
    """ YOUR CODE HERE """ 
    lst = [(value)]
    if value[1] == level: 
        children = Sliding.children(WIDTH,HEIGHT,Sliding.hash_to_board(WIDTH, HEIGHT, value[0])) 
        for child in children:
            lst.append((Sliding.board_to_hash(WIDTH, HEIGHT, child),level+1))    
        return lst
    return lst

def bfs_reduce(value1, value2):
    """ YOUR CODE HERE """
    return min(value1, value2)


def solve_puzzle(master, output, height, width, slaves):
    global HEIGHT, WIDTH, level
    HEIGHT=height
    WIDTH=width
    level = 0

    sc = SparkContext(master, "python")

    """ YOUR CODE HERE """
    sol = Sliding.board_to_hash(WIDTH, HEIGHT, Sliding.solution(WIDTH, HEIGHT))    
    RDD = sc.parallelize([(sol,level)]) 
    count = RDD.count()
    RDD_count = 0
    search = True
    k = 1
    """ YOUR MAP REDUCE PROCESSING CODE HERE """
    while search:
        if k % 3== 0:
            RDD = RDD.flatMap(bfs_map).partitionBy(PARTITION_COUNT).reduceByKey(bfs_reduce) #PUT PARTITION_COUNT FOR 16
        else:
            RDD = RDD.flatMap(bfs_map).reduceByKey(bfs_reduce) 
        if k % 2 == 0:
            RDD_count = RDD.count() 
            if RDD_count == count: 
                search = False
            count = RDD_count
        k = k + 1
        level = level + 1
    """ YOUR OUTPUT CODE HERE """
    RDD = RDD.map(swap_map)  
    RDD.coalesce(slaves).saveAsTextFile(output)    
    #outputLst = RDD.collect()
    #for elem in outputLst:
       #output(str(elem[0]) + " " + str(elem[1])) #output the elements
    sc.stop()



""" DO NOT EDIT PAST THIS LINE

You are welcome to read through the following code, but you
do not need to worry about understanding it.
"""

def main():
    """
    Parses command line arguments and runs the solver appropriately.
    If nothing is passed in, the default values are used.
    """
    parser = argparse.ArgumentParser(
            description="Returns back the entire solution graph.")
    parser.add_argument("-M", "--master", type=str, default="local[8]",
            help="url of the master for this job")
    parser.add_argument("-O", "--output", type=str, default="solution-out",
            help="name of the output file")
    parser.add_argument("-H", "--height", type=int, default=2,
            help="height of the puzzle")
    parser.add_argument("-W", "--width", type=int, default=2,
            help="width of the puzzle")
    parser.add_argument("-S", "--slaves", type=int, default=6,
            help="number of slaves executing the job")
    args = parser.parse_args()

    global PARTITION_COUNT
    PARTITION_COUNT = args.slaves * 16

    # call the puzzle solver
    solve_puzzle(args.master, args.output, args.height, args.width, args.slaves)

# begin execution if we are running this file directly
if __name__ == "__main__":
    main()
