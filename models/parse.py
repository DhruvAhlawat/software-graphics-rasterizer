import argparse
import sys
# parser = argparse.ArgumentParser(description='Parse 3D model data.')
# parser.add_argument('input', type=str, help='Path to the input file', default='models/icosphere.obj')
# parser.add_argument('output', type=str, help='Path to the output file', default='models/icosphere_data.txt')
# args = parser.parse_args()

# INPUT_PATH = args.input_path

if(len(sys.argv) < 2):
	print("pls add input file");
	exit();

INPUT_PATH = sys.argv[1]; 
OUTPUT_PATH = 'output.txt'
if(len(sys.argv) >= 3):
	OUTPUT_PATH = sys.argv[2];


with open(INPUT_PATH, 'r') as file:
	content = file.read().split('\n')

with open(OUTPUT_PATH, 'w') as outfile:
	outfile.write("Vertices\n")	# Vertices
	ind = 0
	while content[ind].split()[0] != 'v':
		ind += 1
	nv=0
	while content[ind].split()[0] == 'v':
		nv+=1
		line = content[ind].split()
		if line[0] != 'v':
			break
		line = line[1:]
		outfile.write(', '.join(line) + ', 1.00 ,\n')
		ind+=1
	outfile.write(f"{nv} vertices\n")
	outfile.write('-'*50+'\n\n')

	outfile.write("Normals\n")	# Normals

	while content[ind].split()[0] != 'vn':
		ind+=1
	nn = 0
	while content[ind].split()[0] == 'vn':
		nn+=1
		line = content[ind]
		if line.split()[0] != 'vn':
			continue
		line = line.split()[1:]
		outfile.write(', '.join(line) + ',\n')
		ind+=1

	outfile.write(f"{nn} normals\n")
	outfile.write('-'*50+'\n\n')

	outfile.write("Faces\n")

	while content[ind].split()[0] != 'f':
		ind+=1
	nf = 0
	while ind < len(content) and content[ind].split()[0] == 'f':
		nf+=1
		line = content[ind]
		
		line = line.split()[1:]
		for j in range(len(line)):
			line[j] = str(int(line[j].split('/')[0])-1)
		outfile.write(', '.join(line) + ',\n')
		ind+=1
		if ind>=len(content) or content[ind]=="": break
	outfile.write(f"{nf} faces\n")