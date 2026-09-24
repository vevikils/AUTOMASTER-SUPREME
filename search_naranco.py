import urllib.request
import urllib.parse
import json

headers = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)',
    'Accept': 'application/json'
}

queries = [
    'santa maria del naranco',
    'naranco oviedo',
    'santa maria naranco'
]

for q in queries:
    print("=== Query: " + q + " ===")
    encoded_q = urllib.parse.quote(q)
    url = "https://3dwarehouse.sketchup.com/warehouse/v1.0/entities?recordType=model&q=" + encoded_q + "&contentType=SKETCHUP"
    try:
        req = urllib.request.Request(url, headers=headers)
        with urllib.request.urlopen(req) as resp:
            data = json.loads(resp.read().decode('utf-8'))
            entries = data.get('entries', [])
            print("Total: " + str(data.get('totalEntries', 0)))
            for e in entries:
                print("  - ID: " + str(e.get('id')))
                print("    Title: " + str(e.get('title')))
                desc = str(e.get('description', ''))[:100]
                print("    Description: " + desc)
    except Exception as err:
        print("Error: " + str(err))
