#include <SFML/Graphics.hpp>
#include <map>
#include <list>
#include <cmath>
#include <iostream>

struct noeud
{
    float cout_g, cout_h, cout_f;
    std::pair<int, int> parent;
};

struct point
{
    int x, y;
};

typedef std::map<std::pair<int, int>, noeud> l_noeud;

sf::Image laby;
l_noeud openList;
l_noeud closeList;
point arrivee;
noeud depart;
std::list<point> chemin;

float distance(int x1, int x2, int y1, int y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

bool inList(std::pair<int, int> n, l_noeud& l)
{
    l_noeud::iterator i = l.find(n);
    if(i == l.end())
        return false;
    else
        return true;
}

void addNoeud(std::pair<int, int>& n)
{
    noeud tmp;
    for(int i = n.first - 1; i <= n.first + 1; ++i)
    {
        if((i < 0) || (i >= (int)laby.getWidth()))
            continue;
        for(int j = n.second - 1; j <= n.second + 1; ++j)
        {
            if((j < 0) || (j >= (int)laby.getHeight()))
                continue;
            if((i == n.first) && (j == n.second))
                continue;
            if((i != n.first) && (j != n.second))
                continue;
            if(laby.getPixel(i, j) == sf::Color::Black)
                continue;

            std::pair<int, int> it(i, j);

            if(!inList(it, closeList))
            {
                tmp.cout_g = closeList[n].cout_g + distance(i, j, n.first, n.second);
                tmp.cout_h = distance(i, j, arrivee.x, arrivee.y);
                tmp.cout_f = tmp.cout_g + tmp.cout_h;
                tmp.parent = n;

                if(inList(it, openList))
                {
                    if(tmp.cout_f < openList[it].cout_f)
                        openList[it] = tmp;
                }
                else
                    openList[std::pair<int, int>(i, j)] = tmp;
            }
        }
    }
}

std::pair<int, int> bestNoeud(l_noeud& l)
{
    float m_coutf = l.begin()->second.cout_f;
    std::pair<int, int> m_noeud = l.begin()->first;

    for(l_noeud::iterator i = l.begin(); i!=l.end(); ++i)
    {
        if(i->second.cout_f < m_coutf)
        {
            m_coutf = i->second.cout_f;
            m_noeud = i->first;
        }
    }

    return m_noeud;
}

void addCloseList(std::pair<int, int>& p)
{
    noeud& n = openList[p];
    closeList[p] = n;

    if(openList.erase(p) == 0)
        std::cerr << "Erreur, impossible de supprimer de la liste" << std::endl;

    return;
}

void searchChemin()
{
    noeud& tmp = closeList[std::pair<int, int>(arrivee.x, arrivee.y)];

    point n;
    std::pair<int, int> prec;
    n.x = arrivee.x;
    n.y = arrivee.y;
    prec.first = tmp.parent.first;
    prec.second = tmp.parent.second;
    chemin.push_front(n);

    while(prec != std::pair<int, int>(depart.parent.first, depart.parent.second))
    {
        n.x = prec.first;
        n.y = prec.second;
        chemin.push_front(n);

        tmp = closeList[tmp.parent];
        prec.first = tmp.parent.first;
        prec.second = tmp.parent.second;
    }
    n.x = prec.first;
    n.y = prec.second;
    chemin.push_front(n);
}

void writeChemin()
{
    std::string fichier;
    std::list<point>::iterator j;

    for(j = chemin.begin(); j != chemin.end(); ++j)
        laby.setPixel(j->x, j->y, sf::Color::Blue);

    std::cout << "Chemin finale de longueur: " << chemin.size() << std::endl;

    std::cout << "Enregistrer sous : ";
    std::cin >> fichier;
    laby.saveToFile(fichier);
}

point checkArrivee()
{
    point fin;
    for(unsigned int i = 0; i < laby.getWidth(); ++i)
    {
        for(unsigned int j = 0; j < laby.getHeight(); ++j)
        {
            if(laby.getPixel(i, j) == sf::Color::Red)
            {
                fin.x = i;
                fin.y = j;
                return fin;
            }
        }
    }
    fin.x = laby.getWidth() - 1;
    fin.y = laby.getHeight() - 1;
    return fin;
}

std::pair<int, int> checkDepart()
{
    std::pair<int, int> fin;
    for(unsigned int i = 0; i < laby.getWidth(); ++i)
    {
        for(unsigned int j = 0; j < laby.getHeight(); ++j)
        {
            if(laby.getPixel(i, j) == sf::Color::Green)
            {
                fin.first = i;
                fin.second = j;
                return fin;
            }
        }
    }
    fin.first = 0;
    fin.second = 0;
    return fin;
}

int main()
{
    std::string fichier;

    std::cout << "Nom du fichier a charger : ";
    std::cin >> fichier;
    if(!laby.loadFromFile(fichier))
        return 1;

    arrivee = checkArrivee();

    depart.parent = checkDepart();

    std::cout << "Depart > " << depart.parent.first << " : " << depart.parent.second << std::endl;
    std::cout << "Arrivee > " << arrivee.x << " : " << arrivee.y << std::endl;

    std::pair<int, int> courant;

    courant = depart.parent;

    openList[courant] = depart;
    addCloseList(courant);
    addNoeud(courant);

    while(!((courant.first == arrivee.x) && (courant.second == arrivee.y)) && !(openList.empty()))
    {
        courant = bestNoeud(openList);
        addCloseList(courant);
        addNoeud(courant);
    }

    if((courant.first == arrivee.x) && (courant.second == arrivee.y))
    {
        std::cout << "Solution trouvee" << std::endl;
        searchChemin();
        writeChemin();
    }
    else
    {
        std::cout << "Pas de solution trouvee" << std::endl;
    }

    std::string quitter;

    std::cout << "Entrez q ou Q pour quitter : ";
    do
    {
        std::cin >> quitter;
    }
    while (quitter != "q" && quitter != "Q");

    return 0;
}
