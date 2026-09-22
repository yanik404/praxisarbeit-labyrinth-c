# Praxisarbeit Labyrinth C

Ein C-Konsolenspiel mit optionaler grafischer raylib-Ansicht für die Praxisarbeit Programmiertechnik.

## Spiel

- `P` = Spieler
- `T` = Schatz
- `O` = Hindernis
- Steuerung mit `W`, `A`, `S`, `D`
- `Q` beendet das Spiel

Beim Start kann zwischen Konsolenmodus und grafischer Ansicht gewählt werden.

## Kompilieren

Konsolenmodus:

```bash
gcc main.c -o labyrinth -Wall -Wextra -Wpedantic
```

Mit raylib unter Windows:

```bash
gcc main.c graphic_view.c -o labyrinth.exe -IC:\\msys64\\ucrt64\\include -LC:\\msys64\\ucrt64\\lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -Wpedantic
```
