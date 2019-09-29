#ifndef __Zoekboom_H
#define __Zoekboom_H
#include <cstdlib>
#include <iostream>
#include <queue>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
//#include <random>

using std::ostream;
using std::ofstream;
using std::ostringstream;
using std::unique_ptr;
using std::cerr;
using std::pair;
using std::string;
using std::vector;
/**********************************************************************

   Class: Zoekboom
   
   beschrijving: Binaire Zoekboom waarin duplicaatsleutels wel of niet zijn toegestaan.
   
***************************************************************************/

template <class Sleutel,class Data>
class zoekKnoop;

template <class Sleutel,class Data>
class Zoekboom : public unique_ptr<zoekKnoop<Sleutel,Data>>{
//....move en copy. Noot: als er geen copy nodig is, zet hem beste op delete.
public:
    void inorder(std::function<void(const zoekKnoop<Sleutel,Data>&)> bezoek) const;
    void schrijf(ostream& os) const;
    void teken(const char * bestandsnaam);
    string tekenrec(ostream& uit,int&knoopteller);

	Zoekboom() {}
	Zoekboom(const unique_ptr<zoekKnoop<Sleutel, Data>>& other) = delete;

	// Move assignment
	Zoekboom(unique_ptr<zoekKnoop<Sleutel, Data>>&& other) : unique_ptr<zoekKnoop<Sleutel, Data>>(std::move(other))
	{	
	}


    //te implementeren
    bool repOK() const;
    int geefDiepte();
	void geefDiepteRec(int diepte, int& grootsteDiepte);

	void rotate(bool isLeftRotation);
	void maakOnevenwichtig(bool isLeftRotation=true); // Gaat een bestaande boom herhalend blijven roteren tot een gelinkte lijst
	void maakEvenwichtig();
	Zoekboom<Sleutel, Data>* maakEvenwichtigRec(vector<Zoekboom<Sleutel, Data>*>& ptrs, int l, int r);

    // geefBoomBovenKnoop: gegeven een knooppointer, wele boom wijst naar de knoop
    // preconditie: knoop moet een naar een geldige knoop wijzen.
Zoekboom<Sleutel, Data>* geefBoomBovenKnoop(zoekKnoop<Sleutel, Data>& knoopptr);
void voegtoe(const Sleutel& sleutel,const Data& data,bool dubbelsToestaan=false);

protected:
//zoekfunctie zoekt sleutel en geeft de boom in waaronder de sleutel zit (eventueel een lege boom als de sleutel
//ontbreekt) en de pointer naar de ouder (als die bestaat, anders nulpointer).
//noot: alhoewel de functie niets verandert aan de boom is ze geen const functie.
    void zoek(const Sleutel& sleutel, zoekKnoop<Sleutel,Data>*& ouder, Zoekboom<Sleutel,Data>*& plaats);
};

template <class Sleutel,class Data>
class zoekKnoop {
    friend class Zoekboom<Sleutel,Data>;
    public:
        zoekKnoop():ouder(0){}
        zoekKnoop(const Sleutel& sl, const Data& d):sleutel{sl},data(d),ouder(0){};
        zoekKnoop(Sleutel&& sl, Data&& d):sleutel{move(sl)},data(move(d)),ouder(0){};
        Zoekboom<Sleutel,Data>& geefKind(bool links);
        Sleutel sleutel;
        Data data;
        zoekKnoop<Sleutel,Data>* ouder;
        Zoekboom<Sleutel,Data> links,rechts;

		// Implementatietip
		// Ik vermoed dat de reference geldig is, maar de unique_ptr kan ook null zijn
		// Is dit nodig? de linker en rechter deelbomen hebben public access
		Zoekboom<Sleutel,Data>& giveChildTree(bool isLeft)
		{
			if (isLeft)
			{
				return links;
			}
			else
			{
				return rechts;
			}
		}
};


/*****************************************************************************

    Implementatie
    
*****************************************************************************/


template <class Sleutel,class Data>
void Zoekboom<Sleutel,Data>::inorder(std::function<void(const zoekKnoop<Sleutel,Data>&)> bezoek) const{
    if (*this){
        (*this)->links.inorder(bezoek);
        bezoek(**this);
        (*this)->rechts.inorder(bezoek);
    };
}

