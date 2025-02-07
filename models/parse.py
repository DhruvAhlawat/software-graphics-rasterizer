with open('icosphere.obj', 'r') as file:
	content = file.read().split('\n')

with open('output.txt', 'w') as outfile:
	outfile.write("Vertices\n")	# Vertices
	ind = 0
	while content[ind].split()[0] != 'v':
		ind += 1

	while content[ind].split()[0] == 'v':
		line = content[ind].split()
		if line[0] != 'v':
			break
		line = line[1:]
		outfile.write(', '.join(line) + ',\n')
		ind+=1

	outfile.write('-'*50+'\n\n')

	outfile.write("Normals\n")	# Normals

	while content[ind].split()[0] != 'vn':
		ind+=1

	while content[ind].split()[0] == 'vn':
		line = content[ind]
		if line.split()[0] != 'vn':
			continue
		line = line.split()[1:]
		outfile.write(', '.join(line) + ',\n')
		ind+=1

	outfile.write('-'*50+'\n\n')

	outfile.write("Faces\n")

	while content[ind].split()[0] != 'f':
		ind+=1
	
	while ind < len(content) and content[ind].split()[0] == 'f':
		line = content[ind]
		
		line = line.split()[1:]
		for j in range(len(line)):
			line[j] = str(int(line[j].split('/')[0])-1)
		outfile.write(', '.join(line) + ',\n')
		ind+=1
		if ind>=len(content) or content[ind]=="": break