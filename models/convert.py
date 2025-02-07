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

# import sys

# def process_obj(input_file, output_file):
#     with open(input_file, 'r') as f:
#         lines = f.readlines()
    
#     new_lines = []
#     vertices = []
#     normals = []
#     objects = []
#     current_object = []
#     face_lines = []
    
#     for line in lines:
#         if line.startswith('o '):
#             if current_object:
#                 objects.append((current_object, face_lines))
#                 current_object = []
#                 face_lines = []
#             new_lines.append(line.strip())
#         elif line.startswith('v '):
#             vertices.append(line.strip())
#         elif line.startswith('vn '):
#             normals.append(line.strip())
#         elif line.startswith('f '):
#             face_lines.append(line.strip())
    
#     if current_object:
#         objects.append((current_object, face_lines))
    
#     new_vertex_list = []
#     new_normal_list = []
#     new_faces = []
#     vertex_index = 1
    
#     for obj, faces in objects:
#         new_lines.append(f"o {obj}")
#         for face in faces:
#             parts = face.split()
#             face_indices = []
#             for part in parts[1:]:
#                 v_idx, _, vn_idx = part.split('//')
#                 new_v_idx = vertex_index
#                 new_vertex_list.append(vertices[int(v_idx) - 1])
#                 new_normal_list.append(normals[int(vn_idx) - 1])
#                 face_indices.append(str(new_v_idx))
#                 vertex_index += 1
#             new_faces.append("f " + " ".join(face_indices))
    
#     new_lines.extend(new_vertex_list)
#     new_lines.extend(new_normal_list)
#     new_lines.extend(new_faces)
    
#     with open(output_file, 'w') as f:
#         f.write('\n'.join(new_lines) + '\n')

# if __name__ == "__main__":
#     if len(sys.argv) != 3:
#         print("Usage: python script.py input.obj output.obj")
#         sys.exit(1)
    
#     process_obj(sys.argv[1], sys.argv[2])