template <class Sleutel,class Data>
void Zoekboom<Sleutel,Data>::schrijf(ostream& os) const{
    inorder([&os](const zoekKnoop<Sleutel,Data>& knoop){
        os<<"("<<knoop.sleutel<<" -> "<<knoop.data<<")";
        os<<"\n  Linkerkind: ";
        if (knoop.links)
            os<<knoop.links->sleutel;
        else
            os<<"-----";
        os<<"\n  Rechterkind: ";
        if (knoop.rechts)
            os<<knoop.rechts->sleutel;
        else
            os<<"-----";
        os<<"\n";
    });
}

template <class Sleutel,class Data>
void Zoekboom<Sleutel,Data>::teken(const char* bestandsnaam){
    ofstream uit(bestandsnaam);
    assert(uit);
    int knoopteller=0;//knopen moeten een eigen nummer krijgen.
    uit<<"digraph {\n";
    this->tekenrec(uit,knoopteller);
    uit<<"}";
}

template<class Sleutel, class Data>
int Zoekboom<Sleutel, Data>::geefDiepte()
{
	// Doet een preorder lookup, de grootste gevonden diepte wordt bijgehouden
	int depth = 1;
	int greatest_depth = 1;

	if (*this) {
		(*this)->links.geefDiepteRec(depth + 1, greatest_depth);
		(*this)->rechts.geefDiepteRec(depth + 1, greatest_depth);
	};

	return greatest_depth;
}

template<class Sleutel, class Data>
void Zoekboom<Sleutel, Data>::geefDiepteRec(int diepte, int& grootsteDiepte)
{
	if (diepte > grootsteDiepte)
	{
		grootsteDiepte = diepte;
	}

	if (*this) {
		(*this)->links.geefDiepteRec(diepte + 1, grootsteDiepte);
		(*this)->rechts.geefDiepteRec(diepte + 1, grootsteDiepte);
	};
}



template<class Sleutel, class Data>
void Zoekboom<Sleutel, Data>::rotate(bool isLeftRotation)
{
	if (*this)
	{
		// Indien de knoop geen kinderen heeft dan kun je niet roteren
		// Indien de knoop twee kinderen heeft dan kun je roteren

		// Je roept de rotate functie op de ouder van de deelboom waar je omwenst te roteren

		// Geef de deelboom op basis van het argument
		Zoekboom<Sleutel, Data> temp = std::move((*this)->giveChildTree(isLeftRotation));
		
		if (temp)
		{
			// Verplaats de deelboom van de deelboom naar de nu ledige deelboom
			(*this)->giveChildTree(isLeftRotation) = std::move(temp->giveChildTree(!isLeftRotation)); // Dit kan enkel als 
			if ((*this)->giveChildTree(isLeftRotation)) // Enkel als het een geldige knoop is
			{
				(*this)->giveChildTree(isLeftRotation).get()->ouder = &(**this); // Fix de ouder pointer
			}
			temp->ouder = (*this)->ouder;
			(*this)->ouder = &*temp;
			temp->giveChildTree(!isLeftRotation) = std::move(*this);
			(*this) = std::move(temp);
		}
	}
}



template<class Sleutel, class Data>
void Zoekboom<Sleutel, Data>::maakOnevenwichtig(bool isLeftRotation)
{
	// Vorm een bestaande boom om naar een gelinkte lijst door middel van rotaties

	// Je kunt dit doen door herhaaldelijk te (n - 1 keer de root te roteren)
	// Blijven roteren tot niks te roteren valt
	Zoekboom<Sleutel, Data>* ptr = this;

	while (ptr != nullptr && *ptr)
	{
		if ((*ptr)->giveChildTree(true) && (*ptr)->giveChildTree(false))
		{
			// Roteer!
			ptr->rotate(isLeftRotation);
		}
		else
		{
			ptr = &(*ptr)->giveChildTree(!isLeftRotation);
		}
	}
}

template<class Sleutel, class Data>
void Zoekboom<Sleutel, Data>::maakEvenwichtig()
{
	bool left_to_right = true;
	
	// First, make the tree as uneven as possible -> transform to linked list	
	this->maakOnevenwichtig(left_to_right);

	// Tel het aantal elementen in de gelinkte lijst en vul op
	// Wat als je een enorm aantal hebt? dan nog komt het overeen, divide and conquer?
	vector<Zoekboom<Sleutel, Data>*> nodes;
	nodes.resize(this->geefDiepte() - 1);

	Zoekboom<Sleutel, Data>* ptr = this;
	int i = 0;
	while (ptr && *ptr && i < nodes.size())
	{
		nodes[i++] = ptr;
		ptr = &(*ptr)->giveChildTree(!left_to_right); // Assumes that the LL goes from left to right
	}
	
	(*this) = std::move(* this->maakEvenwichtigRec(nodes, 0, nodes.size() - 1));
}

