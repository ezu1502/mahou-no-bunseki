# Bunseki

A cross-language audio analysis library built with C++ and Python.

## Overview

Bunseki decodes audio files and provides specialized tools for extracting and analyzing different audio features.

Its main capabilities include:

- Audio analysis
- Audio transformation
- Musical key detection through the Perception module
- Audio metadata extraction

## Installation

**Bunseki v0.1.0** can be installed directly through your terminal using pip:

```powershell
pip install pybunseki
```

## How to Use

Once installed, Bunseki's features can be accessed as follows:

```python
from bunseki import Bunseki

song = Bunseki("song.mp3")
```

`Bunseki` is the main class of the package. It provides access to the library's audio analysis capabilities through a unified interface.

## Functionalities

1. Audio Analysis (`Bunseki.analysis`)
2. Audio Transformation (`Bunseki.transform`)
3. Machine-learning-based algorithms (`Bunseki.perception`)
4. Audio Metadata (`Bunseki.metadata`)

## Dependencies

Bunseki is built using several external libraries and technologies, including:

- C++23
- Python
- pybind11
- miniaudio
- TagLib

Python 3.13 or later is required.

## Roadmap

- [x] Audio decoding
- [x] Analysis and Transform modules
- [x] Initial Perception models
- [x] PyPI distribution
- [x] Metadata extraction
- [x] Initial test suite
- [ ] Enhanced feature extraction algorithms
- [ ] More detailed documentation
- [ ] Improved Perception models
- [ ] Bunseki Web