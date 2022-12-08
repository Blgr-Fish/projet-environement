#include <iostream>
#include <fstream>
#include "liste.hpp"
#include <chrono>
using namespace std::chrono;
using namespace std;


// STRUCTURES DE DONNEES //


struct Production{

    struct Energie{                      // enregistrement imbriqué à Production qui permet d'avoir la production ainsi que le taux de production 

        double production;               // un double c'est comme un flottant mais en plus précis
        double taux_production = 0;
    };

    int region, mois, jour, heure;  
    Energie thermique, nucleaire, eolien, solaire, hydraulique, bioenergie,importation; // ce qui donne {production,taux_production} pour chaque type de production
};


struct Tache_de_calcul{ // enregistrement qui contient les données de la feuille de calcul (nombre de régions, cout marginal , etc)

	int identifiant,mois_depart,jour_depart,horaire_depart,mois_terminaison,jour_terminaison,horaire_terminaison;
	string nom;
	float duree,cout_moyen_maximum,cout_marginal_maximum,pourcentage_minimum_production_marginale,pourcentage_maximal_importation,pourcentage_maximal_importation_nationale;
    liste<int> region;	

};


struct Couts{  // enregistrement qui contient les couts de productions de chaque énergie
	int cout_thermique, cout_nucleaire, cout_eolien, cout_solaire, cout_hydraulique, cout_bioenergie;
};


struct Region{

    int id;
    string nom;
    liste<Production> valeurs_production = {};

};




void taux_de_production_energie(Production & p_r, int & production_totale){

/*
    permet de calculer le taux de production de chaque type de 
    production d’une région à partir de l’enregistrement Production
	
*/
    // on créé la production totale qui est la somme des productions.
    
    
    int production_totale_echanges = 0;
    production_totale += p_r.thermique.production + p_r.nucleaire.production + p_r.eolien.production +p_r.solaire.production + p_r.hydraulique.production + p_r.bioenergie.production ;
    production_totale_echanges += p_r.importation.production + production_totale; // C'est la production totale avec les echanges physiques

    // pour faire le taux on fait : (production * 100) / production_totale.

    p_r.thermique.taux_production   = (p_r.thermique.production * 100) / production_totale;
    p_r.nucleaire.taux_production   = (p_r.nucleaire.production * 100) / production_totale;
    p_r.eolien.taux_production      = (p_r.eolien.production * 100) / production_totale;
    p_r.solaire.taux_production     = (p_r.solaire.production * 100) / production_totale;
    p_r.hydraulique.taux_production = (p_r.hydraulique.production * 100) / production_totale;
    p_r.bioenergie.taux_production  = (p_r.bioenergie.production * 100) / production_totale;

	// le if-else sert pour le taux d'importation, en fonction de si c'est de l'exportation ou de l'importation, on change le dénominateur

    if (p_r.importation.production >= 0){
        p_r.importation.taux_production = (p_r.importation.production * 100 ) / production_totale_echanges; // importation
    }
    else {
         p_r.importation.taux_production = (p_r.importation.production * 100 ) / production_totale; // exportation
    }
}


float echanges_nationaux (int echanges_totaux, int production_totale) { // renvoie les echanges physiques nationaux.
	float resultats = (echanges_totaux*100) / production_totale;
	return resultats;
}


float couts_moyen(Production p_r, Couts cout) { // renvoie le cout de production moyen d'une region 
	float resultats = (cout.cout_thermique * (p_r.thermique.taux_production/100)) + (cout.cout_nucleaire* (p_r.nucleaire.taux_production/100)) + (cout.cout_eolien*(p_r.eolien.taux_production/100)) + (cout.cout_solaire * (p_r.solaire.taux_production/100)) + (cout.cout_hydraulique *  (p_r.hydraulique.taux_production/100)) + (cout.cout_bioenergie *  (p_r.bioenergie.taux_production/100));
	return resultats;
}


