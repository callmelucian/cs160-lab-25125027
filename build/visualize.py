import pandas as pd
import geopandas as gpd
from shapely.geometry import Point
import matplotlib.pyplot as plt
import subprocess
import os
import io

def readAndProcessFiles(visualizeFile, answerFile):
    """Reads coordinate files into Pandas DataFrames."""
    def readFileToDataFrame(filePath):
        try:
            with open(filePath, 'r') as f:
                k_line = f.readline().strip()
                if not k_line:
                    return None
                
                k = int(k_line)
                data_lines = [f.readline().strip() for _ in range(k)]
                data_content = "\n".join(data_lines)
                
                data_io = io.StringIO(data_content)
                
                df = pd.read_csv(
                    data_io,
                    sep=r'\s+',
                    header=None,
                    names=['X', 'Y']
                )
                return df
        except Exception:
            return None

    visualizeDf = readFileToDataFrame(visualizeFile)
    answerDf = readFileToDataFrame(answerFile)

    return visualizeDf, answerDf

# --- Visualization Function ---
def visualizeTrajectories(visualizeDf, answerDf):
    """Converts DataFrames to GeoDataFrames and visualizes the results."""
    if visualizeDf is None and answerDf is None:
        print("No data available for visualization.")
        return

    def toGeoDataFrame(df, crs=None):
        if df is None or df.empty:
            return None
        geometry = [Point(xy) for xy in zip(df['X'], df['Y'])]
        return gpd.GeoDataFrame(df, geometry=geometry, crs=crs)

    visualizeGdf = toGeoDataFrame(visualizeDf)
    answerGdf = toGeoDataFrame(answerDf)

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    ax.set_title("Map Matching Trajectory Comparison")

    if visualizeGdf is not None:
        visualizeGdf.plot(
            ax=ax, marker='o', color='blue', markersize=5, label='Output Trajectory', zorder=1
        )
    if answerGdf is not None:
        answerGdf.plot(
            ax=ax, marker='x', color='red', markersize=10, label='True Path', zorder=2
        )

    ax.legend()
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.grid(True, linestyle=':', alpha=0.5)
    plt.show()

visualizeDf, answerDf = readAndProcessFiles('./build/output.txt', './build/answer.txt')
visualizeTrajectories(visualizeDf, answerDf)