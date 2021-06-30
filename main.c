#include <stdlib.h>
#include <ncurses.h>

#include "Librerias/Interfaz/interfaz.h"
#include "Librerias/TDA_Mapa/hashmap.h"
#include "Librerias/TDA_Lista/list.h"
#include "Librerias/personajes.h"
#include "Librerias/logros.h"
#include "Librerias/items.h"
#include "Librerias/enemigos.h"

/* PROTOTIPADO DE FUNCIONES */

//Funcion que delimita el tamaño del menu a usar
WINDOW * crearVentana(int cantOpciones);

//Funcion para mostrar el menu principal
void mostrarMenu(HashMap * mapaPersonajes, HashMap * mapaLogros, HashMap * mapaItems, HashMap * mapaEnemigos, List *);

//Funcion para mostrar el subMenu de funciones
void mostrarSubMenu(HashMap * mapaPersonajes, HashMap * mapaLogros, HashMap * mapaItems, HashMap * mapaEnemigos, List *);

//Funcion que inicia las funciones principales del programa
void funcionesOpcion(int opcion,HashMap * mapaPersonajes, HashMap * mapaLogros, HashMap * mapaItems, HashMap * mapaEnemigos, List *);

int main()
{
	pantallaInicial();
	//Iniciar la libreria ncurses.h
	initscr();

	iniciarColores();

	//Creacion de Mapas
	HashMap * mapaPersonajes = createMap(10);
	HashMap * mapaLogros = createMap(576);
	HashMap * mapaItems = createMap(600);
	HashMap * mapaEnemigos = createMap(410);
	List * listaPersonajes = createList();
	
	//Importacion de Informacion
	importarArchivoPersonajes(mapaPersonajes, listaPersonajes);
	importarArchivoItems(mapaItems);
	importarArchivoLogros(mapaLogros);
	importarArchivoEnemigos(mapaEnemigos);
	
	mostrarMenu(mapaPersonajes, mapaLogros, mapaItems, mapaEnemigos, listaPersonajes);

	//Libera memoria
	free(mapaPersonajes);
	free(mapaLogros);
	free(mapaItems);
	free(mapaEnemigos);
	free(listaPersonajes);
	
	clear();
	attron(COLOR_PAIR(4));
	printw("FIN DEL PROGRAMA\n");
	attroff(COLOR_PAIR(4));
	getch();

	//Fin de la libreria ncurses.h
	endwin();
	return 0;
}

WINDOW * crearVentana(int cantOpciones)
{
	int xMax, yMax;
	clear();

	//Conseguir el tamaño actual de la terminal
    getmaxyx(stdscr, yMax, xMax);

	//Dependiendo del tamaño de la terminal, ajustarlo para nuestro uso
    WINDOW * ventana = newwin(cantOpciones, xMax/2, yMax/4, xMax/4);

    //Hacer que funcionen las teclas de flecha
    keypad(ventana, TRUE);
	return ventana;
}


void mostrarMenu(HashMap * mapaPersonajes, HashMap * mapaLogros, HashMap * mapaItems, HashMap * mapaEnemigos, List * listaPersonajes)
{
    char opciones[10][40] = {"Menu de Desbloqueo","Guardar Informacion","Buscar un Item Especifico","Buscar un Logro Especifico","Buscar un Enemigo Especifico","Mostrar Todos los Personajes","Mostrar Todos los Items","Mostrar Todos los Logros","Mostrar Todos los Enemigos","Salir del Programa"};
    int eleccion = -1, iluminar = 0;

    WINDOW * ventana = crearVentana(12);
    
    while(eleccion)
    {
        //Delimitar el menu
        box(ventana, 0, 0);
        refresh();
        //Muestra las opciones
        for(int i = 0; i < 10; i++)
        {
        	//Si es la posicion actual, la ilumina
            if(i == iluminar) wattron(ventana, A_REVERSE);

            //Si no, la muestra como texto plano
            mvwprintw(ventana, i+1, 1, opciones[i]);
            wattroff(ventana, A_REVERSE);
        }

        //El usuario ingresa alguna entrada desde el teclado (sin necesidad de usar enter)
        eleccion = wgetch(ventana);

        switch (eleccion) //Dependiendo de la elección
        {
        case KEY_UP:
            iluminar--;
            if(iluminar == -1) iluminar = 9; //Si llega hasta arriba, mueve el cursor al final del menu
            break;
        case KEY_DOWN:
            iluminar++;
            if(iluminar == 10) iluminar = 0; //Si llega al final mueve el cursor al inicio del menu
            break;
		case 10: //Si se apreta la tecla ENTER, significa que se quiere usar una opcion del menu
			if(iluminar == 9) return;
			funcionesOpcion(iluminar, mapaPersonajes, mapaLogros, mapaItems, mapaEnemigos, listaPersonajes);
            break;
        }

    }
}

