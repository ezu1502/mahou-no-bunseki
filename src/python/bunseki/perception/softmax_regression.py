import numpy as np
from pathlib import Path

class SoftmaxRegression:
    def __init__(self, x_values: np.ndarray, y_values: np.ndarray):
        if x_values.ndim != 2 or y_values.ndim != 2:
            raise ValueError("X and Y must be 2-dimensional arrays!")

        if x_values.size == 0 or y_values.size == 0:
            raise ValueError("X and Y must not be empty!")

        if x_values.shape[1] != 36 or y_values.shape[1] != 24:
            raise ValueError(f"Invalid x or y values shape: {x_values.shape} - {y_values.shape}")

        if x_values.shape[0] != y_values.shape[0]:
            raise ValueError("X and Y must contain the same amount of songs!")

        

        self.X: np.ndarray = x_values
        self.Y: np.ndarray = y_values

        self.W: np.ndarray = np.random.randn(36, 24) * 0.01
        
        self.b: np.ndarray = np.zeros(24)
        self.LEARNING_RATE = 5 / 100

        self.SONG_COUNT = self.X.shape[0] # 5488 idealmente

        self.start_loss: float = self.cross_entropy(self.predict())

        self.epochs_elapsed: int = 0
        
    @property
    def parameters(self):
        return (self.W, self.b)

    @property
    def weights(self) -> np.ndarray:
        return self.W

    @property
    def biases(self) -> np.ndarray:
        return self.b

    
    def predict(self) -> np.ndarray: # vai retornar uma matriz de possibilidades pra cada tonalidade
        """
            Uses the global variables to calculate Z = XW + b. Applies softmax to return a probability for each key
            for each song, returns a matrix containing such probabilities.
        """

        Z = self.X @ self.W + self.b 

        Z -= np.max(Z, axis = 1, keepdims = True) # pra evitar valores infinitos na exponenciação

        exp_z = np.exp(Z)

        sum_z = np.sum(exp_z, axis = 1, keepdims = True)

        return exp_z / sum_z


    def cross_entropy(self, P) -> float: # Y: respostas certas P: resultado de predict()
        ln_values = np.log(np.clip(P, 1e-15, 1.0))

        yp = self.Y * ln_values

        i_sum = np.sum(yp, axis = 1, keepdims = True)

        return -np.sum(i_sum) / self.SONG_COUNT


    def deriv(self, P) -> tuple[np.ndarray, np.ndarray]:
        dz = (P - self.Y) / self.SONG_COUNT

        dw = np.transpose(self.X) @ dz

        db = np.sum(dz, axis = 0)

        return dw, db


    def update_weights(self, derivatives: tuple[np.ndarray, np.ndarray]):
        dw, db = derivatives

        self.W -= dw * self.LEARNING_RATE
        self.b -= db * self.LEARNING_RATE

    
    def epochs(self, amount: int = 1, log_loss: bool = False):
        if amount <= 0:
            return

        for i in range(amount):
            P: np.ndarray = self.predict()

            if(log_loss):
                print(f"Epoch {self.epochs_elapsed : 03d} Loss: {self.cross_entropy(P)}")

        
            derivatives = self.deriv(P)

            self.update_weights(derivatives = derivatives)

            self.epochs_elapsed += 1


    def save_weights(self, destination: Path):
        np.savez_compressed(
            destination,
            W = self.W,
            b = self.b
        )

        print(f"Weights saved to {destination}!")

    