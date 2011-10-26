#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <SDL/SDL.h>

// ----------------- Settings --------------- //
#define NOMBRECASE 10
#define TAILLECASE 15 // les cases feront 15x15 px
#define HAUTEUR (NOMBRECASE * TAILLECASE + 110)
#define LARGEUR (NOMBRECASE * TAILLECASE + 20)
#define NOMBREMINE 20
#define MIN 0
#define MAX 9

SDL_Surface *ecran, *caseNormale, *caseVide, *mine, *mineRouge, *flag, *interro;
SDL_Surface *caseUne, *caseDeux, *caseTrois, *caseQuattre, *caseCinq, *caseSix, *caseSept, *caseHuit;
SDL_Surface *sJeu, *sClick, *sPerdu, *sGagner;

typedef struct carte carte;
struct carte
{
        int mine;
        int etat;
        int mprox;
        SDL_Surface surface;
        SDL_Rect position;
};

enum {NOMINE, MINE};
enum {FREE, FLAG, INTERRO, CHECK};
// ---------------- **Settings** -------------- //

// ---------------- Fonctions ----------------- //
void autoCompleter (carte cases[][NOMBRECASE], short x, short y)
{
    SDL_Rect positionClique;


    positionClique.x = x*TAILLECASE+10;
    positionClique.y = y*TAILLECASE+100;
    if (cases[x][y].etat == FREE && cases[x][y].mine == NOMINE && cases[x][y].mprox == 0)
    {
        cases[x][y].etat = CHECK;
        cases[x][y].surface = *caseVide;
        /* Si la ligne est supérieure à 0. On peut regarder la ligne au-dessus */
        if (x != 0)
        {
            autoCompleter (cases, x-1, y);
            /* Si la colonne est supérieure à 0, on peut regarder les colonnes inférieures */
            if (y != 0)
                autoCompleter (cases, x-1, y-1);
            /* Si la colonne n'est pas la dernière, on peut regarder les colonnes supérieures */
            if (y < NOMBRECASE-1)
                autoCompleter (cases, x-1, y+1);
        }

        /* Si la ligne est inférieure au nombre de lignes -1, on peut consulter la ligne suivante. */
        if (x < NOMBRECASE-1)
        {
            autoCompleter (cases, x+1, y);

            if (y != 0)
                autoCompleter (cases, x+1, y-1);

            if (y < NOMBRECASE-1)
                autoCompleter (cases, x+1, y+1);
        }
        /* Idem sur la ligne courante */
        if (y != 0)
            autoCompleter (cases, x, y-1);

        if (y < NOMBRECASE-1)
            autoCompleter (cases, x, y+1);
    } else if (!(cases[x][y].etat == FLAG && cases[x][y].etat == INTERRO)) {
        cases[x][y].etat = CHECK;
        if(cases[x][y].mprox == 1)
            cases[x][y].surface = *caseUne;
        else if(cases[x][y].mprox == 2)
            cases[x][y].surface = *caseDeux;
        else if(cases[x][y].mprox == 3)
            cases[x][y].surface = *caseTrois;
        else if(cases[x][y].mprox == 4)
            cases[x][y].surface = *caseQuattre;
        else if(cases[x][y].mprox == 5)
            cases[x][y].surface = *caseCinq;
        else if(cases[x][y].mprox == 6)
            cases[x][y].surface = *caseSix;
        else if(cases[x][y].mprox == 7)
            cases[x][y].surface = *caseSept;
        else if(cases[x][y].mprox == 8)
            cases[x][y].surface = *caseHuit;
    }
    SDL_BlitSurface(&cases[x][y].surface, NULL, ecran, &positionClique);
}

// -------------- **Fonctions** --------------- //


