from pathlib import Path
import mirdata
# nesse estamos usando o Python 3.14

DATA_HOME = Path(__file__).parent.parent / "datasets" / "fma_keys"

dataset = mirdata.initialize("fma_keys", data_home = str(DATA_HOME))

# TODO continuar daqui!!!