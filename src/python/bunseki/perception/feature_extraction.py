print("Starting...!")

import numpy as np
from pathlib import Path
import json

from bunseki import Bunseki

print("Loading dataset...")

ROOT = Path(__file__).parents[4]

DATASET_PATH: Path = ROOT / "datasets" / "metadata.json"

dataset: list[dict] = json.loads(DATASET_PATH.read_text(encoding = "utf-8"))

# each song dict contains "track_id", "audio_path", "key_number" and "mode_number"

FEATURES_FOLDER: Path = ROOT / "datasets" / "features"
FEATURES_FOLDER.mkdir(parents = True, exist_ok = True)

track_ids: list[int] = []
features: list[np.ndarray] = []
y_answers: list[np.ndarray] = []

failures: list[tuple[int, dict]] = []

def save_batch(file_name: str) -> bool:
    if not all([track_ids, features, y_answers]):
        return False

    if not len(track_ids) == len(features) == len(y_answers):
        raise RuntimeError("Dataset arrays have different lengths!")
    
    if not file_name:
        return False
    
    np.savez_compressed(
        FEATURES_FOLDER / file_name,
        track_ids = np.array(track_ids, dtype = np.int64),
        features = np.stack(features).astype(np.float32),
        y_answers = np.stack(y_answers).astype(np.float32)
    )

    track_ids.clear()
    features.clear()
    y_answers.clear()


    print(f"{file_name.upper()} SAVED!")
    return True

batch_count: int = 0

for indx, song in enumerate(dataset):
    if indx > 0 and indx % 500 == 0 and track_ids and features and y_answers:
        if not save_batch(f"dataset_{batch_count :03}.npz"):

            print(f"Last batch: {batch_count :03}")
            raise RuntimeError("save_batch() can't save!")

        batch_count += 1

        

    track_id = song.get("track_id")
    audio_path = song.get("audio_path")
    key_number = song.get("key_number")
    mode_number = song.get("mode_number")

    if any(value is None for value in [track_id, audio_path, key_number, mode_number]):
        failures.append((indx, song))
        continue

    if not (0 <= key_number <= 11 and mode_number in {0, 1}): #type: ignore
        failures.append((indx, song))
        continue

    try:
        print("Creating Bunseki object..")
        song_bunseki = Bunseki(audio_path) #type: ignore

        print(f"Loading Chroma Features - song {indx}")
        feats: np.ndarray = song_bunseki.perception.numpy_chroma_features
    except Exception as e:
        print(e)
        failures.append((indx, song))
        continue

    if feats.shape != (36,):
        failures.append((indx, song))
        continue

    y_answer = np.zeros(24, dtype = np.float32)

    index = key_number + 12 * mode_number # portanto a ordem seria: tudo menor e depois tudo maior #type: ignore

    y_answer[index] = 1

    track_ids.append(int(track_id)) #type: ignore
    features.append(feats.astype(np.float32))   
    y_answers.append(y_answer)

    print(f"Song {indx} finished!")


if track_ids and features and y_answers:
    if not save_batch(f"dataset_{batch_count :03}.npz"):
        raise RuntimeError("save_batch() can't save!")

    

    