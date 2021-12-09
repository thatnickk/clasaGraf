#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>

using namespace std;

const int inf = INT_MAX;

ifstream fin("darb.in");
ofstream fout("darb.out");

class Graf{

    private:
        bool orientat;
        bool ponderat;
        int nrNoduri;
        int nrMuchii;
        vector<vector<int>> listaAdiacenta; //neponderat
        vector<pair<int, pair<int, int>>> listaMuchii; //ponderat (forma: <cost, <nod1, nod2>>)
        vector<int> topologie;

        void bfs(vector<int>& distante, vector<bool>& vizitat, int nodStart){
            queue<int> coada;
            coada.push(nodStart);
            distante[nodStart] = 0;

            while (coada.empty() == false){
                int nodCurent = coada.front();
                vizitat[nodCurent] = true;
                for (auto i : listaAdiacenta[nodCurent]){
                    if (vizitat[i] == false){
                        coada.push(i);
                        distante[i] = distante[nodCurent] + 1;
                        vizitat[i] = true;
                    }
                }
                coada.pop();
            }
        }        

        void dfs(vector<bool>& vizitat, int nodCurent){
            vizitat[nodCurent] = true;
            for (auto i : listaAdiacenta[nodCurent])
                if (vizitat[i] == false)
                    dfs(vizitat, i);
        }

