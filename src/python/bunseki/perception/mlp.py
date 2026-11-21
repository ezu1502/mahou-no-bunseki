import numpy as np
from pathlib import Path

SONG_COUNT: int = 5488
FEATURE_COUNT: int = 36
KEY_COUNT: int = 24

ROOT = Path(__file__).parents[4]
MODELS_DATASET = ROOT / "datasets" / "models"

import time

ValueTrio = tuple[np.ndarray, np.ndarray, np.ndarray]

class Perceptron:
    def __init__(self, x_values: np.ndarray, y_values: np.ndarray) -> None:
        if x_values.shape != (SONG_COUNT, FEATURE_COUNT) or y_values.shape != (SONG_COUNT, KEY_COUNT):
            raise ValueError("Invalid X and Y shapes")

        self.X = x_values
        self.Y = y_values

        self.W1 = np.random.randn(FEATURE_COUNT, 128) * np.sqrt(2 / FEATURE_COUNT)
        self.b1 = np.zeros(128)

        self.W2 = np.random.randn(128, 64) * np.sqrt(2 / 128)
        self.b2 = np.zeros(64)

        self.W_OUT = np.random.randn(64, 24) * np.sqrt(2 / 64)
        self.b_OUT = np.zeros(24)

        self.learning_rate: float = 0.01

        self.epoch_index: int = 0

    def set_epoch_index(self, epoch: int) -> None:
        if epoch < 0:
            raise ValueError("Invalid epoch number")
        self.epoch_index = epoch

    def set_learning_rate(self, lr: float) -> None:
        if lr < 0 or lr > 2:
            raise ValueError("Learning rate must be between 0 and 2")

        self.learning_rate = lr
        
    @property
    def weights(self) -> ValueTrio:
        return (self.W1, self.W2, self.W_OUT)

    @property
    def biases(self) -> ValueTrio:
        return (self.b1, self.b2, self.b_OUT)
        
    @property
    def parameters(self) -> tuple[ValueTrio, ValueTrio]:
        return (self.weights, self.biases)

    @staticmethod
    def softmax(input: np.ndarray) -> np.ndarray:
        exp_input = np.exp(input - np.max(input, axis = 1, keepdims = True))

        return exp_input / np.sum(exp_input, axis = 1, keepdims = True)

    def forward_pass(self) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
        # Primeira camada oculta
        z1: np.ndarray = self.X @ self.W1 + self.b1
        a1 = np.maximum(0, z1) # ReLU

        # Segunda camada oculta
        z2: np.ndarray = a1 @ self.W2 + self.b2
        a2 = np.maximum(0, z2)

        # Camada de saída
        z3: np.ndarray = a2 @ self.W_OUT + self.b_OUT

        P: np.ndarray = self.softmax(z3)

        return (z1, a1, z2, a2, z3, P)

    def cross_entropy(self, P: np.ndarray) -> float:
        P = np.clip(P, 1e-12, 1.0)
        
        log_probabilities = self.Y * np.log(P) # Multiplicação comum, tá certo

        s = np.sum(log_probabilities, axis = 1)

        mean = np.mean(s)

        return -mean

    def accuracy(self, P: np.ndarray) -> float:
        predictions = np.argmax(P, axis = 1)
        targets = np.argmax(self.Y, axis = 1)

        return float(np.mean(predictions == targets))

    def backward_pass(self, forward_pass: tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray]):
        z1, a1, z2, a2, z3, P = forward_pass

        dZ3 = P - self.Y
        dW_OUT = (a2.T @ dZ3) / SONG_COUNT
        db_OUT = np.sum(dZ3, axis=0) / SONG_COUNT

        dA2 = dZ3 @ self.W_OUT.T
        dZ2 = dA2 * (z2 > 0)

        dW2 = (a1.T @ dZ2) / SONG_COUNT
        db2 = np.sum(dZ2, axis = 0) / SONG_COUNT

        dA1 = dZ2 @ self.W2.T
        dZ1 = dA1 * (z1 > 0)

        dW1 = (self.X.T @ dZ1) / SONG_COUNT
        db1 = np.sum(dZ1, axis = 0) / SONG_COUNT

        return (dW1, db1, dW2, db2, dW_OUT, db_OUT)

    def update_weights(self, dW1: np.ndarray, db1: np.ndarray, dW2: np.ndarray, db2: np.ndarray, dW_OUT: np.ndarray, db_OUT: np.ndarray):
        self.W1 -= dW1 * self.learning_rate
        self.b1 -= db1 * self.learning_rate

        self.W2 -= dW2 * self.learning_rate
        self.b2 -= db2 * self.learning_rate

        self.W_OUT -= dW_OUT * self.learning_rate
        self.b_OUT -= db_OUT * self.learning_rate

    def save_parameters(self, parameters_path: Path) -> None:
        np.savez_compressed(
            parameters_path,
            W1 = self.W1,
            b1 = self.b1,
            W2 = self.W2,
            b2 = self.b2,
            W_OUT = self.W_OUT,
            b_OUT = self.b_OUT
        )

        print(f"Parameters saved to {parameters_path}")

    def load_parameters(self, parameters_path: Path) -> None:
        if not parameters_path.is_file():
            raise FileNotFoundError(f"Couldn't find file: {parameters_path}")
        
        with np.load(parameters_path) as data:
            if not all(value in data for value in ["W1", "b1", "W2", "b2", "W_OUT", "b_OUT"]):
                raise ValueError("File does not contain all needed parameters")

            w1 = data.get("W1")
            b1 = data.get("b1")

            w2 = data.get("W2")
            b2 = data.get("b2")

            w_out = data.get("W_OUT")
            b_out = data.get("b_OUT")

            if (
                w1.shape != (36, 128) or
                b1.shape != (128,) or
                w2.shape != (128, 64) or
                b2.shape != (64,) or
                w_out.shape != (64, 24) or
                b_out.shape != (24,)
            ):
                raise ValueError("Invalid parameter shapes")

            self.W1 = w1
            self.b1 = b1

            self.W2 = w2
            self.b2 = b2

            self.W_OUT = w_out
            self.b_OUT = b_out

    def training(self, epoch_number: int):
        for _ in range(epoch_number):
            # s = time.perf_counter()
            forward = self.forward_pass()

            backward = self.backward_pass(forward)

            self.update_weights(*backward)

            self.epoch_index += 1

            if self.epoch_index % 50 == 0:
                P = self.forward_pass()[-1]
                # f = time.perf_counter()
                print(
                    f"Epoch {self.epoch_index :02d}\n"
                    f"\tLoss: {self.cross_entropy(P)}\n"
                    f"\tAccuracy: {self.accuracy(P) :.2%}\n"
                    # f"\tTime: {f - s}"
                )

        
            