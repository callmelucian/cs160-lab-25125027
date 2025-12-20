### THIS CODE IS WRITEN BY GEMINI 3.0

import matplotlib.pyplot as plt
import os

def visualize_map_matching_from_files(input_filename="input.txt", output_filename="output.txt"):
    ### Visualizes the map, raw GPS trajectory, and map-matched trajectory 
    ### by reading data from specified input and output files.
    
    ### The output file now expects an integer 'Length' on the first line 
    ### indicating the number of matched points that follow.
    
    ### Args:
    ###    input_filename (str): Path to the file containing map and raw GPS data.
    ###    output_filename (str): Path to the file containing map-matched data.
    ###
    
    # --- File Existence Check and Reading ---
    if not os.path.exists(input_filename):
        print(f"Error: Input file '{input_filename}' not found.")
        return
    if not os.path.exists(output_filename):
        print(f"Error: Output file '{output_filename}' not found.")
        return

    try:
        with open(input_filename, 'r') as f:
            input_lines = [line.strip() for line in f if line.strip()]
        with open(output_filename, 'r') as f:
            output_lines = [line.strip() for line in f if line.strip()]
    except Exception as e:
        print(f"Error reading files: {e}")
        return

    # --- Parsing Input (Same as before) ---
    
    if not input_lines:
        print("Error: Input file is empty.")
        return

    # Header: N (nodes), M (edges), K (trajectory points)
    try:
        header = input_lines[0].split()
        N, M, K = map(int, header)
    except ValueError:
        print("Error: Invalid header format in input file.")
        return

    # Parse Edges (Map)
    edges_polylines = []
    line_idx = 1
    
    for _ in range(M):
        if line_idx + 1 >= len(input_lines): break

        geom_line = input_lines[line_idx + 1].split()
        
        try:
            num_points = int(geom_line[0])
            coords = list(map(float, geom_line[1:]))
        except ValueError:
            print(f"Error parsing polyline coordinates near line {line_idx + 2}.")
            return

        polyline = []
        for i in range(0, len(coords), 2):
            if i + 1 < len(coords):
                polyline.append((coords[i], coords[i+1]))
            
        edges_polylines.append(polyline)
        line_idx += 2

    # Parse Raw Trajectory (P)
    raw_traj = []
    for _ in range(K):
        if line_idx >= len(input_lines): break
            
        p_info = input_lines[line_idx].split()
        try:
            raw_traj.append((float(p_info[0]), float(p_info[1])))
        except ValueError:
            print(f"Error parsing raw GPS point coordinates on line {line_idx + 1}.")
            return

        line_idx += 1

    # --- Parsing Matched Trajectory (NEW LOGIC) ---
    
    if not output_lines:
        print("Error: Output file is empty.")
        return

    # 1. Read Length L
    try:
        L = int(output_lines[0])
    except ValueError:
        print("Error: First line of output.txt must be an integer 'Length'.")
        return
    
    # 2. Parse L matched points
    matched_traj = []
    output_line_idx = 1
    
    for _ in range(L):
        if output_line_idx >= len(output_lines):
            print(f"Warning: Expected {L} matched points but ran out of data in output.txt.")
            break
            
        line = output_lines[output_line_idx]
        p_prime = line.split()
        
        try:
            matched_traj.append((float(p_prime[0]), float(p_prime[1])))
        except ValueError:
            print(f"Error parsing matched GPS point coordinates on line {output_line_idx + 1} of output.txt. Skipping.")
            
        output_line_idx += 1


    # --- Visualization ---
    fig, ax = plt.subplots(figsize=(10, 8))

    # 1. Plot Map Edges (Road Network)
    for poly in edges_polylines:
        if poly:
            xs, ys = zip(*poly)
            ax.plot(xs, ys, c='#d3d3d3', linewidth=2, zorder=1) # Light gray roads

    # 2. Plot Raw Trajectory (Red dashed line)
    if raw_traj:
        rx, ry = zip(*raw_traj)
        ax.plot(rx, ry, c='red', linestyle='--', alpha=0.6, label=f'Raw GPS ($P$, K={K})', zorder=2)
        ax.scatter(rx, ry, c='red', s=30, marker='x', zorder=2)

    # 3. Plot Matched Trajectory (Green solid line)
    if matched_traj:
        mx, my = zip(*matched_traj)
        ax.plot(mx, my, c='green', linewidth=3, label=f"Map Matched ($P'$, L={L})", zorder=3)
        ax.scatter(mx, my, c='green', s=40, marker='o', zorder=3)
        
    # --- NOTE on Blue Lines ---
    # Since the lengths K and L are now different, we CANNOT draw the direct 
    # connection (blue dotted lines) from P_i to P'_i as they no longer correspond
    # 1-to-1. The visualization is simplified to show the two distinct trajectories.

    # Styling
    ax.set_aspect('equal')
    ax.grid(True, linestyle=':', alpha=0.6)
    ax.legend()
    ax.set_title(f'Map Matching Result (Variable Length Output)\nMap: N={N}, M={M}')
    ax.set_xlabel('Longitude / X')
    ax.set_ylabel('Latitude / Y')

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":    
    # Run the visualization function
    visualize_map_matching_from_files()