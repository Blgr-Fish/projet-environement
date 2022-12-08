#pragma once
/**
 * Bibliothèque définissant un type liste générique doté de deux interfaces :
 * 1) une interface procédurale :
 * - taille (nombre d'éléments)
 * - accès (lecture/écriture) à un élément par son indice (entre 1 et taille)
 * - insertion d'un nouvel élément à un indice donné
 * - suppression d'un élément d'indice donné
 * - affichage sur un flux de sortie
 * 2) une interface fonctionnelle :
 * - nil, constante de liste vide **du bon type**
 * - construction d'une nouvelle liste par collation d'un élément et d'une liste
 * - accès à la tête de la liste
 * - accès au reste de la liste
 */
#include <list>     // pour le type standard std::list et ses opérations
#include <iostream> // pour le type std::ostream et le flux d'erreur std::cerr
#include <cassert> // pour std::terminate()

//----------------------------------------------------------
// Définition de la classe liste<T>
template < typename T >
class liste : protected std::list<T>
{
    public :

        //----------------------------------------------------------
        // Rôle : construire une liste vide
        // Entrée : -
        // Sortie : la nouvelle liste
        // Précondition : aucune
        liste(){}

        //----------------------------------------------------------
        // Rôle : construire une liste à partir d'une séquence
        // Entrée : ilt la séquence constante de la forme {t1, t2, ...}
        // Sortie : la nouvelle liste
        // Précondition : aucune
        liste(std::initializer_list<T> ilt) : std::list<T>(ilt.begin(),ilt.end()) {}



        //----------------------------------------------------------
        // Rôle : accéder (en lecture/écriture) à un élément
        // Entrée : idx l'indice de l'élément souhaité (size_t)
        // Sortie : un élément de type T
        // Précondition : 1 ≤ idx ≤ taille
        T & operator[] ( std::size_t idx )
        {
            std::size_t taille = this->size();
            assert(1<=idx); assert(idx<=taille);
            auto it = this->begin();
            while ( idx > 1 ) { it++ ; idx-- ; }
            return *it;
        }

        //----------------------------------------------------------
        // Rôle : accéder (en lecture seule) à un élément (d'une liste constante)
        // Entrée : idx l'indice de l'élément souhaité (size_t)
        // Sortie : un élément de type T
        // Précondition : 1 ≤ idx ≤ taille
        const T & operator[] ( std::size_t idx ) const
        {
            std::size_t taille = this->size();
            assert(1<=idx); assert(idx<=taille);
            auto it = this->begin();
            while ( idx > 1 ) { it++ ; idx-- ; }
            return *it;
        }

    ///==========================================================
    /// Déclaration de fonctions/procédures amies pour définir une
    /// interface non-orientée objet sur les listes

        //----------------------------------------------------------
        // Rôle : donner la taille d'une liste
        // Entrée : l_t la liste à mesurer
        // Sortie : un entier positif ou nul (size_t)
        // Précondition : aucune
        [[nodiscard]] friend std::size_t taille (const liste<T> & l_t)
        {
            return l_t.size();
        }

        //----------------------------------------------------------
        // Rôle : insérer un élément à une position donnée
        // Entrées :
        //     - l_t la liste où insérer
        //     - idx l'indice d'insertion (size_t)
        //     - val l'élément à insérer (T)
        // Sortie : la liste modifiée
        // Précondition : 1 ≤ idx ≤ taille()+1
        friend void inserer( const T & val, liste<T> & l_t, std::size_t idx)
        {
            std::size_t taille = l_t.size();
            assert(1<=idx); assert(idx<=taille+1);
            auto it = l_t.begin();
            while ( idx > 1 ) { it++; idx--; }
            l_t.insert(it,val);
        }

        //----------------------------------------------------------
        // Rôle : supprimer l'élément à une position donnée
        // Entrée :
        //     - l_t la liste où supprimer
        //     - idx l'indice de suppression (size_t)
        // Sortie : la liste modifiée
        // Précondition : 1 ≤ idx ≤ taille()
        friend void supprimer( liste<T> & l_t, std::size_t idx )
        {
            std::size_t taille = l_t.size();
            assert(1<=idx); assert(idx<=taille);
            auto it = l_t.begin();
            while ( idx > 1 ) { it++; idx--; }
            l_t.erase(it);
        }