template<class Sleutel, class Data>
Zoekboom<Sleutel, Data>* Zoekboom<Sleutel, Data>::maakEvenwichtigRec(vector<Zoekboom<Sleutel, Data>*>& ptrs, int l, int r)
{
	// Stopconditie
	if (l > r)
		return nullptr;

	int middle_index =  (l + r) / 2;

	if (l == r)
		return ptrs[middle];

	Zoekboom<Sleutel, Data>* left = this->maakEvenwichtigRec(ptrs, l, middle_index - 1);
	Zoekboom<Sleutel, Data>* right = this->maakEvenwichtigRec(ptrs, middle_index + 1, r);

	if (left != nullptr)
	{
		(**left).ouder = ptrs[middle_index];
		(*ptrs[middle_index])->giveChildTree(true) = std::move(*left);
	}

	if (right != nullptr)
	{
		(**right).ouder = ptrs[middle_index];
		(*ptrs[middle_index])->giveChildTree(false) = std::move(*right);
	}
		
	return ptrs[middle];
}


template <class Sleutel,class Data>
string Zoekboom<Sleutel,Data>::tekenrec(ostream& uit,int&knoopteller ){
    ostringstream wortelstring;
    wortelstring<<'"'<<++knoopteller<<'"';
    if (!*this){
        uit<<wortelstring.str()<<" [shape=point];\n";
    }
    else{
        uit<<wortelstring.str()<<"[label=\""<<(*this)->sleutel<<":"<<(*this)->data<<"\"]";
        uit<<";\n";
        string linkskind=(*this)->links.tekenrec(uit,knoopteller);
        string rechtskind=(*this)->rechts.tekenrec(uit,knoopteller);
        uit<<wortelstring.str()<<" -> "<<linkskind<<";\n";
        uit<<wortelstring.str()<<" -> "<<rechtskind<<";\n";
    };
    return wortelstring.str();
}

template <class Sleutel,class Data>
Zoekboom<Sleutel, Data>* Zoekboom<Sleutel,Data>::geefBoomBovenKnoop(zoekKnoop<Sleutel, Data>& knoop){
    if (knoop.ouder == nullptr)
        return this;
    else
        if (knoop.ouder->links.get() == &knoop)
            return &(knoop.ouder->links);
        else
            return &(knoop.ouder->rechts);
}

template <class Sleutel,class Data>
void Zoekboom<Sleutel,Data>::voegtoe(const Sleutel& sleutel,const Data& data,bool dubbelsToestaan){
    zoekKnoop<Sleutel, Data>* ouder;
    Zoekboom<Sleutel, Data>* plaats;
    Zoekboom<Sleutel, Data>::zoek(sleutel,ouder,plaats);
    if (dubbelsToestaan)
        while (*plaats)
            (*plaats)->geefKind(rand()%2).zoek(sleutel,ouder,plaats);
    if (!*plaats){
        Zoekboom<Sleutel, Data> nieuw=
                std::make_unique<zoekKnoop<Sleutel, Data> >(sleutel,data); // copy constructor, waarbij een unique_ptr<zoekKnoop<Sleutel, Data>> als arugment wordt gegeven, of copy constructor?
        nieuw->ouder=ouder;
        *plaats=move(nieuw);
    }
}


template <class Sleutel,class Data>
void Zoekboom<Sleutel,Data>::zoek(const Sleutel& sleutel, zoekKnoop<Sleutel,Data>*& ouder, Zoekboom<Sleutel,Data>*& plaats){
    plaats=this;
    ouder=0;
    while (*plaats && (*plaats)->sleutel !=sleutel){
        ouder=plaats->get();
        if ((*plaats)->sleutel < sleutel)
            plaats=&(*plaats)->rechts;
        else
            plaats=&(*plaats)->links;
    };
};

template <class Sleutel,class Data>
Zoekboom<Sleutel,Data>& zoekKnoop<Sleutel,Data>:: geefKind(bool linkerkind){
    if (linkerkind)
        return links;
    else
        return rechts;
};


#endif
