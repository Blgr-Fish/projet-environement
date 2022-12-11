/*
| Titre : Projet d'informatique de fin de semestre.
|
| Rôle : Planifier une tâche de calcul coûteuse mais découpable en 
| 	     fonction de critères multiples dont environnementaux.
|
| Autheurs : IJJA Ziad, ROGER Aurivel et SAINTE-LUCE Clément.
*/

#include <iostream>
#include <fstream>
#include "liste.hpp"
#include <chrono>			// pour utiliser un chrono, permet de calculer la vitesse d'execution du programme
using namespace std::chrono;//
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

// -----------------------------------------------------------------------------

struct Tache_de_calcul{ // enregistrement qui contient les données de la feuille de calcul (nombre de régions, cout marginal , etc)

	int identifiant,mois_depart,jour_depart,horaire_depart,mois_terminaison,jour_terminaison,horaire_terminaison;
	string nom;
	float duree,cout_moyen_maximum,cout_marginal_maximum,pourcentage_minimum_production_marginale,pourcentage_maximal_importation,pourcentage_maximal_importation_nationale;
    liste<int> region;	

};

// -----------------------------------------------------------------------------

struct Couts{  // enregistrement qui contient les couts de productions de chaque énergie
	int cout_thermique, cout_nucleaire, cout_eolien, cout_solaire, cout_hydraulique, cout_bioenergie;
};

// -----------------------------------------------------------------------------

struct Region{ // enregistrement qui contient les valeurs d'une région : l'id, le nom et la liste de ses productions.

    int id;
    string nom;
    liste<Production> valeurs_production = {};

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

// -----------------------------------------------------------------------------

float echanges_nationaux (int echanges_totaux, int production_totale) { // renvoie les echanges physiques nationaux.
	float resultats = (echanges_totaux*100) / production_totale;
	return resultats;
}

// -----------------------------------------------------------------------------

float couts_moyen(Production p_r, Couts cout) { // renvoie le cout de production moyen d'une region 
	float resultats = (cout.cout_thermique * (p_r.thermique.taux_production/100)) + (cout.cout_nucleaire* (p_r.nucleaire.taux_production/100)) + (cout.cout_eolien*(p_r.eolien.taux_production/100)) + (cout.cout_solaire * (p_r.solaire.taux_production/100)) + (cout.cout_hydraulique *  (p_r.hydraulique.taux_production/100)) + (cout.cout_bioenergie *  (p_r.bioenergie.taux_production/100));
	return resultats;
}

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

void insere_region(Production p_r, liste<Production> & r, Tache_de_calcul tache_de_calcul,Couts coute){ // permet l'insertion des régions dans la liste parallele

	if (taille(r) < tache_de_calcul.duree){  		 
		inserer(p_r,r, taille(r)+1);
	}
}

// -----------------------------------------------------------------------------

void insere_region_mono (Production p_r, Region & r, Tache_de_calcul tache_de_calcul){ // on accède à la liste de production direcement depuis la région
		
			if (taille(r.valeurs_production) < tache_de_calcul.duree){				
				inserer(p_r, r.valeurs_production, taille(r.valeurs_production)+1);		
			}	
}
	

// -----------------------------------------------------------------------------

void lire_production (liste<Region> & regions,liste<Production> & parallele, liste<Production> & sequentielle, string fichier,Couts couts,Tache_de_calcul tache_de_calcul){
																									
	/* Paramètres :
	
	regions : c'est la liste contenant les régions. Sert pour la méthode monoregion
	parallele : c'est la liste des productions pour la méthode parallele
	sequentielle : c'est la liste des productions pour la méthode sequentielle
	fichier : c'est le fichier où on lit les productions
	couts : c'est la variable qui contient les couts de productions
	tache_de_calcul : c'est la variable contenant les données pour traiter les régions.
	
	*/

    fstream flux;
    Production production_region;


	const int nombre_regions = taille(tache_de_calcul.region); 	// nombre de régions
    int prod_totale_region = 0; 						 		// la production totale d'une région qui est initialisée à 0
	int region_compteur = 1 ; 							 		// il va s'incrementer à chaque fois qu'on calcul une nouvelle region jusqu'a ce qu'on fasse toutes les régions
    int prod_totale_nation = 0; 						 		// la production tôtale des 12 régions
    int echanges_totaux = 0; 							 		// les échanges physiques totaux des 12 régions
	int cout_marginal = 0; 										// le cout marginal d'une région	
	float cout_moyen = 0; 										//le cout moyen des productions d'une region 
	float importation_nationale = 0; 							// importation nationale 
	
	bool depassement_date = false;
	liste<Production> liste_regions_temp = {}; 			 		// Création d'une liste temportaire qui prendra comme valeurs les Productions qui passent les contraintes

    flux.open(fichier, ios::in); // on met pas de while (flux.good()) pour la première leture car on part du principe que les fichiers ne sont pas corrompus ou autre.
    if (flux.is_open()) {

        flux >>production_region.region;  
	    flux >>production_region.mois; 
	    flux >>production_region.jour; 
	    flux >>production_region.heure;

        flux >>production_region.thermique.production;
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

					inserer(production_region, liste_regions_temp, taille(liste_regions_temp)+1); // une liste temporaire qui va contenir les productions des régions sur 1 heure
					
				}

			}			

