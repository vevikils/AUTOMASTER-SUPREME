import json

data = {
  "$schema": "https://raw.githubusercontent.com/CitiesSkylines2Mods/schemas/main/building-prefab.json",
  "id": "SantaMariaDelNaranco",
  "name": "Santa María del Naranco (Monumento Prerrománico)",
  "category": "Landmarks",
  "subCategory": "CultureAndReligion",
  "service": "Park",
  "lotSize": {
    "width": 4,
    "depth": 3,
    "unitSizeMeters": 8,
    "totalWidthMeters": 32,
    "totalDepthMeters": 24
  },
  "geometry": {
    "lod0": "Santa_Maria_Naranco_LOD0.fbx",
    "lod1": "Santa_Maria_Naranco_LOD1.fbx",
    "lod2": "Santa_Maria_Naranco_LOD2.fbx",
    "collider": "Santa_Maria_Naranco_Collider.fbx",
    "heightMeters": 13.08
  },
  "textures": {
    "baseColor": "Santa_Maria_Naranco_BaseColor.png",
    "normal": "Santa_Maria_Naranco_Normal.png",
    "mask": "Santa_Maria_Naranco_Mask.png",
    "emissive": "Santa_Maria_Naranco_Emissive.png"
  },
  "gameplayAttributes": {
    "constructionCost": 120000,
    "upkeepCostPerMonth": 1800,
    "attractiveness": 95,
    "entertainment": 85,
    "entertainmentRadius": 2500,
    "touristAttractionFactor": 4.5,
    "maxVisitorCapacity": 250,
    "garbageAccumulation": 5,
    "waterConsumption": 5,
    "electricityConsumption": 12,
    "pollution": {
      "noise": 1,
      "air": 0,
      "ground": 0
    }
  },
  "culturalTags": [
    "Monument",
    "UNESCO",
    "WorldHeritage",
    "PreRomanesque",
    "PrerromanicoAsturiano",
    "Oviedo",
    "Asturias",
    "Spain",
    "Landmark",
    "SignatureBuilding",
    "RamiroI",
    "MonteNaranco"
  ],
  "author": "Cities Skylines 2 Modding Team & Oviedo Preservation"
}

with open(r"C:\Users\alfaswz\Desktop\MOODS CITYES SKYLINES\05_Cities_Skylines_Asset\SantaMariaDelNaranco.json", "w", encoding="utf-8") as f:
    json.dump(data, f, indent=2, ensure_ascii=False)

with open(r"C:\Users\alfaswz\Desktop\MOODS CITYES SKYLINES\SantaMariaDelNaranco.json", "w", encoding="utf-8") as f:
    json.dump(data, f, indent=2, ensure_ascii=False)

print("Saved SantaMariaDelNaranco.json!")

csv_path = r"C:\Users\alfaswz\Desktop\MOODS CITYES SKYLINES\05_Cities_Skylines_Asset\i18n\i18n.csv"
with open(csv_path, "a", encoding="utf-8") as f:
    f.write("Assets.NAME[SantaMariaDelNaranco]\tSanta María del Naranco\tSanta María del Naranco\n")
    f.write("Assets.DESCRIPTION[SantaMariaDelNaranco]\tPre-Romanesque palace and church on Mount Naranco in Oviedo, Asturias (built in 848 AD under King Ramiro I). UNESCO World Heritage site.\tPalacio y templo prerrománico en la falda del Monte Naranco, Oviedo (año 848 d.C., Reino de Asturias, rey Ramiro I). Joya cumbre del arte ramirense y Patrimonio de la Humanidad UNESCO.\n")
print("Updated i18n.csv!")
