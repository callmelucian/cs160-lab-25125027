import dataload
import utility as util

rawTrajectory = dataload.loadJSON('./data/unprocessed/pVehicle-all-22_03_24/pVehicle-all-22_03_24.json',
                                  dataload.extractGPSTrajectories, 'EPSG:32648', 'EPSG:32648')
groundTruth = dataload.loadJSON('./data/unprocessed/bus 1 ngay/2022_03_27_businfo/paths.json',
                                dataload.extractGroundTruth, 'EPSG:4326', 'EPSG:32648')

rawTrajectory, groundTruth, syntheticData = util.processData(rawTrajectory, groundTruth)

# SAVE THE GEODATAFRAME AS GPKG FILE
util.saveGPKG(rawTrajectory, './data/processed/raw-trajectory.gpkg')
util.saveGPKG(groundTruth, './data/processed/ground-truths.gpkg')
util.saveGPKG(syntheticData, './data/processed/synthetic-data.gpkg')

# PREVIEW RESULTS
print(f'RAW TRAJECTORIES ({len(rawTrajectory)}):')
print(rawTrajectory.head(), '\n')

print(f'GROUND TRUTHS ({len(groundTruth)}):')
print(groundTruth.head(), '\n')

print(f'SYNTHETIC DATA ({len(syntheticData)}):')
print(syntheticData.head(), '\n')