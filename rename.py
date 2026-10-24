from pathlib import Path

folder = Path(__file__).parent / "test_tracks"

for path in folder.iterdir():
    path.rename(path.with_name(path.name.lower().replace(" ", "_").replace("\ufeff", "")))

    # print(path.stem)

print("Done!")