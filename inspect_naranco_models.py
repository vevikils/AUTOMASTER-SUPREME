import urllib.request
import json

headers = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)',
    'Accept': 'application/json'
}

ids = [
    '1b9d28ca-af7b-49ca-9cc1-c03bbefd369b',
    '11f002ef-1fae-4d4b-b423-8da4655f73b8',
    '7af7c636-8360-44b6-8f9d-566ccb6d204c'
]

for mid in ids:
    print(f"\n================ MID: {mid} ================")
    url = f"https://3dwarehouse.sketchup.com/warehouse/v1.0/entities/{mid}"
    try:
        req = urllib.request.Request(url, headers=headers)
        with urllib.request.urlopen(req) as resp:
            data = json.loads(resp.read().decode('utf-8'))
        print("Title:", data.get('title'))
        print("Description:", data.get('description'))
        binaries = data.get('binaries', {})
        print("Binaries available:")
        for b_name, b_info in binaries.items():
            print(f"  - {b_name}: size={b_info.get('fileSize')} url={b_info.get('url')[:60]}...")
    except Exception as e:
        print("Error fetching:", e)
