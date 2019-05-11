import sys
from string import ascii_lowercase
from random import randint, choice

def main():
    n = 25000
    if len(sys.argv) > 1:
        n = int(sys.argv[1])

    for i in range(n):
        print ("{0}".format(i))
    for i in range(n):
        print ("{0} {1}".format(i, 100))
    for i in range(n):
        print ("{0} {1}".format(i, -10))

main()
