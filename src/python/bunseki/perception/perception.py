from __future__ import annotations
from typing import Callable
from functools import cached_property

from .._pybunseki import ChromaFeatures

class Perception:
    def __init__(self, get_chroma_features: Callable[[], ChromaFeatures]) -> None:
        if not callable(get_chroma_features):
            raise TypeError("get_chroma_features must be callable")

        self._get_chroma_features: Callable[[], ChromaFeatures] = get_chroma_features


    @cached_property
    def chroma_features(self) -> ChromaFeatures:
        feats: ChromaFeatures = self._get_chroma_features()

        if not isinstance(feats, ChromaFeatures):
            raise TypeError("Expected ChromaFeatures")

        return feats

    


# global chroma, mean chroma, chroma variance, chroma windows
#    array[12],   array[12],     array[12],   vetor<array[12]>