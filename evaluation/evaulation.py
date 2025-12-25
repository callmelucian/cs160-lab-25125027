import similaritymeasures
import numpy as np
from shapely.geometry import LineString, Point
from tqdm import tqdm
import subprocess
import os
import geopandas as gpd

# EXPORT GRAPH
def exportGraph (filePath, nodes, edges):
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

# EXPORT TRAJECTORIES
def exportTrajectories(rawFile, rawGPS, index):
    rawRow = rawGPS.iloc[index]
    rawPoints = list(rawRow.geometry.coords)
    
    with open(rawFile, 'w') as f:
        f.write(f"{len(rawPoints)}\n")
        timeStamp = 0
        for point in rawPoints:
            timeStamp += 30
            f.write(f"{point[0]} {point[1]} {timeStamp}\n")

# EVALUATE
def evaluate(rawFile, outputFile, mainFile, rawGPS, maxIter = 10):
    # Iterate through each trajectory
    numTrajectories = len(rawGPS)
    print(f'Starting evaluation of {numTrajectories} trajectories...')

    dataList = []
    for i in range(numTrajectories):
        if i == maxIter: break
        print(f'Processing trajectory {i + 1}/{numTrajectories}')
        exportTrajectories(rawFile, rawGPS, i)
        
        try:
            subprocess.run([mainFile], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Error running external executable: {e}")
            continue # Skip to the next iteration if the executable fails
    
        rawTrajectory = np.genfromtxt(outputFile, skip_header = 1)
        trajectory = LineString([Point(item) for item in rawTrajectory])
        dataList.append({ 'geometry': trajectory })

    return gpd.GeoDataFrame(
        data = dataList,
        geometry = 'geometry',
        crs = 'EPSG:32648'
    )

def calculateMetrics (matched, true):
    matchedData, trueData = np.array(matched.coords), np.array(true.coords)
    frechet = similaritymeasures.frechet_dist(matchedData, trueData)
    dtw, _ = similaritymeasures.dtw(matchedData, trueData)
    area = similaritymeasures.area_between_two_curves(matchedData, trueData)
    return frechet, dtw, area

def printList (nparr):
    print(f'- SUM: {np.sum(nparr) / len(nparr)}')
    print(f'- MIN: {np.min(nparr)}')
    print(f'- MAX: {np.max(nparr)}')

def compare (matchedGPS, trueGPS):
    frechetList, DTWList, areaList = [], [], []
    for matchedLine, trueLine in zip(matchedGPS['geometry'], trueGPS['geometry']):
        frechet, dtw, area = calculateMetrics(matchedLine, trueLine)
        frechetList.append(frechet)
        DTWList.append(dtw)
        areaList.append(area)

    print('Frechet distance:')
    printList(np.array(frechetList))
    print('Dynamic time warping:')
    printList(np.array(DTWList))
    print('Area')
    printList(np.array(areaList))

print('Starting reading data...')
nodes = gpd.read_file('../data/processed/map-nodes.gpkg')
edges = gpd.read_file('../data/processed/map-edges.gpkg')
trueGPS = gpd.read_file('../data/processed/ground-truths.gpkg')
rawGPS = gpd.read_file('../data/processed/raw-trajectory.gpkg')

print('Exporting graph...')
# exportGraph('network.txt', nodes, edges)
print('Evaluating...')
matchedGPS = evaluate('raw-path.txt', 'matched-path.txt', 'sample-mm.exe', rawGPS, 200)

# print(f'Found this matched GPS {len(matchedGPS)}')
# print(matchedGPS)

# print(f'Found this raw GPS {len(rawGPS)}')
# print(rawGPS)

# print(f'Found this true GPS {len(trueGPS)}')
# print(trueGPS)

print('Calculating Score...')
compare(matchedGPS, trueGPS)