			if (region_compteur > taille(regions)){ // la production nationale repasse à 0 quand on a fait le tour de toutes les regions
				
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
							cout_eleve = couts_moyen(ele_temp,couts); // puis cout_eleve prend la valeur du cout moyen de ele, sert au mode séquentiel.
														
						}
						
						insere_region(ele, parallele, tache_de_calcul,couts); // INSERTION PARALLELE

						for (long unsigned int region_id : tache_de_calcul.region){ // on fait une boucle pour pouvoir inserer la production dans la bonne région

								if (ele.region == region_id){
							
								insere_region_mono(ele, regions[region_id], tache_de_calcul); // INSERTION MONOREGION
								}
						}
								
					}

					if (couts_moyen(ele_temp,couts) > 0){

						insere_region(ele_temp,sequentielle, tache_de_calcul,couts); // INSERTION SEQUENTIELLE

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

// -----------------------------------------------------------------------------

Tache_de_calcul lire_tache_de_calcul(string nom_fichier){
	Tache_de_calcul tache_de_calcul;
	fstream flux;
	liste<int> li =  {};
	int region_id;
	
	flux.open(nom_fichier, ios::in); // on met pas de while (flux.good()) car on part du principe que les fichiers ne sont pas corrompus ou autre.
	if (flux.is_open()) {
		flux >> tache_de_calcul.identifiant;  
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
		flux >> region_id;

		while (flux.good()){

			inserer(region_id, li, taille(li)+1);
			flux >> region_id ;
		}
			inserer(region_id, li, taille(li)+1); // on insere une dernière fois pour pas passer la dernière région
			tache_de_calcul.region = li;

		
		flux.close();   
	}
	else {
		cout << "Erreur : impossible d'ouvrir " << nom_fichier << endl;
		}

	return tache_de_calcul;

}

// -----------------------------------------------------------------------------

Couts lire_couts(string fichier){
	Couts couts;
	fstream flux;
	flux.open(fichier, ios::in); // on met pas de while (flux.good()) car on part du principe que les fichiers ne sont pas corrompus ou autre.
	
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

// -----------------------------------------------------------------------------

liste<string> lire_regions_noms(string fichier){

	liste<string> liste_region_nom = {};
	fstream flux;
	string region_nom;
	flux.open(fichier , ios::in);
	if(flux.is_open()){ // on met pas de while (flux.good()) pour le premier element car on part du principe que les fichiers ne sont pas corrompus ou autre.

		flux >> region_nom;

	while(flux.good()){

		inserer(region_nom,liste_region_nom,taille(liste_region_nom)+1);
		flux  >> region_nom;

	}

	flux.close();

	}

	else {
		cout << "Erreur : impossible d'ouvrir " << fichier << endl;
	}

	return liste_region_nom;

}

// -----------------------------------------------------------------------------

void lire_regions (liste<Region> & liste_region,liste<string> regions_noms){ // c'est ici qu'on créé le nombre de régions pour la liste de régions

	int num = 1;

	for (string ele : regions_noms){

        Region temp;				
		temp.nom = ele;				// son nom
		temp.id = num;				// son numéro d'identification
		num ++;
		inserer(temp,liste_region,taille(liste_region)+1);

	}
}

// -----------------------------------------------------------------------------

int afficher_contenu_region(liste<Production> region, int identifiant, Couts couts, string fichier){

	fstream flux;
	flux.open(fichier,ios::out); 
	
	if (flux.is_open()){ // on met pas de while (flux.good()) car on part du principe que les fichiers ne sont pas corrompus ou autre.

	
		switch (identifiant){ 

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

// -----------------------------------------------------------------------------

int afficher_contenu_region_mono (liste<Region> region, int identifiant, Couts couts, string fichier){
	
	fstream flux;
	flux.open(fichier,ios::app); // ios::app permet d'ecrire à la suite du fichier, sans supprimer les données précédentes, indispensable pour le mode monorégion.
	
	if (flux.is_open()){ // on met pas de while (flux.good()) car on part du principe que les fichiers ne sont pas corrompus ou autre.

		flux << region[identifiant].nom << " " << taille(region[identifiant].valeurs_production) << endl; 
		
		if ( taille(region[identifiant].valeurs_production)>= 1){ // protection pour s'assurer qu'aucune liste vide vienne ici, (mais pas obligatoire)
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

// -----------------------------------------------------------------------------

int afficher_regions (liste<Region> liste_r,liste<Production> liste_p, liste<Production> liste_s,Couts couts, int mode_execution, string fichier){ 

	switch (mode_execution){

		case 1:
			afficher_contenu_region(liste_p,0,couts,fichier); // on choisit 0 l'id pour la liste parallele
			break;

		case 2:
			for (int i = 1; i<= taille(liste_r); i++){
				afficher_contenu_region_mono(liste_r,liste_r[i].id,couts,fichier);
			}
			break;

		default:
			afficher_contenu_region(liste_s,-1,couts,fichier); // on choisit -1 l'id pour la liste sequentielle
			break;

	}


	return 0;

}

// -----------------------------------------------------------------------------

int main(int argc , char * argv[]){ // tache_de_calcul couts regions production    ex : tache.txt couts.txt regions.txt t5.ssv


    liste<string> arguments_programme = arguments(argc,argv); // la liste des arguments ecrit depuis le terminal


    liste<Region> les_regions = {};												// liste des régions, sert pour la méthode monoregions
    liste<Production> methode_parallele = {};
    liste<Production> methode_sequentielle = {};
    Tache_de_calcul tache_calcul = lire_tache_de_calcul(arguments_programme[1]);// tache de calcul
    Couts couts_productions = lire_couts(arguments_programme[2]); 
    liste<string> liste_regions = lire_regions_noms(arguments_programme[3]);	// liste des régions
    lire_regions(les_regions,liste_regions);
    ofstream file0("parallele.txt");											//
    ofstream file1("monoregion.txt");											// On ouvre les fichiers pour les reset
    ofstream file2("sequentielle.txt");											//
	liste<string> fichiers_ecriture = {"parallele.txt", "monoregion.txt","sequentielle.txt"}; // liste des fichiers d'écriture
    string fichier_production ;
    for (string ele : arguments_programme){										// le dernier élement de arguments_programme ne marche pas pour je ne sais quelle raison
								
        fichier_production = ele;												// j'ai donc fait ça pour le lire

    }

    cout << "Chargement... Cela peut prendre jusqu'a plusieurs dizaines de secondes..." << endl;

    auto start = high_resolution_clock::now(); // pour lancer le chrono
	
	lire_production(les_regions,methode_parallele,methode_sequentielle,fichier_production,couts_productions,tache_calcul);
	for (int i = 1; i<= taille(fichiers_ecriture); i++){			
		afficher_regions(les_regions,methode_parallele,methode_sequentielle,couts_productions,i,fichiers_ecriture[i]);
	}

    cout << "Fin." << endl ;
    auto stop = high_resolution_clock::now(); // fin du chrono
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << "Temps d'execution : " << duration.count() <<  " millisecondes" << endl;

    return 0;
}