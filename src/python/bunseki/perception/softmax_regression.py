import numpy as np


W: np.ndarray = np.random.rand(36, 24) # pesos: 36 características, 24 tonalidades

X: np.ndarray = np.zeros((100, 36)) # valores: 1 música por linha, 36 características

b: np.ndarray = np.zeros(24) # 24 viéses, um pra cada tonalidade

Z = X @ W + b # cada característica com seu peso, cada bias com a sua tonalidade

def softmax(logits: np.ndarray):
    exp_values = np.exp(logits) 

    s = np.sum(exp_values, axis = 1, keepdims = True)

    return exp_values / s

probabilities = softmax(Z)

# TODO continuar aqui! 