int cout_marginal_regional(Production regionale,Tache_de_calcul Tache_de_calcul, Couts couts){ // renvoie le cout marginal d'une region en fonction de la tache de calcul
	
	int cout_marginale = 0;
	
	if(regionale.thermique.taux_production > Tache_de_calcul.pourcentage_minimum_production_marginale){
		cout_marginale = couts.cout_thermique; // 560
		}
	else{
		if(regionale.bioenergie.taux_production > Tache_de_calcul.pourcentage_minimum_production_marginale){
			cout_marginale = couts.cout_bioenergie; // 230
			}
		else{
			if(regionale.solaire.taux_production > Tache_de_calcul.pourcentage_minimum_production_marginale){
				cout_marginale = couts.cout_solaire; // 27
				}
			else{
				if(regionale.hydraulique.taux_production > Tache_de_calcul.pourcentage_minimum_production_marginale){
					cout_marginale = couts.cout_hydraulique; // 24
					}
				else{
					if(regionale.nucleaire.taux_production > Tache_de_calcul.pourcentage_minimum_production_marginale){
						cout_marginale = couts.cout_nucleaire; // 12
						}
					else{
						if(regionale.eolien.taux_production > Tache_de_calcul.pourcentage_minimum_production_marginale){
							cout_marginale = couts.cout_eolien; // 11
							}
						}
					}
				}
			}
		}
	
	return cout_marginale;
}


bool contraintes(Production production ,Tache_de_calcul Tache_de_calcul, int cout_marg, float cout_moy,int production_region){

	// les différentes contraintes sont : 
	// 1. le cout moyen
	// 2. le cout marginal
	// 3. le taux d'importation
	// Si toutes les contraintes sont respectées, la fonction renvoie true

	bool conditions = false;

	if (cout_moy < Tache_de_calcul.cout_moyen_maximum){ // 1

		if (cout_marg < Tache_de_calcul.cout_marginal_maximum){ // 2

			if (production_region >= Tache_de_calcul.pourcentage_maximal_importation){ // 3

				if (production.mois >= Tache_de_calcul.mois_depart and production.jour >= Tache_de_calcul.jour_depart and production.heure >= Tache_de_calcul.horaire_depart){
				
					conditions = true;
				}

			}
		}
	}

	return conditions;
}


void insere_region(Production p_r, liste<Production> & r, Tache_de_calcul Tache_de_calcul){ // permet l'insertion des régions dans la liste parallele

	if (taille(r) < Tache_de_calcul.duree){  
		

		inserer(p_r,r, taille(r)+1);

	}
}

void insere_region_mono (Production p_r, Region & r, Tache_de_calcul Tache_de_calcul){

	cout << taille(r.valeurs_production) << endl;

	for (int i = 1; i<= 5; i++){
		cout << "haha" << endl;
		if (r.id == p_r.region){

			

			if (taille(r.valeurs_production) < Tache_de_calcul.duree){

				inserer(p_r, r.valeurs_production, taille( r.valeurs_production)+1);
			}	
		}
	}
	
	
}