void mostrarSubMenu(HashMap * mapaPersonajes, HashMap * mapaLogros, HashMap * mapaItems, HashMap * mapaEnemigos, List * listaPersonajes)
{
	//Opciones dentro de este submenú
	char opciones[6][50] = {"Desbloquear Personaje","Avance de Marcas de Logros","Encontrar Item","Desbloquear Logro","Encontrar Enemigo","Salir del Menu"};
    int eleccion = -1, iluminar = 0;

	//Creación de ventana del menú
    WINDOW * ventana = crearVentana(8);
    
    while(eleccion)
    {
        //Delimitar el menu
        box(ventana, 0, 0);
        refresh();

        for(int i = 0; i < 6; i++)
        {
            if(i == iluminar) wattron(ventana, A_REVERSE);
            mvwprintw(ventana, i+1, 1, opciones[i]);
            wattroff(ventana, A_REVERSE);
        }

		//Ingreso de las teclas
        eleccion = wgetch(ventana);
        switch (eleccion)
        {
        case KEY_UP:
            iluminar--;
            if(iluminar == -1) iluminar = 5; //Si llega hasta arriba, mueve el cursor al final del menu
            break;
        case KEY_DOWN:
            iluminar++;
            if(iluminar == 6) iluminar = 0; //Si llega al final mueve el cursor al inicio del menu
            break;
 		case 10: //Si se apreta la tecla ENTER, significa que se quiere usar una opcion del menu
			if(iluminar == 5) return;
			funcionesOpcion(iluminar + 9, mapaPersonajes, mapaLogros, mapaItems, mapaEnemigos, listaPersonajes);
            break;
        }
        wrefresh(ventana);
    }
}

void funcionesOpcion(int opcion,HashMap * mapaPersonajes, HashMap * mapaLogros, HashMap * mapaItems, HashMap * mapaEnemigos, List * listaPersonajes)
{
	char nombreBuscado[40];
	int idBuscado;

	clear();
	echo();

	switch (opcion)
	{
	case 0:
		mostrarSubMenu(mapaPersonajes, mapaLogros, mapaItems, mapaEnemigos, listaPersonajes);
		break;
	case 1:
		guardarInfoPersonajes(listaPersonajes);
		guardarInfoItems(mapaItems);
		clear();
		printw("Se guardaron los cambios\nApriete cualquier tecla para avanzar");
		getch();
		break;	
	case 2:
		printw("\nIngrese el nombre del item: ");
		scanw("%39[^\n]s", nombreBuscado);
		convertirMayuscula(nombreBuscado);
		buscarItemEspecifico(mapaItems, nombreBuscado);
		break;
	case 3:
		printw("\nIngrese el ID del logro: ");
		scanw("%i", &idBuscado);
		buscarLogroEspecifico(mapaLogros, idBuscado);
		break;
	case 4:
		printw("\nIngrese el nombre del enemigo: ");
		scanw("%39[^\n]s", nombreBuscado);
		convertirMayuscula(nombreBuscado);
		buscarEnemigoEspecifico(mapaEnemigos, nombreBuscado);
		break;
	case 5:
		mostrarPersonajes(mapaPersonajes);
		break;
	case 6:
		mostrarTodosItems(mapaItems);
		break;
	case 7:
		mostrarLogros(mapaLogros);
		break;
	case 8:
		mostrarEnemigos(mapaEnemigos);
		break;
	case 9:
		printw("\nIngrese el nombre del personaje a desbloquear: ");
		scanw("%39[^\n]s", nombreBuscado);
		convertirMayuscula(nombreBuscado);
		desbloquearPersonajes(mapaPersonajes, nombreBuscado);
		break;
	case 10:
		printw("\nIngrese el nombre del personaje: ");
		scanw("%39[^\n]s", nombreBuscado);
		convertirMayuscula(nombreBuscado);
		avanceMarcasLogros(mapaPersonajes, nombreBuscado);
		break;
	case 11:
		printw("\nIngrese el nombre del Item que encontro: ");
		scanw("%39[^\n]s", nombreBuscado);
		convertirMayuscula(nombreBuscado);
		encontrarItem(mapaItems, nombreBuscado);
		break;
	case 12:
		printw("\nIngrese el ID del logro que desbloqueo: ");
		scanw("%i", &idBuscado);
		desbloquearLogro(mapaLogros, idBuscado);
		break;
	case 13:
		printw("\nIngrese el nombre del Item que encontro: ");
		scanw("%39[^\n]s", nombreBuscado);
		convertirMayuscula(nombreBuscado);
		encontrarEnemigo(mapaEnemigos,nombreBuscado);
		break;
	}

	clear();
	noecho();
}