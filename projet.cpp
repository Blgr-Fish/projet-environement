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


struct tache_calcul{ // enregistrement qui contient les données de la feuille de calcul (nombre de régions, cout marginal , etc)

	int identifiant,mois_depart,jour_depart,horaire_depart,mois_terminaison,jour_terminaison,horaire_terminaison;
	string nom;
	float duree,cout_moyen_maximum,cout_marginal_maximum,pourcentage_minimum_production_marginale,pourcentage_maximal_importation,pourcentage_maximal_importation_nationale;
    liste<int> region;	

};

struct Couts{  // enregistrement qui contient les couts de productions de chaque énergie
	int cout_thermique, cout_nucleaire, cout_eolien, cout_solaire, cout_hydraulique, cout_bioenergie;
};

struct Regions{ // enregistrement qui contient des liste d'enregistrement Production. chaque liste correspond à une des 12 régions

	// mono-region //

	liste<Production> ile_de_france = {};
	liste<Production> centre_val_de_loire = {};
	liste<Production> bourgogne_franche_comte = {};
	liste<Production> normandie = {};
	liste<Production> hauts_de_france = {};
	liste<Production> grand_est = {};
	liste<Production> pays_de_la_loire = {};
	liste<Production> bretagne = {};
	liste<Production> nouvelle_aquitaine = {};
	liste<Production> occitanie = {};
	liste<Production> auvergne_rhone_alpes = {};
	liste<Production> provence_alpes_cote_d_azur = {};

	// parallele //

	liste<Production> parallele = {} ;

	// sequentielle //

	liste<Production> sequentielle = {};

};






// FONCTIONS ET PROCEDURES //





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




int cout_marginal_regional(Production regionale,tache_calcul tache_de_calcul, Couts couts){ // renvoie le cout marginal d'une region en fonction de la tache de calcul
	
	int cout_marginale = 0;
	
	if(regionale.thermique.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
		cout_marginale = couts.cout_thermique; // 560
		}
	else{
		if(regionale.bioenergie.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
			cout_marginale = couts.cout_bioenergie; // 230
			}
		else{
			if(regionale.solaire.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
				cout_marginale = couts.cout_solaire; // 27
				}
			else{
				if(regionale.hydraulique.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
					cout_marginale = couts.cout_hydraulique; // 24
					}
				else{
					if(regionale.nucleaire.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
						cout_marginale = couts.cout_nucleaire; // 12
						}
					else{
						if(regionale.eolien.taux_production > tache_de_calcul.pourcentage_minimum_production_marginale){
							cout_marginale = couts.cout_eolien; // 11
							}
						}
					}
				}
			}
		}
	
	return cout_marginale;
}


bool contraintes(Production production ,tache_calcul tache_de_calcul, int cout_marg, float cout_moy,int production_region){

	// les différentes contraintes sont : 
	// 1. le cout moyen
	// 2. le cout marginal
	// 3. le taux d'importation
	// Si toutes les contraintes sont respectées, la fonction renvoie true

	bool conditions = false;

	if (cout_moy < tache_de_calcul.cout_moyen_maximum){ // 1

		if (cout_marg < tache_de_calcul.cout_marginal_maximum){ // 2

			if (production_region >= tache_de_calcul.pourcentage_maximal_importation){ // 3

				if (production.mois >= tache_de_calcul.mois_depart and production.jour >= tache_de_calcul.jour_depart and production.heure >= tache_de_calcul.horaire_depart){
				
					conditions = true;
				}

			}
		}
	}

	return conditions;
}

void insere_region_parallele(Production p_r, Regions & r, tache_calcul tache_de_calcul){ // permet l'insertion des régions dans la liste parallele

	if (taille(r.parallele) < tache_de_calcul.duree){  

		inserer(p_r,r.parallele, taille(r.parallele)+1);

	}

}

