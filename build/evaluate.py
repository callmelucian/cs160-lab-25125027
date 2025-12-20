import geopandas as gpd
from tqdm import tqdm
import subprocess
import os

def exportInput (filePath, nodes, edges):
    with open(filePath, 'w') as f:
        N, M = len(nodes), len(edges)
        f.write(f'{N} {M}\n')

        for index, row in tqdm(edges.iterrows()):
            u, v = row['u_compressed'] + 1, row['v_compressed'] + 1
            limit, isOneway = row['maxspeed'], int(row['oneway'])
            coords = list(row['geometry'].coords)
            polyline_coords_flat = [coord for point in coords for coord in point]
            coords_string = " ".join([f"{c:.6f}" for c in polyline_coords_flat])

            f.write(f"{u} {v} {limit:.6f} {isOneway}\n")
            f.write(f"{len(coords)} {coords_string}\n")

# This function is written by Gemini 3.0 Pro
def exportTrajectories(rawFile, trueFile, rawGPS, trueGPS, index):
    # Process Raw GPS
    rawRow = rawGPS.iloc[index]
    rawPoints = list(rawRow.geometry.coords)
    rawTimestamps = rawRow.timestamps
    
    with open(rawFile, 'w') as f:
        f.write(f"{len(rawPoints)}\n")
        timeStamp = 0
        for point, time in zip(rawPoints, rawTimestamps):
            timeStamp += 30
            f.write(f"{point[0]} {point[1]} {timeStamp}\n")

    # Process True GPS (Ground Truth)
    targetRouteID = rawRow.routeID
    trueRow = trueGPS[trueGPS['routeID'] == targetRouteID].iloc[0]
    truePoints = list(trueRow.geometry.coords)

    with open(trueFile, 'w') as f:
        f.write(f"{len(truePoints)}\n")
        for point in truePoints:
            f.write(f"{point[0]} {point[1]}\n")

# This function is written by Gemini 3.0 Pro
def evaluate(inputFile, rawFile, trueFile, nodes, edges, rawGPS, trueGPS, maxIter = 10):
    # 1. Export the graph structure once
    # exportInput(inputFile, nodes, edges)
    
    # Initialize list to store scores
    allScores = []
    
    # 2. Iterate through each trajectory
    numTrajectories = len(rawGPS)
    print(f"Starting evaluation of {numTrajectories} trajectories...")

    for i in range(numTrajectories):
        if i == maxIter: break
        print(f"--- Processing trajectory {i + 1}/{numTrajectories} ---")
        
        # A. Export the current raw and true trajectories
        exportTrajectories(rawFile, trueFile, rawGPS, trueGPS, i)
        
        # B. Call the external C++ executables
        # Assuming map-matching.exe outputs to a file read by checker.exe,
        # and checker.exe outputs the score to verdict.txt.
        try:
            subprocess.run(["./map-matching.exe"], check=True)
            subprocess.run(["./checker.exe"], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Error running external executable: {e}")
            continue # Skip to the next iteration if the executable fails

        # C. Read the floating point score from verdict.txt
        verdictFile = "./build/verdict.txt"
        
        if os.path.exists(verdictFile):
            try:
                with open(verdictFile, 'r') as f:
                    score = float(f.readline().strip())
                    allScores.append(score)
                    print(f"Score: {score}")
            except (ValueError, IOError) as e:
                print(f"Could not read or parse score from {verdictFile}: {e}")
        else:
            print(f"Warning: {verdictFile} not found after checker.exe ran.")

    # 4. Calculate and return the average score
    if allScores:
        averageScore = sum(allScores) / len(allScores)
        print(f"\n--- Evaluation Complete ---")
        print(f"Total Trajectories Scored: {len(allScores)}")
        print(f"Average Score: {averageScore}")
        return averageScore
    else:
        print("\nNo scores were successfully recorded.")
        return 0.0

nodes = gpd.read_file('./data/processed/map-nodes.gpkg')
edges = gpd.read_file('./data/processed/map-edges.gpkg')
rawGPS = gpd.read_file('./data/processed/raw-trajectory.gpkg')
trueGPS = gpd.read_file('./data/processed/ground-truths.gpkg')

# print('Nodes\n', nodes.head(), '\n')
# print('Edges\n', edges[edges['maxspeed'].isnull()]['maxspeed'], edges.info(), '\n')
# print('Raw GPS\n', rawGPS.head(), '\n')
# print('True GPS\n', trueGPS.head(), '\n')

evaluate('./build/network.txt', './build/trajectory.txt', './build/answer.txt', nodes, edges, rawGPS, trueGPS)