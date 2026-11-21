from enum import IntEnum

class Keys(IntEnum):
    C_MINOR = 0
    C_SHARP_MINOR = 1
    D_MINOR = 2
    D_SHARP_MINOR = 3
    E_MINOR = 4
    F_MINOR = 5
    F_SHARP_MINOR = 6
    G_MINOR = 7
    G_SHARP_MINOR = 8
    A_MINOR = 9
    A_SHARP_MINOR = 10
    B_MINOR = 11

    C_MAJOR = 12
    C_SHARP_MAJOR = 13
    D_MAJOR = 14
    D_SHARP_MAJOR = 15
    E_MAJOR = 16
    F_MAJOR = 17
    F_SHARP_MAJOR = 18
    G_MAJOR = 19
    G_SHARP_MAJOR = 20
    A_MAJOR = 21
    A_SHARP_MAJOR = 22
    B_MAJOR = 23

    def __str__(self):
        return (
            self.name
            .lower()
            .capitalize()
            .replace("_", "")
            .replace("sharp", "#")
            .replace("major", "")
            .replace("minor", "m")
        )