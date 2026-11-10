import sys
import numpy as np
from pathlib import Path

ROOT = Path(__file__).parents[4]

sys.path.insert(0, str(ROOT / "src" / "python"))

DATASET_FOLDER = ROOT / "datasets" / "features"


def get_dataset_matrices() -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
        Reads the Bunseki dataset and returns (ids, X, Y)
    """
    ids = [] # ids, autoexplicativo
    X = [] # features das musicas
    Y = [] # respostas em one-hot encoding

    for npz_file in sorted(DATASET_FOLDER.rglob("*.npz")):
        with np.load(npz_file) as batch:
            track_ids = batch["track_ids"]
            feats = batch["features"]
            answers = batch["y_answers"]

            ids.append(track_ids)
            X.append(feats)
            Y.append(answers)


    assert ids and X and Y
    assert len(ids) == len(X) == len(Y)




    return (
        np.concatenate(ids),
        np.concatenate(X),
        np.concatenate(Y),
    )
