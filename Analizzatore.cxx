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
  ifstream filemisure("Data/DatiEstensimetro.csv");

  if (!filemisure) { //Controllo eventuali errori nella lettura del file delle misure
    cout << "Errore Input\n";
    return 1;
  }



  //Ciclo che scorre righe, ogni riga viene mandata alla funzione parseRow, che la divide in celle.
  //Basandomi sugli ID, che leggo nella prima cella di ogni riga, creo delle SerieMisurazioni per ogni ID unico
  //Quindi leggo e inserisco i dati all'interno di ogni struct
  //Alla fine avrò un vettore di SerieMisurazioni, con ciascun elemento che costituiscce una singola serie di dati prelevati dallo stesso estensimetro
  string riga;
  vector <SerieMisurazioni> misurazioni;
  int nserie = 0-1;

  string saved_id_estensimetro = "?";

  while (filemisure >> riga) { //Per ogni riga
    vector <string> celle_riga = parseRow(riga); //Divido la riga in celle
    //cout << celle_riga[0] << endl;
    string actual_id_estensimetro = celle_riga[0]; //Prelevo dalla prima cella l'attuale ID dell'estensimetro
    if (saved_id_estensimetro != actual_id_estensimetro) { //Se è cambiata la serie di misurazioni (ovvero se è cambiato l'estensimetro che sto usando)
      //Crea la nuova serie e la Inizializzo
      misurazioni.push_back(SerieMisurazioni()); //SerieMisurazioni() crea una nuova variabile del tipo di quella struttura, che inserisco in fondo al vettore
      ++nserie; //Ora c'è una serie in più!
      misurazioni[nserie].id_estensimetro = stoi(celle_riga[0]); //Imposto già l'ID dell'estensimetro

      //misurazioni[nserie].getParametri("Data/ParametriEstensimetri.csv"); //Invoco la funzione che imposta i parametri, ora che ho già sistemato l'ID

      //E poi inserisco dentro i primi dati
      //cout << celle_riga[1] << " " << celle_riga[2] << " " << celle_riga[3] << endl;
      misurazioni[nserie].forza_applicata.push_back(stod(celle_riga[1]));
      misurazioni[nserie].allungamento.push_back(stod(celle_riga[2]));
      misurazioni[nserie].compressione.push_back(stod(celle_riga[3]));

      saved_id_estensimetro = actual_id_estensimetro;
    } else {
      //Sono nella stesse serie di prima
      //Aggiungo i nuovi dati
      misurazioni[nserie].forza_applicata.push_back(stod(celle_riga[1]));
      misurazioni[nserie].allungamento.push_back(stod(celle_riga[2]));
      misurazioni[nserie].compressione.push_back(stod(celle_riga[3]));
    }
  }

  //DEBUG Stampa tutto
  for (auto &serie: misurazioni) {
    cout << serie.id_estensimetro << "-----\n";
    cout << "Forza\tAllung\tCompres\n";
    for (int i = 0; i < serie.forza_applicata.size(); ++i) {
      cout << serie.forza_applicata[i] << "\t" << serie.allungamento[i] << "\t" << serie.compressione[i] << "\n";
    }
    cout << "\n\n\n";
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
