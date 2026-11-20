from pathlib import Path

class Metadata:
    def __init__(self, song_path: Path) -> None:
        if not song_path.is_file():
            raise ValueError("Invalid song path!")
        
        self.song_path = song_path
