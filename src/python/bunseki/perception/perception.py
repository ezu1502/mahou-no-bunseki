from __future__ import annotations
from typing import Callable
from functools import cached_property

from .._pybunseki import ChromaFeatures

lazy import numpy as np

CHROMA_FEATURE_COUNT = 12

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

    @cached_property
    def numpy_chroma_features(self) -> np.ndarray:
        chroma_feats = self.chroma_features

        if any(
            len(x) != CHROMA_FEATURE_COUNT
            for x in [chroma_feats.global_chroma, chroma_feats.mean_chroma, chroma_feats.chroma_variance]
        ):
            raise RuntimeError("Invalid Chroma features format")

        result: np.ndarray = np.zeros(CHROMA_FEATURE_COUNT * 3)

        result[0:CHROMA_FEATURE_COUNT] = chroma_feats.global_chroma
        result[CHROMA_FEATURE_COUNT:CHROMA_FEATURE_COUNT * 2] = chroma_feats.mean_chroma
        result[CHROMA_FEATURE_COUNT * 2:CHROMA_FEATURE_COUNT * 3] = chroma_feats.chroma_variance

        return result
        
# global chroma, mean chroma, chroma variance, chroma windows
#    array[12],   array[12],     array[12],   vetor<array[12]>