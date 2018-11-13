import sys
import string

def readFile(file):
	mapping = dict()

	with open(file, 'r', encoding='big5-hkscs') as text:
		for line in text:
			first = line.split(' ')[0]
			wordList = line.split(' ')[1].split('/')
			if first not in mapping:
				mapping[first] = set(first)

			for i in range(len(wordList)):
				ZhuYin = wordList[i][0]
				if ZhuYin not in mapping:
					mapping[ZhuYin] = set(ZhuYin)
				else:
					mapping[ZhuYin].add(first)
	return mapping

def outputFile(fileName, mapping):
	file = open(fileName, 'w', encoding='big5-hkscs', newline=None)
	for c in mapping:
		file.write(c + ' ')
		for v in mapping[c]:
			file.write(v + ' ')		
		file.write("\n")
	file.close()

def main():
	mapping = readFile(sys.argv[1])
	outputFile(sys.argv[2], mapping)

if __name__ == "__main__":
	main()
