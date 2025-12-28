import utility as util

# LOAD MAP, RAW TRAJECTORY AND GROUND TRUTH
nodes, edges = util.loadMap([
    # 'District 1, Ho Chi Minh City, Vietnam',
    # 'District 4, Ho Chi Minh City, Vietnam',
    # 'District 7, Ho Chi Minh City, Vietnam',
    'Ho Chi Minh City, Vietnam'
])
rawTrajectory = util.loadJSON('./data/unprocessed/pVehicle-all-22_03_24/pVehicle-all-22_03_24.json',
                         util.extractGPSTrajectories, edges.crs, edges.crs)
groundTruth = util.loadJSON('./data/unprocessed/bus 1 ngay/2022_03_27_businfo/paths.json',
                       util.extractGroundTruth, 'EPSG:4326', edges.crs)


# DROP THE ALL ROWS THAT DOESN'T HAVE MATCHED GROUND TRUTH
commonID = util.commonID(rawTrajectory, groundTruth, 'routeID')
rawTrajectory = util.filterGDF(rawTrajectory, 'routeID', commonID)
groundTruth = util.filterGDF(groundTruth, 'routeID', commonID)


# SAVE THE GEODATAFRAME AS GPKG FILE
util.saveGPKG(edges, './data/processed/map-edges.gpkg')
util.saveGPKG(nodes, './data/processed/map-nodes.gpkg')
util.saveGPKG(rawTrajectory, './data/processed/raw-trajectory.gpkg')
util.saveGPKG(groundTruth, './data/processed/ground-truths.gpkg')

# PREVIEW RESULTS
print(f'RAW TRAJECTORIES ({len(rawTrajectory)}):')
print(rawTrajectory.head(), '\n')

print(f'GROUND TRUTHS ({len(groundTruth)}):')
print(groundTruth.head(), '\n')


# VISUALIZE RESULT ON MAP
util.visualize(edges, rawTrajectory, groundTruth, '101')
