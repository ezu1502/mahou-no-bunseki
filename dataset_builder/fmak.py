from pathlib import Path
import mirdata
# nesse estamos usando o Python 3.14

DATA_HOME = Path(__file__).parent.parent / "datasets" / "fma_keys"

dataset = mirdata.initialize("fma_keys", data_home = str(DATA_HOME))

available = []

print("started...")

for track_id in dataset.track_ids:
    track = dataset.track(track_id)

    if Path(track.audio_path).exists():
        available.append(track_id)


print(available[:20])
print(len(available))