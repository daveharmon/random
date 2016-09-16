import math as math

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

