from bunseki import Bunseki
from pathlib import Path

p = Path(r"C:\Users\dudul\Mahou no Ongaku")
print("Import successful!")

song = Bunseki(p / "0.mp3")

print(song.analysis.rms())
print(song.metadata.get("artist"))

print(song.perception.key)