import os
import sys
import json

try:
    import nbtlib
except ImportError:
    os.system("pip install nbtlib")
    import nbtlib

json_data = nbtlib.parse_nbt(sys.argv[1])
print(json.dumps(json_data.unpack(json=True)))