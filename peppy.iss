#include "include\version.h"

[Setup]
AppName=Peppy Protein Puzzle
AppVerName=Peppy Protein Puzzle {#APPLICATION_VERSION}
AppPublisher=Helixsoft.nl
AppPublisherURL=http://www.helixsoft.nl
DefaultDirName={pf}\Helixsoft.nl\PeppyProteinPuzzle
DefaultGroupName=PeppyProteinPuzzle
UninstallDisplayIcon={app}\peppy.exe
Compression=lzma
SolidCompression=yes
#ifdef DEBUG
OutputBaseFilename=Install_PeppyProteinPuzzle-Debug-{#APPLICATION_VERSION}
#else
OutputBaseFilename=Install_PeppyProteinPuzzle-{#APPLICATION_VERSION}
#endif
OutputDir=dist

[Files]

#ifdef DEBUG
Source: "build\debug_win\peppy.exe"; DestDir: "{app}"
Source: "build\debug_win\*.dll"; DestDir: "{app}"; Flags: ignoreversion;
#else
Source: "build\release_win\peppy.exe"; DestDir: "{app}"
Source: "build\release_win\*.dll"; DestDir: "{app}"; Flags: ignoreversion;
#endif

Source: "README.txt"; DestDir: "{app}"; Flags: isreadme
Source: "LICENSE.txt"; DestDir: "{app}";
Source: "data\*"; DestDir: "{app}\data";

[Icons]
Name: "{group}\Peppy Protein Puzzle"; Filename: "{app}\peppy.exe";
Name: "{commondesktop}\Peppy Protein Puzzle"; Filename: "{app}\peppy.exe";

[Run]
Filename: "{app}\peppy.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