void lire_production (liste<Region> & regions,liste<Production> & parallele, liste<Production> & sequentielle, string fichier,Couts couts,Tache_de_calcul tache_de_calcul, int mode_calcul){
																									
	/* Paramètres :
	
	fichier: permet de lire le fichier des productions (celui de 100 000+ lignes)
	couts : c'est l'enregistrement permettant des lire les couts
	Tache_de_calcul : c'est l'enegistrement permettant de lire la feuille de calcul.
	mode_calcul : c'est l'entier qui va determiner la méthode d'execution
	prend les valeurs suivantes : 1 : mode_calcul = parallele, 2 : mode_calcul = monoregion, autre : mode_calcul = sequentielle
	
	*/

    fstream flux;
    Production production_region;


	int nombre_regions = taille(tache_de_calcul.region); // nombre de régions
    int prod_totale_region = 0; 						 // la production totale d'une région qui est initialisée à 0
	int region_compteur = 1 ; 							 // il va s'incrementer à chaque fois qu'on calcul une nouvelle region jusqu'a ce qu'on fasse toutes les régions
    int prod_totale_nation = 0; 						 // la production tôtale des 12 régions
    int echanges_totaux = 0; 							 // les échanges physiques totaux des 12 régions
	int cout_marginal = 0; 								 // le cout marginal d'une région	
	float cout_moyen = 0; 								 //le cout moyen des productions d'une region 
	float importation_nationale = 0; 					 // importation nationale 
	
	bool depassement_date = false;
	liste<Production> liste_regions_temp = {}; 			 // Création d'une liste temportaire qui prendra comme valeurs les Productions qui passent les contraintes

    flux.open(fichier, ios::in);
    if (flux.is_open()) {

        flux >>production_region.region;  // première lecture avant le tant que
	    flux >>production_region.mois; 
	    flux >>production_region.jour; 
	    flux >>production_region.heure;

        flux >>production_region.thermique.production; // les productions de chaque moyen de production
        flux >>production_region.nucleaire.production;
        flux >>production_region.eolien.production;
        flux >>production_region.solaire.production;
        flux >>production_region.hydraulique.production;
        flux >>production_region.bioenergie.production;

        flux >>production_region.importation.production;

        taux_de_production_energie(production_region,prod_totale_region); // cette procedure permet d'avoirs les taux de productions et de récuperer la production totale
        prod_totale_nation += prod_totale_region; 
		region_compteur ++;
        echanges_totaux += production_region.importation.production; 
		cout_moyen = couts_moyen(production_region,couts);
		cout_marginal = cout_marginal_regional(production_region,tache_de_calcul,couts);


        while (flux.good()) { 
       			
			for (long unsigned int region_id : tache_de_calcul.region){ // on vérifie que l'id de la région est présent dans la liste des régions de la feuille de calcul

				if (region_id == production_region.region and contraintes(production_region,tache_de_calcul,cout_marginal,cout_moyen,prod_totale_region)){

					inserer(production_region, liste_regions_temp, taille(liste_regions_temp)+1);

				}

			}			

			if (region_compteur > nombre_regions){ // la production nationale repasse à 0 quand on a fait le tour de toutes le regions
				importation_nationale = echanges_nationaux(echanges_totaux,prod_totale_nation);	
				prod_totale_nation = 0;
				echanges_totaux = 0;
				region_compteur = 1;

				Production ele_temp;
				float cout_eleve = 1000;     // on met un nombre très grand nombre pour l'initialisation.

				if (importation_nationale <= tache_de_calcul.pourcentage_maximal_importation_nationale){ // importation nationale
					
					for (Production ele : liste_regions_temp){

						if ( cout_eleve >= couts_moyen(ele,couts)){ // vu que cout_eleve est très grand, il sera forcément supérieur à couts_moyen.
							
							ele_temp = ele;
							cout_eleve = couts_moyen(ele_temp,couts); // puis cout_eleve prend la valeur du cout moyen de ele.
														
						}
						
						switch (mode_calcul){ // en fonction du mode de calcul, on choisit une méthode d'execution

						case 1 :
							insere_region(ele, regions[production_region.region].valeurs_production, tache_de_calcul);
							break;
						
						case 2:
							cout << "ici" << " " << taille(regions[production_region.region].valeurs_production)<< endl;
							insere_region_mono(ele, regions[production_region.region], tache_de_calcul);
							break;

						}						
					}

					if (mode_calcul != 1 and mode_calcul != 2 and couts_moyen(ele_temp,couts) > 0){ // des fois, couts_moyen(ele_temp,couts) est = 0, ce qui fait
																									// que la liste se remplie de 0.
						insere_region(ele_temp, regions[production_region.region].valeurs_production, tache_de_calcul);

					}

					liste_regions_temp = {};
				}
			}
					
            int prod_totale_region = 0;

            flux >>production_region.region; 
	        flux >>production_region.mois; 
	        flux >>production_region.jour; 
	        flux >>production_region.heure;

			if (production_region.mois >= tache_de_calcul.mois_terminaison){ // on s'assure qu'on a pas dépassé la date de fin dans la tache de calcul
				if(production_region.jour >= tache_de_calcul.jour_terminaison){
					if(production_region.heure > tache_de_calcul.horaire_terminaison){

						depassement_date = true; // si c'est la cas on passe depassement_date à true
							
					}
				}
			}

            flux >>production_region.thermique.production; 
        	flux >>production_region.nucleaire.production; 
        	flux >>production_region.eolien.production;
        	flux >>production_region.solaire.production;
            flux >>production_region.hydraulique.production;
        	flux >>production_region.bioenergie.production;
            flux >>production_region.importation.production;

            taux_de_production_energie(production_region,prod_totale_region); 
            prod_totale_nation += prod_totale_region;
			region_compteur ++;
            echanges_totaux += production_region.importation.production;
			
			cout_moyen = couts_moyen(production_region,couts);

			cout_marginal = cout_marginal_regional(production_region,tache_de_calcul,couts); 

			if (depassement_date){ 
				flux.close();	 
			}
		}
					

        flux.close();   
    }
    else {
        cout << "Erreur : impossible d'ouvrir " << fichier << endl;
    }
    
}

