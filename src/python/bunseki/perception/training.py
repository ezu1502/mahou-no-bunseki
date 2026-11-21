from softmax_regression import SoftmaxRegression
from mlp import Perceptron
from feature_loading import get_dataset_matrices
import numpy as np
from pathlib import Path

ROOT = Path(__file__).parents[4]
MODELS_DATASET = ROOT / "datasets" / "models"
MODEL_WEIGHTS = MODELS_DATASET / "softmax_regression_v2.npz"

ids, X, Y = get_dataset_matrices()

def softmax_training(): # Epoch atual: 600_000
    regression = SoftmaxRegression(X, Y)
    regression.load_weights(MODEL_WEIGHTS)
    regression.set_start_epoch(599_800)
    regression.set_learning_rate(1.2)

    for i in range(300001):

        to_print = i % 200 == 0
        regression.epochs(1, log_loss=to_print)

        if to_print:
            predictions = np.argmax(regression.predict(), axis=1)
            answers = np.argmax(regression.Y, axis=1)

            accuracy = np.mean(predictions == answers)

            print(f"Accuracy: {accuracy:.2%}")


    regression.save_weights(
        MODELS_DATASET / "softmax_regression_v2.npz"
    )


def perceptron_training(): # Epoch atual: 30_000
    mlp = Perceptron(X, Y)
    mlp.load_parameters(MODELS_DATASET / "perceptron_v1.npz")
    mlp.set_learning_rate(0.1)
    mlp.set_epoch_index(15000)

    mlp.training(epoch_number = 15000)
    mlp.save_parameters(MODELS_DATASET / "perceptron_v2.npz")

perceptron_training()
