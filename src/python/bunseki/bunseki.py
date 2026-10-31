from . import _pybunseki
from pathlib import Path

from functools import cached_property

EXTENSIONS = [".mp3", ".wav", ".flac"] # extensões que o meu sample reader feito com miniaudio suporta

class Bunseki:
    def __init__(self, song_path: str | Path) -> None:
        song_path = Path(song_path)
        if not song_path.is_file():
            raise FileNotFoundError(f"Invalid song path: {song_path}")

        if not song_path.suffix.lower() in EXTENSIONS:
            raise ValueError(f"Unsupported extension: {song_path.suffix}. Expected any of: {EXTENSIONS}")

        samples = _pybunseki.read_samples(str(song_path), to_mono = True)

        self._cbunseki = _pybunseki.CBunseki(samples)

    @cached_property
    def samples(self):
        return self._cbunseki.samples

    @cached_property
    def frame_count(self):
        return self._cbunseki.frame_count

    @property
    def analysis(self):
        return self._cbunseki.analysis

    @property
    def transform(self):
        return self._cbunseki.transform