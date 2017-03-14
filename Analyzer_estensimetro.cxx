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

  bool getParametri() {
    //Legge i parametri dal file "nomefile.csv" e li imposta
    //Apriamo il file con i parametri
    ifstream fileparametri("Data/Parametri2.csv");
    if(!fileparametri) {
      cout << "Errore di lettura parametri" << endl;
      return false;
    }
    //Ciclo while che
     string riga;

    while(fileparametri >> riga) {
      //Dividere riga in celle
      vector<string>celle = parseRow(riga);    //Recuperare ID dalla prima cella
      int id_cella = stoi(celle[0]);
      if (id_cella == this->id_estensimetro) {
        //Recupera i parametri
        this->materiale = stoi(celle[1]);
        this->L0 = stod(celle[2]);
        this->err_L0 = stod(celle[3]);
        this->d = stod(celle[4]);
        this->err_d = stod(celle[5]);
      }
    }
    return true;
  }

  bool AnalisiStatistica(void) {
    //Esegue i calcoli statistici (medie, E, errori su E...)
    //inizio interpolazione
    double sum_x = 0, sum_x2 = 0;
     for(auto &x : this->forza_applicata){
       sum_x += x;
       sum_x2+=x*x;
     }
    double sum_y_all = 0, sum_y_acc=0;
     for(auto &y_all: this->allungamento){
       sum_y_all+= y_all;
     }
     for(auto &y_acc: this->compressione){
       sum_y_acc+= y_acc;
     }
    double sum_xy_all = 0, sum_xy_acc = 0;
     for (int i=0;i < this->forza_applicata.size();i++){
       sum_xy_all += this->forza_applicata[i]*this->allungamento[i];
       sum_xy_acc += this->forza_applicata[i]*this->compressione[i];
     }
    int N = this->forza_applicata.size();
    double delta = N*sum_x2 - pow(sum_x,2);
    double a = 0, a_err = 0, b = 0, b_err = 0;

      a = (1/delta)*((sum_x2 *sum_y_all) - (sum_x*sum_xy_all));
      b = (1/delta)*(N*sum_xy_all - sum_x*sum_y_all);

    double c = 0, c_err = 0, d = 0, d_err = 0;

      c = (1/delta)*((sum_x2 *sum_y_acc) - (sum_x*sum_xy_acc));
      d = (1/delta)*(N*sum_xy_acc - sum_x*sum_y_acc);

    double sum_sigma_all = 0, sum_sigma_acc = 0;
      for(int i=0; i < this->forza_applicata.size(); i++){
        sum_sigma_all += pow((a+b*this->forza_applicata[i])-this->allungamento[i],2);
        sum_sigma_acc += pow((c+d*this->forza_applicata[i])-this->compressione[i],2);
    }
    double sigma_all = sqrt(sum_sigma_all/(N-2));
    double sigma_acc = sqrt(sum_sigma_acc/(N-2));

    a_err = sigma_all*sqrt(sum_x2/delta);
    b_err = sigma_all*sqrt(N/delta);

    c_err = sigma_acc*sqrt(sum_x2/delta);
    d_err = sigma_acc*sqrt(N/delta);

      cout << "a: " << a << "\t" << a_err << endl;
      cout << "b: " << b << "\t" << b_err << endl;
      cout << "c: " << c << "\t" << c_err << endl;
      cout << "d: " << d << "\t" << d_err << endl;

    //fine interpolazione


    //calcolo modulo di Young
    double E_all = (this->L0*4)/(M_PI*pow(this->d,2)*b);
    double E_acc = (this->L0*4)/(M_PI*pow(this->d,2)*d);

    double serr_E_all = (16*pow(this->err_L0,2)/(pow(M_PI,2)*pow(this->d,4)*pow(b,2)));
    serr_E_all+= (64*pow(this->L0*this->d,2))/(pow(M_PI*b,2)*pow(this->d,6));
    serr_E_all+=(16*pow(this->L0,2)*pow(b_err,2))/(pow(M_PI,2)*pow(this->d*b,4));

    double err_E_all = sqrt(serr_E_all);

    cout << "Errore all: "<< err_E_all << endl;



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
    serie.getParametri();
    serie.AnalisiStatistica();
    cout << "Materiale: " << serie.materiale << " x0: " << serie.L0 << " +- " << serie.err_L0 << "\n"
         << "d: " << serie.d << " +- " << serie.err_d << "\n";
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