Tache_de_calcul lire_tache_de_calcul(string nom_fichier){
	Tache_de_calcul tache_de_calcul;
	fstream flux;
	liste<int> li =  {};
	int nb;
	
	flux.open(nom_fichier, ios::in);
	if (flux.is_open()) {
		flux >> tache_de_calcul.identifiant;  // première lecture avant le tant que
		flux >> tache_de_calcul.nom;
		flux >> tache_de_calcul.duree;
		flux >> tache_de_calcul.mois_depart;
		flux >> tache_de_calcul.jour_depart;
		flux >> tache_de_calcul.horaire_depart;
		flux >> tache_de_calcul.mois_terminaison;
		flux >> tache_de_calcul.jour_terminaison;
		flux >> tache_de_calcul.horaire_terminaison;
		flux >> tache_de_calcul.cout_moyen_maximum;
		flux >> tache_de_calcul.cout_marginal_maximum;
		flux >> tache_de_calcul.pourcentage_minimum_production_marginale;
		flux >> tache_de_calcul.pourcentage_maximal_importation;
		flux >> tache_de_calcul.pourcentage_maximal_importation_nationale;
		flux >> nb;

		while (flux.good()){

			inserer(nb, li, taille(li)+1);
			flux >> nb ;
		}
			inserer(nb, li, taille(li)+1); // on insere une dernière fois pour pas passer la dernière région
			tache_de_calcul.region = li;

		
		flux.close();   
	}
	else {
		cout << "Erreur : impossible d'ouvrir " << nom_fichier << endl;
		}

	return tache_de_calcul;

}

Couts lire_couts(string fichier){
	Couts couts;
	fstream flux;
	flux.open(fichier, ios::in);
	
	if (flux.is_open()) {
		
		flux >> couts.cout_thermique;
		flux >> couts.cout_nucleaire;
		flux >> couts.cout_eolien;
		flux >> couts.cout_solaire;
		flux >> couts.cout_hydraulique;
		flux >> couts.cout_bioenergie;

		flux.close();
	}
	else {
		cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}
	return couts;
	
}

liste<string> lire_regions_noms(string fichier){

	liste<string> region_nom = {};
	fstream flux;
	string r;
	flux.open(fichier , ios::in);
	if(flux.is_open()){

		flux >> r;

	while(flux.good()){

		inserer(r,region_nom,taille(region_nom)+1);
		flux  >> r;

	}

	flux.close();

	}

	else {
		cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}

	return region_nom;

}





