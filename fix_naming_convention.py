import os, shutil

locations = [
    r'C:\Users\alfaswz\Desktop\MOODS CITYES SKYLINES\SourceAssets',
    os.path.expandvars(r'%USERPROFILE%\AppData\LocalLow\Colossal Order\Cities Skylines II\CustomAssets')
]

for loc in locations:
    if not os.path.exists(loc):
        continue
    print(f'Processing location: {loc}')
    
    # 1. Estadio_Carlos_Tartiere -> Estadio_Tartiere
    old_estadio = os.path.join(loc, 'Estadio_Carlos_Tartiere')
    new_estadio = os.path.join(loc, 'Estadio_Tartiere')
    if os.path.exists(old_estadio):
        # Rename files inside first
        for f in os.listdir(old_estadio):
            if f.startswith('Estadio_Carlos_Tartiere_'):
                new_f = f.replace('Estadio_Carlos_Tartiere_', 'Estadio_Tartiere_')
                old_file_path = os.path.join(old_estadio, f)
                new_file_path = os.path.join(old_estadio, new_f)
                os.rename(old_file_path, new_file_path)
                print(f'  Renamed {f} -> {new_f}')
        if not os.path.exists(new_estadio):
            os.rename(old_estadio, new_estadio)
            print(f'Renamed directory {old_estadio} -> {new_estadio}')
            
    # 2. Santa_Maria_Naranco -> SantaMaria_Naranco
    old_naranco = os.path.join(loc, 'Santa_Maria_Naranco')
    new_naranco = os.path.join(loc, 'SantaMaria_Naranco')
    if os.path.exists(old_naranco):
        for f in os.listdir(old_naranco):
            if f.startswith('Santa_Maria_Naranco_'):
                new_f = f.replace('Santa_Maria_Naranco_', 'SantaMaria_Naranco_')
                old_file_path = os.path.join(old_naranco, f)
                new_file_path = os.path.join(old_naranco, new_f)
                os.rename(old_file_path, new_file_path)
                print(f'  Renamed {f} -> {new_f}')
        if not os.path.exists(new_naranco):
            os.rename(old_naranco, new_naranco)
            print(f'Renamed directory {old_naranco} -> {new_naranco}')

print('All renames completed successfully!')
