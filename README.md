# Praxisarbeit Labyrinth C

Ein C-Konsolenspiel mit optionaler grafischer Ansicht in raylib 5.5 für die Praxisarbeit Programmiertechnik.

## Spiel

- `P` = Spieler
- `T` = Schatz
- `O` = Hindernis
- Steuerung mit `W`, `A`, `S`, `D`
- `Q` beendet das Spiel

Beim Start kann zwischen Konsolenmodus und grafischer Ansicht gewählt werden. Beide verwenden dieselben Bewegungs- und Siegfunktionen.

Die Konsole verwendet ein 10×10-Feld. Der Grafikmodus zeigt ein Ganglabyrinth mit Steinmauern, animierter Figur, Fackeln, Schatztruhe und Zugzähler. Die erste Karte orientiert sich an der Bildvorlage. Nach dem Sieg startet `R` ein neues zufälliges Ganglabyrinth. `Esc` beendet ebenfalls den Grafikmodus.

## Kompilieren

Konsolenmodus:

```bash
gcc -DCONSOLE_ONLY main.c -o labyrinth -Wall -Wextra -Wpedantic
```

Mit raylib unter Windows:

```bash
gcc main.c graphic_view.c -o labyrinth.exe -IC:/msys64/ucrt64/include -LC:/msys64/ucrt64/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lm -Wall -Wextra -Wpedantic
```

Zum Starten muss `C:/msys64/ucrt64/bin` im `PATH` liegen. Der Ordner `assets` muss im Arbeitsverzeichnis, neben der EXE oder in deren übergeordnetem Ordner liegen.

Die Dungeon-Grafik wurde mit KI erstellt. Die Schriften [Pirata One](https://github.com/google/fonts/tree/main/ofl/pirataone) und [VT323](https://github.com/google/fonts/tree/main/ofl/vt323) stehen unter der SIL Open Font License; die Lizenztexte liegen in `assets`.
