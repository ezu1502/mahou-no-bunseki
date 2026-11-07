from pathlib import Path
import mirdata
from mirdata.datasets.fma_keys import Track
import json


# nesse estamos usando o Python 3.14

DATA_HOME = Path(__file__).parent.parent / "datasets" / "fma_keys"

METADATA_JSON = Path(__file__).parent.parent / "datasets" / "fma_songs_metadata.json"

dataset = mirdata.initialize("fma_keys", data_home = str(DATA_HOME))

dataset.download(partial_download = ["metadata"])

metadata = []

length = len(dataset.track_ids)

for i, track_id in enumerate(dataset.track_ids):
    track: Track = dataset.track(track_id)

    if i % (length // 10) == 0:
        print(f"{i * 100 / length :.0f}%")
        
    metadata.append({
        "track_id": track_id,
        "audio_path": track.audio_path,
        "key_number": track.key_number,
        "mode_number": track.mode_number
    })

with open(METADATA_JSON, "w", encoding = "utf-8") as file:
    json.dump(metadata, file, indent = 4)






