from __future__ import annotations
from typing import Callable, Self
from functools import cached_property

from .._pybunseki import ChromaFeatures

import numpy as np

from .inference import PerceptionInference, Methods
from .keys import Keys

CHROMA_FEATURE_COUNT = 12

class Perception:
    def __init__(self, get_chroma_features: Callable[[], ChromaFeatures]) -> None:
        if not callable(get_chroma_features):
            raise TypeError("get_chroma_features must be callable")

        self._get_chroma_features: Callable[[], ChromaFeatures] = get_chroma_features
        
        self._inference: PerceptionInference | None = None
        self.method: Methods = Methods.PERCEPTRON

    def set_method(self, method: Methods) -> Self:
        if not isinstance(method, Methods):
            raise TypeError(f"Invalid method: {method}")
        
        self.method = method
        self.__dict__.pop("key", None) # Remove do cache da cached_property

        return self
    
    @cached_property
    def chroma_features(self) -> ChromaFeatures:
        feats: ChromaFeatures = self._get_chroma_features()

        if not isinstance(feats, ChromaFeatures):
            raise TypeError("Expected ChromaFeatures")

        return feats

    @cached_property
    def numpy_chroma_features(self) -> np.ndarray:
        print("Getting chroma features...")
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

    @cached_property
    def key(self) -> Keys:
        if self._inference is None:
            self._inference = PerceptionInference(self.numpy_chroma_features)

        return self._inference.predict_key(method = self.method)
    
# global chroma, mean chroma, chroma variance, chroma windows
#    array[12],   array[12],     array[12],   vetor<array[12]>