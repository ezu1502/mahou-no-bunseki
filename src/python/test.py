from pathlib import Path
from bunseki import Bunseki

print("Running!")
SONGS = Path(r"C:\Users\dudul\Mahou no Ongaku")

zero = Bunseki(SONGS / "Show.mp3")

print(zero.perception.key)
