import os
import shutil
import glob

def organize_sketches():
    # Get current directory
    current_dir = os.getcwd()
    
    # List all .ino files
    ino_files = glob.glob("*.ino")
    
    print(f"Found {len(ino_files)} .ino files to process.")
    
    for ino_file in ino_files:
        sketch_name = os.path.splitext(ino_file)[0]
        
        # Determine target folder path
        target_folder = os.path.join(current_dir, sketch_name)
        
        # Check if folder already exists
        if not os.path.exists(target_folder):
            print(f"Creating folder: {sketch_name}")
            os.makedirs(target_folder)
        
        # Move the .ino file
        source_path = os.path.join(current_dir, ino_file)
        dest_path = os.path.join(target_folder, ino_file)
        
        # Handle case where file might already exist in destination (rare if we just globbed it from root, but good safety)
        if os.path.exists(dest_path):
            print(f"Warning: {dest_path} already exists. Skipping move for {ino_file}.")
        else:
            print(f"Moving {ino_file} to {target_folder}")
            shutil.move(source_path, dest_path)
            
        # Check for dependencies
        # ref: https://www.arduino.cc/en/Guide/Libraries
        # If the sketch uses local "SimpleIR.h", it must be in the same folder.
        
        # Read the file content to check for includes
        has_simple_ir = False
        try:
            with open(dest_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                if '#include "SimpleIR.h"' in content:
                    has_simple_ir = True
        except Exception as e:
            print(f"Error reading {dest_path}: {e}")
            
        if has_simple_ir:
            print(f"  - Detected SimpleIR dependency in {ino_file}")
            # Copy SimpleIR.h and SimpleIR.cpp
            for lib_file in ["SimpleIR.h", "SimpleIR.cpp"]:
                lib_source = os.path.join(current_dir, lib_file)
                lib_dest = os.path.join(target_folder, lib_file)
                
                if os.path.exists(lib_source):
                    if not os.path.exists(lib_dest):
                        print(f"    - Copying {lib_file} to {target_folder}")
                        shutil.copy(lib_source, lib_dest)
                    else:
                        print(f"    - {lib_file} already exists in {target_folder}")
                else:
                    print(f"    - Warning: {lib_file} not found in root, cannot copy!")

    print("Reorganization complete.")

if __name__ == "__main__":
    organize_sketches()