void insere_region_mono (Production p_r, Regions & r, tache_calcul tache_de_calcul){

	switch(p_r.region){ // en fonction de l'id de la région, on a un cas différent : s'inserer dans une des 12 listes des régions

		case 1 :
			if (taille(r.ile_de_france) < tache_de_calcul.duree){ // ce if permet de stoper l'insertion lorsqu'on a dépasse la limite de calcul pour cette région
				inserer(p_r, r.ile_de_france, taille(r.ile_de_france)+1);
			}
			break;

		case 2:
			if (taille(r.centre_val_de_loire) < tache_de_calcul.duree){
				inserer(p_r, r.centre_val_de_loire, taille(r.centre_val_de_loire)+1);
			}
			break;

		case 3:
			if (taille(r.bourgogne_franche_comte) < tache_de_calcul.duree){
				inserer(p_r, r.bourgogne_franche_comte, taille(r.bourgogne_franche_comte)+1);
			}
			break;

		case 4:
			if (taille(r.normandie) < tache_de_calcul.duree){
				inserer(p_r, r.normandie, taille(r.normandie)+1);
			}
			break;

		case 5:
			if (taille(r.hauts_de_france) < tache_de_calcul.duree){
				inserer(p_r, r.hauts_de_france, taille(r.hauts_de_france)+1);
			}
			break;
			
		case 6:
			if (taille(r.grand_est) < tache_de_calcul.duree){
				inserer(p_r, r.grand_est, taille(r.grand_est)+1);
			}
			break;

		case 7:
			if (taille(r.pays_de_la_loire) < tache_de_calcul.duree){
				inserer(p_r, r.pays_de_la_loire, taille(r.pays_de_la_loire)+1);
			}
			break;

		case 8:
			if (taille(r.bretagne) < tache_de_calcul.duree){
				inserer(p_r, r.bretagne, taille(r.bretagne)+1);
			}
			break;

		case 9:
			if (taille(r.nouvelle_aquitaine) < tache_de_calcul.duree){
				inserer(p_r, r.nouvelle_aquitaine, taille(r.nouvelle_aquitaine)+1);
			}
			break;

		case 10:
			if (taille(r.occitanie) < tache_de_calcul.duree){
				inserer(p_r, r.occitanie, taille(r.occitanie)+1);
			}
			break;

		case 11:
			if (taille(r.auvergne_rhone_alpes) < tache_de_calcul.duree){
				inserer(p_r, r.auvergne_rhone_alpes, taille(r.auvergne_rhone_alpes)+1);
			}
			break;

		case 12:
			if (taille(r.provence_alpes_cote_d_azur) < tache_de_calcul.duree){
				inserer(p_r, r.provence_alpes_cote_d_azur, taille(r.provence_alpes_cote_d_azur)+1);
			}
			break;

	}


}


void insere_region_sequentielle(Production p_r, Regions & r, tache_calcul tache_de_calcul){ // permet l'insertion des régions dans la liste séquentielle

	if (taille(r.sequentielle) < tache_de_calcul.duree){  

		inserer(p_r,r.sequentielle, taille(r.sequentielle)+1);

	}

}

// ALGORITHMES POUR LIRE LES FICHIERS //


Regions lire_production (string fichier,Couts couts,tache_calcul tache_de_calcul, int mode_calcul = 0){
																									
	/* Paramètres :
	
	fichier: permet de lire le fichier des productions (celui de 100 000+ lignes)
	couts : c'est l'enregistrement permettant des lire les couts
	tache_de_calcul : c'est l'enegistrement permettant de lire la feuille de calcul.
	mode_calcul : c'est l'entier qui va determiner la méthode d'execution
	prend les valeurs suivantes : 1 : mode_calcul = parallele, 2 : mode_calcul = monoregion, autre : mode_calcul = sequentielle
	
	*/

    fstream flux;
    Production production_region;
	Regions regions;

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
							insere_region_parallele(ele, regions, tache_de_calcul);
							break;
						
						case 2:
						cout<<'b'<<endl;
							insere_region_mono(ele, regions, tache_de_calcul);
							break;

					
						}

						
					}

					if (mode_calcul != 1 and mode_calcul != 2 and couts_moyen(ele_temp,couts) > 0){ // des fois, couts_moyen(ele_temp,couts) est = 0, ce qui fait
																									// que la liste se remplie de 0.
						insere_region_sequentielle(ele_temp, regions, tache_de_calcul);

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
    

    return regions;
}

