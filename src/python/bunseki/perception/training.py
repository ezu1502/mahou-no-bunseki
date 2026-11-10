from softmax_regression import SoftmaxRegression
from feature_loading import get_dataset_matrices

ids, X, Y = get_dataset_matrices()

regression = SoftmaxRegression(X, Y) # essa vai conhecer os pesos de vdd 

print("Loss at start:", regression.start_loss)

for i in range(10001):

    to_print = i % 200 == 0
    regression.epochs(1, log_loss = to_print)

