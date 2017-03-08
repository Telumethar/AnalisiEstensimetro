#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

vector <string> parseRow(string riga, string delimiter = ",");
//Data una riga di un file csv, la divide in sottostringhe basandosi su una sequenza di caratteri di limite (nel caso dei csv, la virgola)
//Restituisce un vettore contenente le sottostringhe così trovate
//es "ciao,a,tutti" diventa ["ciao", "a", "tutti"]

struct SerieMisurazioni {
  int materiale, id_estensimetro;
  double L0, err_L0, d, err_d;
  
  vector <double> forza_applicata, allungamento, compressione;

  bool getParametri(string nomefile) {
    //Legge i parametri dal file "nomefile.csv" e li imposta

    return true;
  }

  bool AnalisiStatistica(void) {
    //Esegue i calcoli statistici (medie, E, errori su E...)

    return true;
  }
};

int main (void) {
  //Apriamo il file con le misure
  ifstream filemisure("...");

  if (!filemisure) {
    //
  }

  //Ciclo che scorre righe, ogni riga viene mandata alla funzione parseRow. Ciclo interno che scorre le celle di ogni riga,
  //
  string riga;
  vector <SerieMisurazioni> misurazioni;

  string saved_id_estensimetro = "?";
  while (filemisure << riga) { //Per ogni riga
    vector <string> celle_riga = parseRow(riga);
    string actual_id_estensimetro = celle_riga[0];
    if (saved_id_estensimetro != actual_id_estensimetro) {
      //Cambia la serie
      //Crea la nuova serie e la inizializza
      misurazioni[i].forza_applicata.push_back(stod(celle_riga[1]));
      misurazioni[i].allungamento.push_back(stod(celle_riga[2]));
      misurazioni[i].compressione.push_back(stod(celle_riga[3]));

      saved_id_estensimetro = actual_id_estensimetro;
    } else {
      //Sono nella stesse serie di prima
      //Aggiungo i nuovi dati
    }
  }


  return 0;
}

vector <string> parseRow(string riga, string delimiter) {
  size_t pos = 0;
  string token;
  vector <string> parsedString;

  int cont = 0;
  while (pos = riga.find(delimiter)) { //Cerca la prossima occorrenza del delimiter all'interno della stringa
  //Se il delimiter non viene trovato, str.find() restituisce la posizione finale della stringa (string::npos)
  //e quindi il ciclo while si interrompe
    if (pos == string::npos)
      break;
    token = riga.substr(0, pos); //Se invece find trova delimiter, restituisce la sua posizione. Estraggo tutto ciò che c'era prima
    parsedString.push_back(token); //lo inserisco nel vettore
    riga.erase(0, pos + delimiter.length()); //Lo rimuovo dalla stringa originale. Così, alla prossima iterazione
    //del ciclo, find troverà il delimiter successivo, e quindi estrarrò la porzione successiva
  }
  //L'ultimo elemento lo devo aggiungere a mano
  parsedString.push_back(riga);

  return parsedString;
}