void lire_regions (liste<Region> & r,liste<string> regions_noms){		// on met le nom d'une région dans une Caracterisitiques, puis on la met dans une liste d eCaracteristiques.

	int num = 1;
	
	for (string ele : regions_noms){

        Region temp;

		temp.nom = ele;
		temp.id = num;
		num ++;

		inserer(temp,r,taille(r)+1);

	}


}

int afficher_contenu_region(liste<Production> region, int identifiant, Couts couts, string fichier){

	fstream flux;
	flux.open(fichier,ios::app); // ios::app permet d'ecrire à la suite du fichier, sans supprimer les données précédentes.
	
	if (flux.is_open()){

	
		switch (identifiant){ // MODIFICATION ICI //

			case -1 :

				flux << "Sequentielle" << " " << taille(region) << endl;
				break;

			case 0 :	// on choisit 0 pour le mode parallèle, cela nous évite de devoir créer une seconde fonction juste pour l'affichage de cette méthode d'execution.
				
				flux << "Parallele" << " " << taille(region) << endl;				
				break;

		}

			for (Production ele : region){

				flux << ele.mois << " " << ele.jour << " " << ele.heure << " " << ele.region << " " << couts_moyen(ele, couts) << endl;

			}
		

		flux.close();
	}
	else{
		 cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}

	return 0;
}

int afficher_contenu_region_mono (liste<Region> region, int identifiant, Couts couts, string fichier){
	
	fstream flux;
	flux.open(fichier,ios::app); // ios::app permet d'ecrire à la suite du fichier, sans supprimer les données précédentes.
	cout << "caca" << endl;
	
	if (flux.is_open()){


		for (long unsigned int i = 1; i<= taille(region);i++){

			cout << region[i].nom<< " " << region[i].id << endl;
		}
		

		flux << region[identifiant].nom << " " << taille(region[identifiant].valeurs_production) << endl;
		
		if ( taille(region[identifiant].valeurs_production)>= 1){
			for (int i = 1; i<= taille(region[identifiant].valeurs_production); i++){
			
				flux << region[identifiant].valeurs_production[i].mois << " " << region[identifiant].valeurs_production[i].jour << " " << region[identifiant].valeurs_production[i].heure << " " << region[identifiant].valeurs_production[i].region<< " " << couts_moyen(region[identifiant].valeurs_production[i], couts) << endl;

		    }
		}
		
		flux.close();
	}
	else{
		 cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}

	return 0;
}

int afficher_regions (liste<Region> r,liste<Production> p, liste<Production> s,Couts couts, int mode, string fichier){ 

	if (mode == 1){

		afficher_contenu_region(p,0,couts,fichier); // on choisit 0 l'id pour la liste parallele
	}

	else{

		if (mode == 2){

			//afficher_contenu_region_mono(r,4,couts,fichier);

			for (int i = 1; i<= taille(r); i++){
				
				afficher_contenu_region_mono(r,r[i].id,couts,fichier);

			}

		}

		else{

			afficher_contenu_region(s,-1,couts,fichier); // on choisit -1 l'id pour la liste parallele
		}
	}
	
	return 0;

}

