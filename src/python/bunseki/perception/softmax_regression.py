import numpy as np

SONG_COUNT: int = 100

W: np.ndarray = np.random.rand(36, 24) # 36 características, 24 tonalidades

X: np.ndarray = np.zeros((SONG_COUNT, 36)) # 100 músicas, 36 características

Y: np.ndarray = np.zeros((SONG_COUNT, 24)) 

b: np.ndarray = np.zeros(24) # um bias pra cada tonalidade

LEARNING_RATE = 1 / 100_000


def predict() -> np.ndarray: # vai retornar uma matriz de possibilidades pra cada tonalidade
    """
        Uses the global variables to calculate Z = XW + b. Applies softmax to return a probability for each key
        for each song, returns a matrix containing such probabilities.
    """

    Z = X @ W + b 

    Z -= np.max(Z, axis = 1, keepdims = True) # pra evitar valores infinitos na exponenciação

    exp_z = np.exp(Z)

    sum_z = np.sum(exp_z, axis = 1, keepdims = True)

    return exp_z / sum_z


def cross_entropy(Y, P) -> float: # Y: respostas certas P: resultado de predict()
    ln_values = np.log(P)

    yp = Y * ln_values

    i_sum = np.sum(yp, axis = 1, keepdims = True)

    return -np.sum(i_sum) / SONG_COUNT


def deriv(Y, P) -> tuple[np.ndarray, np.ndarray]:
    dz = (P - Y) / SONG_COUNT

    dw = np.transpose(X) @ dz

    db = np.sum(dz, axis = 0)

    return dw, db


def update_weights(derivatives: tuple[np.ndarray, np.ndarray]):
    global W
    global b

    dw, db = derivatives

    W -= dw * LEARNING_RATE
    b -= db * LEARNING_RATE

    
def epochs(amount: int = 1):
    for i in range(amount):
        P: np.ndarray = predict()

        if i % max(1, amount // 8) == 0:
            print(f"Epoch {i : 03d} Loss: {cross_entropy(Y, P)}")

    
        derivatives = deriv(Y, P)

        update_weights(derivatives = derivatives)


        if i == amount - 1:
            print(f"FINAL Epoch {i : 03d} Loss: {cross_entropy(Y, predict())}")


epochs(1000)
