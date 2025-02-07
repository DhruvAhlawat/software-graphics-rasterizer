import sys
def process_obj(input_file, output_file):
    vertices = []  # List to store vertex positions
    normals = []   # List to store normal vectors
    faces = []     # List to store faces
    
    with open(input_file, 'r') as file:
        for line in file:
            parts = line.split()
            if not parts:
                continue
            if parts[0] == 'v':
                vertices.append(parts[1:])
            elif parts[0] == 'vn':
                normals.append(parts[1:])
            elif parts[0] == 'f':
                faces.append(parts[1:])
    
    new_vertices = []
    new_normals = []
    new_faces = []
    
    for face in faces:
        face_indices = []
        normal_index = None
        for vert in face:
            v_parts = vert.split('//')
            v_index = int(v_parts[0]) - 1
            normal_index = int(v_parts[1]) - 1 if len(v_parts) > 1 else None
            
            new_vertices.append(vertices[v_index])
            if normal_index is not None:
                new_normals.append(normals[normal_index])
            face_indices.append(len(new_vertices))
        
        new_faces.append(face_indices)
    
    with open(output_file, 'w') as file:
        for v in new_vertices:
            file.write(f'v {" ".join(v)}\n')
        for vn in new_normals:
            file.write(f'vn {" ".join(vn)}\n')
        for face in new_faces:
            file.write(f'f {" ".join(f"{idx}//{idx}" for idx in face)}\n')

if __name__ == "__main__":
    if(len(sys.argv) < 2):
        print("pls add input file");
        exit();
    input_path = sys.argv[1]; 
    output_path = 'perface.txt'
    if(len(sys.argv) >= 3):
        output_path = sys.argv[2];
    process_obj(input_path, output_path)
    print(f"Processed file saved as {output_path}")
