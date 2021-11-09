#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

using namespace std;

ifstream fin("ctc.in");
ofstream fout("ctc.out");

class Graf{
    private:
        bool orientat;
        int nrNoduri;
        int nrMuchii;
        vector<vector<int>> listaAdiacenta;
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
                        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], adancime[i]);
                    }
                    else{
                        dfsbiconex(i, nodCurent, adancimeCurenta + 1, componente, stiva, vizitat, adancime, adancimeMin);
                        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], adancimeMin[i]);
                        if(adancime[nodCurent] <= adancimeMin[i]){
                            vector<int> componenta;
                            componenta.push_back(nodCurent);
                            int nod = stiva.top();
                            stiva.pop();
                            componenta.push_back(nod);
                            while (i != nod){
                                nod = stiva.top();
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
		        if (ordineDescoperire[i] == -1)
		        {
			        TarjanDFS(i, componente, stiva, peStiva, ordineDescoperire, adancimeMin, timp);
			        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], adancimeMin[i]);
		        }
		        else if (peStiva[i] == true)
			        adancimeMin[nodCurent] = min(adancimeMin[nodCurent], ordineDescoperire[i]);
	        }
 
	        if (adancimeMin[nodCurent] == ordineDescoperire[nodCurent])	
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

    public:
        Graf(){
            nrNoduri = 0;
            nrMuchii = 0;
            orientat = false;
            vector<int> gol;
            for (int i = 0; i < nrNoduri; i++)
                listaAdiacenta.push_back(gol);
        }
        Graf(int nrNod, int nrMuc, bool esteOrientat){
            nrNoduri = nrNod;
            nrMuchii = nrMuc;
            orientat = esteOrientat;            
            vector<int> gol;
            for (int i = 0; i < nrNoduri; i++)
                listaAdiacenta.push_back(gol);
        }

        void setareGraf(){
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

};

int main(){
    // int n, m;
	// vector< pair<int, int> > componente;
	// fin >> n >> m;
	// Graf graph(n, m, false);
	// graph.setareGraf();
	// componente = graph.muchiiCritice();
    // for (auto i : componente)
    //     cout<<i.first<< " "<< i.second << endl;
    return 0;
}