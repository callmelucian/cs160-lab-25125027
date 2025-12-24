import matplotlib.pyplot as plt
import geopandas as gpd
import numpy as np
from shapely.geometry import Point, LineString
import math

# SAVE GEODATAFRAME AS GPKG FILE
def saveGPKG (gdf, filePath):
    print('Exporting...')
    gdf.to_file(filePath, driver = 'GPKG')
    print(f'GeoDataFrame stores as {filePath}\n')

# VISUALIZE
def visualize (edges, rawTrajectory, groundTruth, routeID):
    rawGPS = rawTrajectory[rawTrajectory['routeID'] == routeID].head()
    trueGPS = groundTruth[groundTruth['routeID'] == routeID].head()

    fig, ax = plt.subplots(1, 1, figsize=(20, 24))
    edges.plot(ax = ax, color = 'red', label = 'Road edges', zorder = 1, linewidth = 0.8)
    trueGPS.plot(ax = ax, marker = 'x', color = 'green', markersize = 5, label = 'True GPS', zorder = 2)
    rawGPS.plot(ax = ax, marker = 'o', color = 'blue', markersize = 5, label = 'Raw GPS', zorder = 3)

    ax.set_title('Road Network, Raw GPS, True GPS')
    ax.legend(loc='lower right')
    plt.show()

# FILTER OUT USABLE RAW TRAJECTORY/GROUND TRUTH
# AND GENERATE SYNTHETIC DATASET
def processData (rawTrajectory, groundTruth):
    # Filter out common keys
    keyRaw = set(zip(rawTrajectory['routeID'], rawTrajectory['varID']))
    keyTruth = set(zip(groundTruth['routeID'], groundTruth['routeVarID']))
    keyCommon = sorted(list(keyRaw.intersection(keyTruth)))
    print(f'Found {len(keyCommon)} common keys')

    rawData, trueData, syntheticData = [], [], []
    for route, var in keyCommon:
        filteredRaw = rawTrajectory[(rawTrajectory['routeID'] == route) & (rawTrajectory['varID'] == var)]
        filteredTruth = groundTruth[(groundTruth['routeID'] == route) & (groundTruth['routeVarID'] == var)]

        if len(filteredRaw) and len(filteredTruth):
            filteredRaw = filteredRaw.iloc[[0]]
            filteredTruth = filteredTruth.iloc[0]

            rawData.append({ 'geometry': filteredRaw.geometry.translate(xoff = 148, yoff = -40).iloc[0] })
            trueData.append({ 'geometry': filteredTruth['geometry'] })
            syntheticData.append({ 'geometry': syntheticGPS(filteredTruth['geometry']) })
    
    return (
        gpd.GeoDataFrame(data = rawData, geometry = 'geometry', crs = 'EPSG:32648'),
        gpd.GeoDataFrame(data = trueData, geometry = 'geometry', crs = 'EPSG:32648'),
        gpd.GeoDataFrame(data = syntheticData, geometry = 'geometry', crs = 'EPSG:32648'),
    )

# GENERATE SYNTHETIC DATA WITH A STANDARD DEVIATION OF 50 METERS
# BY GENERATING A GPS PING EVERY 1-KM INTERVAL
def syntheticGPS (line, interval = 1000, stdDeviation = 50):
    distances = np.arange(0, line.length, interval)
    syntheticPoints = []

    for d in distances:
        perfectPoint = line.interpolate(d)

        r = np.random.normal(0, stdDeviation)
        theta = np.random.uniform(0, 2 * math.pi)
        dx = r * math.cos(theta)
        dy = r * math.sin(theta)

        syntheticPoints.append(Point(
            perfectPoint.x + dx,
            perfectPoint.y + dy,
        ))
    return LineString(syntheticPoints)