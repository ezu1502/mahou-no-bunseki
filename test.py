import librosa
import numpy as np

samples, sr = librosa.load(
    "aitakute.mp3",
    sr=44100,
    mono=True
)


rms = np.sqrt(np.mean(samples ** 2))


print(f"Sample rate: {sr}")
print(f"Samples: {len(samples)}")
print(f"Peak: {np.max(np.abs(samples)):.6f}")
print(f"RMS: {np.sqrt(np.mean(samples ** 2)):.6f}")