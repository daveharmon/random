
import argparse

def generate_domain(year, month, day):
    """Generates a domain name for the given date."""
    domain = "www."

    for i in range(16):
        year = ((year ^ 8 * year) >> 11) ^ ((year & 0xFFFFFFF0) << 17)
        month = ((month ^ 4 * month) >> 25) ^ 16 * (month & 0xFFFFFFF8)
        day = ((day ^ (day << 13)) >> 19) ^ ((day & 0xFFFFFFFE) << 12)
        domain += chr(((year ^ month ^ day) % 25) + 97)
    return domain

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-o', '--output_file', dest='out_file', action='store', type=argparse.FileType('w'))
    args = parser.parse_args()


    for i in range(0, 1000000):
    	# this should be a random function to add a number of possible extensions

    	domain = generate_domain(i % 2016, i % 12, i % 30) + '.com'
    	args.out_file.write(domain + '\n') 
