Gitar DSP Effektprosessor
Dette er et åpen kildekode-prosjekt for å bygge en frittstående gitar-effektprosessor for Windows ved hjelp av C++ og JUCE-rammeverket. Målet er å skape en applikasjon med lav latency (forsinkelse) som tilbyr høykvalitets gitareffekter.

Denne første versjonen implementerer kun en grunnleggende audio pass-through. Det vil si at den tar imot lyd fra en valgt inngang (input) og sender den direkte til en valgt utgang (output) uten noen form for prosessering. Dette bekrefter at kjernefunksjonaliteten for lydhåndtering er på plass.

Status
Fase 1: Grunnleggende Prosjekt-oppsett - Fullført

[X] Lyd kan sendes fra input til output via ASIO.

[X] Grunnleggende prosjektstruktur er på plass.

Nødvendig programvare for å bygge
JUCE: Last ned og sett opp JUCE-rammeverket fra den offisielle nettsiden.

Microsoft Visual Studio: Visual Studio 2019 eller nyere anbefales. Sørg for at "Desktop development with C++" er installert.

ASIO-drivere: For best ytelse trenger du et lydkort med ASIO-drivere. Hvis lydkortet ditt ikke har dette, kan du installere ASIO4ALL.

Git: For versjonskontroll.

Hvordan kompilere og kjøre
Klone repoet:

git clone [URL-til-ditt-repo]
cd [repo-navn]

Åpne i Projucer:

Start Projucer-applikasjonen som følger med JUCE.

Velg File -> Open... og finn .jucer-filen i prosjektmappen.

I Projucer, velg riktig "Exporter" (f.eks. "Visual Studio 2022").

Trykk på "Save and Open in IDE..."-knappen. Dette vil generere Visual Studio-prosjektfilene og åpne prosjektet.

Bygg i Visual Studio:

Sett build-konfigurasjonen til "Debug" eller "Release".

Bygg prosjektet (Trykk F5 for å bygge og starte med debugging).

Konfigurer lyd i applikasjonen:

Når applikasjonen starter, vil den sannsynligvis ikke finne noen lydenhet. Et innstillingsvindu vil dukke opp.

Velg ASIO som "Audio Device Type".

Velg ditt lydkort som "Device".

Sett bufferstørrelsen (sample rate og buffer size) til en lav verdi for minst mulig latency (f.eks. 44100 Hz, 128 Samples).

Nå skal du høre lyden fra din valgte input (f.eks. gitaren din) i dine høyttalere eller hodetelefoner.