int main(int argc , char * argv[]){ // Tache_de_calcul couts mode fichier_production   ex : tache_deb.txt couts.txt 4 t5.ssv


    liste<string> arguments_programme = arguments(argc,argv); // la liste des arguments ecrit depuis le terminal


    liste<Region> les_regions = {};
    liste<Production> methode_parallele = {};
    liste<Production> methode_sequentielle = {};


    Tache_de_calcul tache_calcul = lire_tache_de_calcul(arguments_programme[1]);
    Couts couts_productions = lire_couts(arguments_programme[2]);


    
    

    liste<string> liste_regions = lire_regions_noms(arguments_programme[3]);
    lire_regions(les_regions,liste_regions);

    ofstream file0("parallele.txt");	
    ofstream file1("monoregion.txt");	
    ofstream file2("sequentielle.txt");	

    string fichier_production ;

    for (string ele : arguments_programme){

        fichier_production = ele;

    }

    cout << "Chargement... Cela peut prendre jusqu'a plusieurs dizaines de secondes..." << endl;

    auto start = high_resolution_clock::now(); // pour lancer le chrono

    lire_production(les_regions,methode_parallele,methode_sequentielle,fichier_production,couts_productions,tache_calcul,1);
    lire_production(les_regions,methode_parallele,methode_sequentielle,fichier_production,couts_productions,tache_calcul,2);
    lire_production(les_regions,methode_parallele,methode_sequentielle,fichier_production,couts_productions,tache_calcul,3);

    cout << "Fin." << endl ;


    auto stop = high_resolution_clock::now(); // fin du chrono
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << "Temps d'execution : " << duration.count() <<  " millisecondes" << endl;

    return 0;








}
	/*liste<string> arguments_programme = arguments(argc,argv); // la liste des arguments ecrit depuis le terminal
	string production;
    Region mes_regions ;
    string Tache_de_calcul = arguments_programme[1];
	Couts couts_productions = lire_couts(arguments_programme[2]);
    Tache_de_calcul t = lire_Tache_de_calcul(Tache_de_calcul);
	string fichier_ecriture;
	int mode ;


	liste<string> r = lire_regions_noms(arguments_programme[4]);
	lire_regions(mes_regions,r);

	Production aaa ;
    
	inserer(aaa,mes_regions.liste_regions[1].valeur_region,1);
	cout << "yo" << endl;
	mes_regions.liste_regions[1].valeur_region[1].jour = 1;
	cout << "hahahahahahah" << endl;
	cout <<"siuuu"<< mes_regions.liste_regions[1].valeur_region[1].jour<< endl;

	

	for(Regions::Caracteristiques ele : mes_regions.liste_regions){

		//cout << ele.nom_region << " " << ele.id <<endl;
	}

	for (long unsigned int i = 1; i<= taille(mes_regions.liste_regions);i++){

	//	cout << mes_regions.liste_regions[i].nom_region << " " << mes_regions.liste_regions[i].id << endl;
	}



	for (string ele : arguments_programme){
		production = ele;
	}

	if (arguments_programme[3] == "1"){
		mode = 1;
	}
	else{
		if (arguments_programme[3]== "2"){
			mode = 2;
		}
		else{
			mode = 3;
		}
	}
	
	switch (mode){

		case 1 : 
			{fichier_ecriture = "parallele.txt";	// on met des accolades pour pouvoir créer une variable, sinon on ne peut pas en créer.
			ofstream file("parallele.txt");			//permet de supprimer ce qui est déjà présent dans le fichier
			break;
			}

		case 2 :
			{fichier_ecriture = "monoregion.txt";
			ofstream file("monoregion.txt");
			break;
			}
			
		default:
			fichier_ecriture = "sequentielle.txt";
			ofstream file("sequentielle.txt");
			break;

	}

	cout << "Chargement... Cela peut prendre jusqu'a plusieurs dizaines de secondes..." << endl;

	auto start = high_resolution_clock::now(); // pour lancer le chrono
    
	mes_regions = lire_production(production,couts_productions,t,mode);  // Erreur qui fait que quand on utilise arguments_programme au lieu d'un vrai string ça plante.
	cout <<"la " << taille(mes_regions.liste_regions) << endl;
    afficher_regions(mes_regions,couts_productions,mode,fichier_ecriture);
	cout << "Fin." << endl;

	auto stop = high_resolution_clock::now(); // fin du chrono
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << "Temps d'execution : " << duration.count() <<  " millisecondes" << endl;*/