    ///==========================================================
    /// Déclaration d'itérateurs pour la répétitive for-each

        //----------------------------------------------------------
        // Rôle : retourner l'itérateur sur le début de la liste interne
        // Entrée : aucune
        // Sortie : l'itérateur en début de liste
        // Précondition : aucune
        typename std::list<T>::iterator begin()
        {
            return dynamic_cast<std::list<T>*>(this)->begin();
        }

        //----------------------------------------------------------
        // Rôle : retourner l'itérateur sur le début de la liste interne
        // Entrée : aucune
        // Sortie : l'itérateur en début de liste
        // Précondition : aucune
        typename std::list<T>::const_iterator begin() const
        {
            return dynamic_cast<const std::list<T>*>(this)->begin();
        }

        //----------------------------------------------------------
        // Rôle : retourner l'itérateur sur la fin de la liste interne
        // Entrée : aucune
        // Sortie : l'itérateur en fin de liste
        // Précondition : aucune
        typename std::list<T>::iterator end()
        {
            return dynamic_cast<std::list<T>*>(this)->end();
        }

        //----------------------------------------------------------
        // Rôle : retourner l'itérateur sur la fin de la liste interne
        // Entrée : aucune
        // Sortie : l'itérateur en fin de liste
        // Précondition : aucune
        typename std::list<T>::const_iterator end() const
        {
            return dynamic_cast<const std::list<T>*>(this)->end();
        }

    ///==========================================================
    /// Déclaration de fonctions/procédures amies pour définir une
    /// interface fonctionnelle sur les listes

        //----------------------------------------------------------
        // Constante représentant une liste vide **du bon type**
        static const liste<T> VIDE;

        //----------------------------------------------------------
        // Rôle : construction d'une nouvelle liste par ajout en tête
        // Entrée :
        //     - val valeur en tête
        //     - l_t reste de la liste
        // Sortie : une nouvelle liste commençant par val suivi de l_t
        // Précondition : aucune
        [[nodiscard]] friend liste<T> cons( const T & val, const liste<T> & l_t )
        {
            liste<T> l = l_t;
            inserer(val,l,1);
            return l;
        }

        //----------------------------------------------------------
        // Rôle : accès à l'élément de tête
        // Entrée : l_t la liste considérée
        // Sortie : l'élément de tête
        // Précondition : liste non vide
        [[nodiscard]] friend T tete( const liste<T> & l_t )
        {
            std::size_t taille = l_t.size();
            assert(1<=taille);
            return l_t[1];
        }

        //----------------------------------------------------------
        // Rôle : accès au reste
        // Entrée : l_t la liste considérée
        // Sortie : la liste privée de son premier élément
        // Précondition : liste non vide
        [[nodiscard]] friend liste<T> reste( const liste<T> & l_t )
        {
            std::size_t taille = l_t.size();
            assert(1<=taille);
            liste<T> l = l_t;
            supprimer(l,1);
            return l;
        }

        //----------------------------------------------------------
        // Rôle : transformer les paramètres transmis au main en une liste de string
        friend liste<std::string> arguments(int argc, char* argv []);

}; // fin de la définition du type liste<T>

//----------------------------------------------------------
// Initialisation de la constante représentant une liste vide **du bon type**
template < typename T >
const liste<T> liste<T>::VIDE = liste<T>();
#define VIDE(T) liste<T>::VIDE

//---------------------------------------------------------
// Rôle : transformer les paramètres transmis au main en une liste de string
// la fonction main doit utiliser la signature
// int main(int argc, char* argv [])
// Entrées :
//      - argc le nombre de paramètres
//      - argv tableau contenant les paramètres
// Sortie : liste des paramètres
// Précondition : argc >=0  et  argv.length == argc

[[nodiscard]]  liste<std::string> arguments(int argc, char* argv []){
    liste<std::string> res;
    for (; argc>1; argc--){
        res.insert(res.begin(),argv[argc-1]);
    }
    return res;
}