tache_calcul lire_tache_calcul(string nom_fichier){
	tache_calcul tache_de_calcul;
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
			inserer(nb, li, taille(li)+1);
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


liste<string> lire_region (string fichier){


	liste<string> r;
	string m;
	fstream flux;
	flux.open(fichier, ios::in);
	
	if (flux.is_open()) {
		
		while(flux.good()){
			flux >> m ;
			inserer (m, r, taille(r)+1);

		}

		flux.close();
	}
	else {
		cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}
	return r;


}

int afficher_contenu_region (liste<Production> region, int id, Couts couts, string fichier){

	fstream flux;
	flux.open(fichier,ios::app); // ios::app permet d'ecrire à la suite du fichier, sans supprimer les données précédentes.
	
	if (flux.is_open()){

	
		switch (id){

			

			case -1 :

				flux << "Sequentielle" << " " << taille(region) << endl;
				break;

			case 0 :	// on choisit 0 pour le mode parallèle, cela nous évite de devoir créer une seconde fonction juste pour l'affichage de cette méthode d'execution.
				
				flux << "Parallele" << " " << taille(region) << endl;
				
				break;

			case 1 :
				
				flux << "Ile-de-France" << " " << taille(region) << endl;
				
				break;

			case 2 :
				
				flux << "Centre-Val_de_Loire" << " " << taille(region) << endl;
				
				break;

			case 3 :
				
				flux << "Bourgogne-Franche-Comte" << " " << taille(region) << endl;
				
				break;

			case 4 :
				
				flux << "Normandie" << " " << taille(region) << endl; ;
				
				break;

			case 5 :
				
				flux << "Hauts-de-France" << " " << taille(region) << endl;
				
				break;

			case 6 :
				
				flux << "Grand_Est" << " " << taille(region) << endl;
				
				break;

			case 7 :
				
				flux << "Pays_de_la_Loire" << " " << taille(region) << endl;
				
				break;

			case 8 :
				
				flux << "Bretagne" << " " << taille(region) << endl;
				
				break;

			case 9 :
				
				flux << "Nouvelle-Aquitaine" << " " << taille(region) << endl;
				
				break;

			case 10 :
				
				flux << "Occitanie" << " " << taille(region) << endl;
				
				break;

			case 11 :
				
				flux << "Auvergne-Rhone-Alpes" << " " << taille(region) << endl;
				
				break;

			case 12 :
				
				flux << "Provence-Alpes-Cote_d'Azur" << " " << taille(region) << endl;
				
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


int afficher_regions (Regions r,Couts couts, int mode, string fichier){ 

	if (mode == 1){

		afficher_contenu_region(r.parallele,0,couts,fichier); // on choisit 0 l'id pour la liste parallele
	}

	else{

		if (mode == 2){

			afficher_contenu_region(r.ile_de_france,1,couts,fichier);
			afficher_contenu_region(r.centre_val_de_loire,2,couts,fichier);
			afficher_contenu_region(r.bourgogne_franche_comte,3,couts,fichier);
			afficher_contenu_region(r.normandie,4,couts,fichier);
			afficher_contenu_region(r.hauts_de_france,5,couts,fichier);
			afficher_contenu_region(r.grand_est,6,couts,fichier);
			afficher_contenu_region(r.pays_de_la_loire,7,couts,fichier);
			afficher_contenu_region(r.bretagne,8,couts,fichier);				
			afficher_contenu_region(r.nouvelle_aquitaine,9,couts,fichier);
			afficher_contenu_region(r.occitanie,10,couts,fichier);
			afficher_contenu_region(r.auvergne_rhone_alpes,11,couts,fichier);
			afficher_contenu_region(r.provence_alpes_cote_d_azur,12,couts,fichier);

		}

		else{

			afficher_contenu_region(r.sequentielle,-1,couts,fichier); // on choisit -1 l'id pour la liste parallele
		}
	}
	
	return 0;

}

int main(int argc , char * argv[]){ // tache_de_calcul couts mode fichier_production   ex : tache_deb.txt couts.txt 4 t5.ssv

	liste<string> arguments_programme = arguments(argc,argv);
	string production;


    Regions mes_regions ;
    string tache_de_calcul = arguments_programme[1];
	Couts couts_productions = lire_couts(arguments_programme[2]);
    tache_calcul t = lire_tache_calcul(tache_de_calcul);
	string fichier_ecriture;
	int mode ;

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
    afficher_regions(mes_regions,couts_productions,mode,fichier_ecriture);
	cout << "Fin." << endl;

	auto stop = high_resolution_clock::now(); // fin du chrono
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << "Temps d'execution : " << duration.count() <<  " millisecondes" << endl;

	liste<string> aaa = lire_region("regions.txt");
	
    

    return 0;

}