        void dfsbiconex(int nodCurent, int nodAnterior, int adancimeCurenta, vector<vector<int>>& componente, stack<int>& stiva, vector<bool>& vizitat, vector<int>& adancime, vector<int>& adancimeMin){
            adancime[nodCurent] = adancimeCurenta;
            vizitat[nodCurent] = true;
            adancimeMin[nodCurent] = adancimeCurenta;
            stiva.push(nodCurent);

            for (auto i : listaAdiacenta[nodCurent]){
                if (i != nodAnterior){
                    if (vizitat[i] == true){
                        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], adancime[i]); //gasim muchie de intoarcere
                    }
                    else{
                        dfsbiconex(i, nodCurent, adancimeCurenta + 1, componente, stiva, vizitat, adancime, adancimeMin); //rulam la adancime +1 cu vecinul
                        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], adancimeMin[i]);
                        if(adancime[nodCurent] <= adancimeMin[i]){  // daca e adevarat am gasit un nod care desparte doua componente biconexe
                            vector<int> componenta;
                            componenta.push_back(nodCurent);
                            int nod = stiva.top();
                            stiva.pop();
                            componenta.push_back(nod);
                            while (i != nod){
                                nod = stiva.top();  // dam pop din stiva pana la vecin
                                stiva.pop();
                                componenta.push_back(nod);
                            }

                            componente.push_back(componenta);
                        }
                    }
                }
            }
            }

        void TarjanDFS(int nodCurent, vector<vector<int>>& componente, stack<int>& stiva, vector<bool>& peStiva, vector<int>& ordineDescoperire, vector<int>& adancimeMin, int& timp)
        {
	        ordineDescoperire[nodCurent] = adancimeMin[nodCurent] = timp++;
	        stiva.push(nodCurent);
            peStiva[nodCurent] = true;
 
	        for (auto i : listaAdiacenta[nodCurent])
	        {
		        if (ordineDescoperire[i] == -1) //daca nu am vizitat, facem dfs pe nodul respectiv
		        {
			        TarjanDFS(i, componente, stiva, peStiva, ordineDescoperire, adancimeMin, timp);
			        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], adancimeMin[i]);
		        }
		        else if (peStiva[i] == true) //daca e pe stiva atunci face parte dintr-o componenta tare conexa
			        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], ordineDescoperire[i]);
	        }
 
	        if (adancimeMin[nodCurent] == ordineDescoperire[nodCurent])	 //daca sunt egale inseamna ca nodCurent este punct de inceput pt o componenta
	        {
		        vector <int> componenta;
		        int nod = stiva.top();
		        stiva.pop();
		        peStiva[nod] = false;
		        componenta.push_back(nod);

		        while (nod != nodCurent)
		        {
			        nod = stiva.top();
			        stiva.pop();
			        peStiva[nod] = false;
			        componenta.push_back(nod);
		        }
 
		        componente.push_back(componenta);
	        }
        }

        void sortareTopologica(int nod, vector<bool>& vizitat){
            vizitat[nod] = true;
            for (int i : listaAdiacenta[nod]){
                if (!vizitat[i])
                    sortareTopologica(i, vizitat);
            }
            topologie.push_back(nod);
        }
        // void conexiuniCriticeNetwork(int nodCurent, vector<pair<int,int>>& conexiuni, vector<int>& timpDescoperire, vector<int>& adancimeMin, vector<bool>& vizitat, vector<int>& parinte, int& timp) {
 
        //     timpDescoperire[nodCurent] = adancimeMin[nodCurent] = ++timp;
        //     vizitat[nodCurent] = true;
 
        //     for(int i : listaAdiacenta[nodCurent]){
        //         if(!vizitat[i]){
        //             parinte[i] = nodCurent;
        //             conexiuniCriticeNetwork(i, conexiuni, timpDescoperire, adancimeMin, vizitat, parinte, timp);
        //             adancimeMin[nodCurent] = min(adancimeMin[nodCurent],adancimeMin[i]);
 
        //             if(timpDescoperire[nodCurent] < adancimeMin[i])
        //                 conexiuni.push_back({nodCurent,i});
        //         }
        //         else if(parinte[nodCurent] != i){
        //             adancimeMin[nodCurent] = min(adancimeMin[nodCurent],timpDescoperire[nodCurent]);
        //         }
        //     }
        // } nu merge

        int cautaRoot(int nod, vector<int> tati){
            while (nod != tati[nod])
                nod = tati[nod];
            return nod;
        }

        void combina(int tatax, int tatay, vector<int>& tati, vector<int>& inaltimi){
            if(inaltimi[tatax] > inaltimi[tatay])
                tati[tatay] = tatax;
            else if (inaltimi[tatax] < inaltimi[tatay])
                tati[tatax] = tatay;
            else{
                tati[tatax] = tatay;
                inaltimi[tatay]++;
            }
        }

    public:
        Graf(int nrNod = 0, int nrMuc = 0, bool esteOrientat = false, bool estePonderat = false){
            nrNoduri = nrNod;
            nrMuchii = nrMuc;
            orientat = esteOrientat;
            ponderat = estePonderat;            
            vector<int> gol;
            pair<int, pair<int, int>> gol2;
            for (int i = 0; i <= nrMuchii; i++){
                listaAdiacenta.push_back(gol);
                listaMuchii.push_back(gol2);
            }
                
        }

        void setareGraf(){
            if (ponderat){
                int x, y, c;
                for (int i = 1; i <= nrMuchii; i++){
                    fin >> x >> y >> c;
                    listaMuchii[i] = {c,{x, y}};
                }
            }
            else{
            if (orientat){
                int x, y;
                for(int i = 0; i < nrMuchii; i++){
                    fin >> x >> y;
                    x--;
                    y--;
                    listaAdiacenta[x].push_back(y);
                }
            }
            else{
                int x, y;
                for(int i = 0; i < nrMuchii; i++){
                    fin >> x >> y;
                    x--;
                    y--;
                    listaAdiacenta[x].push_back(y);
                    listaAdiacenta[y].push_back(x);
                }
            }
            }
        }

        vector<int> distanteMinime(int nodStart){
            vector<int> distante(nrNoduri, -1);
            vector<bool> vizitat(nrNoduri, false);

            bfs(distante, vizitat, nodStart);
            return distante;
        }

        int nrCompConexe(){
            int nr = 0;
            vector<bool> vizitat(nrNoduri, false);
            for (int i = 0; i < nrNoduri; i++)
                if (vizitat[i] == false){
                    dfs(vizitat, i);
                    nr++;
                }
            
            return nr;
        }

        vector<vector<int>> ComponenteBiconexe(){
            vector<bool> vizitat(nrNoduri, false);
            vector<int> adancime(nrNoduri, -1);
            vector<int> adancimeMin(nrNoduri, -1);
            stack<int> stiva;
            vector<vector<int>> componente;

            dfsbiconex(0, -1, 0, componente, stiva, vizitat, adancime, adancimeMin);

            return componente;
        }

        vector< vector <int> > ComponenteTareConexe()
        {
	        vector< vector<int> > componente;
	        stack<int> stiva;																
	        vector<bool> peStiva(nrNoduri, false);
	        vector<int> ordineDescoperire(nrNoduri, -1);
	        vector<int> adancimeMin(nrNoduri, -1);
	        int timp = 0;
 
	        for (int i = 0; i < nrNoduri; i++)
	        {
		        if (ordineDescoperire[i] == -1)	
		        {
			        TarjanDFS(i, componente, stiva, peStiva, ordineDescoperire, adancimeMin, timp);
		        }
	    }
	return componente;
}
        void topologic(){
            vector<bool> vizitat(nrNoduri, false);
            for (int i = 0; i < nrNoduri; i++){
                if(!vizitat[i])
                    sortareTopologica(i, vizitat);
            }

            for (int i = topologie.size() - 1; i >= 0; i--){
                fout << topologie[i] + 1 << " ";
            }
        }

        bool havelHakimi(vector<int> grade){
            while (1){
                sort(grade.begin(), grade.end(), greater<>());

                if(grade[0] == 0)
                    return true;
                int x = grade[0];
                grade.erase(grade.begin() + 0);

                if (x > grade.size())
                    return false;

                for (int i = 0; i < x; i++){
                    grade[i]--;
                    if(grade[i] < 0)
                        return false;
                } 
            }
        }
        // vector<pair<int,int>> muchiiCritice(){
        //     vector<pair<int, int>> conexiuni;
        //     vector<bool> vizitat(nrNoduri, false);
        //     vector<int> timpDescoperire(nrNoduri, -1);
        //     vector<int> adancimeMin(nrNoduri, -1);
        //     vector<int> parinte(nrNoduri, -1);
        //     int timp = 0;
        //     parinte[0] = 0;

        //     for (int i = 0; i < nrNoduri; i++){
        //         if (!vizitat[i])
        //             conexiuniCriticeNetwork(i, conexiuni, timpDescoperire, adancimeMin, vizitat, parinte, timp);
        //     }

        //     return conexiuni;
        // } nu merge

        void apmSiCost(){   //Kruskal
            vector<pair<int,int>> apm;
            vector<int> tati(nrNoduri + 1);
            vector<int> inaltimi(nrNoduri + 1);
            int cost = 0;
            sort(listaMuchii.begin() + 1, listaMuchii.end()); //Sortam dupa cost muchiile
            for (int i = 1; i <= nrNoduri; i++){
                tati[i] = i;
                inaltimi[i] = 0;
            }

            for (int i = 1; i <= nrMuchii; i++){
                int tatax, tatay;
                tatax = cautaRoot(listaMuchii[i].second.first, tati); //cautam radacinile nodurilor din muchia curenta
                tatay = cautaRoot(listaMuchii[i].second.second, tati);

                if (tatax != tatay){    //daca tatii sunt diferiti putem combina
                    combina(tatax, tatay, tati, inaltimi);
                    apm.push_back(listaMuchii[i].second);
                    cost += listaMuchii[i].first;
                }
            }
            int muchiiAPM = nrNoduri - 1;
            fout<< cost << "\n" << muchiiAPM << "\n"; // nr muchii apm = nr noduri - 1
            for (auto i : apm){
                fout << i.first << " " << i.second << "\n"; 
            } 
        }

        void disjoint(){
            vector<int> tati(nrNoduri + 1);
            vector<int> inaltimi(nrNoduri + 1, 0);
            for (int i = 1; i <= nrNoduri; i++){
                tati[i] = i;
                //inaltimi[i] = 0;
            }
            
            for (int i = 1; i <= nrMuchii; i++){
                int operatie, x, y;
                fin >> operatie >> x >> y;
                int tatax = cautaRoot(x, tati);
                int tatay = cautaRoot(y, tati);
                if (operatie == 1)
                    {   
                        combina(tatax, tatay, tati, inaltimi);
                    }
                else if (tatax == tatay)    //daca au acelasi tata atunci sunt din aceeasi multime
                    fout << "DA\n";
                    else fout << "NU\n";
            }
        }

	
        vector<int> dijkstra() {
            
            vector<vector<pair<int, int>>> listaCosturi(nrNoduri + 1, vector<pair<int, int>>(1, {-1, -1}));
            for (int i = 1; i <= nrMuchii; i++) {
                listaCosturi[listaMuchii[i].second.first].push_back(make_pair(listaMuchii[i].second.second, listaMuchii[i].first));
            }
            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> heap; //heap ul folosit (pair: cost-nod destinatie)
            vector<int> distanta(nrNoduri + 1, inf);
            vector<int> vizitat(nrNoduri + 1, 0);
            heap.push(make_pair(0, 1)); //punem in heap primul nod cu costul 0
            distanta[1] = 0;
 
            while (!heap.empty()) {
                int nod = heap.top().second; // luam nodul din varful heapului si il scoatem, iar daca nu e vizitat luam toate nodurile care pleaca din el
                heap.pop();
                if (!vizitat[nod]) {
                    vizitat[nod] = 1;
                    for (int i = 1; i < listaCosturi[nod].size(); i++)
                        if (distanta[nod] + listaCosturi[nod][i].second < distanta[listaCosturi[nod][i].first]) { // verificam distanta pana la nodul curent + cost pentru nod adiacent < distanta pana la nodul adiacent si actualizam
                            distanta[listaCosturi[nod][i].first] = distanta[nod] + listaCosturi[nod][i].second;
                            heap.push(make_pair(distanta[listaCosturi[nod][i].first], listaCosturi[nod][i].first)); // punem in heap distanta noua pana la nodul adiacent + nodul adiacent
                        }
                }
            }
            return distanta;
        }

        void bellmanFord(){
            vector<vector<pair<int, int>>> listaCosturi(nrNoduri + 1, vector<pair<int, int>>(1, {-1, -1}));
            for (int i = 1; i <= nrMuchii; i++) {
                listaCosturi[listaMuchii[i].second.first].push_back(make_pair(listaMuchii[i].second.second, listaMuchii[i].first));
            }
            vector<int> distanta(nrNoduri + 1, inf);
            vector<int> vizitat(nrNoduri + 1, 0);
            vector<int> inCoada(nrNoduri + 1, 0); // verif daca un nod se afla in coada deja (altfel se incrementeaza counterul de mai multe ori pe acelasi pas si strica edtectia ciclului)

            queue<int> coada;

            bool areCiclu = false;

            coada.push(1); //punem in coada nodul 1
            inCoada[1] = 1;
            distanta[1] = 0;

            while (!coada.empty() && !areCiclu){
                int nod = coada.front();
                coada.pop();
                inCoada[nod] = 0;

                for (int i = 1; i < listaCosturi[nod].size(); i++){
                    if (distanta[nod] + listaCosturi[nod][i].second < distanta[listaCosturi[nod][i].first]){
                        distanta[listaCosturi[nod][i].first] = distanta[nod] + listaCosturi[nod][i].second;
                        vizitat[listaCosturi[nod][i].first]++;

                        if(vizitat[listaCosturi[nod][i].first] >= nrNoduri){
                            //daca am vizitat de mai mult de nrNoduri ori acelasi nod avem ciclu negativ
                            areCiclu = true;
                            break;
                        }

                        if (!inCoada[listaCosturi[nod][i].first]){
                            coada.push(listaCosturi[nod][i].first);
                            inCoada[listaCosturi[nod][i].first] = 1;
                        }
                            

                    }
                        
                }
            }

            if (areCiclu)
                fout << "Ciclu negativ!";
            else
                for (int i = 2; i <= nrNoduri; i++)
                    fout << distanta[i] << " ";
        }

        vector<vector<int>> royFloyd(vector<vector<int>> &matrice);
        int diametruArbore();

};

