# WebServer

## Panoramica Generale

WebServer è un server HTTP semplice scritto in C, basato su socket, capace di gestire più connessioni simultanee e di elaborare richieste e risposte HTTP di base, come la richiesta di file statici. Utilizza un watcher chiamato `fdwatch` per monitorare gli eventi di lettura e scrittura sui file descriptor. Inoltre, è progettato per essere compatibile con sistemi operativi Windows e POSIX (come Linux), grazie alle macro di compatibilità definite nel file `definitions.h`.

## Caratteristiche principali

- **Gestione Connessioni Simultanee**: Il server può gestire più connessioni contemporaneamente grazie all'uso di `fdwatch`.
- **Supporto per il Metodo GET**: È in grado di servire file statici dal filesystem e fornire risposte HTTP appropriate, come `200 OK`, `404 Not Found` o `400 Bad Request`.
- **Socket Non Bloccanti**: Utilizza socket non bloccanti.
- **Gestione Errori**: Fornisce messaggi di errore per operazioni socket fallite o richieste HTTP malformate.
- **Buffer Dinamici**: Impiega buffer dinamici per adattarsi alle dimensioni dei dati in arrivo e in uscita, ottimizzando così l'uso della memoria.
- **Compatibilità Multi-Piattaforma**: È compatibile con sistemi operativi Windows e POSIX.


## Struttura del Progetto

La struttura del progetto è organizzata come segue:

```
WebServer/
│
├── bin/             # Directory contenente l'eseguibile
├── build/           # File generati da CMake per la build
├── public/          # File statici (HTML, CSS, ecc.)
├── src/             # Codice sorgente
└── CMakeLists.txt   # Configurazione CMake
```

## Files principali

- **`main.c`**: Contiene il loop principale del server, gestisce l'ascolto delle connessioni e la chiusura graduale del server.

- **`socket.c`**: Si occupa della creazione e configurazione dei socket. Include la creazione del socket, l'impostazione delle opzioni come `SO_REUSEADDR` e `NONBLOCK`, il binding e l'ascolto delle nuove connessioni, oltre alla gestione delle connessioni in arrivo.

- **`fdwatch.c`**: Implementa un wrapper attorno alla funzione `select()`, utilizzata per monitorare eventi di lettura e scrittura sui socket. Questo approccio consente al server di gestire più connessioni simultanee in modo efficiente, senza la necessità di creare un thread separato per ciascuna connessione.

- **`connection.c`**: Gestisce le connessioni con i client. Le principali funzioni sono `process_new_connection()` per le nuove connessioni, `process_client_read()` per la lettura dei dati inviati dal client e `process_client_write()` per la scrittura dei dati.

- **`http.c`**: Implementa la logica per l'elaborazione delle richieste HTTP e inviare le appropriate risposte. Supporta il metodo `GET` per servire file statici e gestisce errori HTTP comuni come `400 Bad Request` e `404 Not Found`.

- **`network_io.c`**: Gestisce l'invio e la ricezione dei dati sui socket.

- **`buffer.c`**: Gestisce la riallocazione dinamica dei buffer per l'invio e la ricezione, adattandosi alle dimensioni variabili dei dati e ottimizzando l'uso della memoria.

- **`definitions.h`**: Definisce costanti e macro per garantire la portabilità e la consistenza del codice tra diverse piattaforme (Windows e Unix-like). Specifica anche limiti e dimensioni massime per il protocollo HTTP.



## Requisiti

- **CMake** 3.10 o successivo
- **Compilatore C** (MinGW, MSVC, ecc.)

## Installazione e Configurazione

Per installare e compilare **WebServer**, segui i seguenti passaggi:

1. **Clona il repository:**
   ```bash
   git clone https://github.com/DanieleAlessandro98/WebServer.git
   cd WebServer
   ```

2. **Compila il progetto:**
   ```bash
   cd build
   cmake .. && cmake --build .
   ```

3. **Esegui l'applicazione:**
   L'eseguibile si troverà nella cartella `bin`.

## Utilizzo

Una volta avviato il server, vi si può accedere tramite browser. Per impostazione predefinita, il server risponderà con i file HTML presenti nella cartella `public/`.

Esempio:
```
http://localhost:8080
```
