import utility as util
import dataload

# LOAD MAP, RAW TRAJECTORY AND GROUND TRUTH
nodes, edges = dataload.loadMap([
    # 'District 1, Ho Chi Minh City, Vietnam',
    # 'District 4, Ho Chi Minh City, Vietnam',
    # 'District 7, Ho Chi Minh City, Vietnam',
    'Ho Chi Minh City, Vietnam'
])

# SAVE THE GEODATAFRAME AS GPKG FILE
util.saveGPKG(edges, './data/processed/map-edges.gpkg')
util.saveGPKG(nodes, './data/processed/map-nodes.gpkg')
