COMPILAZIONE SU LINUX (CONSIGLIATO)

Installare Qt5 e Python 3.8 dal proprio gestore di pacchetti
Creare una cartella per il build dentro la cartella del progetto
Dentro la cartella build, usare i comandi 'qmake ..' e 'make' per generare l'eseguibile
In alternativa, importare il .pro da Qt Creator e compilare




COMPILAZIONE SU WINDOWS

Installare Python 3.8 e creare una variabile d'ambiente PYTHONPATH che punta alla cartella di Python (contenente include, bin, ecc.)
Installare Qt 5.15 da https://www.qt.io/download-qt-installer, selezionando MINGW 64 tra i componenti
Cercare dentro la cartella Qt la cartella mingwxx_64\bin, e aggiungere il percorso assoluto di tale cartella alla variabile d'ambiente Path
Aprire la shell Qt MINGW appena installata ed eseguire i seguenti comandi:

qmake
mingw32-make install

Copiare poi la cartella plugins dentro la cartella con l'eseguibile (release)
In caso di problemi, creare la variabile di ambiente QT_QPA_PLATFORM_PLUGIN_PATH settandola al percorso assoluto di mingwxx_64\plugins\platform e aggiungere %PYTHONPATH% a Path