vector<vector<int>> Graf::royFloyd(vector<vector<int>> &matrice){
    vector<vector<int>> dist = matrice;
    for (int i = 1; i <= nrNoduri; i++)
        for (int j = 1; j <= nrNoduri; j++)
            if (matrice[i][j] == 0 && i != j)
                dist[i][j] = 1001;  
    for (int i = 1; i <= nrNoduri; i++)
        for (int j = 1; j <= nrNoduri; j++)
            for (int k = 1; k <= nrNoduri; k++)
                if (dist[j][k] > dist[j][i] + dist[i][k])
                    dist[j][k] = dist[j][i] + dist[i][k];
    return dist;
}

int Graf::diametruArbore(){
    vector<int> dist = distanteMinime(0);
    int maxDiametru = -1, lastNode;
    for (int i = 0; i < nrNoduri; i++)
        if (dist[i] > maxDiametru) {
            //maxDiametru = dist[i];
            lastNode = i;
        }
    dist = distanteMinime(lastNode);
    maxDiametru = -1;
    for (int i = 0; i < nrNoduri; i++)
        if (dist[i] > maxDiametru)
            maxDiametru = dist[i];
    return maxDiametru;
}

int main(){
    int n;
    fin >> n;
    Graf g(n, n-1, false, false);
    g.setareGraf();
    fout << g.diametruArbore() + 1;
    fin.close();
    fout.close();
    return 0;
}