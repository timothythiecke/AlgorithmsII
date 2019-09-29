// GebalanceerdeBinaireZoekbomen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include "zoekboom17.h"

int main()
{
	Zoekboom<int, std::string> tree;
	/*tree.voegtoe(0, "Hallo");
	tree.voegtoe(1, "mijn");
	tree.voegtoe(-1, "naam'");
	tree.voegtoe(2, "is");
	tree.voegtoe(3, "is");
	tree.voegtoe(-100, "Timothy");
	tree.voegtoe(100, "Thiecke");
	tree.voegtoe(1000, "en");
	tree.voegtoe(500, "dit");
	tree.voegtoe(-2, "werkt");*/
	
	// Boom voor linker rotatie
	tree.voegtoe(-2, "l");
	tree.voegtoe(2, "gamma");
	tree.voegtoe(-3, "alpha");
	tree.voegtoe(-1, "beta");
	tree.voegtoe(0, "p");
	tree.voegtoe(3, "zoddo");
	tree.voegtoe(-4, "grimbo");

	// Boom voor rechtse rotatie
	/*tree.voegtoe(0, "l");
	tree.voegtoe(-1, "alpha");
	tree.voegtoe(2, "p");
	tree.voegtoe(1, "beta");
	tree.voegtoe(3, "gamma");*/


	tree.schrijf(std::cout);
	tree.teken("temp.txt");

	std::cout << "Diepte: " << tree.geefDiepte();

	//Zoekboom<int, std::string>& linker_deel_boom = tree->giveChildTree(true);
	
	std::cout << "\n" << tree->data << "\n";

	//linker_deel_boom.rotate(true);
	//tree.rotate(true);
	
	//tree.teken("rotate.txt");

	//tree.maakOnevenwichtig(true);
	tree.maakEvenwichtig();
	tree.teken("onevenwichtig.txt");


	/*if (tree)
	{
		tree = tree->links;
	}*/
}
