import numpy as np
from pathlib import Path

SONG_COUNT: int = 5488
FEATURE_COUNT: int = 36
KEY_COUNT: int = 24

ValueTrio = tuple[np.ndarray, np.ndarray, np.ndarray]

class Perceptron:
    def __init__(self, x_values: np.ndarray, y_values: np.ndarray) -> None:
        if x_values.shape != (SONG_COUNT, FEATURE_COUNT) or y_values.shape != (SONG_COUNT, KEY_COUNT):
            raise ValueError("Invalid X and Y shapes")

        self.X = x_values
        self.Y = y_values

        self.W1 = np.random.randn(FEATURE_COUNT, 128) * 0.01
        self.b1 = np.zeros(128)

        self.W2 = np.random.randn(128, 64) * 0.01
        self.b2 = np.zeros(64)

        self.W_OUT = np.random.randn(64, 24) * 0.01
        self.b_OUT = np.zeros(24)

        self.learning_rate: float = 0.01

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

    def forward_pass(self) -> np.ndarray:
        # Primeira camada oculta
        z1: np.ndarray = self.X @ self.W1 + self.b1
        a1 = np.maximum(0, z1) # ReLU

        # Segunda camada oculta
        z2: np.ndarray = a1 @ self.W2 + self.b2
        a2 = np.maximum(0, z2)

        # Camada de saída
        z3: np.ndarray = a2 @ self.W_OUT + self.b_OUT

        z: np.ndarray = self.softmax(z3)

        return z



        