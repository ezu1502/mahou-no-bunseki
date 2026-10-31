import sys; sys.path.append("src/python")

from bunseki import Bunseki


song = Bunseki("test_tracks/aitakute.mp3")

print(song.analysis.rms())

print(song.analysis.peak_amplitude())

