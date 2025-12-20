import json
import os

# Define the file name
filename = '.\data\\bus 1 ngay\\2022_03_27_businfo\\test.json'

try:
    # 1. Read the existing JSON data from the file
    with open(filename, 'r') as f:
        data = json.load(f)

    # 2. Write the data back to the file, using the 'indent' parameter
    # The 'indent=4' argument specifies 4 spaces for indentation.
    # The file is opened in 'w' (write) mode to overwrite the existing content.
    with open(filename, 'w') as f:
        json.dump(data, f, indent=4) 
        
    print(f"✅ Success! The file '{filename}' has been prettified and saved.")

except FileNotFoundError:
    print(f"❌ Error: File '{filename}' not found.")
except json.JSONDecodeError:
    print(f"❌ Error: File '{filename}' is not a valid JSON. Check for syntax issues like missing commas or quotes.")