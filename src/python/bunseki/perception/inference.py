from pathlib import Path
from enum import Enum

import numpy as np
from .keys import Keys

MODELS_DATASET = Path(__file__).parent / "models"

SOFTMAX_NEWEST = MODELS_DATASET / "softmax_regression_v2.npz"
PERCEPTRON_NEWEST = MODELS_DATASET / "perceptron_v3.npz"

class Methods(Enum):
    SOFTMAX = "softmax"
    PERCEPTRON = "perceptron"

class PerceptionInference:
    def __init__(self, chroma_features: np.ndarray) -> None:
        self.chroma_features = chroma_features

        if self.chroma_features.shape != (36,):
            raise ValueError("Expected 36 chroma features")

        self.softmax, self.perceptron = self.load_models()

    def load_models(self) -> tuple[dict[str, np.ndarray], dict[str, np.ndarray]]:
        with np.load(SOFTMAX_NEWEST) as data:
            softmax = {key: data[key] for key in data.files}

        with np.load(PERCEPTRON_NEWEST) as data:
            perceptron = {key: data[key] for key in data.files}

        return softmax, perceptron

    def predict_key(self, method: Methods) -> Keys:
        if not isinstance(method, Methods):
            raise ValueError(f"Invalid method chosen: {method}")
        
        if method == Methods.SOFTMAX:
            # Carregamento dos pesos
            W, b = self.softmax["W"], self.softmax["b"] 

            # Forward Pass
            Z = self.chroma_features @ W + b
            Z -= np.max(Z) # pra evitar valores infinitos na exponenciação

            # Softmax
            exp_z = np.exp(Z)
            sum_z = np.sum(exp_z)
            chances = exp_z / sum_z

        elif method == Methods.PERCEPTRON:
            # Carregamento dos pesos
            W1, b1 = self.perceptron["W1"], self.perceptron["b1"]
            W2, b2 = self.perceptron["W2"], self.perceptron["b2"]
            W_OUT, b_OUT = self.perceptron["W_OUT"], self.perceptron["b_OUT"]

            # Primeira camada oculta
            z1: np.ndarray = self.chroma_features @ W1 + b1
            a1 = np.maximum(0, z1) # ReLU
    
            # Segunda camada oculta
            z2: np.ndarray = a1 @ W2 + b2
            a2 = np.maximum(0, z2)
    
            # Camada de saída
            z3: np.ndarray = a2 @ W_OUT + b_OUT
            # Softmax
            exp_z3 = np.exp(z3 - np.max(z3))
            chances = exp_z3 / np.sum(exp_z3)

            
        else:
            raise ValueError(f"Invalid method chosen: {method}")

        # Retornando um tom
        favorite = int(np.argmax(chances))
        return Keys(favorite)
            