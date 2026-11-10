
print("Starting...!")

import sys
import numpy as np
from pathlib import Path
import json
import time
from datetime import datetime

ROOT = Path(__file__).parents[4]

sys.path.insert(0, str(ROOT / "src" / "python"))

from bunseki import Bunseki

print("Imports finished!")
print("Loading dataset...")

DATASET_FOLDER = ROOT / "datasets" / "features"

failure_files = sorted(DATASET_FOLDER.glob("dataset_*_failures.json"))

failures = []

for failure_file in failure_files:
    item = json.loads(failure_file.read_text(encoding="utf-8"))
    failures.extend(item)

print(f"Total failures: {len(failures)}")

new_ids = []
new_songs = []
new_answers = []

remaining_failures = []

for indx, song_dict in failures:

    try:
        track_id = song_dict["track_id"]
        audio_path = song_dict["audio_path"]
        key_number = song_dict["key_number"]
        mode_number = song_dict["mode_number"]

        if not (0 <= key_number <= 11 and mode_number in {0, 1}):
            raise ValueError("Invalid key or mode")

        song_path = Path(audio_path)

        song = Bunseki(song_path=song_path)

        start = time.perf_counter()

        feats: np.ndarray = song.perception.numpy_chroma_features

        elapsed = time.perf_counter() - start

        if feats.shape != (36,):
            raise ValueError(f"Invalid features shape: {feats.shape}")

        y_answer = np.zeros(24, dtype=np.float32)

        class_index = key_number + 12 * mode_number
        y_answer[class_index] = 1

        new_ids.append(int(track_id))
        new_songs.append(feats.astype(np.float32))
        new_answers.append(y_answer)

        print(f"Recovered {indx} | {elapsed:.4f}s")

    except Exception as error:
        print(f"FAILED {indx}: {error}")
        remaining_failures.append([indx, song_dict, str(error)])


if new_songs:
    np.savez_compressed(
        DATASET_FOLDER / "dataset_CORRECTED.npz",
        track_ids=np.array(new_ids, dtype=np.int64),
        features=np.stack(new_songs).astype(np.float32),
        y_answers=np.stack(new_answers).astype(np.float32)
    )

    print(f"\nRecovered: {len(new_songs)}")

else:
    print("\nNo songs recovered.")

remaining_path = DATASET_FOLDER / "remaining_failures.json"

remaining_path.write_text(
    json.dumps(remaining_failures, ensure_ascii=False, indent=4),
    encoding="utf-8"
)

print(f"Still failing: {len(remaining_failures)}")
print(f"Process ended at {datetime.now():%d/%m/%Y %H:%M:%S}")
