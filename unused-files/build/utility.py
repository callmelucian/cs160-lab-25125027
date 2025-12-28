# THIS FILE CONTAINS HELPER FUNCTIOS FOR LOADING DATA
# SUCH AS MAP (FROM OSMNX), RAW TRAJECTORY AND GROUND TRUTHS
# ALL FUNCTIONS ARE IMPLEMENTED BY ME WITH GUIDANCE FROM
# GEMINI 2.5 FLASH AND GEMINI 3.0 PRO

# IMPORT LIBRARIES
from shapely.geometry import LineString
import matplotlib.pyplot as plt
import geopandas as gpd
import pandas as pd
import numpy as np
import osmnx as ox
import json


# LOAD MAP FROM OPEN-STREET-MAP
def loadMap (places):
    # Download graph from osmnx
    print('Download map...')
    G = ox.graph_from_place(places, network_type='drive')

    # Graph projection
    print('Project map...')
    G_proj = ox.project_graph(G)
    nodes, edges = ox.graph_to_gdfs(G_proj)
    edges = edges.reset_index()

    # Compress node IDs
    print('Compress node IDs...')
    node_id_map = {osm_id: new_id for new_id, osm_id in enumerate(nodes.index)}
    edges['u_compressed'] = edges['u'].map(node_id_map)
    edges['v_compressed'] = edges['v'].map(node_id_map)
    edges['maxspeed'] = pd.to_numeric(edges['maxspeed'], errors='coerce')
    edges['maxspeed'] = edges['maxspeed'].fillna(40)
    # edges['maxspeed'] = pd.to_numeric(edges['maxspeed'].fillna(40), errors = 'coerce')
    nodes['compressed_id'] = nodes.index.map(node_id_map)

    # Drop excessive columns
    edges = edges[['u_compressed', 'v_compressed', 'oneway', 'maxspeed', 'geometry']].reset_index()
    nodes = nodes[['y', 'x', 'compressed_id', 'geometry']].reset_index()

    # Report graph size
    print(f'Number of nodes: {len(nodes)}')
    print(f'Number of edges: {len(edges)}')
    print()

    return nodes, edges


# FIND COMMON ROUTE IDS
def commonID (gdfA, gdfB, columnName):
    common = np.intersect1d(gdfA[columnName].unique(), gdfB[columnName].unique())
    return common


# EXTRACTOR FOR LOADING GROUND TRUTHS
def extractGroundTruth (objectJSON):
    # Extract needed information
    routeID, routeVarID = objectJSON['RouteId'], objectJSON['RouteVarId']
    latList, lngList = objectJSON['lat'], objectJSON['lng']
    points = [(lngList[i], latList[i]) for i in range(len(latList))]

    # Return path as a dict
    return {
        'routeID': routeID,
        'routeVarID': routeVarID,
        'geometry': LineString(points)
    }


# EXTRACTOR FOR LOADING RAW GPS TRAJECTORIES
def extractGPSTrajectories (objectJSON):
    # Extract needed information
    pointList = objectJSON['tripList'][0]['pointList']
    points = [(pointList[i], pointList[i + 1]) for i in range(0, len(pointList), 2)]
    timeStamps = [(i + 1) * 30 for i in range(len(points))]

    # Return path as a dict
    if len(points) == 1: return None
    return {
        'vehicle-number': objectJSON['vehicleNumber'],
        'routeID': objectJSON['routeId'],
        'varID': objectJSON['varId'],
        'geometry': LineString(points),
        'timestamps': timeStamps
    }


# LOAD DATA FROM JSON FILE
def loadJSON (filePath, extractor, usedCRS, targetCRS, maxRow = 5000):
    # Initialize data list
    print('Initialize...')
    dataList = []
    with open(filePath, 'r') as f:
        lineID = 0
        for line in f:
            # Control maximum number of JSON lines
            if lineID >= maxRow or line == None: break
            lineID += 1

            # Get the next line of the target file
            try:
                objectJSON = json.loads(line.strip())
            except json.JSONDecodeError:
                print(f'Line {lineID} skipped')
            
            # Append new item
            item = extractor(objectJSON)
            if item != None: dataList.append(item)
    
    # Return a GeoDataFrame that matches the map's crs
    print('Creating GeoDataFrame...')
    print(f'Found {len(dataList)} items\n')

    gdf = gpd.GeoDataFrame(
        data = dataList,
        geometry = 'geometry',
        crs = usedCRS
    ).to_crs(targetCRS)
    return gdf


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


# SAVE GEODATAFRAME AS GPKG FILE
def saveGPKG (gdf, filePath):
    print('Exporting...')
    gdf.to_file(filePath, driver = 'GPKG')
    print(f'GeoDataFrame stores as {filePath}\n')


# FILTER GEODATAFRAME TO SAVE MEMORY
def filterGDF (gdf, columnName, targetValue):
    return gdf[gdf[columnName].isin(targetValue)]