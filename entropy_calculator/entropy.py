import math as math
import argparse

def entropy(string):
        "Calculates the Shannon entropy of a string"

        # get probability of chars in string
        prob = [ float(string.count(c)) / len(string) for c in dict.fromkeys(list(string)) ]

        # calculate the entropy
        entropy = - sum([ p * math.log(p) / math.log(2.0) for p in prob ])

        return entropy


def entropy_ideal(length):
        "Calculates the ideal Shannon entropy of a string with given length"

        prob = 1.0 / length

        return -1.0 * length * prob * math.log(prob) / math.log(2.0)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input_file', dest='in_file', action='store', type=argparse.FileType('r'))
    parser.add_argument('-o', '--output_file', dest='out_file', action='store', type=argparse.FileType('w'))
    args = parser.parse_args()

    for line in args.in_file:
    	args.out_file.write(str(entropy(line)) + '\t' + str(entropy_ideal(len(line))) + '\n')
