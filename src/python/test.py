from pathlib import Path

import os


from bunseki import Bunseki
from bunseki.perception.inference import Methods

print("Running!")
SONGS = Path(r"C:\Users\dudul\Mahou no Ongaku")

for song in SONGS.iterdir():
    z = Bunseki(song)

    print(z.metadata.get_tags())