int main(int argc, char *argv[])
{
        // ------------------------ Initialisation ------------------------------ //
        SDL_Init(SDL_INIT_VIDEO);
        SDL_WM_SetCaption("Demineur", NULL);

                // --------------------- Variables et affectation ----------------------- //
                ecran = SDL_SetVideoMode(LARGEUR, HAUTEUR, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
                SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 225, 225, 225));

                caseNormale = SDL_LoadBMP("case.bmp");

                caseVide = SDL_LoadBMP("0.bmp");

                caseUne = SDL_LoadBMP("1.bmp");

                caseDeux = SDL_LoadBMP("2.bmp");

                caseTrois = SDL_LoadBMP("3.bmp");

                caseQuattre = SDL_LoadBMP("4.bmp");

                caseCinq = SDL_LoadBMP("5.bmp");

                caseSix = SDL_LoadBMP("6.bmp");

                caseSept = SDL_LoadBMP("7.bmp");

                caseHuit = SDL_LoadBMP("8.bmp");

                mine = SDL_LoadBMP("mine.bmp");

                mineRouge = SDL_LoadBMP("mine_rouge.bmp"); // la mine sur laquelle on clique

                flag = SDL_LoadBMP("drapeau.bmp"); //le drapeau signalant une mine

                interro = SDL_LoadBMP("interro.bmp"); // le point d'interrogation
                sJeu = SDL_LoadBMP("smileycontent.bmp");
                sClick = SDL_LoadBMP("smileyclique.bmp");
                sPerdu = SDL_LoadBMP("smileyperdu.bmp");
                sGagner = SDL_LoadBMP("smileygagner.bmp");


                carte cases[NOMBRECASE][NOMBRECASE]; // la "carte" du démineur


                // ----------------------- **variables et affectation** --------------------- //

                // ------------------- Les cases ------------------ //
                int x, y;

                for(x=0; x<NOMBRECASE; x++)
                {
                        for(y=0; y<NOMBRECASE; y++)
                        {
                                cases[x][y].mine = NOMINE;
                                cases[x][y].etat = FREE;
                                cases[x][y].mprox = 0;
                                cases[x][y].position.x = x * TAILLECASE + 10;
                                cases[x][y].position.y = y * TAILLECASE + 100;
                                cases[x][y].surface = *caseNormale;
                                SDL_BlitSurface(&cases[x][y].surface, NULL, ecran, &cases[x][y].position);
                        }
                }

                SDL_Flip(ecran);
                // ------------------- **les cases** ------------------ //

        // ----------------------------------- **Initialisation** ------------------------------- //


        // ------------------------------------- Les mines ------------------------------------- //

        SDL_Rect positionMine[NOMBREMINE];



        srand(time(NULL)); // on initialyse l'alea


        int i = 0;
        int j = 0;
        long nombreAleatoireX, nombreAleatoireY;

        while(i<NOMBREMINE)
        {
                nombreAleatoireX = (rand() % (MAX - MIN + 1)) + MIN;
                nombreAleatoireY = (rand() % (MAX - MIN + 1)) + MIN;
                if(cases[nombreAleatoireX][nombreAleatoireY].mine == MINE)
                        continue;
                else
                {
                        cases[nombreAleatoireX][nombreAleatoireY].mine = MINE;
                        positionMine[i].x = nombreAleatoireX * TAILLECASE;
                        positionMine[i].y = nombreAleatoireY * TAILLECASE;
                        i++;
                }
        }

        SDL_Flip(ecran);
        // ----------------------------------- **Les mines** -----------------------------------//

                /* Algorithme calculant le nombre de drapeaux à extrémité pour chaque case (excepté les mines) */
                for (x=0; x<NOMBRECASE; x++)
                {
                    for (y=0; y<NOMBRECASE; y++)
                    {
                        if (cases[x][y].mine == NOMINE)
                        {
                            /* On vérifie la ligne précédente si le numéro de ligne (x) est différent de 0 */
                            if (x != 0)
                            {
                                if (cases[x-1][y].mine == MINE)
                                    cases[x][y].mprox++;
                                if (y != 0)
                                {
                                    if (cases[x-1][y-1].mine == MINE)
                                        cases[x][y].mprox++;
                                }
                                if (y < NOMBRECASE-1)
                                {
                                    if (cases[x-1][y+1].mine == MINE)
                                        cases[x][y].mprox++;
                                }
                            }
                            /* On vérifie la ligne suivante si le numéro de ligne (x) est inférieur au nombre de lignes -1 */
                            if (x < NOMBRECASE-1)
                            {
                                if (cases[x+1][y].mine == MINE)
                                    cases[x][y].mprox++;
                                if (y != 0)
                                {
                                    if (cases[x+1][y-1].mine == MINE)
                                        cases[x][y].mprox++;
                                }
                                if (y < NOMBRECASE-1)
                                {
                                    if (cases[x+1][y+1].mine == MINE)
                                        cases[x][y].mprox++;
                                }
                            }
                            /* On vérifie la colonne précédente si le numéro de colonne (y) est différent de 0 */
                            if (y != 0)
                            {
                                if (cases[x][y-1].mine == MINE)
                                    cases[x][y].mprox++;
                            }
                            /* On vérifie la colonne suivante si le numéro de colonne (y) est inférieur au nombre de colonnes -1 */
                            if (y < NOMBRECASE-1)
                            {
                                if (cases[x][y+1].mine == MINE)
                                    cases[x][y].mprox++;
                            }
                        }
                    }
                }

        // --------------------------------- boucle principale ---------------------------------- //

        SDL_Event event;
        int bouclePrincipale = 1;
        SDL_Rect positionClique, positionSmiley;

        positionSmiley.x = 73;
        positionSmiley.y = 60;

        while(bouclePrincipale)
        {
                SDL_BlitSurface(sJeu, NULL, ecran, &positionSmiley);
                SDL_Flip(ecran);

                SDL_WaitEvent(&event);

                x = event.button.x - 10;
                x = x / TAILLECASE;
                y = event.button.y - 100;
                y = y / TAILLECASE;
                positionClique.x = x * TAILLECASE + 10;
                positionClique.y = y * TAILLECASE + 100;

                switch(event.type)
                {
                        case SDL_QUIT:
                                bouclePrincipale = 0;
                        break;
                        case SDL_KEYDOWN:
                                switch(event.key.keysym.sym)
                                {
                                        case SDLK_ESCAPE:
                                                bouclePrincipale = 0;
                                                break;
                                        default:
                                                break;
                                }
                        break;
                        case SDL_MOUSEBUTTONUP:
                                if(event.button.button == SDL_BUTTON_LEFT)
                                {
                                        SDL_BlitSurface(sClick, NULL, ecran, &positionSmiley);
                                        SDL_Flip(ecran);
                                        Sleep(100);
                                        if(cases[x][y].etat != CHECK && cases[x][y].etat != FLAG)
                                        {
                                            if(cases[x][y].mine == MINE)
                                            {
                                                    cases[x][y].surface = *mineRouge;
                                                    SDL_BlitSurface(&cases[x][y].surface, NULL, ecran, &positionClique);
                                                    SDL_BlitSurface(sPerdu, NULL, ecran, &positionSmiley);
                                                    for(i=0; i<NOMBRECASE; i++)
                                                    {
                                                            for(j=0; j<NOMBRECASE; j++)
                                                            {
                                                                    if (!(x==i && y==j))
                                                                    {
                                                                        if(cases[i][j].mine == MINE)
                                                                        {
                                                                            cases[i][j].surface = *mine;
                                                                            positionClique.x = i * TAILLECASE + 10;
                                                                            positionClique.y = j * TAILLECASE + 100;
                                                                            SDL_BlitSurface(&cases[i][j].surface, NULL, ecran, &positionClique);
                                                                        }
                                                                    }
                                                            }
                                                    }
                                                    bouclePrincipale=0;
                                                    // penser a afficher toutes les mines lorsque l'on clique sur une
                                            }
                                            else if(cases[x][y].mine == NOMINE)
                                            {
                                                    autoCompleter (cases, x, y);
                                            }
                                        }
                                        else if(cases[x][y].etat == CHECK)
                                        {

                                        }
                                }
                        break;
                        case SDL_MOUSEBUTTONDOWN:
                                if(event.button.button == SDL_BUTTON_RIGHT)
                                {
                                        switch(cases[x][y].etat)
                                        {
                                                case FREE:
                                                        cases[x][y].etat = INTERRO;
                                                        SDL_BlitSurface(interro, NULL, ecran, &positionClique);
                                                break;
                                                case INTERRO:
                                                        cases[x][y].etat = FLAG;
                                                        SDL_BlitSurface(flag, NULL, ecran, &positionClique);
                                                break;
                                                case FLAG:
                                                        cases[x][y].etat = FREE;
                                                        SDL_BlitSurface(caseNormale, NULL, ecran, &positionClique);
                                                break;
                                        }

                                }
                        break;
//                        case SDL_MOUSEMOTION: // =========== Ne marche pas
//                                if(event.button.button == SDL_BUTTON_LEFT && cases[x][y].etat == FREE)
//                                {
//                                        SDL_BlitSurface(caseVide, NULL, ecran, &positionSurvolee);
//                                        SDL_Flip(ecran);
//                                        for(x=0; x<NOMBRECASE; x++)
//                                        {
//                                                for(y=0; y<NOMBRECASE; y++)
//                                                {
//                                                        SDL_BlitSurface(&cases[x][y].surface, NULL, ecran, &cases[x][y].position);
//                                                }
//                                        }
//                                }
//
//                        break;

                        default:
                        break;
                }
        SDL_Flip(ecran);
        }

        // -------------------------- **Boucle principale**------------------------------------- //

        // --------------------------- Phase de Nettoyage ---------------------------------- //
        while(event.type != SDL_QUIT)
        {
            SDL_WaitEvent(&event);
        }

        SDL_Quit();
        return EXIT_SUCCESS;
        // ------------------------------- **phase de nettoyage** ------------------------ //
}
