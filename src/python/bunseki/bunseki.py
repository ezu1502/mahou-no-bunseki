from . import _pybunseki
from ._pybunseki import read_samples # import pra facilitar o __init__.py sem acessar diretamente a interface privada

from pathlib import Path
from functools import cached_property

from .perception.perception import Perception

EXTENSIONS = [".mp3", ".wav", ".flac"] # extensões que o meu sample reader feito com miniaudio suporta

class Bunseki:
    def __init__(self, song_path: str | Path) -> None:
        song_path = Path(song_path)
        if not song_path.is_file():
            raise FileNotFoundError(f"Invalid song path: {song_path}")

        if not song_path.suffix.lower() in EXTENSIONS:
            raise ValueError(f"Unsupported extension: {song_path.suffix}. Expected any of: {EXTENSIONS}")

        self.song_path = song_path

        samples = _pybunseki.read_samples(str(song_path), to_mono = True)

        self._cbunseki = _pybunseki.CBunseki(samples)
        self._perception = Perception(get_chroma_features = self._cbunseki.transform.chroma_features)
        self._metadata = _pybunseki.Metadata(song_path = str(self.song_path)).load_metadata()


    @cached_property
    def samples(self) -> list[float]:
        return self._cbunseki.samples

    @cached_property
    def frame_count(self) -> int:
        return self._cbunseki.frame_count

    @property
    def analysis(self) -> _pybunseki.Analysis:
        return self._cbunseki.analysis

    @property
    def transform(self) -> _pybunseki.Transform:
        return self._cbunseki.transform

    @property
    def perception(self) -> Perception:
        return self._perception

    @property
    def metadata(self) -> _pybunseki.Metadata:
        return self._metadata

    def __repr__(self) -> str:
        return f"Bunseki(song_path={str(self.song_path)!r})"