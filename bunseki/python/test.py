from bunseki import Analysis, Transform, read_samples

result = read_samples("aitakute.mp3")

if result is not None:
    samples, frame_count = result
    

    a = Analysis(samples, frame_count)
    b = Transform(samples, frame_count)

    print(a.rms())
    print(b.fft())